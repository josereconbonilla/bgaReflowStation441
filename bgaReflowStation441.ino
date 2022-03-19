#pragma GCC optimize ("-Os")
#pragma GCC push_options

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "Arduino.h"
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include "src/Settings.h"
#include "src/encoder.h"
#include "src/switch.h"
#include "src/PID.h"
#include "src/Variables.h"
#include "src/max6675.h"
#include "src/system.h"

LiquidCrystal_I2C lcd(0x27,
                      16,
                      2);

modedEncoder *encoder;
SWITCH toggleSwitch(A0);
modedPID_Tuner tuner1;
modedPID_Tuner tuner2;

modedPID regulator1(TUNE_P_PID,
                    TUNE_I_PID,
                    TUNE_D_PID,
                    SENSOR_SAMPLING_TIME);

modedPID regulator2(TUNE_P_PID,
                    TUNE_I_PID,
                    TUNE_D_PID,
                    SENSOR_SAMPLING_TIME);


void setup() {
  Serial.begin(9600);

  initPins();
  initTimers();
  encoder = new modedEncoder(6,
                             5,
                             2);

  attachInterrupt(1,
                  Dimming,
                  RISING);

  toggleSwitch.init(500,
                    3000);

  initDisplay();
  welcomeScreen();
  welcomeTone();

  delay(3000);

  initEeprom();

  nextRead1 = millis();

  curCount = 0;
  P_D = 0;
  P_U = 0;
  currentStep = 1;
  updateScreen = true;
  flag = false;
  flagB = false;
  TopStart = false;
}

