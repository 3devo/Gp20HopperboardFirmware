/*
 * Copyright (C) 2017 3devo (http://3devo.eu)
 *
 */

#define _GNU_SOURCE 1 // For fopencookie

#include <Arduino.h>
#include <Wire.h>
#include <stdio.h>
#include "Config.h"

// Note: This reuses the stepper MODE2 pin!
#define ENABLE_SERIAL

#if defined(ENABLE_SERIAL)
HardwareSerial DebugSerial(DEBUG_TX);
#endif

HardwareSerial RS485(RS485_RX, RS485_TX);
TwoWire WireIR(IRBOARD_SCL, IRBOARD_SDA);
TwoWire WireIface(IFACEBOARD_SCL, IFACEBOARD_SDA);

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
  digitalWrite(STATUS_PIN, HIGH);
}

void clear_interrupt_pin() {
  digitalWrite(STATUS_PIN, LOW);
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
  RS485.begin(115200);
  // Configure PA11/PA12 to disable remapping of PA9/PA10
  // https://github.com/stm32duino/Arduino_Core_STM32/issues/1180
  pinMode(PA11, INPUT);
  pinMode(PA12, INPUT);

  pinMode(STATUS_PIN, OUTPUT);
}

void loop() {
}
