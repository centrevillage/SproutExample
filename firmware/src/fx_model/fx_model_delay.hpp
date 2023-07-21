#pragma once

#include "fx_model/fx_model_base.hpp"
#include "util/clock_sync.hpp"
#include <igb_stm32/periph/systick.hpp>

// simple delay
struct FxModelDelay {
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

  constexpr static size_t buffer_size = SPR_SR * 2UL; // delay buffer size (max 2 sec)
  constexpr static double buffer_size_d = (double)buffer_size;
  constexpr static double read_pos_max = buffer_size_d - 1.0;
  constexpr static double read_pos_max_normalize = read_pos_max / buffer_size_d;
  constexpr static double pos_filter_coeff = 0.99995; // read position smoothing filter coeff

  MemoryArea area[2];                   // assigned sdram area for this delay
  igb::dsp::DelayLine delay[2];         // delay

  double read_pos_normalize = 0.5;      // normalized read position [0.0, 1.0)
  double read_pos_mod_normalize = 0.0;  // cv modulation
  double read_pos_real = 0.0;           // read position for delay line
  float feedback = 0.5f;                // delay feedback rate [0.0, 1.1)
  float wet_balance = 0.5f;             // dry/wet balance

  IGB_FAST_INLINE void initSync() {
    clock_sync.init();
  }

  IGB_FAST_INLINE void init() {
    for (size_t i = 0; i < 2; ++i) {
      auto area_opt = memory_manager.assignArea(buffer_size);
      if (area_opt) {
        area[i] = area_opt.value();
        delay[i].init(getFloatPtr(area[i]), area[i].size);
      }
    }
    updateBeatLfo();
    initSync();
    read_pos_real = read_pos_normalize * buffer_size_d;
  }

  IGB_FAST_INLINE void deinit() {
    for (uint8_t i = 0; i < 2; ++i) {
      if (area[i].size) {
        memory_manager.releaseArea(area[i]);
      }
    }
    beat_lfo.active = false;
  }

  void updateBeatLfo() {
    beat_lfo.active = true;
    double rp = std::clamp(read_pos_normalize + read_pos_mod_normalize, 0.00001, 0.99999);
    beat_lfo.setTempo(1.0f / (rp * 10.0f));
  }

  void setReadPosBySync(float v) {
    switch ((uint8_t)(v * 17.0f)) {
      case 0: // 1/16 / 8 = 1/128
        clock_sync.mul = 1;
        clock_sync.div = 8;
        break;
      case 1: // 1/16 * 3 / 16 = 3/256
        clock_sync.mul = 3;
        clock_sync.div = 16;
        break;
      case 2: // 1/16 / 4 = 1/64
        clock_sync.mul = 1;
        clock_sync.div = 4;
        break;
      case 3: // 1/16 * 3 / 8 = 3/128
        clock_sync.mul = 3;
        clock_sync.div = 8;
        break;
      case 4: // 1/16 / 2 = 1/32
        clock_sync.mul = 1;
        clock_sync.div = 2;
        break;
      case 5: // 1/16 * 3 / 4 = 3/64
        clock_sync.mul = 3;
        clock_sync.div = 4;
        break;
      case 6: // 1/16 
        clock_sync.mul = 1;
        clock_sync.div = 1;
        break;
      case 7: // 1/16 * 3 / 2 = 3/32
        clock_sync.mul = 3;
        clock_sync.div = 2;
        break;
      case 8: // 1/16 * 2 =  1/8
        clock_sync.mul = 2;
        clock_sync.div = 1;
        break;
      case 9: // 1/16 * 3 = 3/16
        clock_sync.mul = 3;
        clock_sync.div = 1;
        break;
      case 10: // 1/16 * 4 = 1/4
        clock_sync.mul = 4;
        clock_sync.div = 1;
        break;
      case 11: // 1/16 * 6 = 3/8
        clock_sync.mul = 6;
        clock_sync.div = 1;
        break;
      case 12: // 1/16 * 8 = 1/2
        clock_sync.mul = 8;
        clock_sync.div = 1;
        break;
      case 13: // 1/16 * 12 = 3/4
        clock_sync.mul = 12;
        clock_sync.div = 1;
        break;
      case 14: // 1/16 * 16 = 1
        clock_sync.mul = 16;
        clock_sync.div = 1;
        break;
      case 15: // 3/2
        clock_sync.mul = 24;
        clock_sync.div = 1;
        break;
      case 16: // 2
        clock_sync.mul = 32;
        clock_sync.div = 1;
        break;
      default:
        break;
    }
    read_pos_normalize = std::clamp(clock_sync.cycleSec() / 10.0f, 0.0f, 1.0f - igb::numbers::epsilon_f);
  }

