#define VERSION 4
#define REVISION 41

#define TUNE_P_PID 0.2
#define TUNE_I_PID 0.5
#define TUNE_D_PID 0.33

const int8_t eeprom_id = 'b';
const int8_t eeprom_reset = 'a';

const int8_t def_profileSteps[] __attribute__((__progmem__)) = {
  6, 9, 5, 3, 7, 4, 1, 2, 8
};

const int8_t def_Setpoint2[] __attribute__((__progmem__)) = {
  150, 150, 150, 150, 150, 150, 150, 150, 150
};

const int8_t def_temperatureStep[9][9] __attribute__((__progmem__)) = /*setpoint1_Steps*/
{
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*1*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*2*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*3*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*4*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*5*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*6*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*7*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225},/*8*/
  {100, 120, 140, 150, 180, 190, 210, 215, 225} /*9*/
};

const int8_t def_rampRateStep[9][9] __attribute__((__progmem__)) = /*ramprate_Steps*/
{ /*1    2     3    4   5   6   7   8   9*/
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5},
  {5, 5, 5, 5, 5, 5, 5, 5, 5}
};

const int8_t def_dwellTimerStep[9][9] __attribute__((__progmem__)) = /*soakTimer_Steps*/
{
  {5, 5, 5, 5, 5, 5, 5, 5, 60},
  {1, 1, 1, 1, 30, 60, 90, 90, 120},
  {3, 2, 1, 3, 2, 1, 15, 30, 60},
  {3, 3, 2, 2, 4, 15, 30, 90, 120},
  {10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10},
  {10, 10, 10, 10, 10, 10, 10, 10, 10}
};

const int8_t def_PWR_D[] __attribute__((__progmem__)) = {
  100, 75, 80, 50, 100, 60, 90, 100, 50
};

const int8_t def_PWR_U[] __attribute__((__progmem__)) = {
  100, 75, 80, 50, 100, 60, 90, 100, 50
};

const float DF_PID_P[]  __attribute__((__progmem__)) = {15.92, 1.99};
const float DF_PID_I[]  __attribute__((__progmem__)) = {0.64, 0.35};
const float DF_PID_D[]  __attribute__((__progmem__)) = {1640.79, 46.66};

#define SENSOR_SAMPLING_TIME 496
#define GRAPHICS_SAMPLING_TIME 1000
