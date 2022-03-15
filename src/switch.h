class SWITCH {
  public:
    SWITCH(uint8_t SwitchPIN);
    void init(uint32_t on_to = 500, uint32_t off_to = 500);
    bool status(void);
  private:
    uint8_t switch_pin; bool mode;
    bool last_mode;
    uint32_t pt;
    uint32_t on_time = 500;
    uint32_t off_time = 500;
};

SWITCH::SWITCH(uint8_t SwitchPIN) {
  pt = 0;
  switch_pin = SwitchPIN;
}

void SWITCH::init(uint32_t on_to, uint32_t off_to) {
  on_time = on_to;
  off_time = off_to;
  pinMode(switch_pin, INPUT_PULLUP);
  last_mode = mode = digitalRead(switch_pin); pt = 0;
}

bool SWITCH::status(void) {
  bool sw_on = digitalRead(switch_pin);
  uint32_t now_t = millis();
  if (last_mode != sw_on) {
    pt = now_t; last_mode = sw_on;
  } else {
    uint32_t switch_time = off_time;
    if (sw_on) switch_time = on_time;
    if (pt == 0) pt = now_t;
    if ((now_t - pt) >= switch_time) {
      mode = sw_on; pt = 0;
    }
  }
  return mode;
}
