void toggleIdleFunc() {
  curCount = 0;
  P_D = 0;
  P_U = 0;
  currentStep = 1;

  flag = false;
  flagB = false;
  TopStart = false;

  PORTC |= B00000010;
  nextRead2 = millis();
  nextRead3 = millis();

  byte pwr1 = profile.pwr_TOP;
  byte pwrMaxOut1;
  pwrMaxOut1 = map(pwr1, 0, 100, 0, 150);
  regulator1.setDirection(0);
  regulator1.setDt(SENSOR_SAMPLING_TIME);
  regulator1.setLimits(0, pwrMaxOut1);

  byte pwr2 = profile.pwr_BOTTOM;
  byte pwrMaxOut2;
  pwrMaxOut2 = map(pwr2, 0, 100, 0, 150);
  regulator2.setDirection(0);
  regulator1.setDt(SENSOR_SAMPLING_TIME);
  regulator2.setLimits(0, pwrMaxOut2);

  screen = 11;
  Status = 1;
  return;
}

void toggleRunFunc() {
  beep_(200, 1094);
  beep_(80, 1046);
  P_D = 0;
  P_U = 0;
  previousMillis = 0;
  previousMillisB = 0;
  updateScreen = true;
  screen = 0;
  Status = 0;
}

void beginTuner1() {
  analogWrite(4, 128);
  Status = 4;
  screen = 16;
  PORTC |= B00000010;
  nextRead3 = millis();
  nextRead2 = millis();
  tuning1 = false;
  tuner1.setParameters(
    0,
    75,
    75,
    SENSOR_SAMPLING_TIME,
    SENSOR_SAMPLING_TIME,
    SENSOR_SAMPLING_TIME / 10,
    SENSOR_SAMPLING_TIME);
  regulator1.setDirection(0);
  regulator1.setDt(SENSOR_SAMPLING_TIME);
  regulator1.setLimits(0, 150);
  startSerialCom();
  return;
}

void beginTuner2() {
  Status = 2;
  screen = 16;
  PORTC |= B00000010;
  nextRead3 = millis();
  nextRead2 = millis();
  tuning2 = false;
  tuner2.setParameters(
    0,
    75,
    75,
    SENSOR_SAMPLING_TIME,
    SENSOR_SAMPLING_TIME,
    SENSOR_SAMPLING_TIME / 10,
    SENSOR_SAMPLING_TIME);
  regulator2.setDirection(0);
  regulator2.setDt(SENSOR_SAMPLING_TIME);
  regulator2.setLimits(0, 150);
  startSerialCom();
  return;
}
