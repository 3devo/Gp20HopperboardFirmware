#pragma once

/*
 * Copyright (C) 2017-2025 3devo (http://www.3devo.eu)
 *
 * Permission is hereby granted, free of charge, to anyone obtaining
 * a copy of this document, to do whatever they want with them without
 * any restriction, including, but not limited to, copying, modification
 * and redistribution.
 *
 * NO WARRANTY OF ANY KIND IS PROVIDED.
 */

// This file is intended to be shared between the child and mainboard
// firmware, to prevent duplication of constants.

#include <Arduino.h>

namespace hbfirmware {
  enum class Command {
    GET_LAST_STATUS = 0x80,
    SET_STATE = 0x81,
    GET_MEASUREMENT = 0x82,
    GET_AND_CLEAR_SENSOR_VALUES = 0x83,
  };

  inline const __FlashStringHelper *to_string(Command cmd) {
    switch (cmd) {
      case Command::GET_LAST_STATUS:              return F("GET_LAST_STATUS");
      case Command::SET_STATE:                    return F("SET_STATE");
      case Command::GET_MEASUREMENT:              return F("GET_MEASUREMENT");
      case Command::GET_AND_CLEAR_SENSOR_VALUES:  return F("GET_AND_CLEAR_SENSOR_VALUES");
      default:                                    return nullptr;
    }
  }

  enum class SensorState : uint8_t {
    STOPPED,
    MEASURING,
  };

  inline const __FlashStringHelper *to_string(SensorState state) {
    switch (state) {
      case SensorState::STOPPED:    return F("STOPPED");
      case SensorState::MEASURING:  return F("MEASURING");
      default:                      return nullptr;
    }
  }

} // namespace hbfirmware
