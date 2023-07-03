#pragma once

#include <igb_util/cast.hpp>
#include <igb_util/macro.hpp>
#include <igb_stm32/periph/i2c.hpp>
#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/fast_gpio.hpp>
#include <igb_stm32/periph/tim.hpp>
#include <igb_sdk/device/is31fl3731_i2c.hpp>
#include <array>

#include "periph/app_i2c.hpp"
#include "per/i2c.h"

using namespace igb;
using namespace igb::stm32;
using namespace igb::sdk;

enum class LedMeterPos : uint8_t {
  left = 0,
  right
};

enum class LedPos : uint8_t {
  step1 = 0, 
  step2,
  step3,
  step4,
  step5,
  step6,
  step7,
  step8,
  step9,
  step10,
  step11,
  step12,
  step13,
  step14,
  step15,
  step16,

  time,
  sprout,
  sun,
  wind,
  wet
};

enum class LedColor : uint8_t {
  red = 0,
  green,
  blue,
  yellow,
  purple,
  cyan,
  white,
  black,
  orange,
  yellow_green,
  pink
};


struct LedDisplay {
  Is31Fl3731I2c<AppI2c> display_driver;
  constexpr static float default_brightness = 1.0f;

  constexpr static std::array<std::array<float, 3>, 11> color_defs = {{
    {1.0f, 0.0f, 0.0f}, // red
    {0.0f, 1.0f, 0.0f}, // green
    {0.0f, 0.0f, 1.0f}, // blue
    {0.6f, 0.6f, 0.0f}, // yellow
    {0.6f, 0.0f, 0.6f}, // purple
    {0.0f, 0.6f, 0.6f}, // cyan
    {0.5f, 0.5f, 0.5f}, // white
    {0.0f, 0.0f, 0.0f}, // black
    {0.8f, 0.4f, 0.0f}, // orange
    {0.4f, 0.8f, 0.0f}, // yellow green
    {0.8f, 0.2f, 0.4f}, // pink
  }};

  constexpr static std::array<std::array<uint8_t, 3>, 21> pos_def = {{
    // diode: step: R, G, B
    {16*3+1-1, 16*2+1-1, 16+1-1},   // D1:  step1:  C4-1, C3-1, C2-1
    {16*6+1-1, 16*5+1-1, 16*4+1-1}, // D2:  step2:  C7-1, C6-1, C5-1
    {16*3+2-1, 16*2+2-1, 1-1},      // D3:  step3:  C4-2, C3-2, C1-1
    {16*6+2-1, 16*5+2-1, 16*4+2-1}, // D4:  step4:  C7-2, C6-2, C5-2
    {16*3+3-1, 16*1+2-1, 2-1},      // D5:  step5:  C4-3, C2-2, C1-2
    {16*6+3-1, 16*5+3-1, 16*4+3-1}, // D6:  step6:  C7-3, C6-3, C5-3
    {16*2+3-1, 16*1+3-1, 16*0+3-1}, // D7:  step7:  C3-3, C2-3, C1-3
    {16*6+4-1, 16*5+4-1, 16*4+4-1}, // D8:  step8:  C7-4, C6-4, C5-4
    {16*2+4-1, 16*1+4-1, 16*0+4-1}, // D9:  step9:  C3-4, C2-4, C1-4
    {16*6+5-1, 16*5+5-1, 16*3+4-1}, // D10: step10: C7-5, C6-5, C4-4
    {16*2+5-1, 16*1+5-1, 16*0+5-1}, // D11: step11: C3-5, C2-5, C1-5
    {16*6+6-1, 16*4+5-1, 16*3+5-1}, // D12: step12: C7-6, C5-5, C4-5
    {16*2+6-1, 16*1+6-1, 16*0+6-1}, // D13: step13: C3-6, C2-6, C1-6
    {16*5+6-1, 16*4+6-1, 16*3+6-1}, // D14: step14: C6-6, C5-6, C4-6
    {16*2+7-1, 16*1+7-1, 16*0+7-1}, // D15: step15: C3-7, C2-7, C1-7
    {16*5+7-1, 16*4+7-1, 16*3+7-1}, // D16: step16: C6-7, C5-7, C4-7

    {16*3+9-1, 16*2+9-1, 16*1+9-1}, // D17: time:   C4-9, C3-9, C2-9
    {16*3+10-1, 16*2+10-1, 16*0+9-1}, // D18: sprout: C4-10, C3-10, C1-9
    {16*3+11-1, 16*1+10-1, 16*0+10-1}, // D19: sun: C4-11, C2-10, C1-10
    {16*2+11-1, 16*1+11-1, 16*0+11-1}, // D20: wind: C3-11, C2-11, C1-11
    {16*2+12-1, 16*1+12-1, 16*0+12-1} // D21: wet: C3-12, C2-12, C1-12
  }};

