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

// UART2
static const uint32_t DEBUG_TX = PA2;

// I2C2
static const uint32_t IRBOARD_SCL = PB10;
static const uint32_t IRBOARD_SDA = PB11;

// UART1
static const uint32_t RS485_TX = PA9;
static const uint32_t RS485_RX = PA10;
static const uint32_t RS485_DE = PA12;

static const uint32_t STATUS_PIN = PB9;

static const uint32_t MODE0 = PA5;
static const uint32_t MODE1 = PA6;
static const uint32_t MODE2 = PA7;

static const uint32_t STEPPER_NSLEEP = PB13;
static const uint32_t STEPPER_NENBL = PB1;
static const uint32_t STEPPER_DIR = PB2;
static const uint32_t STEPPER_STEP = PB0;
static const uint32_t STEPPER_NFAULT = PB12;

//I2C addresses
static const uint32_t LED_DRIVER_ADDR = 0x01;
static const uint32_t ADC_ADDR = 0x65;


// Used for SET_ADDRESS, just use 0 to only respond to the
// wildcard type.
const uint8_t INFO_HW_TYPE = 0;

// By default, listen to addresses a single address, different from the
// bootloader and auto-assigned ranges
const uint8_t INITIAL_ADDRESS = 8;
const uint8_t INITIAL_BITS = 8;

const uint8_t PROTOCOL_VERSION = 0x0000;
const uint16_t MAX_PACKET_LENGTH = 32;

#define BUS_USE_INTERRUPTS
#define USE_RS485
#define USE_LL_HAL

// Note: On protoboard < v0.3, this reuses the stepper MODE2 pin!
#define ENABLE_SERIAL
