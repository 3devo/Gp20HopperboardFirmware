#pragma once

/*
 * Copyright (C) 2017-2025 3devo (http://www.3devo.eu)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    void get_min_values(uint8_t (&output)[ADC_NUM_CHANNELS]);

    void detect_material(bool print_values);

    uint8_t get_last_blockage_pct();
    uint8_t get_and_clear_detected_blockage_pct_max();
    uint8_t get_and_clear_detected_blockage_pct_avg();
    void apply_correction();

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

    int8_t correction_budget;
    const int8_t MAX_CORRECTION_BUDGET = 20;

    void config_leds(uint8_t reg, uint8_t val);
    void config_adc(uint8_t cfg);
    void read_adc(uint8_t (&output)[ADC_NUM_CHANNELS]);

    void print_measurement(const char* title, const uint8_t (&output)[ADC_NUM_CHANNELS]);
};