  IGB_FAST_INLINE void updateParam(AppParamId param_id, int16_t param_value) {
    float v = (float)param_value / 1023.0f;
    switch(param_id) {
      case AppParamId::cv:
        {
          if (0.49f < v && v < 0.51f) {
            read_pos_mod_normalize = 0.0;
          } else {
            read_pos_mod_normalize = v - 0.5;
          }
          updateBeatLfo();
        }
        break;
      case AppParamId::time:
        {
          if (clock_sync.isActive()) {
            setReadPosBySync(v);
          } else {
            read_pos_normalize = v * v;
          }
          updateBeatLfo();
        }
        break;
      case AppParamId::sun:
        feedback = igb::dsp::log_curve(v) * 0.4f + v * 0.7f;
        break;
      case AppParamId::wet:
        if (v > 0.995f) {
          wet_balance = 1.0f;
        } else {
          wet_balance = v;
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

      double read_pos = std::clamp(read_pos_normalize + read_pos_mod_normalize, 0.00001, 1.0 - (double)igb::numbers::epsilon_f) * buffer_size_d;
      read_pos_real = std::lerp(read_pos, read_pos_real, pos_filter_coeff);

      float wet_l = delay[0].readF(read_pos_real);
      float wet_r = delay[1].readF(read_pos_real);

      float wet_l_feed = wet_l * feedback;
      float wet_r_feed = wet_r * feedback;

      float tape_in_l = l + wet_l_feed;
      float tape_in_r = r + wet_r_feed;

      delay[0].write(igb::dsp::softclip(tape_in_l));
      delay[1].write(igb::dsp::softclip(tape_in_r));

      float out_l;
      float out_r;
      if (wet_balance > 0.99f) {
        out_l = wet_l;
        out_r = wet_r;
      } else if (wet_balance > 0.5f) {
        float dry_rate = 1.0f - (wet_balance - 0.5f) * 2.0f;
        out_l = l * dry_rate + wet_l;
        out_r = r * dry_rate + wet_r;
      } else {
        out_l = l + wet_l * wet_balance * 2.0f;
        out_r = r + wet_r * wet_balance * 2.0f;
      }

      level_analyzer.process({out_l, out_r});

      *(out_buf++) = igb::dsp::softclip(out_l);
      *(out_buf++) = igb::dsp::softclip(out_r);
    }
  }

  IGB_FAST_INLINE void tapTempo(uint16_t tap_tempo_msec) {
    if (tap_tempo_msec > 2000) {
      read_pos_normalize = read_pos_max_normalize;
    } else if (tap_tempo_msec < 50) {
      read_pos_normalize = 0.0f;
    } else {
      read_pos_normalize = (float)(tap_tempo_msec - 50) / 1000.0f / 10.0f;
    }
    updateBeatLfo();
  }

  IGB_FAST_INLINE void gatein(bool on) {
    if (on) {
      if (clock_sync.trigger(current_usec())) {
        read_pos_normalize = std::clamp(clock_sync.cycleSec() / 10.0f, 0.0f, 1.0f - igb::numbers::epsilon_f);
        updateBeatLfo();
      }
    }
  }
};

