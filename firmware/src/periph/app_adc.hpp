#pragma once

#include "hardware.hpp"
#include <functional>
#include <igb_util/value_stabilizer.hpp>
#include <igb_util/macro.hpp>
#include <igb_stm32/periph/systick.hpp>

using namespace igb;
using namespace igb::stm32;

enum class AppAnalogId : uint8_t {
  cvin = 0,
  time_knob,
  sun_knob,
  wet_knob
};
constexpr size_t analog_count = 4;

enum AnalogPins {
  A_CVIN_PIN = 18,
  A_TIME_KNOB_PIN = 19,
  A_SUN_KNOB_PIN = 20,
  A_WET_KNOB_PIN = 21,
};

struct AppAdc {
  std::function<void(AppAnalogId, float, float)> on_change;

  const ValueStabilizerConf conf = {
    .chattering_count = 3,
    .filter_coeff = 0.001f,
    .chattering_threshold = 200.0f,
    .value_change_threshold = 63.0f
  };
  ValueStabilizerContext context[analog_count];
  ValueStabilizerMethod stabilizer;

  void init() {
    daisy::AdcChannelConfig adc_configs[analog_count];
    adc_configs[0].InitSingle(hardware.GetPin(A_CVIN_PIN));
    adc_configs[1].InitSingle(hardware.GetPin(A_TIME_KNOB_PIN));
    adc_configs[2].InitSingle(hardware.GetPin(A_SUN_KNOB_PIN));
    adc_configs[3].InitSingle(hardware.GetPin(A_WET_KNOB_PIN));
    hardware.adc.Init(adc_configs, analog_count);
    hardware.adc.Start();
    delay_msec(10);
    initContext();
  }

  void initContext() {
    for (uint8_t i = 0; i < analog_count; ++i) {
      context[i].current_value = context[i].new_value = (float)hardware.adc.Get(i);
      context[i].change_count = 0;
    }
  }

  IGB_FAST_INLINE void process() {
    for (uint8_t i = 0; i < analog_count; ++i) {
      float value = (float)hardware.adc.Get(i);
      AppAnalogId id = static_cast<AppAnalogId>(i);
      float prev_value = getFloatValue(id);
      if (stabilizer.updateCurrentValue(value, context[i], conf)) {
        if (on_change) {
          on_change(id, getFloatValue(id), prev_value);
        }
      }
    }
  }

  float getFloatValue(AppAnalogId id) {
    uint8_t idx = static_cast<int8_t>(id);
    return context[idx].current_value / 65536.0f;
  }
};

extern AppAdc app_adc;

