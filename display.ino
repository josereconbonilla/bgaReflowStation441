void initDisplay()
{
  Wire.begin();
  Wire.setClock(10000);
  lcd.begin();
  lcd.backlight();
  lcd.createChar(0, GC);
  lcd.createChar(1, Th);
  lcd.createChar(2, Blh);
  lcd.createChar(3, Bch);
  lcd.createChar(4, Brh);
  lcd.createChar(5, customChar);
}

void displayEraseInf()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("ERASE"));
  lcd.setCursor(0, 1);
  for (int i = 0; i < 15; i++) {
    lcd.print(F(".")); delay(100);
  }
}

void displayRestoreInf()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("RESTORE"));
}

void displayRestoreInfDots()
{
  lcd.setCursor(0, 1);
  for (int i = 0; i < 15; i++)
  {
    lcd.print(F("."));
    delay(150);
  }
}

void welcomeScreen()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("-=SouthBridge"));
  lcd.write(byte(5));
  lcd.print(F("=-"));
  lcd.setCursor(2, 1);
  lcd.print(F("BGAIRS v"));
  lcd.print(VERSION);
  lcd.print(F("."));
  lcd.print(REVISION);
}

void lcdTemp(double inp, byte inpt, byte loc)
{
  lcd.setCursor(12, loc);
  if ((inp >= 250)) {
    lcd.print(F("Fail"));
  } else {
    if (inp <= 99) {
      lcd.print(inp);
    }
    else if (inp >= 100)
    {
      lcd.print(inpt);
      lcd.write(byte(0));
    }
  }
}

void showIdleScreen() {
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("RDY!"));
    showTopIcon(true);
    showBottomIcon(true);
    updateScreen = false;
  }

  lcd.setCursor(0, 1);
  lcd.print(F("Cp"));

  lcd.setCursor(2, 1);
  lcd.print(currentProfile, HEX);
  if (profile.temperatureStep[profile.profileSteps - 2] < 181)
  {
    lcd.print(F("+"));
  }
  else
  {
    lcd.print(F("-"));
  }

  lcd.setCursor(7, 0);
  if ((profile.temperatureStep[profile.profileSteps - 1]) < 100 )
  {
    lcd.print(" ");
    lcd.print(profile.temperatureStep[profile.profileSteps - 1]);
  }
  else
  {
    lcd.print(profile.temperatureStep[profile.profileSteps - 1]);
  }
  lcd.write(byte(0));
  lcd.print(F(" "));

  lcd.setCursor(7, 1);
  if ((profile.Setpoint2) < 100 )
  {
    lcd.print(" ");
    lcd.print(profile.Setpoint2);
  }
  else
  {
    lcd.print(profile.Setpoint2);
  }
  lcd.write(byte(0));
  lcd.print(F(" "));
}

void showSave() {
  lcd.setCursor(13, 1);
  lcd.print(F("<Sv"));
}

