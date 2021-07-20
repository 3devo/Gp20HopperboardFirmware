#include "IrSensor.h"
#include "PCA9955B.h"

// TODO: Led fault checks
// TODO: Led power-on delay to reduce current spike?

void IrSensor::setup() {
  // MAX11605: The MSB selects between setup and config byte, which
  // decides the meaning of the lower bits.
  // TODO: Constants for these values
  // Setup byte: VDD as reference voltage, internal clock
  config_adc(0b10000010);
  // Config byte: scan upto channel 12, single-ended
  config_adc(0b00010111);

  set_leds_pwm(0);

  // Set 12 leds to PWM-controlled, leave the other four channels disabled
  const uint8_t PWM = Pca9955bRegs::LedOutX::PWM;
  const uint8_t LEDOUTX_VALUE = (PWM << 6) | (PWM << 4) | (PWM << 2) | (PWM << 0);
  config_leds(Pca9955bRegs::LEDOUT0, LEDOUTX_VALUE);
  config_leds(Pca9955bRegs::LEDOUT1, LEDOUTX_VALUE);
  config_leds(Pca9955bRegs::LEDOUT2, LEDOUTX_VALUE);

  //IREFALL: Current 0-255
  config_leds(Pca9955bRegs::IREFALL, 127);
}

void IrSensor::do_reading() {
  uint8_t measurement[ADC_NUM_CHANNELS];

  set_leds_pwm(255);
  // ADC measures during I²C transaction (stretching the clock), so no
  // need to wait for ADC measurement to complete before reading.
  read_adc(measurement);
  set_leds_pwm(0);

  // Disable interrupts to allow atomically reading the last_measurement
  // from an ISR
  noInterrupts();
  memcpy(this->last_measurement, measurement, sizeof(this->last_measurement));
  interrupts();

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

void IrSensor::config_leds(uint8_t reg, uint8_t val) {
  // PCA9955B: Write register address and value to write to register
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
  // PWMALL: Duty cycle 0-255
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
  // 8 bits per channel
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
