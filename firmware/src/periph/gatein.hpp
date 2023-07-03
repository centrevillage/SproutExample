#pragma once

#include <functional>
#include <igb_stm32/base.hpp>
#include <igb_stm32/periph/systick.hpp>
#include <igb_stm32/periph/fast_gpio.hpp>
#include <igb_stm32/periph/exti.hpp>
#include <igb_daisy/periph/gpio.hpp>

using namespace igb;
using namespace igb::stm32;

struct GateIn {
  FastGpioPin<daisy_pin_to_stm32_pin(DaisyGpioPinType::d24)> gate_in_io;
  std::function<bool(bool)> on_change = nullptr;

  bool state = false;
  
  void init() {
    gate_in_io.initInputDefault();
    gate_in_io.enableExti(ExtiTrigType::falling_and_rising, ExtiMode::interrupt, 0x0F);
    state = false;
  }

  void process();
  void irqHandler();
};

extern GateIn gatein;

extern "C" {
  void EXTI1_IRQHandler();
}

