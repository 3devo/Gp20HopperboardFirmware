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

struct Pca9955bRegs {
  enum {
    MODE1          = 0x00,
    MODE2          = 0x01,
    LEDOUT0        = 0x02,
    LEDOUT1        = 0x03,
    LEDOUT2        = 0x04,
    LEDOUT3        = 0x05,
    GRPPWM         = 0x06,
    GRPFREQ        = 0x07,
    PWM0           = 0x08,
    PWM1           = 0x09,
    PWM2           = 0x0A,
    PWM3           = 0x0B,
    PWM4           = 0x0C,
    PWM5           = 0x0D,
    PWM6           = 0x0E,
    PWM7           = 0x0F,
    PWM8           = 0x10,
    PWM9           = 0x11,
    PWM10          = 0x12,
    PWM11          = 0x13,
    PWM12          = 0x14,
    PWM13          = 0x15,
    PWM14          = 0x16,
    PWM15          = 0x17,
    IREF0          = 0x18,
    IREF1          = 0x19,
    IREF2          = 0x1A,
    IREF3          = 0x1B,
    IREF4          = 0x1C,
    IREF5          = 0x1D,
    IREF6          = 0x1E,
    IREF7          = 0x1F,
    IREF8          = 0x20,
    IREF9          = 0x21,
    IREF10         = 0x22,
    IREF11         = 0x23,
    IREF12         = 0x24,
    IREF13         = 0x25,
    IREF14         = 0x26,
    IREF15         = 0x27,
    RAMP_RATE_GRP0 = 0x28,
    STEP_TIME_GRP0 = 0x29,
    HOLD_CNTL_GRP0 = 0x2A,
    IREF_GRP0      = 0x2B,
    RAMP_RATE_GRP1 = 0x2C,
    STEP_TIME_GRP1 = 0x2D,
    HOLD_CNTL_GRP1 = 0x2E,
    IREF_GRP1      = 0x2F,
    RAMP_RATE_GRP2 = 0x30,
    STEP_TIME_GRP2 = 0x31,
    HOLD_CNTL_GRP2 = 0x32,
    IREF_GRP2      = 0x33,
    RAMP_RATE_GRP3 = 0x34,
    STEP_TIME_GRP3 = 0x35,
    HOLD_CNTL_GRP3 = 0x36,
    IREF_GRP3      = 0x37,
    GRAD_MODE_SEL0 = 0x38,
    GRAD_MODE_SEL1 = 0x39,
    GRAD_GRP_SEL0  = 0x3A,
    GRAD_GRP_SEL1  = 0x3B,
    GRAD_GRP_SEL2  = 0x3C,
    GRAD_GRP_SEL3  = 0x3D,
    GRAD_CNTL      = 0x3E,
    OFFSET         = 0x3F,
    SUBADR1        = 0x40,
    SUBADR2        = 0x41,
    SUBADR3        = 0x42,
    ALLCALLADR     = 0x43,
    PWMALL         = 0x44,
    IREFALL        = 0x45,
    EFLAG0         = 0x46,
    EFLAG1         = 0x47,
    EFLAG2         = 0x48,
    EFLAG3         = 0x49,

    // Flag to be set in the register address byte
    AUTO_INCREMENT = 0x80,
  };

  struct Mode1 {
    enum {
      ALLCALL      = (1 << 0),
      SUB3         = (1 << 1),
      SUB2         = (1 << 2),
      SUB1         = (1 << 3),
      SLEEP        = (1 << 4),
      AI0          = (1 << 5),
      AI1          = (1 << 6),
      AIF          = (1 << 7),
    };
  };

  struct Mode2 {
    enum {
      EXP_EN       = (1 << 2),
      OCH          = (1 << 3),
      CLRERR       = (1 << 4),
      DMBLNK       = (1 << 5),
      ERROR        = (1 << 6),
      OVERTEMP     = (1 << 7),
    };
  };

  struct LedOutX {
    // Repeated for each output channel in LEDOUT1/2/3/4
    enum {
      OFF          = 0b00,
      FULL_ON      = 0b01,
      PWM          = 0b10,
      GRP_PWM      = 0b11,
    };
  };
};
