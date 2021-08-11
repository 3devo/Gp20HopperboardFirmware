#pragma once


// This file is intended to be shared between the child and mainboard
// firmware, to prevent duplication of constants.

namespace hbfirmware {
  enum class Commands {
    GET_LAST_STATUS = 0x80,
    SET_STATE = 0x81,
    GET_MEASUREMENT = 0x82,
    START_FEEDING = 0x83,
  };
} // namespace hbfirmware
