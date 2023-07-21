#pragma once

#include "fx_model/fx_model_base.hpp"
#include <igb_stm32/periph/systick.hpp>

// simple fdn reverb 
struct FxModelReverb {
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

  constexpr static size_t reverb_order = 4; /* 2, 4, 8, 16 ... */

  const static size_t main_buffer_size = 90000UL / 2UL;

  double feedback_taps[reverb_order] = {
    4.0 / 5.0, 4.0 / 7.0, 4.0 / 11.0, 4.0 / 13.0
  };

  igb::dsp::DelayLine main_line[reverb_order];
  MemoryArea main_area[reverb_order];

  constexpr static double delay_min = 0.02;
  constexpr static double delay_max = 1.0;
  double delay = 0.5f;
  double delay_smooth = 0.5;
  float feedback = 0.5f;
  float wet_balance = 0.5f;

  IGB_FAST_INLINE void init() {
    for (size_t i = 0; i < reverb_order; ++i) {
      auto main_area_opt = memory_manager.assignArea(main_buffer_size);
      if (main_area_opt) {
        main_area[i] = main_area_opt.value();
        main_line[i].init(getFloatPtr(main_area[i]), main_area[i].size);
      }
    }
    delay_smooth = delay;
  }

  IGB_FAST_INLINE void deinit() {
    for (size_t i = 0; i < reverb_order; ++i) {
      if (main_area[i].size) {
        memory_manager.releaseArea(main_area[i]);
      }
    }
  }

  IGB_FAST_INLINE void updateParam(AppParamId param_id, int16_t param_value) {
    float v = (float)param_value / 1023.0f;
    switch(param_id) {
      case AppParamId::time:
        delay = std::lerp(delay_min, delay_max, (double)v);
        break;
      case AppParamId::sun:
        feedback = 1.0f - ((1.0f - v) * (1.0f - v));
        break;
      case AppParamId::wet:
        wet_balance = v;
        break;
      default:
        break;
    }
  }


  IGB_FAST_INLINE void process(const float* in_buf, float* out_buf, size_t count) {
    for (size_t x = 0; x < count; x += 2) {
      delay_smooth = std::lerp(delay, delay_smooth, 0.99995);

      float l = *(in_buf++);
      float r = *(in_buf++);

      float bus_v[reverb_order];
      float wet_v[reverb_order];

      // sum input & feedback
      for (size_t i = 0; i < reverb_order; ++i) {
        float v = (i & 1) ? r : l;
        v /= igb::numbers::sqrt2;
        bus_v[i] = v;
        wet_v[i] = main_line[i].readF((double)main_buffer_size * feedback_taps[i] * delay_smooth);
        bus_v[i] += wet_v[i] * feedback;
      }

      float reverb_l = 0.0f;
      float reverb_r = 0.0f;
      float feed_v[reverb_order];
      // feedback matrix (Householder Feedback Matrix)
      // NOTE: optimized for reverb_order == 4, if other order selected, should change this.
      for (size_t i = 0; i < reverb_order; ++i) {
        feed_v[i] = 0.0f;
        for (size_t j = 0; j < reverb_order; ++j) {
          if (i == j) {
            feed_v[i] += bus_v[j];
          } else {
            feed_v[i] -= bus_v[j];
          }
        }
        feed_v[i] /= 2.0f;

        main_line[i].write(igb::dsp::softclip(feed_v[i]));

        if (i & 1) {
          reverb_r += feed_v[i];
        } else {
          reverb_l += feed_v[i];
        }
      }

      reverb_l /= 2.0f;
      reverb_r /= 2.0f;

      float out_l, out_r;
      if (wet_balance > 0.99f) {
        out_l = reverb_l;
        out_r = reverb_r;
      } else if (wet_balance > 0.5f) {
        float dry_rate = 1.0f - (wet_balance - 0.5f) * 2.0f;
        out_l = l * dry_rate + reverb_l;
        out_r = r * dry_rate + reverb_r;
      } else {
        out_l = l + reverb_l * wet_balance * 2.0f;
        out_r = r + reverb_r * wet_balance * 2.0f;
      }

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

