class modedPID {
  public:
    modedPID() {}
    modedPID(float new_kp, float new_ki, float new_kd, int16_t new_dt = 496) {
      setDt(new_dt); Kp = new_kp; Ki = new_ki; Kd = new_kd;
    }
    void setDirection(boolean direction) {
      _direction = direction;
    }
    void setLimits(int min_output, int max_output) {
      _minOut = min_output; _maxOut = max_output;
    }
    void setDt(int16_t new_dt) {
      _dt_s = new_dt / 1000.0f; _dt = new_dt;
    }
    float setpoint = 0; float input = 0; float output = 0;
    float Kp = 0.0; float Ki = 0.0; float Kd = 0.0;
    float integral = 0.0;
    float getResult() {
      float error = setpoint - input;
      float delta_input = prevInput - input;
      prevInput = input;
      if (_direction) {
        error = -error;
        delta_input = -delta_input;
      }
      output = _mode ? 0 : (error * Kp);
      output += delta_input * Kd / _dt_s;
      integral += error * Ki * _dt_s;
      if (_mode) integral += delta_input * Kp;
      integral = constrain(integral, _minOut, _maxOut);
      output += integral;
      output = constrain(output, _minOut, _maxOut);
      return output;
    }
  private:
    int16_t _dt = 496; float _dt_s = 0.1;
    boolean _mode = 0, _direction = 0;
    int _minOut = 0, _maxOut = 255;
    float prevInput = 0; uint32_t pidTimer = 0;
};

#define BUF_SIZE  4
#define M_PI 3.14159265358979323846

class modedPID_Tuner {
  public:
    void setParameters(bool newDirection, int newSteady, int newStep, int newWait, float newWindow, uint16_t newPulse, int newPeriod) {
      steady = newSteady;
      step = newDirection ? -newStep : newStep;
      wait = newWait;
      window = newWindow;
      pulse = newPulse;
      period = newPeriod;
    }
    void setInput(float input) {
      thisValue = input;
    }
    void compute() {
      if (millis() - tmr >= period) {
        tmr = millis();
        for (byte i = 0; i < BUF_SIZE - 1; i++) buf[i] = buf[i + 1];
        buf[BUF_SIZE - 1] = thisValue;
        derivative();
        realValue = thisDerivative * (BUF_SIZE - 1 ) + b;
        switch (state) {
          case 0:
            state = 1;
            startTime = millis();
            output = (steady);
            trigger = true;
            changeDir = false;
            oscTime = 1;
            prevOscTime = 0 ;
            break;
          case  1 :
            if ( millis () - startTime >= wait ) {
              startTime = millis();
              if (abs(prevValue - realValue) < window) {
                state = 2;
                steadyValue = realValue;
                maxVal = steadyValue;
                minVal = steadyValue;
                output = (steady - step);
              }
              prevValue = realValue;
            }
            break;
          case  2 :
            if (millis() - startTime >= pulse) {
              state = 3;
              startTime = millis();
              output = (steady + step);
            }
            break;
          case  3 :
            if (thisDerivative < 0 ) {
              if (changeDir && ! trigger) {
                changeDir = false;
                maxVal = thisDerivative * (BUF_SIZE / 2 - 1 ) + b;
              }
              if (realValue < steadyValue) {
                if (!trigger) {
                  trigger = 1;
                  output = (steady + step);
                }
              }
            } else {
              if (! changeDir && trigger) {
                changeDir = true;
                minVal = thisDerivative * (BUF_SIZE / 2 - 1 ) + b;
              }
              if (realValue > steadyValue) {
                if (trigger) {
                  trigger = 0;
                  output = (steady - step);
                  prevOscTime = oscTime;
                  oscTime = millis() - startTime;
                  accuracy = 100.0f * min(oscTime, prevOscTime) / max(oscTime, prevOscTime);
                  startTime = millis();
                  float Ku = abs ( 4.0 * step / (maxVal - minVal) / M_PI);
                  float Pu = oscTime / 1000.0;
                  PID_k [ 0 ] = TUNE_P_PID * Ku;
                  PID_k [ 1 ] = TUNE_I_PID * Ku / Pu;
                  PID_k [ 2 ] = TUNE_D_PID * Ku * Pu;
                }
              }
            }
            break;
        }
      }
    }
    float getPID_p() {
      return PID_k[0];
    }
    float getPID_i() {
      return PID_k[1];
    }
    float getPID_d() {
      return PID_k[2];
    }
    int getOutput() {
      return output;
    }
    byte getAccuracy() {
      return (state == 3) ? accuracy : 0;
    }
    void  reset () {
      state = 0;
    }

    void derivative() {
      float sumX = 0, sumY = 0, sumX2 = 0, sumXY = 0;
      for (int i = 0; i < BUF_SIZE; i++) {
        sumX += i;
        sumY += buf[i];
        sumX2 += i * i;
        sumXY += buf[i] * i;
      }
      thisDerivative = (float)(BUF_SIZE * sumXY - sumX * sumY) / (BUF_SIZE * sumX2 - sumX * sumX);
      b = (float)(sumY - thisDerivative * sumX) / BUF_SIZE;
    }

  private:
    int accuracy;
    int steady, step, wait;
    float window;
    uint16_t pulse, period;
    float steadyValue, thisValue, prevValue, realValue, thisDerivative, b;
    uint32_t startTime, tmr;
    int output;
    byte state = 0;
    bool trigger = true;
    bool changeDir = false;
    float maxVal, minVal;
    float buf[BUF_SIZE];
    int32_t oscTime = 0, prevOscTime = 0;
    float PID_k[3];
};
