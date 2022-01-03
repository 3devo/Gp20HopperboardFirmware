/*
 * Copyright (C) 2017 3devo (http://3devo.eu)
 *
 */

#define _GNU_SOURCE 1 // For fopencookie

#include <Arduino.h>
#include <Wire.h>
#include <stdio.h>
#include "Bus.h"
#include "BaseProtocol.h"
#include "Config.h"
#include "Stepper.h"
#include "IrSensor.h"
#include "SharedConstants.h"

using namespace hbfirmware;

#if defined(ENABLE_SERIAL)
HardwareSerial DebugSerial(DEBUG_TX);
#endif

TwoWire WireIR(IRBOARD_SDA, IRBOARD_SCL);

Stepper stepper(STEPPER_NENBL, STEPPER_DIR, STEPPER_STEP, STEPPER_NFAULT);
IrSensor ir_sensor(WireIR, LED_DRIVER_ADDR, ADC_ADDR);

#if defined(ENABLE_SERIAL)
static ssize_t uart_putchar (void *, const char *buf, size_t len) {
  return DebugSerial.write(buf, len);
}

static cookie_io_functions_t functions = {
  .read = NULL,
  .write = uart_putchar,
  .seek = NULL,
  .close = NULL
};
#endif // defined(ENABLE_SERIAL)

#if defined(ENABLE_SERIAL)
// Override the (weak) error handler defined by the STM32 core to
// capture errors with our usual handling.
void _Error_Handler(const char *file, int line) {
  printf("STM32 core error: %s:%d\n", file, line);
}
#endif // defined(ENABLE_SERIAL)

void assert_interrupt_pin() {
  digitalWrite(STATUS_PIN, LOW);
}

void clear_interrupt_pin() {
  digitalWrite(STATUS_PIN, HIGH);
}

void general_call_reset(TwoWire& wire) {
  wire.beginTransmission(0x00);
  wire.write(0x06);
  wire.endTransmission();
}

static bool reset_requested = false;

void resetSystem() {
  // This is called by BaseProtocol on an incoming general call reset.
  // Rather then resetting immediately, let the mainloop do it, to
  // prevent resetting halfway an I²C transaction, which might leave the
  // bus blocked.
  reset_requested = true;
}


// Note: This runs inside an ISR, so be sure to use volatile and disable
// interrupts elsewhere as appropriate.
// Also, debug prints that fill up the buffer will lock up the
// firmware...
cmd_result processCommand(uint8_t cmd, uint8_t * datain, uint8_t len, uint8_t *dataout, uint8_t maxLen) {
  switch ((Command)cmd) {
    case Command::GET_LAST_STATUS: {
      if (len != 0 || maxLen < 2)
        return cmd_result(Status::INVALID_ARGUMENTS);

      // Note that we run inside an interrupt, so there is no race condition here.
      clear_interrupt_pin();

      dataout[0] = ir_sensor.get_and_clear_errors();
      dataout[1] = stepper.get_and_clear_errors();

      return cmd_result(Status::COMMAND_OK, 2);
    }
    case Command::SET_STATE: {
      if (len != 4)
        return cmd_result(Status::INVALID_ARGUMENTS);

      uint16_t speed = datain[0] << 8 | datain[1];
      bool enabled = datain[2];
      bool reversed = datain[3];
      //Disabled, locks up ISR
      //printf("SET_STATE enabled=%u, speed=%u, reversed=%u\n", enabled, speed, reversed);
      stepper.set_speed(speed);
      stepper.set_enabled(enabled);
      stepper.set_reversed(reversed);

      return cmd_result(Status::COMMAND_OK);
    }
    case Command::GET_MEASUREMENT: {
      constexpr size_t N = ir_sensor.ADC_NUM_CHANNELS;
      // Return last raw sensor measurement, mostly for debugging the hopper sensor.
      if (len != 0 || maxLen < N)
        return cmd_result(Status::INVALID_ARGUMENTS);

      ir_sensor.get_last_reading(*(uint8_t (*)[N])dataout);

      return cmd_result(Status::COMMAND_OK, N);
    }
    default:
      return cmd_result(Status::COMMAND_NOT_SUPPORTED);
  }
}

void setup() {
#if defined(ENABLE_SERIAL)
  DebugSerial.begin(1000000);
  DebugSerial.println("Starting");
  // Setup stdout for printf. This is a GNU-specific extension to libc.
  stdout = fopencookie(NULL, "w", functions);
  // Disable buffering, so the callbacks get called right away.
  setbuf(stdout, nullptr);
#endif

  // Use fast-mode 400kHz speed. PCA9955B can go up to 1Mhz and MAX11600
  // up to 1.7Mhz, but limit speed to decrease noise susceptibility.
  // Also, HS mode seems to require additional handshaking.
  WireIR.begin();
  WireIR.setClock(400000);

  general_call_reset(WireIR);
  // PCA9955B needs max 1ms.
  delay(1);

  // Configure PA11/PA12 to disable remapping of PA9/PA10.
  // https://github.com/stm32duino/Arduino_Core_STM32/issues/1180
  pinMode(PA11, INPUT);
  pinMode(PA12, INPUT);

  pinMode(STATUS_PIN, OUTPUT);
  clear_interrupt_pin();

  // For simplicity, just hardcode these pins that can have fixed values
  // here, rather than passing them into the Stepper class with all the
  // extra overhead that results in.
  pinMode(MODE0, OUTPUT);
  pinMode(MODE1, OUTPUT);
  pinMode(MODE2, OUTPUT);
  digitalWrite(MODE0, HIGH);
  digitalWrite(MODE1, HIGH);
  digitalWrite(MODE2, HIGH);

  pinMode(STEPPER_NSLEEP, OUTPUT);
  digitalWrite(STEPPER_NSLEEP, HIGH);

  stepper.setup();
  ir_sensor.setup();

  BusInit(INITIAL_ADDRESS, INITIAL_BITS);
}

static uint16_t loop_count = 0;
void loop() {
  bool print = (loop_count++ % 512) == 0;
  ir_sensor.do_reading(print);

  if (ir_sensor.error_occured() || stepper.error_occured())
    assert_interrupt_pin();

  if (reset_requested)
    NVIC_SystemReset();
}