void showProfEditStepScr() {
  if (updateScreen) {
    clearLine2();
    lcd.setCursor(0, 1);
    lcd.print(F("Stp:"));
    lcd.print(profile.profileSteps);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  lcd.setCursor(9, 1);
  lcd.print(profile.profileSteps);
}

void showValScr(int val) {
  if (val < 100 && val >= 10) {
    lcd.print(" ");
    lcd.print(val);
  } else {
    lcd.print(val);
  }
}

void showEditPwrBtm() {
  if (updateScreen) {
    clearLine2();
    lcd.setCursor(0, 1);
    lcd.print(F("PB:"));
    showValScr(profile.pwr_BOTTOM);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  lcd.setCursor(9, 1);
  showValScr(profile.pwr_BOTTOM);
  lcd.print(F("%"));
}

void showEditPwrTop() {
  if (updateScreen) {
    clearLine2();
    lcd.setCursor(0, 1);
    lcd.print(F("PU:"));
    showValScr(profile.pwr_TOP);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  lcd.setCursor(9, 1);
  showValScr(profile.pwr_TOP);
  lcd.print(F("%"));
}

void showProfEditDwlScr() {
  if (updateScreen) {
    clearLine2();
    lcd.setCursor(0, 1);
    lcd.print(F("s-"));
    lcd.print(editStep + 1);
    lcd.print(F("d:"));
    showValScr(profile.dwellTimerStep[editStep]);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  lcd.setCursor(9, 1);
  showValScr(profile.dwellTimerStep[editStep]);
  lcd.print(F("s"));
}

void showTempStpEditScr() {
  if (updateScreen) {
    clearLine2();
    lcd.setCursor(0, 1);
    lcd.print(F("s-"));
    lcd.print(editStep + 1);
    lcd.print(F("t:"));
    showValScr(profile.temperatureStep[editStep]);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  lcd.setCursor(9, 1);
  showValScr(profile.temperatureStep[editStep]);
  lcd.write(byte(0));
}

void showProfSP2Edit() {
  if (updateScreen) {
    clearLine2();
    lcd.setCursor(0, 1);
    lcd.print(F("btmH:"));
    showValScr(profile.Setpoint2);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  lcd.setCursor(9, 1);
  showValScr(profile.Setpoint2);
  lcd.write(byte(0));
}

void showEditStepScreen() {
  float out_float;
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(F("s-"));
    lcd.print(editStep + 1);
    lcd.print(F("r:"));
    out_float = profile.rampRateStep[editStep] / 10.0;
    lcd.print(out_float, 1);
    lcd.print(F(">"));
    showSave();
    updateScreen = false;
  }
  if (editStep == 0) {
    lcd.setCursor(0, 0);
    lcd.print(tc1);
    lcd.write(byte(0));
    lcd.setCursor(12, 0);
    lcd.print(profile.Setpoint2);
    lcd.write(byte(0));
    showSave();
    updateScreen = false;
  }
  else {
    lcd.setCursor(0, 0);
    lcd.print(profile.temperatureStep[editStep - 1]);
    lcd.write(byte(0));
    lcd.setCursor(12, 0);
    lcd.print(profile.temperatureStep[editStep]);
    lcd.write(byte(0));
    updateScreen = false;
  }

  if (editStep == 0)
  {
    out_float = (profile.Setpoint2 - tc1)
                / (profile.rampRateStep[editStep] / 10.0);
    out_floatScr(out_float);
  }
  else
  {
    out_float = (profile.temperatureStep[editStep] - profile.temperatureStep[editStep - 1])
                / (profile.rampRateStep[editStep] / 10.0);
    out_floatScr(out_float);
  }
  lcd.setCursor(9, 1);
  out_float = profile.rampRateStep[editStep] / 10.0;
  lcd.print(out_float, 1);
  lcd.write(byte(0));
}

void out_floatScr(float val) {
  lcd.setCursor(6, 0);
  lcd.print(val, 0);
  lcd.print("s ");
}

void showRampTemp(byte comp,
                  byte val,
                  byte pos)
{
  lcd.setCursor(7, pos);
  if (comp < 100 && comp >= 10)
  {
    lcd.print(" ");
    lcd.print(val);
  } else {
    lcd.print(val);
  }
  lcd.write(byte(0));
}

void showSelLbl() {
  lcd.setCursor(12, 1);
  lcd.print(F("<sel"));
}

void showPIDMenu()
{
  showSelLbl();
  lcd.setCursor(1, 1);
  switch (pidCursor) {
    case 0: lcd.print(F("Bottom")); break;
    case 1: lcd.print(F("Top")); break;
    case 2: lcd.print(F("A-Tune")); break;
    case 3: lcd.print(F("Back")); break;
  }
}

void showPIDMenuHeader()
{
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("-=PID MENU=-"));
    updateScreen = false;
  }
}

void pidEditHeader()
{
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("-=PID EDIT=-"));
    updateScreen = false;
  }
}

void clearLine2()
{
  lcd.setCursor(0, 1);
  lcd.print("                ");
}

void profileEditSelScr()
{
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("-=Pr"));
    lcd.print(currentProfile);
    lcd.print(F(" EditMenu=-"));
    updateScreen = false;
  }
}

void profileEditSel()
{
  showSelLbl();
  lcd.setCursor(1, 1);
  switch (selection2) {
    case 0: lcd.print(F("P-Steps")); break;
    case 1: lcd.print(F("TempBtm")); break;
    case 2: lcd.print(F("TempTop")); break;
    case 3: lcd.print(F("RampRate")); break;
    case 4: lcd.print(F("SoakTime")); break;
    case 5: lcd.print(F("PowerBtm")); break;
    case 6: lcd.print(F("PowerTop")); break;
    case 7: lcd.print(F("Back")); break;
  }
}

void showPIDCurD()
{
  showSelLbl();
  lcd.setCursor(0, 1);
  switch (pidCursorD) {
    case 0: lcd.print(F("BtmP:")); lcd.print(KpD); break;
    case 1: lcd.print(F("BtmI:")); lcd.print(KiD); break;
    case 2: lcd.print(F("BtmD:")); lcd.print(KdD); break;
    case 3: lcd.print(F("Back")); break;
  }
  lcd.print(F("  "));
}

void showPIDCurU()
{
  showSelLbl();
  lcd.setCursor(0, 1);
  switch (pidCursorU) {
    case 0: lcd.print(F("TopP:")); lcd.print(KpU); break;
    case 1: lcd.print(F("TopI:")); lcd.print(KiU); break;
    case 2: lcd.print(F("TopD:")); lcd.print(KdU); break;
    case 3: lcd.print(F("Back")); break;
  }
  lcd.print(F("  "));
}

