#pragma once
/*
 * Copyright (C) 2017 3devo (http://3devo.eu)
 *
 */

#include <Arduino.h>

static const uint32_t DEBUG_TX = PA2;

// I2C2
static const uint32_t IRBOARD_SCL = PB10;
static const uint32_t IRBOARD_SDA = PB11;

// I2C1
static const uint32_t IFACEBOARD_SCL = PB6;
static const uint32_t IFACEBOARD_SDA = PB7;

// I2C1
static const uint32_t RS485_TX = PA9;
static const uint32_t RS485_RX = PA10;
static const uint32_t RS485_DE = PA12;

static const uint32_t STATUS_PIN = PB9;

// Used for SET_ADDRESS, just use 0 to only respond to the
// wildcard type.
const uint8_t INFO_HW_TYPE = 0;

// By default, listen to addresses a single address, different from the
// bootloader and auto-assigned ranges
const uint8_t INITIAL_ADDRESS = 128;
const uint8_t INITIAL_BITS = 8;

const uint8_t PROTOCOL_VERSION = 0x0000;
const uint16_t MAX_PACKET_LENGTH = 32;

// TODO: Enable interrupts instead of polling in loops. Does not work,
// because the Arduino core defines ISRs for all USARTs, leading to
// linker errors.
//#define RS485_USE_INTERRUPTS
#define USE_RS485
#define USE_LL_HAL
