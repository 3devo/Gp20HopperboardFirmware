#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

class IrSensor {
  public:
    static constexpr uint8_t ADC_NUM_CHANNELS = 12;

    IrSensor(TwoWire& wire, uint8_t leds_addr, uint8_t adc_addr)
    : wire(wire), leds_addr(leds_addr), adc_addr(adc_addr)
    { }

    void setup();
    void do_reading(bool print);
    void get_last_reading(uint8_t (&output)[ADC_NUM_CHANNELS]);

    struct ErrorFlags {
      enum {
        LED_WRITE_ERROR = (1 << 0),
        ADC_WRITE_ERROR = (1 << 0),
        ADC_READ_ERROR  = (1 << 0),
      };
    };

    bool error_occured() { return this->error_flags != 0; };
    uint8_t get_and_clear_errors() { auto res = this->error_flags; this->error_flags = 0; return res; }

  private:
    TwoWire& wire;
    const uint8_t leds_addr;
    const uint8_t adc_addr;

    uint8_t last_measurement[ADC_NUM_CHANNELS];
    uint8_t error_flags = 0;

    void config_leds(uint8_t reg, uint8_t val);
    void config_adc(uint8_t cfg);
    void set_leds_pwm(uint8_t dc);
    void read_adc(uint8_t (&output)[ADC_NUM_CHANNELS]);

    void print_measurement(const char* title, const uint8_t (&output)[ADC_NUM_CHANNELS]);
};
