double EEPROM_readDouble(int address)
{
  double value = 0.0;
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(value); i++)
  {
    *p++ = EEPROM.read(address++);
  }
  return value;
}

void EEPROM_writeDouble(int address, double value)
{
  byte* p = (byte*)(void*)&value;
  for (volatile int i = 0; i < sizeof(value); i++)
  {
    EEPROM.update(address++, *p++);
    delay(25);
  }
}

void saveDefPID() {
  EEPROM_writeDouble(KpU_Prof_Address,
                     pgm_read_float(&DF_PID_P[1]));
  EEPROM_writeDouble(KiU_Prof_Address,
                     pgm_read_float(&DF_PID_I[1]));
  EEPROM_writeDouble(KdU_Prof_Address,
                     pgm_read_float(&DF_PID_D[1]));
  EEPROM_writeDouble(KpD_Prof_Address,
                     pgm_read_float(&DF_PID_P[0]));
  EEPROM_writeDouble(KiD_Prof_Address,
                     pgm_read_float(&DF_PID_I[0]));
  EEPROM_writeDouble(KdD_Prof_Address,
                     pgm_read_float(&DF_PID_D[0]));
  return;
}

void loadPIDParam() {
  KpU = EEPROM_readDouble(KpU_Prof_Address);
  KiU = EEPROM_readDouble(KiU_Prof_Address);
  KdU = EEPROM_readDouble(KdU_Prof_Address);

  KpD = EEPROM_readDouble(KpD_Prof_Address);
  KiD = EEPROM_readDouble(KiD_Prof_Address);
  KdD = EEPROM_readDouble(KdD_Prof_Address);
  return;
}

void saveDefProfile() {
  for (int thisCount = 0; thisCount < 9; thisCount++) {
    EEPROM.update(thisCount * 31,
                  (pgm_read_byte(&def_profileSteps[thisCount])));
  }
  for (int thisCount = 0; thisCount < 9; thisCount++) {
    EEPROM.update(thisCount * 31 + 1,
                  (pgm_read_byte(&def_Setpoint2[thisCount])));
  }

  for (int profCount = 0; profCount < 9; profCount++) {
    for (int thisCount = 0; thisCount < 9; thisCount++) {
      EEPROM.update(profCount * 31 + thisCount + 2,
                    (pgm_read_byte(&def_temperatureStep[profCount][thisCount])));
    }
  }

  for (int profCount = 0; profCount < 9; profCount++) {
    for (int thisCount = 0; thisCount < 9; thisCount++) {
      EEPROM.update(profCount * 31 + thisCount + 11,
                    (pgm_read_byte(&def_rampRateStep[profCount][thisCount])));
    }
  }

  for (int profCount = 0; profCount < 9; profCount++) {
    for (int thisCount = 0; thisCount < 9; thisCount++) {
      EEPROM.update(profCount * 31 + thisCount + 20,
                    (pgm_read_byte(&def_dwellTimerStep[profCount][thisCount])));
    }
  }

  for (int thisCount = 0; thisCount < 9; thisCount++) {
    EEPROM.update(thisCount * 31 + 29,
                  (pgm_read_byte(&def_PWR_D[thisCount])));
  }

  for (int thisCount = 0; thisCount < 9; thisCount++) {
    EEPROM.update(thisCount * 31 + 30,
                  (pgm_read_byte(&def_PWR_U[thisCount])));
  }
  return;
}

void loadProfile() {
  profile.profileSteps = EEPROM.read((currentProfile - 1) * 31);
  profile.Setpoint2 = EEPROM.read((currentProfile - 1) * 31 + 1);
  for (int i = 0; i < 9; i + 1) {
    profile.temperatureStep[i] = EEPROM.read((currentProfile - 1) * 31 + i + 2);
    i++;
  }
  for (int i = 0; i < 9; i + 1) {
    profile.rampRateStep[i] = EEPROM.read((currentProfile - 1) * 31 + i + 11);
    i++;
  }
  for (int i = 0; i < 9; i + 1) {
    profile.dwellTimerStep[i] = EEPROM.read((currentProfile - 1) * 31 + i + 20);
    i++;
  }
  profile.pwr_BOTTOM = EEPROM.read((currentProfile - 1) * 31 + 29);
  profile.pwr_TOP = EEPROM.read((currentProfile - 1) * 31 + 30);
  return;
}

void initEeprom() {
  byte firstTime = EEPROM.read(eeprom_id_address);
  if (firstTime == eeprom_id) {
    currentProfile = 1;
    loadProfile();
    loadPIDParam();
    return;
  }
  else {
    displayEraseInf();
    for (int i = 0; i < EEPROM.length(); i++) {
      EEPROM.write(i, 0);
    }
    EEPROM.update (500, eeprom_id);
    displayRestoreInf();
    saveDefProfile();
    saveDefPID();
    displayRestoreInfDots();
    delay(1000);
    currentProfile = 1;
    loadProfile();
    loadPIDParam();
    initEepromComplete = true;
  }
  return;
}
