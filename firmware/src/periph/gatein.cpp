#include "periph/gatein.hpp"
#include "periph/led_display.hpp"

GateIn gatein;

void GateIn::process() {
  //bool new_state = gate_in_io.read();
  //if (state != new_state) {
  //  if (on_change) {
  //    on_change(!state);
  //  }
  //}
}

void GateIn::irqHandler() {
  if (ExtiCtrl::isIt(ExtiLine::line1)) {
    ExtiCtrl::clearItState(ExtiLine::line1);
    bool new_state = gate_in_io.read();
    if (state != new_state) {
      if (on_change) {
        on_change(!state);
      }
    }
  }
}

void EXTI1_IRQHandler() {
  gatein.irqHandler();
}