void showPIDTuneHeader()
{
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("-=PID Tune=-"));
    updateScreen = false;
  }
}

void showPIDTuneSel()
{
  showSelLbl();
  lcd.setCursor(1, 1);
  switch (selection) {
    case 0: lcd.print(F("Bottom")); break;
    case 1: lcd.print(F("Top")); break;
    case 2: lcd.print(F("GoBack")); break;
  }
}

void showPIDEdit()
{
  lcd.setCursor(0, 1);
  switch (pidEdit) {
    case 0: lcd.print(F("dP:")); lcd.print(KpD); break;
    case 1: lcd.print(F("dI:")); lcd.print(KiD); break;
    case 2: lcd.print(F("dD:")); lcd.print(KdD); break;
    case 3: lcd.print(F("uP:")); lcd.print(KpU); break;
    case 4: lcd.print(F("uI:")); lcd.print(KiU); break;
    case 5: lcd.print(F("uD:")); lcd.print(KdU); break;
  }
  lcd.print(F(" "));
  lcd.setCursor(13, 1);
  lcd.print(F("<sv"));
}

void showPulseOut(byte val, byte loc)
{
  lcd.setCursor(11, loc);
  if (val >= 20) {
    lcd.print(F("<"));
  }
  else if (val < 20) {
    lcd.print(F(" "));
  }
}

void rampScr()
{
  lcd.setCursor(0, 0);
  lcd.print(F("PreH"));
}

void showRampStatus()
{
  lcd.setCursor(0, 0);
  lcd.print(F("Stp"));
  lcd.print(currentStep);
}

void showDwellCount(int8_t count)
{
  lcd.setCursor(0, 0);
  lcd.print("c");
  lcd.print(count);
  lcd.print("s ");
}

void showReflowComplete()
{
  if (updateScreen) {
    lcd.setCursor(0, 0);
    lcd.print(F("Done"));
    showPulseOut(0, 0);
    showPulseOut(0, 1);
    updateScreen = false;
  }
}

void rstScreen()
{
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(F("-=REBOOT=-"));
  lcd.setCursor(5, 1);
  lcd.print(F("in:"));
  for (uint8_t i = 20; i > 0; i--) {
    lcd.setCursor(9, 1);
    if (i >= 10)
    {
      lcd.print(i);
    }
    else if (i < 10)
    {
      lcd.print(F("0"));
      lcd.print(i);
    }
    lcd.print(F("s"));
    delay(1000);
  }
}

void showTopIcon(bool stat)
{
  lcd.setCursor(5, 0);
  if (stat) {
    lcd.write(byte(1));
  } else {
    lcd.print(F(" "));
  }
}

void showBottomIcon(bool stat)
{
  lcd.setCursor(4, 1);
  if (stat) {
    lcd.write(byte(2));
    lcd.write(byte(3));
    lcd.write(byte(4));
  } else {
    lcd.print(F("   "));
  }
}

void showTuningHeader()
{
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Tune"));
    lcd.setCursor(0, 1);
    lcd.print(F("Ramp"));
    updateScreen = false;
  }
}

void tuneScreenShow(byte outpt,
                    byte accu,
                    byte loc)
{
  byte out = map(outpt, 0, 150, 0, 100);
  lcd.setCursor(7, 0);
  if (out < 100 && out >= 10)
  {
    lcd.print(F(" "));
    lcd.print (out);
  }
  else if (out < 10 && out >= 1)
  {
    lcd.print(F("  "));
    lcd.print (out);
  }
  else if (out == 0)
  {
    lcd.print(F("  0"));
  }
  lcd.print(F("%"));

  lcd.setCursor(7, 1);
  if (accu < 100 && accu >= 10)
  {
    lcd.print(F(" "));
    lcd.print(accu);
  }
  else if (accu < 10 && accu >= 1)
  {
    lcd.print(F("  "));
    lcd.print(accu);
  }
  else if (accu == 0) {
    lcd.print(F("  0"));
  }
  lcd.print(F("%"));

  showPulseOut(out, loc);
}

void showResetHeader()
{
  if (updateScreen) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Reset Factory?"));
    updateScreen = false;
  }
}

void tuningTrueDisp()
{
  lcd.setCursor(0, 1);
  lcd.print(F("Calc"));
}

void dispFormatConfirm(bool curcnt)
{
  lcd.setCursor(2, 1);
  if (curcnt)
  {
    lcd.print(F("d-clk> cont."));
  }
  else
  {
    lcd.print(F("l-prs> cancl"));
  }
}

void startSerialCom()
{
  sprintf (buf, "SYNC\r\n");
  Serial.print(buf);
}