  std::array<std::array<uint8_t, 3>, 21> _cache = {{
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {0, 0, 0}
  }};

  bool init() {
    daisy::I2CHandle daisy_i2c;
    daisy_i2c.Init(
      daisy::I2CHandle::Config {
        .periph = daisy::I2CHandle::Config::Peripheral::I2C_1,
        .pin_config {
          .scl = {DSY_GPIOB, 8},
          .sda = {DSY_GPIOB, 9}
        },
        .speed  = daisy::I2CHandle::Config::Speed::I2C_400KHZ,
        .mode   = daisy::I2CHandle::Config::Mode::I2C_MASTER
      }
    );

    delay_msec(10);
    if (!display_driver.init()) {
      return false;
    }
    clearSync();
    return true;
  }

  void all(auto&& func) {
    uint8_t spos = as<uint8_t>(LedPos::step1);
    uint8_t epos = as<uint8_t>(LedPos::wet);
    for (uint8_t i = spos; i <= epos; ++i) {
      func(as<LedPos>(i));
    }
  }

  void clear() {
    all([this](LedPos pos){ resetLed(pos); });
  };

  void allLit() {
    all([this](LedPos pos){ setLed(pos, LedColor::white, 1.0f); });
  }

  void clearSync() {
    all([this](LedPos pos){ resetLedSync(pos); });
  }

  void setLedSync(LedPos pos, LedColor color, float brightness /* 0.0 ~ 1.0 */) {
    auto& color_array = color_defs[static_cast<uint8_t>(color)];
    const auto pos_idx = static_cast<uint8_t>(pos);
    auto& pos_array = pos_def[pos_idx];
    for (uint8_t i = 0; i < 3; ++i) {
      const uint8_t b = (uint8_t)((float)(0xFF) * (color_array[i] * brightness * default_brightness));
      if (_cache[pos_idx][i] != b) {
        display_driver.setLed(pos_array[i], b);
        _cache[pos_idx][i] = b;
      }
    }
  }

  void resetLedSync(LedPos pos) {
    const auto pos_idx = static_cast<uint8_t>(pos);
    auto& pos_array = pos_def[pos_idx];
    for (uint8_t i = 0; i < 3; ++i) {
      if (_cache[pos_idx][i] != 0) {
        display_driver.setLed(pos_array[i], 0);
        _cache[pos_idx][i] = 0;
      }
    }
  }

  void setLed(LedPos pos, LedColor color, float brightness /* 0.0 ~ 1.0 */);

  void setLed(LedPos pos, auto&& color_array, float brightness /* 0.0 ~ 1.0 */) {
    const auto pos_idx = static_cast<uint8_t>(pos);
    auto& pos_array = pos_def[pos_idx];
    for (uint8_t i = 0; i < 3; ++i) {
      const auto b = (uint8_t)((float)(0xFF) * (color_array[i] * brightness * default_brightness));
      if (_cache[pos_idx][i] != b) {
        display_driver.addLedCmd(pos_array[i], b);
        _cache[pos_idx][i] = b;
      }
    }
  }

  void resetLed(LedPos pos) {
    const auto pos_idx = static_cast<uint8_t>(pos);
    auto& pos_array = pos_def[pos_idx];
    for (uint8_t i = 0; i < 3; ++i) {
      if (_cache[pos_idx][i] != 0) {
        display_driver.addLedCmd(pos_array[i], 0);
        _cache[pos_idx][i] = 0;
      }
    }
  }

  IGB_SIZE_OPTIMIZE void setLedMeter(LedMeterPos pos, float v, LedColor color = LedColor::green) {
    float meter_v = std::log10(v * 3.3f) / 3.0f;
    if (meter_v < -1.0f) {
      meter_v = -1.0f;
    }
    meter_v += 1.0f;
    uint8_t odd = static_cast<uint8_t>(pos);
    uint8_t i = 0;
    meter_v *= 7.0f;
    for (; i < 7; ++i) {
      if (meter_v >= 1.0f) {
        meter_v -= 1.0f;
        setLed(static_cast<LedPos>((7 - i) * 2 + odd), color, 1.0f);
      } else {
        break;
      }
    }
    for (; i < 8; ++i) {
      if (meter_v >= 1.0f) {
        meter_v -= 1.0f;
        setLed(static_cast<LedPos>((7 - i) * 2 + odd), LedColor::red, 1.0f);
      } else {
        break;
      }
    }
    for (; i < 8; ++i) {
      resetLed(static_cast<LedPos>((7 - i) * 2 + odd));
    }
  }

  IGB_FAST_INLINE void process() {
    display_driver.processLedCmd();
  }
};

extern LedDisplay led_display;

