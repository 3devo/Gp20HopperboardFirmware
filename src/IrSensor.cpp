#include "IrSensor.h"
#include "PCA9955B.h"

// TODO: Led fault checks
// TODO: Led power-on delay to reduce current spike?

void IrSensor::setup() {
  // MAX11605: The MSB selects between setup and config byte, which
  // decides the meaning of the lower bits.
  // TODO: Constants for these values
  // Setup byte: VDD as reference voltage, internal clock.
  config_adc(0b10000010);
  // Config byte: scan upto channel 12, single-ended.
  config_adc(0b00010111);

  set_leds_pwm(0);

  // Set 12 leds to PWM-controlled, leave the other four channels disabled.
  const uint8_t PWM = Pca9955bRegs::LedOutX::PWM;
  const uint8_t LEDOUTX_VALUE = (PWM << 6) | (PWM << 4) | (PWM << 2) | (PWM << 0);
  config_leds(Pca9955bRegs::LEDOUT0, LEDOUTX_VALUE);
  config_leds(Pca9955bRegs::LEDOUT1, LEDOUTX_VALUE);
  config_leds(Pca9955bRegs::LEDOUT2, LEDOUTX_VALUE);

  //IREFALL: Current 0-255.
  config_leds(Pca9955bRegs::IREFALL, 127);

  // TODO: less hardcoded, set this parameter by mainboard?
  detection_threshold = 5;

  memset(this->min_value, UINT8_MAX, sizeof(this->min_value));
}

void IrSensor::do_reading(bool print) {
  uint8_t measurement[ADC_NUM_CHANNELS];

  // ADC measures during I²C transaction (stretching the clock), so no
  // need to wait for ADC measurement to complete before reading.
  read_adc(measurement);

  // Disable interrupts to allow atomically reading the last_measurement
  // from an ISR
  noInterrupts();
  memcpy(this->last_measurement, measurement, sizeof(this->last_measurement));
  interrupts();

  if (print)
    print_measurement("RAW", measurement);
}

void IrSensor::print_measurement(const char *title, const uint8_t (&m)[ADC_NUM_CHANNELS]) {
#if defined(ENABLE_SERIAL)
  printf("%s", title);
  for (uint8_t i = 0; i < ADC_NUM_CHANNELS; ++i)
    printf(" %03u", m[i]);
  printf("\n");
#endif
}

void IrSensor::detect_material(bool print_values) {
  this->do_reading(print_values);

  uint32_t sum = 0, max_sum = 0;
  for (uint8_t i = 0; i < ADC_NUM_CHANNELS; ++i) {
    if (this->last_measurement[i] < this->min_value[i])
      this->min_value[i] = this->last_measurement[i];

    uint16_t threshold = this->min_value[i] + this->detection_threshold;
    if (threshold > UINT8_MAX)
      threshold = UINT8_MAX;
    max_sum += UINT8_MAX - threshold;

    if (this->last_measurement[i] > threshold)
      sum += (int32_t)this->last_measurement[i] - threshold;
  }
  // TODO: Include measurement interval (normalize to per second or so)?
  // TODO: Include max value (ambient light)?

  uint8_t result = 0;
  if (max_sum)
    result = sum * 100 / max_sum;

  noInterrupts();
  if (result > this->detected_blockage_pct_max)
    this->detected_blockage_pct_max = result;

  this->detected_blockage_pct_sum += result;
  this->detected_blockage_count++;

  this->detected_blockage_pct_last = result;
  interrupts();

#if defined(ENABLE_SERIAL)
  printf("material result= %d\%\n", result);
  printf("detected_blockage_pct_sum= %d\%, detected_blockage_count=%d\n", detected_blockage_pct_sum, detected_blockage_count);
#endif
}

void IrSensor::config_leds(uint8_t reg, uint8_t val) {
  // PCA9955B: Write register address and value to write to register.
  this->wire.beginTransmission(this->leds_addr);
  this->wire.write(reg);
  this->wire.write(val);
  auto res = this->wire.endTransmission();
  if (res != 0) {
    #if defined(ENABLE_SERIAL)
    printf("LEDs write failed: %u\n", res);
    #endif
    this->error_flags |= ErrorFlags::LED_WRITE_ERROR;
  }
}

void IrSensor::set_leds_pwm(uint8_t dc) {
  // PWMALL: Duty cycle 0-255.
  config_leds(Pca9955bRegs::PWMALL, dc);
}

void IrSensor::config_adc(uint8_t cfg) {
  this->wire.beginTransmission(this->adc_addr);
  this->wire.write(cfg);
  auto res = this->wire.endTransmission();
  if (res != 0) {
    #if defined(ENABLE_SERIAL)
    printf("ADC write failed: %u\n", res);
    #endif
    this->error_flags |= ErrorFlags::ADC_WRITE_ERROR;
  }
}

void IrSensor::read_adc(uint8_t (&output)[ADC_NUM_CHANNELS]) {
  // 8 bits per channel.
  auto res = this->wire.requestFrom(this->adc_addr, ADC_NUM_CHANNELS);
  if (res == ADC_NUM_CHANNELS) {
    for (int i = 0; i < ADC_NUM_CHANNELS; ++i)
      output[i] = this->wire.read();
  } else {
    #if defined(ENABLE_SERIAL)
    printf("ADC read failed\n");
    #endif
    this->error_flags |= ErrorFlags::ADC_READ_ERROR;
  }
}

void IrSensor::get_last_reading(uint8_t (&output)[ADC_NUM_CHANNELS]) {
  memcpy(output, this->last_measurement, sizeof(output));
}

void IrSensor::get_min_values(uint8_t (&output)[ADC_NUM_CHANNELS]) {
  memcpy(output, this->min_value, sizeof(output));
}

uint8_t IrSensor::get_last_blockage_pct() {
  return this->detected_blockage_pct_last;
}

uint8_t IrSensor::get_and_clear_detected_blockage_pct_max() {
  const uint8_t temp = this->detected_blockage_pct_max;
  this->detected_blockage_pct_max = 0;
  return temp;
}

uint8_t IrSensor::get_and_clear_detected_blockage_pct_avg() {
  uint8_t result = 0;

  if (this->detected_blockage_count)
    result = this->detected_blockage_pct_sum / this->detected_blockage_count;

  this->detected_blockage_pct_sum = 0;
  this->detected_blockage_count = 0;

  return result;
}

void IrSensor::apply_correction() {
  if (this->correction_budget < MAX_CORRECTION_BUDGET)
    this->correction_budget += 2;

  if (this->correction_budget > 0) {
    for (uint8_t i = 0; i < ADC_NUM_CHANNELS; ++i) {
      if (this->last_measurement[i] > this->min_value[i]) {
        ++this->min_value[i];
        --this->correction_budget;
      }
    }
  }
}
