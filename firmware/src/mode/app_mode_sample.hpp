#pragma once

#include <array>

#include <igb_stm32/periph/systick.hpp>
#include <igb_util/bitmagic.hpp>
#include <igb_util/cast.hpp>

#include "mode/app_mode_base.hpp"
#include "periph/led_display.hpp"
#include "periph/app_buttons.hpp"
#include "periph/app_adc.hpp"
#include "fx_model.hpp"
#include "util/beat_lfo.hpp"

using namespace igb;
using namespace igb::sdk;

IGB_FAST_INLINE LedColor get_fx_model_color() {
  auto c = LedColor::black;
  switch(fx_model.modelIdx()) {
    case 0:
      c = LedColor::green;
      break;
    case 1:
      c = LedColor::blue;
      break;
    case 2:
      c = LedColor::yellow;
      break;
    case 3:
      c = LedColor::cyan;
      break;
    case 4:
      c = LedColor::purple;
      break;
    case 5:
      c = LedColor::orange;
      break;
    case 6:
      c = LedColor::white;
      break;
    default:
      break;
  }
  return c;
}

struct AppModeSample {
  SoftTimer<1> level_disp_timer; 
  uint8_t level_disp_timer_idx = 255;

  AppModeBase base;

  void displayModel() {
    led_display.setLed(LedPos::sprout, get_fx_model_color(), 1.0f);
  }

  // Mode API {{{
  void init() {
    base.init();

    refresh();

    level_disp_timer_idx = level_disp_timer.intervalCallback(
      10,
      current_msec(),
      []() {
        auto [level_l, level_r] = level_analyzer.getLevels();
        led_display.setLedMeter(LedMeterPos::left, level_l, get_fx_model_color());
        led_display.setLedMeter(LedMeterPos::right, level_r, get_fx_model_color());

        auto sig = beat_lfo.getSig();
        led_display.setLed(LedPos::wind, LedColor::purple, sig);
      }
    );
  }

  void deinit() {
    base.deinit();

    if (level_disp_timer_idx != 255) {
      level_disp_timer.inactivate(level_disp_timer_idx);
    }
  }

  bool button(AppBtnId id, bool on) {
    if (base.button(id, on)) {
      return true;
    }

    bool needs_refresh = false;
    switch(id) {
      case AppBtnId::sprout:
        if (!on) {
          fx_model.nextModel();
          needs_refresh = true;
        }
        break;
      case AppBtnId::wind:
        if (app_buttons.isAllOff()) {
          uint32_t tap_interval = app_buttons.context.history.findTapInterval(
            AppBtnId::wind,
            1000, // long press limit (msec)
            40, // min tap interval (msec)
            8000 // max tap interval (msec)
          );
          if (tap_interval > 0) {
            fx_model.tapTempo(tap_interval);
          }
        }
        needs_refresh = true;
        break;
      default:
        break;
    }

    if (needs_refresh) {
      refresh();
    }

    return false;
  }

  bool analog(AppAnalogId id, float value, float prev_value) {
    return base.analog(id, value, prev_value);
  }

  bool gatein(bool on) {
    if (base.gatein(on)) {
      return true;
    }
    fx_model.gatein(on);
    return true;
  }

  IGB_FAST_INLINE void process() {
    base.process();
    level_disp_timer.process(current_msec());
  }
  // }}}

  void refresh() {
    base.refresh();
    displayModel();
  }
};

