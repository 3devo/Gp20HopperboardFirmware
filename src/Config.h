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