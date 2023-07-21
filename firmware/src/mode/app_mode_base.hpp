#pragma once

#include <variant>
#include <optional>
#include <array>
#include <igb_sdk/util/soft_timer.hpp>
#include <igb_stm32/periph/systick.hpp>
#include "periph/app_buttons.hpp"
#include "periph/app_adc.hpp"
#include "periph/led_display.hpp"
#include "app_param_id.hpp"
#include "app_params.hpp"
#include "fx_model.hpp"
#include "util/app_knob_mode.hpp"

using namespace igb;
using namespace igb::sdk;

enum class AppModeId : uint8_t {
  delay = 0,
  setting
};

struct AppModeBase {
  inline void init() {
  }

  inline void deinit() {
  }

  AppParamId getParamId(AppAnalogId id, bool is_wind) {
    AppParamId param_id = AppParamId::cv;
    switch (id) {
      case AppAnalogId::cvin:
        param_id = AppParamId::cv;
        break;
      case AppAnalogId::time_knob:
        if (is_wind) {
          param_id = AppParamId::time2;
        } else {
          param_id = AppParamId::time;
        }
        break;
      case AppAnalogId::sun_knob:
        if (is_wind) {
          param_id = AppParamId::sun2;
        } else {
          param_id = AppParamId::sun;
        }
        break;
      case AppAnalogId::wet_knob:
        if (is_wind) {
          param_id = AppParamId::wet2;
        } else {
          param_id = AppParamId::wet;
        }
        break;
    }
    return param_id;
  }

  bool button(AppBtnId id, bool on) {
    switch(id) {
      case AppBtnId::sprout:
        // TODO: change mode
        break;
      case AppBtnId::wind:
        break;
      default:
        break;
    }
    return false;
  }

  bool gatein(bool on) {
    return false;
  }

  bool analog(AppAnalogId id, float value, float prev_value) {
    bool is_wind = app_buttons.isWindOn();
    AppParamId param_id = getParamId(id, is_wind);

    if (id != AppAnalogId::cvin) {
      auto new_value = app_knob_mode.newValue(app_params.getParam(param_id).getRaw(), value, prev_value);
      if (!new_value) {
        return true;
      }
      value = new_value.value();
    }
    dispKnobValue(param_id, value);
    app_params.getParam(param_id).setRaw(value);
    fx_model.updateParam(param_id);

    return true;
  }

  IGB_FAST_INLINE void process() {
  }

  void dispKnobValue(AppParamId param_id, float value) {
    switch (param_id) {
      case AppParamId::time:
        led_display.setLed(LedPos::time, LedColor::white, value);
        break;
      case AppParamId::sun:
        led_display.setLed(LedPos::sun, LedColor::orange, value);
        break;
      case AppParamId::wet:
        led_display.setLed(LedPos::wet, LedColor::blue, value);
        break;
      case AppParamId::time2:
        led_display.setLed(LedPos::time, LedColor::pink, value);
        break;
      case AppParamId::sun2:
        led_display.setLed(LedPos::sun, LedColor::yellow_green, value);
        break;
      case AppParamId::wet2:
        led_display.setLed(LedPos::wet, LedColor::cyan, value);
        break;
      default:
        break;
    }
  }

  inline void refresh() {
    bool is_wind = app_buttons.isWindOn();
    AppParamId param_id = getParamId(AppAnalogId::time_knob, is_wind);
    float value = app_params.getParam(param_id).getRaw();
    dispKnobValue(param_id, value);
    param_id = getParamId(AppAnalogId::sun_knob, is_wind);
    value = app_params.getParam(param_id).getRaw();
    dispKnobValue(param_id, value);
    param_id = getParamId(AppAnalogId::wet_knob, is_wind);
    value = app_params.getParam(param_id).getRaw();
    dispKnobValue(param_id, value);
  }
};


