class modedEncoder
{
  public:
    typedef enum Button_e {Open = 0, Closed, Pressed, Held, Released, Clicked, DoubleClicked} Button;
  public:
    modedEncoder(uint8_t A, uint8_t B, uint8_t BTN = -1, uint8_t stepsPerNotch = 1, bool active = LOW);
    void service(void);
    int16_t getValue(void);
  public:
    Button getButton(void);
  private:
    const uint8_t pinA;
    const uint8_t pinB;
    const uint8_t pinBTN;
    const bool pinsActive;
    volatile int16_t delta;
    volatile int16_t last;
    uint8_t steps;
    volatile Button button;
    bool doubleClickEnabled;
    uint16_t keyDownTicks = 0;
    uint8_t doubleClickTicks = 0;
    unsigned long lastButtonCheck = 0;
};

modedEncoder::modedEncoder(uint8_t A, uint8_t B, uint8_t BTN, uint8_t stepsPerNotch, bool active)
  : doubleClickEnabled(true), delta(0), last(0), button(Open), steps(stepsPerNotch),
    pinA(A), pinB(B), pinBTN(BTN), pinsActive(active)
{
  uint8_t configType = (pinsActive == LOW) ? INPUT_PULLUP : INPUT;
  pinMode(pinA, configType);
  pinMode(pinB, configType);
  pinMode(pinBTN, configType);
  if (digitalRead(pinA) == pinsActive) {
    last = 3;
  }
  if (digitalRead(pinB) == pinsActive) {
    last ^= 1;
  }
}

void modedEncoder::service(void) {
  bool moved = false;
  unsigned long now = millis();
  int8_t curr = 0;
  if (digitalRead(pinA) == pinsActive) curr = 3;
  if (digitalRead(pinB) == pinsActive) curr ^= 1;
  int8_t diff = last - curr;
  if (diff & 1) {
    last = curr;
    delta += (diff & 2) - 1;
    moved = true;
  }
  if (pinBTN > 0 && (now - lastButtonCheck) >= 10) {
    lastButtonCheck = now;
    if (digitalRead(pinBTN) == pinsActive) {
      keyDownTicks++;
      if (keyDownTicks > (1200 / 10)) button = Held;
    }
    if (digitalRead(pinBTN) == !pinsActive) {
      if (keyDownTicks) {
        if (button == Held) {
          button = Released;
          doubleClickTicks = 0;
        }
        else {
          if (doubleClickTicks > 1) {
            if (doubleClickTicks < (600 / 10)) {
              button = DoubleClicked;
              doubleClickTicks = 0;
            }
          }
          else {
            doubleClickTicks = (doubleClickEnabled) ? (600 / 10) : 1;
          }
        }
      }
      keyDownTicks = 0;
    }
    if (doubleClickTicks > 0) {
      doubleClickTicks--;
      if (--doubleClickTicks == 0) button = Clicked;
    }
  }
}

int16_t modedEncoder::getValue(void) {
  int16_t val;
  cli();
  val = delta;
  if (steps == 2) delta = val & 1;
  else if (steps == 4) delta = val & 3;
  else delta = 0;
  sei();
  if (steps == 4) val >>= 2;
  if (steps == 2) val >>= 1;
  int16_t r = 0;
  if (val < 0) r -= 1;
  else if (val > 0) r += 1;
  return r;
}

modedEncoder::Button modedEncoder::getButton(void) {
  modedEncoder::Button ret = button;
  if (button != modedEncoder::Held) button = modedEncoder::Open;
  return ret;
}
