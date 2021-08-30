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

      attachInterrupt(this->pin_fault, [this]() { this->fault_triggered = true; }, FALLING);
      this->fault_triggered = (digitalRead(this->pin_fault) == LOW);
    }

    void set_enabled(bool enabled) {
        digitalWrite(this->pin_enable, enabled ? LOW : HIGH);
    }

    void set_reversed(bool reversed) {
        digitalWrite(this->pin_dir, reversed ? LOW : HIGH);
    }

    void set_speed(uint16_t freq) {
      if (freq != 0) // Ignore 0, that seems to break HardwareTimer somehow
        this->timer->setPWM(this->timer_channel, this->pin_step, freq, 50 /* % DC */);
    }

    bool error_occured() { return this->fault_triggered; };
    uint8_t get_and_clear_errors() {
      // Note: This should be called with interrupts disabled to prevent
      // a small race condition
      auto res = this->fault_triggered;
      this->fault_triggered = false;
      return res;
    }

  private:
    uint16_t pin_enable;
    uint16_t pin_step;
    uint16_t pin_dir;
    uint16_t pin_fault;

    HardwareTimer *timer;
    uint32_t timer_channel;

    volatile bool fault_triggered;
};
