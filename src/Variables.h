byte tunerAccu1,
     tunerAccu2;

byte last,
     value;

float Input1,
      Input2;

byte Setpoint1;
byte Setpoint22;

struct Profile {
  byte profileSteps;
  byte Setpoint2;
  byte temperatureStep[9];
  byte rampRateStep[9];
  byte dwellTimerStep[9];
  byte pwr_BOTTOM;
  byte pwr_TOP;
};

int sizeOfProfile = sizeof(Profile);
Profile profile;

byte currentProfile = 1,
     currentStep = 1,
     editStep = 0;


const int eeprom_id_address = 500;

const int KpU_Prof_Address = 300,
          KiU_Prof_Address = 308,
          KdU_Prof_Address = 316,
          KpD_Prof_Address = 324,
          KiD_Prof_Address = 332,
          KdD_Prof_Address = 340;

double KpU,
       KiU,
       KdU,
       KpD,
       KiD,
       KdD;

char buf[32];

byte P_D,
     P_U,
     P_D_Serial,
     P_U_Serial;


boolean TopStart = false,
        flag = false,
        flagB = false,
        updateScreen = true;

const byte GC[8] = {0b01100,
                    0b10010,
                    0b10010,
                    0b01100,
                    0b00000,
                    0b00000,
                    0b00000,
                    0b00000
                   };
const byte Th[8] = { 0b01110,
                     0b11111,
                     0b10001,
                     0b00000,
                     0b01001,
                     0b10010,
                     0b01001,
                     0b00000
                   };
const byte Blh[8] = { 0b00000,
                      0b01001,
                      0b10010,
                      0b01001,
                      0b00000,
                      0b11111,
                      0b01111,
                      0b00111
                    };
const byte Bch[8] = { 0b00000,
                      0b01001,
                      0b10010,
                      0b01001,
                      0b00000,
                      0b11111,
                      0b11111,
                      0b11111
                    };
const byte Brh[8] = { 0b00000,
                      0b01001,
                      0b10010,
                      0b01001,
                      0b00000,
                      0b11111,
                      0b11110,
                      0b11100
                    };

const byte customChar[8] = {0b11111,
                            0b00100,
                            0b00100,
                            0b00100,
                            0b10001,
                            0b11011,
                            0b10101,
                            0b10001
                           };
byte startTemp,
     startTempB,
     setpointRamp,
     setpointRampB,
     curCount = 0;

volatile byte counter,
         counterB;

unsigned long previousMillis,
         previousMillisB,
         nextRead1,
         nextRead2,
         nextRead3;

byte tc1,
     tc2;

boolean format = false;
boolean initEepromComplete = false;
boolean alarmOn = false;

byte alarmCount = 0;

boolean nex, prev;
unsigned long ms_button = 0;

byte toggleSwitchState = 0;
int increment, decrement;

int screen = 0;
int Status = 0;
int selection = 0;
int selection2 = 0;
int pidCursor = 0;
int pidCursorU = 0;
int pidCursorD = 0;
int pidEdit = 0;
int pidSelect = 0;
boolean tuning1 = false, tuning2 = false;

byte tuneSP = 150;

void sReset(void);
void delay_us(uint16_t tic_us); 
void beep_(uint16_t dur, uint16_t frq) ;
void initPins(void);
void initTimers(void);
void welcomeTone(void);

void sReset(void) 
{
  asm volatile ("jmp 0");
}

void delay_us(uint16_t tic_us) 
{
  tic_us *= 4;
  __asm__ volatile("1: sbiw %0,1"
                   "\n\t""brne 1b":
                   "=w" (tic_us):
                   "0" (tic_us));
}

void beep_(uint16_t dur, uint16_t frq) 
{
  uint16_t per = 500000 / frq;
  dur = dur / (per / 250) * 2;
  for (uint16_t i = 0; i < dur; i++)
  {
    PORTD |= B10000000;
    delay_us(per);
    PORTD &= B01111111;
    delay_us(per);
  }
}

void welcomeTone(void)
{
  beep_(200, 523);
  beep_(200, 659);
  beep_(200, 784);
  beep_(200, 1046);
}
