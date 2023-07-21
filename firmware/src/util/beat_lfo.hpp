#pragma once

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <igb_util/dsp/dsp_tbl_func.hpp>
#include <igb_util/dsp/dsp_func.hpp>
#include <igb_util/macro.hpp>
#include <igb_util/math.hpp>
#include <cmath>
#include "periph/tim.hpp"

struct BeatLFO {
  bool active = false;
  float phase = 0.0f;
  float delta = 0.0f;
  uint32_t tick = 0;
  float sig = 0.0f;
  constexpr static float delta_max = (float)sec_to_tick(0.01);

  IGB_FAST_INLINE void init(uint32_t initial_tick) {
    phase = 0.0f;
    sig = 0.0f;
    tick = initial_tick;
  }

  IGB_FAST_INLINE void setTempo(float sec) {
    delta = sec / (float)sec_to_tick(1.0f);
  }

  IGB_FAST_INLINE float process(uint32_t _tick) {
    if (_tick != tick) {
      uint32_t delta_tick = _tick - tick;
      phase += delta * (float)delta_tick;
      phase -= (uint32_t)phase;
      tick = _tick;
      if (delta >= delta_max) {
        sig = 0.5f;
      } else {
        sig = igb::dsp::sin_wave_uni_fast(phase);
      }
      //sig = std::sin(phase * igb::numbers::pi * 2.0f) * 0.5f + 0.5f;
    }
    return sig;
  }

  IGB_FAST_INLINE float getSig() const {
    return active ? sig : 0.0f;;
  }
};

extern BeatLFO beat_lfo;
