#pragma once


// This file is intended to be shared between the child and mainboard
// firmware, to prevent duplication of constants.

#include <Arduino.h>

namespace hbfirmware {
  enum class Command {
    GET_LAST_STATUS = 0x80,
    SET_STATE = 0x81,
    GET_MEASUREMENT = 0x82,
  };
  inline const __FlashStringHelper *to_string(Command cmd) {
    switch (cmd) {
      case Command::GET_LAST_STATUS: return F("GET_LAST_STATUS");
      case Command::SET_STATE:       return F("SET_STATE");
      case Command::GET_MEASUREMENT: return F("GET_MEASUREMENT");
      default:                       return nullptr;
    }
  }
} // namespace hbfirmware
