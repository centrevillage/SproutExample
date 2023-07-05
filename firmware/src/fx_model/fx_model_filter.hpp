#pragma once

#include "fx_model/fx_model_base.hpp"
#include <igb_stm32/periph/systick.hpp>

// highpass - lowpass crossfading filter
struct FxModelFilter {
  // parameter definition
  constexpr static std::array<ParamRange, param_size> param_ranges = {
    // time
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.0 /* default */},
    // sun
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wet
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.0 /* default */},
    // wind + time
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wind + sun
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // wind + wet
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
    // cv in
    ParamRange {0 /* min */, 1023 /* max */, 0.0 /* zero spacing */, 0.5 /* default */},
  };

  float lp_hp_rate = 0.0f;
  float cutoff_freq = rate2freq(0.5f);;
  float q = rate2q(0.0f);

  igb::dsp::BiQuadFilter lpf;
  igb::dsp::BiQuadFilter hpf;
  igb::dsp::BiQuadFilter::Context lpf_ctx[2];
  igb::dsp::BiQuadFilter::Context hpf_ctx[2];

  IGB_FAST_INLINE void init() {
    lpf.lpf(cutoff_freq, q);
    hpf.hpf(cutoff_freq, q);
  }

  IGB_FAST_INLINE void deinit() {
  }

  IGB_FAST_INLINE void updateParam(AppParamId param_id, int16_t param_value) {
    float v = (float)param_value / 1023.0f;
    switch(param_id) {
      case AppParamId::cv:
        break;
      case AppParamId::time:
        { // LP <-> HP crossface
          lp_hp_rate = v;
        }
        break;
      case AppParamId::sun:
        { // cutoff frequency
          cutoff_freq = rate2freq(v);
        }
        break;
      case AppParamId::wet:
        { // resonancde
          q = rate2q(v);
        }
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

      float lpf_l = lpf.process(lpf_ctx[0], l);
      float lpf_r = lpf.process(lpf_ctx[1], r);
      float hpf_l = hpf.process(hpf_ctx[0], l);
      float hpf_r = hpf.process(hpf_ctx[1], r);

      float out_l = std::lerp(lpf_l, hpf_l, lp_hp_rate);
      float out_r = std::lerp(lpf_r, hpf_r, lp_hp_rate);

      level_analyzer.process({out_l, out_r});

      *(out_buf++) = igb::dsp::softclip(out_l);
      *(out_buf++) = igb::dsp::softclip(out_r);
    }
  }

  IGB_FAST_INLINE void tapTempo(uint16_t tap_tempo_msec) {
  }

  IGB_FAST_INLINE void gatein(bool on) {
  }
};

