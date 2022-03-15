/*
  Arduino Uno Pin Assignment
  D2 = encoder Middle
  D3 = Zero Cross Detect Circuit
  D4 = Topheater Fan
  D5 = encoder aPin
  D6 = encoder bPin
  D7 = Buzzer Pin
  D8 = CS1 Max6675
  D9 = Output preheater
  D10 = Output topheater
  D11 = CS2 Max6675
  D12 = SO Max6675
  D13 = SCK Max6675
  A0 = Toggle Switch
  A1 = protection Relay pin
  A4 = SCL
  A5 = SDA

  NOTE:
  THIS PROJECT HAS BEEN FINALIZED! no more Updates for ARDUINO R3 Edition
  ESP32 w/ NEXTION HMI display has already began.. will be posting it soon
  
  New hardwares:
  >> 1. MLX90614
  >> 2. ESP32
  >> 3. AD8945 
  >> 4. ACS712
  >> 5. DS3231
  >> 6. NEXTION NX4832T035 display
  >> 7. 4 pcs 240*60mm 600w Bottom Ceramic Heating Plate for Bga Rework Station
  >> 8. 220V Infrared Upper Heater Top Head for LY IR6000 BGA Rework Station with Built-in 450W Ceramic
  >> 9. TXS0108E logic level shifter
  
  New Software >> BGA Rework v.01 App (connected via WIFI/BT) //still in progress
               >> BGA Rework v.01 PC (Connected via wifi/Bt/USB) //still in progress
  
  if your an IT or an Engineer with the background in electronics,
  just use your imagination and analyze the flaw of the program to
  make you fully understand how it works.
  LOL

  this is the 441th attempt to finally make it work flawlessly without a glitch
  this one uses library modifications redesigned to work with ARDUINO UNO R3

  the preheater i am using >>
  >>type: ceramic /size:240x240x10mm /power dis: 800W /color:white (power out for this is typically set from 80 > 100%)

  the top heater i am using >>
  >>1. type: ceramic /size:60x60x10mm /power dis: 250W /color:black (power out for this  = 60 > 75%)
  >>2. type: ceramic /size:80x80x10mm /power dis: 400W /color:white (power out for this  = 80 > 90%)
  >>3. type: hotair /power dis: 850W (power out for this  = 50 > 100%)


  "Sketch uses 30042 bytes (97%) of program storage space.
  Global variables use 1137 bytes (50%) of dynamic memory."


  PID for this is included in the progmem
                                                          BOTTOM    *UP
  const float DF_PID_P[]  __attribute__((__progmem__)) = {15.92, 1.99};
  const float DF_PID_I[]  __attribute__((__progmem__)) = {0.64, 0.35};
  const float DF_PID_D[]  __attribute__((__progmem__)) = {1640.79, 46.66};

  parts used:
  1 - arduino uno r3 development board
  2 pcs max 6675
  2 pcs bta 16 triac
  1 pc 4n25 opto isolator
  2 pcs 3021 opto isolator
  1 lcd i2c 16x2
  1 rotary encoder with button
  1 pc 3x5mm on/off toggle switch (rocker switch)
  3 pcs 5x8mm on/off toggle switch(rocker switch)
  1 pc 2 way switching power supply 5v 12v 3A
  1 pc 240x240mm far infrared preheating ceramic
  1 pc 80x80mm far infrared ceramic for top heater
  1 pc 240mm brushless ball bearing fan = for preventing the heat from the preheater to the chassi
  1 pc 80mm brushless ball bearing fan = to cool down the top heater
  2 pcs 60mm brushless ball bearing fan = to cool down the chassi internal parts
  1 pc desoldering pen
  1 pc vacumm pump 12vdc
  2 pcs led lamp
  1 laser diode
  2 set BGA aluminum bracket
  1 set adjustable stand
  4 pcs n type mosfet driver
  1 pc dpst relay 5v
  1 pc pc817 opto isolator
  and a lots of resistors.
  10 Kohms
  1 Kohms
  10 Ohms
  47 ohms
  33 ohms

  the chassi i used came from an old hot air station

  br: Joserecon Bonilla
  email me: josereconbonilla@gmail.com
*/
