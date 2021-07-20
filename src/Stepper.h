#pragma once

#include <Arduino.h>
#include "Config.h"

class Stepper {
  public:
    Stepper(
        uint16_t pin_enable,
        uint16_t pin_dir,
        uint16_t pin_step,
        uint16_t pin_fault
    ) : pin_enable(pin_enable), pin_step(pin_step), pin_dir(pin_dir), pin_fault(pin_fault)
    { }

    void setup() {
      pinMode(this->pin_step, OUTPUT);
      pinMode(this->pin_dir, OUTPUT);
      pinMode(this->pin_fault, INPUT);
      pinMode(this->pin_enable, OUTPUT);
      set_enabled(false);

      this->timer = new HardwareTimer((TIM_TypeDef *)pinmap_peripheral(digitalPinToPinName(this->pin_step), PinMap_PWM));
      this->timer_channel = STM_PIN_CHANNEL(pinmap_function(digitalPinToPinName(this->pin_step), PinMap_PWM));
    }

    void set_enabled(bool enabled) {
        digitalWrite(this->pin_enable, enabled ? LOW : HIGH);
    }

    void set_reversed(bool reversed) {
        digitalWrite(STEPPER_DIR, reversed ? HIGH : LOW);
    }

    void set_speed(uint16_t freq) {
      if (freq != 0) // Ignore 0, that seems to break HardwareTimer somehow
        this->timer->setPWM(this->timer_channel, this->pin_step, freq, 50 /* % DC */);
    }

    bool has_fault() {
      return digitalRead(this->pin_fault) == LOW;
    }

  private:
    uint16_t pin_enable;
    uint16_t pin_step;
    uint16_t pin_dir;
    uint16_t pin_fault;

    HardwareTimer *timer;
    uint32_t timer_channel;
};
