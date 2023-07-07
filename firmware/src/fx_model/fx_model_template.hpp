#pragma once

#include "fx_model/fx_model_base.hpp"

struct FxModelTemplate /* TODO: set your effect name */ {
  // parameter definition
  constexpr static std::array<ParamRange, param_size> param_ranges = {
    // time
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // sun
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wet
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wind + time
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wind + sun
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wind + wet
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // cv in
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
  };

  IGB_FAST_INLINE void init() {
    // initialze (assign memory, reset parameter, etc...)
  }

  IGB_FAST_INLINE void deinit() {
    // de-initialze (release memory, etc...)
  }

  IGB_FAST_INLINE void updateParam(AppParamId param_id, int16_t param_value) {
    float v = (float)param_value / 1023.0f;
    switch(param_id) {
      case AppParamId::cv:
        break;
      case AppParamId::time:
        break;
      case AppParamId::sun:
        break;
      case AppParamId::wet:
        break;
      case AppParamId::time2: // wind + time
        break;
      case AppParamId::sun2: // wind + sun
        break;
      case AppParamId::wet2: // wind + wet
        break;
      default:
        break;
    }
  }

  IGB_FAST_INLINE void process(const float* in_buf, float* out_buf, size_t count) {
    for (size_t x = 0; x < count; x += 2) {
      float l = *(in_buf++);
      float r = *(in_buf++);

      // TODO: implement your effect
      float out_l = l;
      float out_r = r;

      *(out_buf++) = igb::dsp::softclip(out_l);
      *(out_buf++) = igb::dsp::softclip(out_r);
    }
  }

  IGB_FAST_INLINE void tapTempo(uint16_t tap_tempo_msec) {
    // If you use tap tempo function, please implement this.
  }

  IGB_FAST_INLINE void gatein(bool on) {
    // If you use the gate input, please implement this.
  }
};