void loop() {
labelbegin:
  unsigned long currentMillis = millis();

  toggleSwitchState = toggleSwitch.status();
  toggleSwitchState = !toggleSwitchState;

  value += encoder->getValue();
  modedEncoder::Button b = encoder->getButton();
  increment = (value - last);
  decrement = (last - value);

  if (screen == 14 || alarmOn)
  {
    if (alarmCount < 15
        && toggleSwitchState)
    {
      alarmOn = true;
      beep_(80, 1046); delay(40);
      beep_(140, 1046); delay(100);
      alarmCount++;
    }
    else
    {
      alarmCount = 0;
      alarmOn = false;
    }
  }

  if (millis() > nextRead1)
  {
    nextRead1 = millis() + SENSOR_SAMPLING_TIME;
    Input1 = getTempHuman(1);
    Input2 = getTempHuman(2);
  }

  tc1 = Input1;
  tc2 = Input2;

  switch (screen)
  {
    case 0:  //idle screen

      showIdleScreen();
      lcdTemp(Input1, tc1, 0);
      lcdTemp(Input2, tc2, 1);

      if (toggleSwitchState)
      {
        beep_(200, 1046);
        beep_(80, 1094);
        toggleIdleFunc();
        startSerialCom();
      }

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          currentProfile++;
          if (currentProfile >= 9)
          {
            currentProfile = 9;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          currentProfile--;
          if (currentProfile <= 1)
          {
            currentProfile = 1;
          }
        }
      }
      loadProfile();
      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            screen = 17;
            selection2 = 0;
            updateScreen = true;
            break;
          case modedEncoder::Held: beep_(200, 1046); break;
          case modedEncoder::Released:
            beep_(40, 1046);
            Status = 3;
            screen = 15;
            nextRead2 = millis();
            updateScreen = true;
            break;
          case modedEncoder::DoubleClicked:
            beep_(40, 1046);
            updateScreen = true;
            screen = 8;
            pidCursor = 0;
            break;
        }
      }
      break;

    case 1: //edit profile steps screen
      showProfEditStepScr();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          profile.profileSteps++;
          if (profile.profileSteps >= 9)
          {
            profile.profileSteps = 9;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          profile.profileSteps--;
          if (profile.profileSteps <= 1)
          {
            profile.profileSteps = 1;
          }
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            saveProfileSteps();
            screen = 17;
            updateScreen = true;
            break;
        }
      }
      break;

    case 2: //edit preheater setpoint = preheater ramp rate speed missing
      showProfSP2Edit();
      if (value > last)
      { //encoder plus
        last = value;
        beep_(40, 1046);
        profile.Setpoint2 += increment;
        if (profile.Setpoint2 >= 230)
        {
          profile.Setpoint2 = 230;
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        beep_(40, 1046);
        profile.Setpoint2 -= decrement;
        if (profile.Setpoint2 <= 80)
        {
          profile.Setpoint2 = 80;
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(80, 1046);
            saveProfileStpoint2();
            screen = 17;
            editStep = 0;
            updateScreen = true;
            break;
        }
      }
      break;

    case 3: //edit setpoint for topheater >>stepping

      showTempStpEditScr();

      if (value > last)
      { //encoder plus
        last = value;
        beep_(40, 1046);
        profile.temperatureStep[editStep] += increment;
        if (profile.temperatureStep[editStep] >= 238)
        {
          profile.temperatureStep[editStep] = 238;
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        beep_(40, 1046);
        profile.temperatureStep[editStep] -= decrement;
        if (profile.temperatureStep[editStep] <= 80)
        {
          profile.temperatureStep[editStep] = 80;
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            if (editStep + 1 == profile.profileSteps)
            {
              saveProfileTempStep();
              editStep = 0;
              screen = 17;
              updateScreen = true;
            }
            else
            {
              saveProfileTempStep();
              editStep++;
              updateScreen = true;
            }
            break;
          case modedEncoder::Held: beep_(200, 1046); break;
          case modedEncoder::Released:
            beep_(40, 1046);
            saveProfileTempStep();
            loadProfile();
            editStep = 0;
            screen = 17;
            updateScreen = true;
            break;
        }
      }
      break;

    case 4: //edit ramprate for topheater >> stepping
      showEditStepScreen();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          profile.rampRateStep[editStep]++;
          if (profile.rampRateStep[editStep] >= 30)
          {
            profile.rampRateStep[editStep] = 30;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          profile.rampRateStep[editStep]--;
          if (profile.rampRateStep[editStep] <= 1)
          {
            profile.rampRateStep[editStep] = 1;
          }
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            if (editStep + 1 == profile.profileSteps)
            {
              saveProfileRampStep();
              updateScreen = true;
              editStep = 0;
              screen = 17;
            }
            else
            {
              saveProfileRampStep();
              editStep++;
              updateScreen = true;
            }
            break;
          case modedEncoder::Held: beep_(200, 1046); break;
          case modedEncoder::Released:
            beep_(40, 1046);
            saveProfileRampStep();
            loadProfile();
            screen = 17;
            editStep = 0;
            updateScreen = true;
            break;
        }
      }
      break;

    case 5: //edit dwelling timer for topheater >> stepping

      showProfEditDwlScr();

      if (value > last)
      { //encoder plus
        last = value;
        beep_(40, 1046);
        profile.dwellTimerStep[editStep] += increment;
        if (profile.dwellTimerStep[editStep] >= 240)
        {
          profile.dwellTimerStep[editStep] = 240;
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        beep_(40, 1046);
        profile.dwellTimerStep[editStep] -= decrement;
        if (profile.dwellTimerStep[editStep] <= 1)
        {
          profile.dwellTimerStep[editStep] = 1;
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            if (editStep + 1 == profile.profileSteps)
            {
              saveProfileDwell();
              editStep = 0;
              screen = 17;
              updateScreen = true;
            }
            else
            {
              saveProfileDwell();
              editStep++;
              updateScreen = true;
            }
            break;
          case modedEncoder::Held: beep_(200, 1046); break;
          case modedEncoder::Released:
            beep_(40, 1046);
            saveProfileDwell();
            loadProfile();
            screen = 17;
            editStep = 0;
            updateScreen = true;
            break;
        }
      }
      break;

    case 6: //edit power out for preheater

      showEditPwrBtm();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          profile.pwr_BOTTOM++;
          if (profile.pwr_BOTTOM >= 100)
          {
            profile.pwr_BOTTOM = 100;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          profile.pwr_BOTTOM--;
          if (profile.pwr_BOTTOM <= 1)
          {
            profile.pwr_BOTTOM = 1;
          }
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            saveProfilePowerBtm();
            screen = 17;
            updateScreen = true;
            break;
        }
      }
      break;

    case 7: //edit power out for topheater

      showEditPwrTop();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          profile.pwr_TOP++;
          if (profile.pwr_TOP >= 100)
          {
            profile.pwr_TOP = 100;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          profile.pwr_TOP--;
          if (profile.pwr_TOP <= 1)
          {
            profile.pwr_TOP = 1;
          }
        }
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            saveProfilePowerTop();
            loadProfile();
            screen = 17;
            updateScreen = true;
            break;
        }
      }
      break;

    case 8: //pid selection for topheater, preheater, and autotune menu
      showPIDMenuHeader();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        clearLine2();
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          pidCursor++;
          if (pidCursor > 3)
          {
            pidCursor = 0;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        clearLine2();
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          pidCursor--;
          if (pidCursor < 0)
          {
            pidCursor = 3;
          }
        }
      }
      showPIDMenu();
      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            updateScreen = true;
            pidCursorD = 0;
            switch (pidCursor)
            {
              case 0: screen = 9; pidSelect = 0; break;
              case 1: screen = 9; pidSelect = 1; break;
              case 2: screen = 15; break;
              case 3: screen = 0; break;
            }
            break;
        }
      }
      break;

    case 9: //pid select edit screen for preheater and topheater
      showPIDMenuHeader();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        clearLine2();
        if (pidSelect == 0)
        {
          if (nex && ( millis() - ms_button) > 250)
          {
            ms_button =  millis();
            beep_(40, 1046);
            nex = false;
            pidCursorD++;
            if (pidCursorD > 3)
            {
              pidCursorD = 0;
            }
          }
        }
        else if (pidSelect == 1)
        {
          if (nex && ( millis() - ms_button) > 250)
          {
            ms_button =  millis();
            beep_(40, 1046);
            nex = false;
            pidCursorU++;
            if (pidCursorU > 3)
            {
              pidCursorU = 0;
            }
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        clearLine2();
        if (pidSelect == 0)
        {
          if (prev && ( millis() - ms_button) > 250)
          {
            ms_button =  millis();
            beep_(40, 1046);
            prev = false;
            pidCursorD--;
            if (pidCursorD < 0)
            {
              pidCursorD = 3;
            }
          }
        }
        else if (pidSelect == 1)
        {
          if (prev && ( millis() - ms_button) > 250)
          {
            ms_button =  millis();
            beep_(40, 1046);
            prev = false;
            pidCursorU--;
            if (pidCursorU < 0)
            {
              pidCursorU = 3;
            }
          }
        }
      }

      if (pidSelect == 0)
      {
        showPIDCurD();
      }
      else if (pidSelect == 1)
      {
        showPIDCurU();
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(80, 1046);
            updateScreen = true;
            if (pidSelect == 0)
            {
              switch (pidCursorD)
              {
                case 0: screen = 10; pidEdit = 0; break;
                case 1: screen = 10; pidEdit = 1; break;
                case 2: screen = 10; pidEdit = 2; break;
                case 3: screen = 8; break;
              }
              break;
            }
            else if (pidSelect == 1) {
              switch (pidCursorU) {
                case 0: screen = 10; pidEdit = 3; break;
                case 1: screen = 10; pidEdit = 4; break;
                case 2: screen = 10; pidEdit = 5; break;
                case 3: screen = 8; break;
              }
            }
        }
      }

      break;

    case 10: // pid edit values for topheater and preheater
      pidEditHeader();
      if (value > last)
      { //encoder plus
        last = value;
        beep_(40, 1046);
        switch (pidEdit)
        {
          case 0:
            KpD += increment * 0.01;
            if (KpD >= 255.01)
            {
              KpD = 255;
            }
            break;
          case 1:
            KiD += increment * 0.01;
            if (KiD >= 255.01)
            {
              KiD = 255;
            }
            break;
          case 2:
            KdD += increment * 0.01;
            if (KdD >= 255.01)
            {
              KdD = 255;
            }
            break;
          case 3:
            KpU += increment * 0.01;
            if (KpU >= 255.01)
            {
              KpU = 255;
            }
            break;
          case 4:
            KiU += increment * 0.01;
            if (KiU >= 255.01)
            {
              KiU = 255;
            }
            break;
          case 5:
            KdU += increment * 0.01;
            if (KdU >= 255.01)
            {
              KdU = 255;
            }
            break;
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        beep_(40, 1046);
        switch (pidEdit)
        {
          case 0:
            KpD -= decrement * 0.01;
            if (KpD <= 0.00)
            {
              KpD = 0.00;
            }
            break;
          case 1:
            KiD -= decrement * 0.01;
            if (KiD <= 0.00)
            {
              KiD = 0.00;
            }
            break;
          case 2:
            KdD -= decrement * 0.01;
            if (KdD <= 0.00)
            {
              KdD = 0.00;
            }
            break;
          case 3:
            KpU -= decrement * 0.01;
            if (KpU <= 0.00)
            {
              KpU = 0.00;
            }
            break;
          case 4:
            KiU -= decrement * 0.01;
            if (KiU <= 0.00)
            {
              KiU = 0.00;
            }
            break;
          case 5:
            KdU -= decrement * 0.01;
            if (KdU <= 0.00)
            {
              KdU = 0.00;
            }
            break;
        }
      }
      showPIDEdit();
      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            beep_(80, 1046);
            switch (pidEdit) {
              case 0: savePidPBtm(); break;
              case 1: savePidIBtm(); break;
              case 2: savePidDBtm(); break;
              case 3: savePidPTop(); break;
              case 4: savePidITop(); break;
              case 5: savePidDTop(); break;
            }
            screen = 9;
            updateScreen = true;
            break;
        }
      }
      break;

    case 11: //ramp process

      if (flag)
      {
        counter = 0;
        analogWrite(4, 128);
        startTemp = tc1;
        setpointRamp = 0;
        flag = false;
      }

      if (!flagB)
      {
        flagB = true;
        startTempB = tc2;
        setpointRampB = 0;
        counterB = 0;
        rampScr();
      }

      if (profile.Setpoint2 > setpointRampB
          && (currentMillis - previousMillisB) > 1000 / (profile.rampRateStep[0] * 0.1))
      {
        previousMillisB = currentMillis;
        counterB = counterB + 1;
        setpointRampB = startTempB + counterB;
        Setpoint22 = setpointRampB;
        showRampTemp(setpointRampB, Setpoint22, 1);
      }

      if (!TopStart
          && tc2 < profile.Setpoint2)
      {
        showRampTemp(tc1, tc1, 0);
      }
      else if (!TopStart
               && tc2 >= profile.Setpoint2)
      {
        Setpoint22 = profile.Setpoint2;
        showRampTemp(Setpoint22, Setpoint22, 1);
        beep_(40, 1046);
        showRampStatus();
        flag = true;
        TopStart = true;
      }

      if (TopStart
          && (currentMillis - previousMillis) > 1000 / (profile.rampRateStep[currentStep - 1] * 0.1))
      {
        previousMillis = currentMillis;

        if (setpointRamp < profile.temperatureStep[currentStep - 1])
        {
          counter = counter + 1;
          setpointRamp = counter + startTemp;
        }
        else if (setpointRamp >= profile.temperatureStep[currentStep - 1])
        {
          setpointRamp = profile.temperatureStep[currentStep - 1];
        }
        Setpoint1 = setpointRamp;
        showRampTemp(setpointRamp, Setpoint1, 0);
      }

      if (setpointRamp >= profile.temperatureStep[currentStep - 1]
          && (TopStart))
      {
        screen = 12;
      }

      if (profile.temperatureStep[currentStep - 1] >= 180)
      {
        analogWrite(4, 85);
      }

      if (!toggleSwitchState)
      {
        toggleRunFunc();
      }
      break;

    case 12: //step process
      Setpoint1 = profile.temperatureStep[currentStep - 1];
      if ((Input1 >= profile.temperatureStep[currentStep - 1]))
      {
        counter = 0;
        screen = 13;
      }

      if (!toggleSwitchState)
      {
        toggleRunFunc();
      }
      break;

    case 13: //soak process
      if (currentMillis - previousMillis > 1000)
      {
        previousMillis = currentMillis;
        counter = counter + 1;
        int8_t d = profile.dwellTimerStep[currentStep - 1] - counter;
        showDwellCount(d);
      }

      if (counter == profile.dwellTimerStep[currentStep - 1])
      {
        counter = 0;
        setpointRamp = 0;
        if (profile.profileSteps == currentStep)
        {
          currentStep = 1;
          flag = false;
          TopStart = false;
          flagB = false;
          screen = 14;
          nextRead3 = millis();
          PORTC &= B11111101;
          updateScreen = true;
        }
        else
        {
          currentStep++;
          showRampStatus();
          beep_(100, 1046);
          flag = true;
          screen = 11;
        }
      }

      if (!toggleSwitchState)
      {
        toggleRunFunc();
      }
      break;

    case 14: //end of process
      Status = 0;
      P_D = 0;
      P_U = 0;
      previousMillis = 0;
      previousMillisB = 0;
      showPulseOut(0, 0);
      showPulseOut(0, 1);
      showReflowComplete();

      lcdTemp(Input1, tc1, 0);
      lcdTemp(Input2, tc2, 1);

      if (!toggleSwitchState)
      {
        beep_(200, 1094);
        beep_(80, 1046);
        screen = 0;
        updateScreen = true;
      }
      break;

    case 15: //pid autotune and restore default menu

      if (Status == 3)
      {
        showResetHeader();
      }

      if (Status != 3)
      {
        showPIDTuneHeader();
        if (value > last)
        { //encoder plus
          last = value;
          nex = true;
          clearLine2();
          if (nex && ( millis() - ms_button) > 250)
          {
            ms_button =  millis();
            beep_(40, 1046);
            nex = false;
            selection++;
            if (selection > 2)
            {
              selection = 0;
            }
          }
        }

        else if (value < last)
        { //encoder minus
          last = value;
          prev = true;
          clearLine2();
          if (prev && ( millis() - ms_button) > 250)
          {
            ms_button =  millis();
            beep_(80, 1046);
            prev = false;
            selection--;
            if (selection < 0)
            {
              selection = 2;
            }
          }
        }
        showPIDTuneSel();
      }

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Clicked:
            if (Status != 3)
            {
              beep_(40, 1046);
              updateScreen = true;
              if (selection == 0)
              {
                beginTuner2();
              }
              else if (selection == 1)
              {
                beginTuner1();
              }
              else if (selection == 2)
              {
                screen = 8;
              }
            }
            break;
          case modedEncoder::DoubleClicked:
            if (Status == 3)
            {
              beep_(40, 1046);
              format = true;
              updateScreen = true;
            }
            break;
          case modedEncoder::Held: beep_(200, 1046); break;
          case modedEncoder::Released:
            if (Status == 3)
            {
              Status = 0;
              beep_(40, 1046);
              updateScreen = true;
              screen = 0;
            }
            break;
        }
      }
      break;

    case 16: //autotune running
      showTuningHeader();

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b)
        {
          case modedEncoder::Held: beep_(200, 1046); break;
          case modedEncoder::Released:
            beep_(40, 1046);
            updateScreen = true;
            screen = 0;
            Status = 0;
            selection = 0;
            break;
        }
      }
      break;

    case 17: //profile edit menu selection

      profileEditSelScr();

      if (value > last)
      { //encoder plus
        last = value;
        nex = true;
        clearLine2();
        if (nex && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          nex = false;
          selection2++;
          if (selection2 > 7)
          {
            selection2 = 0;
          }
        }
      }

      else if (value < last)
      { //encoder minus
        last = value;
        prev = true;
        clearLine2();
        if (prev && ( millis() - ms_button) > 250)
        {
          ms_button =  millis();
          beep_(40, 1046);
          prev = false;
          selection2--;
          if (selection2 < 0)
          {
            selection2 = 7;
          }
        }
      }

      profileEditSel();

      if (b != modedEncoder::Open)
      { //encoder button
        switch (b) {
          case modedEncoder::Clicked:
            beep_(40, 1046);
            updateScreen = true;
            switch (selection2)
            {
              case 0: screen = 1; break;
              case 1: screen = 2; break;
              case 2: screen = 3; editStep = 0; break;
              case 3: screen = 4; editStep = 0; break;
              case 4: screen = 5; editStep = 0; break;
              case 5: screen = 6; break;
              case 6: screen = 7; break;
              case 7: screen = 0; break;
            }
            break;
        }
      }
      break;
  }

  if (Status == 0) //no process running
  {
    if (Input1 > 50)
    {
      if (((PIND & B00010000) >> 4) == LOW)
      {
        PORTD |= B00010000; //cooling fan for topheater >> on
      }
    }
    else if (Input1 < 50)
    {
      if (((PIND & B00010000) >> 4) == HIGH)
      {
        PORTD &= B11101111; //cooling fan for topheater >> off
      }
    }

    if (((PINC & B00000010) >> 1) == HIGH)
    {
      PORTC &= B11111101; //safety relay must be off in case of triac failure
    }
  }
  else if (Status == 1)  //running loaded profile
  {

    if (millis() > nextRead2)
    {
      nextRead2 = millis() + GRAPHICS_SAMPLING_TIME;
      if (curCount == 0)
      {
        if (TopStart == true)
        {
          showTopIcon(true);
        }
        if (flagB == true)
        {
          showBottomIcon(true);
        }
        curCount = 1;
      }
      else
      {
        showTopIcon(false);
        showBottomIcon(false);
        curCount = 0;
      }
    }

    int x = Setpoint1 - Input1;
    int gap1 = abs(x);
    if (gap1 < 10)
    {
      regulator1.Kp = KpU;
      regulator1.Ki += KiU;
      regulator1.Kd = KdU;
    }
    else
    {
      regulator1.Kp = TUNE_P_PID;
      regulator1.Ki += TUNE_I_PID;
      regulator1.Kd = TUNE_D_PID;
    }

    int y = Setpoint22 - Input2;
    int gap2 = abs(y);
    if (gap2 < 2)
    {
      regulator2.Kp = KpD;
      regulator2.Ki += KiD;
      regulator2.Kd = KdD;
    }
    else
    {
      regulator2.Kp = TUNE_P_PID;
      regulator2.Ki += TUNE_I_PID;
      regulator2.Kd = TUNE_D_PID;
    }

    lcdTemp(Input1, tc1, 0);
    lcdTemp(Input2, tc2, 1);

    if (millis() > nextRead3)
    {
      nextRead3 = millis() + SENSOR_SAMPLING_TIME;

      regulator1.setpoint = Setpoint1;
      regulator1.input = Input1;

      regulator2.setpoint = Setpoint22;
      regulator2.input = Input2;

      if (flagB)
      {
        P_D = regulator2.getResult();
        P_D_Serial = map(P_D, 0, 150 , 0 , 100);
        BottomHeaterON(P_D);
        showPulseOut(P_D_Serial, 1);
      }

      if (TopStart)
      {
        P_U = regulator1.getResult();
        P_U_Serial = map(P_U, 0, 150 , 0 , 100);
        TopHeaterON(P_U);
        showPulseOut(P_U_Serial, 0);
      }
      Serial.println(buf);
    }

    if (!flagB)
    {
      P_D = 0;
      BottomHeaterOFF();
    }

    if (!TopStart)
    {
      P_U = 0;
      TopHeaterOFF();
    }
  }

  else if (Status == 2) //running autotune >> preheater
  {

    lcdTemp(Input1, tc1, 0);
    lcdTemp(Input2, tc2, 1);
    if (millis() > nextRead3)
    {
      nextRead3 = millis() + SENSOR_SAMPLING_TIME;
      if (!tuning2)
      {
        regulator2.Kp = TUNE_P_PID;
        regulator2.Ki += TUNE_I_PID;
        regulator2.Kd = TUNE_D_PID;
        regulator2.setpoint = tuneSP;
        regulator2.input = Input2;
        P_D = regulator2.getResult();
        tunerAccu2 = 0;
      }
      else if (tuning2)
      {
        tuner2.setInput(Input2);
        tuner2.compute();
        P_D = tuner2.getOutput();
        tunerAccu2 = tuner2.getAccuracy();
      }
      BottomHeaterON(P_D);
      Serial.println(buf);
    }

    if (millis() > nextRead2)
    {
      nextRead2 = millis() + GRAPHICS_SAMPLING_TIME;
      if (curCount == 0)
      {
        showBottomIcon(true);
        curCount = 1;
      }
      else
      {
        showBottomIcon(false);
        curCount = 0;
      }
    }

    double tuneGap2 = abs(tuneSP - Input2);
    if (tuneGap2 < 0.5)
    {
      tuning2 = true;
      tuningTrueDisp();
    }

    if (tuning2)
    {
      if (tunerAccu2 > 97)
      {
        KpD = tuner2.getPID_p();
        KiD = tuner2.getPID_i();
        KdD = tuner2.getPID_d();
        tuner2.reset();
        savePidPBtm();
        savePidIBtm();
        savePidDBtm();
        loadPIDParam();
        tuning2 = false;
        screen = 0;
        Status = 0;
        selection = 0;
        BottomHeaterOFF();
        updateScreen = true;
      }
    }
    tuneScreenShow(P_D, tunerAccu2, 1);
    TopHeaterOFF();
  }

  else if (Status == 3) //restore factory settings
  {

    if (millis() > nextRead2)
    {
      nextRead2 = millis() + GRAPHICS_SAMPLING_TIME;
      if (curCount == 0)
      {
        curCount = 1;
        dispFormatConfirm(true);
      }
      else
      {
        dispFormatConfirm(false);
        curCount = 0;
      }
    }

    if (format)
    {
      EEPROM.write(eeprom_id_address, eeprom_reset);
      while (1)
      {
        initEeprom();
        if (initEepromComplete) {
          initEepromComplete = false;
          rstScreen();
          sReset();
        }
      }
    }
  }

  else if (Status == 4) //running autotune topheater
  {

    lcdTemp(Input1, tc1, 0);
    lcdTemp(Input2, tc2, 1);
    if (millis() > nextRead3)
    {
      nextRead3 = millis() + SENSOR_SAMPLING_TIME;
      if (tuning1 == false)
      {
        regulator1.Kp = TUNE_P_PID;
        regulator1.Ki += TUNE_I_PID;
        regulator1.Kd = TUNE_D_PID;
        regulator1.setpoint = tuneSP;
        regulator1.input = Input1;
        P_U = regulator1.getResult();
        tunerAccu1 = 0;
      }
      else
      {
        tuner1.setInput(Input1);
        tuner1.compute();
        P_U = tuner1.getOutput();
        tunerAccu1 = tuner1.getAccuracy();
      }
      TopHeaterON(P_U);
      Serial.println(buf);
    }

    if (millis() > nextRead2)
    {
      nextRead2 = millis() + GRAPHICS_SAMPLING_TIME;
      if (curCount == 0)
      {
        showTopIcon(true);
        curCount = 1;
      }
      else
      {
        showTopIcon(false);
        curCount = 0;
      }
    }

    double tuneGap1 = abs(tuneSP - Input1);
    if (tuneGap1 < 0.5)
    {
      tuning1 = true;
      tuningTrueDisp();
    }

    if (tuning1)
    {
      if (tunerAccu1 > 98)
      {
        KpU = tuner1.getPID_p();
        KiU = tuner1.getPID_i();
        KdU = tuner1.getPID_d();
        tuner1.reset();
        savePidPTop();
        savePidITop();
        savePidDTop();
        loadPIDParam();
        screen = 0;
        Status = 0;
        selection = 0;
        TopHeaterOFF();
        tuning1 = false;
        updateScreen = true;
      }
    }
    tuneScreenShow(P_U, tunerAccu1, 0);
    BottomHeaterOFF();
  }
  goto labelbegin;
}


#pragma GCC pop_options
