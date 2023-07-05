#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <cmath>
#include <variant>
#include <igb_util/macro.hpp>
#include <igb_util/dsp/dsp_func.hpp>
#include <igb_util/dsp/dsp_tbl_func.hpp>
#include "app_param_id.hpp"
#include "app_params.hpp"
#include "periph/sdram.hpp"
#include <igb_util/dsp/digi_tape.hpp>
#include <igb_util/dsp/delay_line.hpp>
#include <igb_util/dsp/biquad_filter.hpp>
#include <igb_util/dsp/noise.hpp>
#include "util/level_analyzer.hpp"
#include "util/beat_lfo.hpp"
#include "config.hpp"
#include <igb_util/dsp/pitch_shifter.hpp>
#include <igb_util/dsp/math.hpp>
#include "periph/system_memory.hpp"

using namespace igb;

constexpr static float rate2freq(float rate) {
  // 16.35 Hz to 23.68 kHz
  return 16.351597831287414f /* C0 */ * std::pow(2.0f, (rate * 126.0f) / 12.0f);
}

constexpr static float rate2q(float rate) {
  return std::lerp(1.0f, std::pow(igb::numbers::sqrt2, 8.0f), rate);
}

constexpr static float rate2gain(float rate) {
  return std::pow(10.0f, (rate * 10.0f / 40.0f));
}
