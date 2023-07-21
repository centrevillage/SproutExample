#pragma once

#include <array>
#include <igb_stm32/base.hpp>
#include <igb_stm32/periph/systick.hpp>
#include <igb_daisy/periph/gpio.hpp>
#include <igb_sdk/ui/button.hpp>
#include <igb_sdk/ui/button_context.hpp>
#include <igb_util/macro.hpp>
#include <periph/led_display.hpp>
#include "periph/tim.hpp"

using namespace igb;
using namespace igb::stm32;
using namespace igb::sdk;


enum class AppBtnId {
  sprout = 0,
  wind
};

typedef ButtonSingle<GpioPin> AppButtonSingle;

struct AppButtonCollection {
  std::array<AppButtonSingle, 2> buttons = {
    AppButtonSingle { GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::d22)) },
    AppButtonSingle { GpioPin::newPin(daisy_pin_to_stm32_pin(DaisyGpioPinType::d23)) },
  };

  void init() {
    for (auto& btn : buttons) {
      btn.pin.enable();
      btn.pin.initInput(GpioPullMode::up, GpioSpeedMode::high);
    }
  }

  IGB_FAST_INLINE void process() {
    for (auto& btn : buttons) {
      btn.process();
    }
  }

  size_t size() const {
    return buttons.size();
  }

  AppButtonSingle& operator[] (size_t idx)  {
    return buttons[idx];
  }

  // iterator api
  decltype(auto) begin() {
    return buttons.begin();
  }
  decltype(auto) end() {
    return buttons.end();
  }
};

struct AppButtons {
  ButtonContext<AppBtnId, AppButtonCollection> context;

  void init() {
    context.init();
  }

  IGB_FAST_INLINE void process() {
    context.process(current_tick());
  }

  bool isSproutOn() {
    return context.buttons[static_cast<size_t>(AppBtnId::sprout)].isOn();
  }

  bool isWindOn() {
    return context.buttons[static_cast<size_t>(AppBtnId::wind)].isOn();
  }

  bool isAllOff() {
    return ButtonMethod::isAllOff(context.state_bits);
  }

  bool exactMatch(uint32_t state_bits, auto&& id, auto&&... rest) {
    return ButtonMethod::exactMatch(context.state_bits, id, rest...);
  }

  bool match(uint32_t state_bits, auto&& state, auto&&... rest) {
    return ButtonMethod::match(context.state_bits, state, rest...);
  }
};

extern AppButtons app_buttons;

