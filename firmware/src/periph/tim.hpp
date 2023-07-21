#pragma once

#include "hardware.hpp"
#include <igb_util/macro.hpp>

IGB_FAST_INLINE uint32_t current_tick() {
  return daisy::System::GetTick();
}

// if you use the boost mode (480Hz), this should be change to 240000000UL
constexpr inline uint32_t tim_base_clock = 200000000UL;

constexpr IGB_FAST_INLINE double sec_to_tick(double interval_sec) {
  return interval_sec * (double)tim_base_clock;
}
constexpr IGB_FAST_INLINE double tick_to_sec(double tick) {
  return tick / (double)tim_base_clock;
}
