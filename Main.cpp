/*
 * Copyright (C) 2017 3devo (http://3devo.eu)
 *
 */

#include <Arduino.h>
#include <Wire.h>
#include "Hardware.h"

//Note: This reuses the SWCLK pin, so this disables SWD!
//#define ENABLE_SERIAL


#if defined(ENABLE_SERIAL)
HardwareSerial DebugSerial(DEBUG_TX, NC)
#endif

HardwareSerial RS485(RS485_TX, RS485_RX);
TwoWire WireIR(IRBOARD_SCL, IRBOARD_SDA);
TwoWire WireIface(IFACEBOARD_SCL, IFACEBOARD_SDA);

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
#endif

  pinMode(STATUS_PIN, OUTPUT);
}

void loop() {
}
