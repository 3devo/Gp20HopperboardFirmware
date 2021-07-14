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

// Note: On protoboard < v0.3, this reuses the stepper MODE2 pin!
#define ENABLE_SERIAL

#if defined(ENABLE_SERIAL)
HardwareSerial DebugSerial(DEBUG_TX);
#endif

TwoWire WireIR(IRBOARD_SCL, IRBOARD_SDA);
TwoWire WireIface(IFACEBOARD_SCL, IFACEBOARD_SDA);

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

void assert_interrupt_pin() {
  digitalWrite(STATUS_PIN, LOW);
}

void clear_interrupt_pin() {
  digitalWrite(STATUS_PIN, HIGH);
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

  // Configure PA11/PA12 to disable remapping of PA9/PA10
  // https://github.com/stm32duino/Arduino_Core_STM32/issues/1180
  pinMode(PA11, INPUT);
  pinMode(PA12, INPUT);

  pinMode(STATUS_PIN, OUTPUT);
  clear_interrupt_pin();

  BusInit(INITIAL_ADDRESS, INITIAL_BITS);
}

void loop() {
}
