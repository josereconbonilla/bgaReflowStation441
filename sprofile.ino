void saveProfileSteps() 
{
  if (profile.profileSteps != EEPROM.read((currentProfile - 1) * 31)) 
  {
    EEPROM.update((currentProfile - 1) * 31,
                  profile.profileSteps);
  }
  
  delay(25);
  return;
}

void saveProfileStpoint2() 
{
  if (profile.Setpoint2 != EEPROM.read((currentProfile - 1) * 31 + 1)) 
  {
    EEPROM.update((currentProfile - 1) * 31 + 1,
                  profile.Setpoint2);
  }
  
  delay(25);
  return;
}

void saveProfileTempStep()
{
  if (profile.temperatureStep[editStep] != EEPROM.read((currentProfile - 1) * 31 + editStep + 2)) 
  {
    EEPROM.update(((currentProfile - 1) * 31 + editStep + 2),
                  profile.temperatureStep[editStep]);
  }
  
  delay(25);
  return;
}

void saveProfileRampStep() 
{
  if (profile.rampRateStep[editStep] != EEPROM.read((currentProfile - 1) * 31 + editStep + 11)) 
  {
    EEPROM.update(((currentProfile - 1) * 31 + editStep + 11),
                  profile.rampRateStep[editStep]);
  }
  
  delay(25);
  return;
}

void saveProfileDwell() 
{
  if (profile.dwellTimerStep[editStep] != EEPROM.read((currentProfile - 1) * 31 + editStep + 20)) 
  {
    EEPROM.update(((currentProfile - 1) * 31 + editStep + 20),
                  profile.dwellTimerStep[editStep]);
  }
  
  delay(25);
  return;
}

void saveProfilePowerBtm() 
{
  if (profile.pwr_BOTTOM != EEPROM.read((currentProfile - 1) * 31 + 29)) 
  {
    EEPROM.update((currentProfile - 1) * 31 + 29,
                  profile.pwr_BOTTOM);
  }
  
  delay(25);
  return;
}

void saveProfilePowerTop() 
{
  if (profile.pwr_TOP != EEPROM.read((currentProfile - 1) * 31 + 30)) 
  {
    EEPROM.update((currentProfile - 1) * 31 + 30,
                  profile.pwr_TOP);
  }
  
  delay(25);
  return;
}

void savePidPBtm()
{
  if (KpD != EEPROM_readDouble(KpD_Prof_Address)) 
  {
    EEPROM_writeDouble(KpD_Prof_Address, KpD);
  }
  
  delay(25);
  return;
}

void savePidIBtm() 
{
  if (KiD != EEPROM_readDouble(KiD_Prof_Address)) 
  {
    EEPROM_writeDouble(KiD_Prof_Address, KiD);
  }
  
  delay(25);
  return;
}

void savePidDBtm() {
  if (KdD != EEPROM_readDouble(KdD_Prof_Address)) {
    EEPROM_writeDouble(KdD_Prof_Address, KdD);
  }
  delay(25);
  return;
}

void savePidPTop() 
{
  if (KpU != EEPROM_readDouble(KpU_Prof_Address)) 
  {
    EEPROM_writeDouble(KpU_Prof_Address, KpU);
  }
  
  delay(25);
  return;
}

void savePidITop()
{
  if (KiU != EEPROM_readDouble(KiU_Prof_Address)) 
  {
    EEPROM_writeDouble(KiU_Prof_Address, KiU);
  }
  
  delay(25);
  return;
}

void savePidDTop() 
{
  if (KdU != EEPROM_readDouble(KdU_Prof_Address)) 
  {
    EEPROM_writeDouble(KdU_Prof_Address, KdU);
  }
  
  delay(25);
  return;
}
