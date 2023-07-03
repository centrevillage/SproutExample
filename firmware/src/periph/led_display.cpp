#include "periph/led_display.hpp"

LedDisplay led_display;

void LedDisplay::setLed(LedPos pos, LedColor color, float brightness /* 0.0 ~ 1.0 */) {
  auto& color_array = color_defs[static_cast<uint8_t>(color)];
  setLed(pos, color_array, brightness);
}
