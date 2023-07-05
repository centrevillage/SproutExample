#pragma once

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <igb_util/dsp/dsp_tbl_func.hpp>
#include <igb_util/dsp/dsp_func.hpp>
#include <igb_util/macro.hpp>
#include <igb_util/math.hpp>
#include <cmath>

struct BeatLFO {
  bool active = false;
  float phase = 0.0f;
  float delta = 0.0f;
  uint32_t usec = 0;
  float sig = 0.0f;
  constexpr static float delta_max = 0.00002f;

  IGB_FAST_INLINE void init(uint32_t initial_usec) {
    phase = 0.0f;
    sig = 0.0f;
    usec = initial_usec;
  }

  IGB_FAST_INLINE void setTempo(float sec) {
    delta = sec / 1000000.0f;
  }

  IGB_FAST_INLINE float process(uint32_t _usec) {
    if (_usec != usec) {
      uint32_t delta_usec = _usec - usec;
      phase += delta * (float)delta_usec;
      phase -= (uint32_t)phase;
      usec = _usec;
      if (delta >= delta_max) {
        sig = 0.5f;
      } else {
        sig = igb::dsp::sin_wave_uni_fast(phase);
      }
    }
    return sig;
  }

  IGB_FAST_INLINE float getSig() const {
    return active ? sig : 0.0f;;
  }
};

extern BeatLFO beat_lfo;
