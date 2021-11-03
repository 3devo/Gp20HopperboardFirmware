#pragma once

#include <Arduino.h>

// This file is intended to be shared between the child and mainboard
// firmware, to prevent duplication of constants.

#include <Arduino.h>

namespace hbfirmware {
  enum class Command {
    GET_LAST_STATUS = 0x80,
    SET_STATE = 0x81,
    GET_MEASUREMENT = 0x82,
    START_FEEDING = 0x83,
  };

  inline const __FlashStringHelper *to_string(Command cmd) {
    switch (cmd) {
      case Command::GET_LAST_STATUS: return F("GET_LAST_STATUS");
      case Command::SET_STATE:       return F("SET_STATE");
      case Command::GET_MEASUREMENT: return F("GET_MEASUREMENT");
      case Command::START_FEEDING:   return F("START_FEEDING");
      default:                       return nullptr;
    }
  }

  enum class FeedResult : uint8_t {
    NONE,
    DETECTED,
    TIMEOUT,
  };

  inline const __FlashStringHelper *to_string(FeedResult res) {
    switch (res) {
      case FeedResult::NONE:     return F("NONE");
      case FeedResult::DETECTED: return F("DETECTED");
      case FeedResult::TIMEOUT:  return F("TIMEOUT");
      default:                   return nullptr;
    }
  }

  enum class State : uint8_t {
    MEASURING,
    CALIBRATING,
    FEEDING,
  };

  inline const __FlashStringHelper *to_string(State state) {
    switch (state) {
      case State::MEASURING:   return F("MEASURING");
      case State::CALIBRATING: return F("CALIBRATING");
      case State::FEEDING:     return F("FEEDING");
      default:                 return nullptr;
    }
  }
} // namespace hbfirmware
