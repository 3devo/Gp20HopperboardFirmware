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

#if defined(ENABLE_SERIAL)
HardwareSerial DebugSerial(DEBUG_TX);
#endif

TwoWire WireIR(IRBOARD_SDA, IRBOARD_SCL);

Stepper stepper(STEPPER_NENBL, STEPPER_DIR, STEPPER_STEP, STEPPER_NFAULT);

struct Commands {
  enum {
    GET_LAST_STATUS = 0x80,
  };
};

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
// // capture errors with our usual handling.
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

// Note: This runs inside an ISR, so be sure to use volatile and disable
// interrupts elsewhere as appropriate.
cmd_result processCommand(uint8_t cmd, uint8_t * datain, uint8_t len, uint8_t *dataout, uint8_t maxLen) {
  switch (cmd) {
    case Commands::GET_LAST_STATUS: {
      if (len != 0 || maxLen < 1)
        return cmd_result(Status::INVALID_ARGUMENTS);

      // Note that we run inside an interrupt, so there is no race condition here
      clear_interrupt_pin();

      // Dummy value
      dataout[0] = 1;

      return cmd_result(Status::COMMAND_OK, 1);
    }
    default:
      return cmd_result(Status::COMMAND_NOT_SUPPORTED);
  }
}

void setup() {
#if defined(ENABLE_SERIAL)
  DebugSerial.begin(1000000);
  DebugSerial.println("Starting");
  /* Setup stdout for printf. This is a GNU-specific extension to libc. */
  stdout = fopencookie(NULL, "w", functions);
  /* Disable buffering, so the callbacks get called right away */
  setbuf(stdout, nullptr);
#endif

  // Use fast-mode 400kHz speed. PCA9955B can go up to 1Mhz and MAX11600
  // up to 1.7Mhz, but limit speed to decrease noise susceptibility.
  // Also, HS mode seems to require additional handshaking.
  WireIR.setClock(400000);
  WireIR.begin();

  general_call_reset(WireIR);
  // PCA9955B needs max 1ms
  delay(1);

  // Configure PA11/PA12 to disable remapping of PA9/PA10
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

  BusInit(INITIAL_ADDRESS, INITIAL_BITS);
}

void loop() {
}
