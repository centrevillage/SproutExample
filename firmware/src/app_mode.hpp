#pragma once

#include "mode/app_mode_base.hpp"
#include "periph/app_buttons.hpp"
#include "periph/gatein.hpp"
#include "periph/app_adc.hpp"
#include <igb_util/macro.hpp>
#include "fx_model_select.hpp"
#include "mode/app_mode_sample.hpp"

struct AppMode {
  AppModeSample state;

  void init() {
    state.init();

    app_buttons.context.on_change = [this](AppBtnId id, bool on) {
      return state.button(id, on);
    };

    app_adc.on_change = [this](AppAnalogId id, float value, float prev_value) {
      return state.analog(id, value, prev_value);
    };

    gatein.on_change = [this](bool on) {
      return state.gatein(on);
    };
  }

  void deinit() {
    state.deinit();
  }

  void refresh() {
    state.refresh();
  }

  IGB_FAST_INLINE void process() {
    state.process();
  }
};

extern AppMode app_mode;

