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
    void set_leds_pwm(uint8_t dc);
    void do_reading(bool print);
    void get_last_reading(uint8_t (&output)[ADC_NUM_CHANNELS]);

    void detect_material(bool print_values);

    uint8_t get_last_blockage_pct();
    uint8_t get_and_clear_detected_blockage_pct_max();
    uint8_t get_and_clear_detected_blockage_pct_avg();

    struct ErrorFlags {
      enum {
        LED_WRITE_ERROR = (1 << 0),
        ADC_WRITE_ERROR = (1 << 1),
        ADC_READ_ERROR  = (1 << 2),
      };
    };

    bool error_occured() { return this->error_flags != 0; };
    uint8_t get_and_clear_errors() { auto res = this->error_flags; this->error_flags = 0; return res; }

  private:
    TwoWire& wire;
    const uint8_t leds_addr;
    const uint8_t adc_addr;

    uint8_t last_measurement[ADC_NUM_CHANNELS];
    uint8_t min_value[ADC_NUM_CHANNELS];
    uint16_t min_values_sum = 0;
    uint8_t error_flags = 0;

    uint8_t detection_threshold; // ADC value
    uint8_t detected_blockage_pct_max; // %
    uint16_t detected_blockage_pct_sum; // %
    uint16_t detected_blockage_count;
    uint8_t detected_blockage_pct_last; // %

    void config_leds(uint8_t reg, uint8_t val);
    void config_adc(uint8_t cfg);
    void read_adc(uint8_t (&output)[ADC_NUM_CHANNELS]);

    void print_measurement(const char* title, const uint8_t (&output)[ADC_NUM_CHANNELS]);
};
