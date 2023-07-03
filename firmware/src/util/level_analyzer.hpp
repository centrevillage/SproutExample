#pragma once

#include <utility>
#include <igb_util/dsp/env_follower.hpp>
#include "config.hpp"

struct LevelAnalyzer {
  igb::dsp::EnvFollower ef;
  igb::dsp::EnvFollower::Context ef_ctx[2];

  std::pair<float, float> levels;

  void init() {
    ef.init(0.1f, 0.1f);
  }

  void process(std::pair<float, float> signals) {
    levels = { 
      ef.process(ef_ctx[0], signals.first),
      ef.process(ef_ctx[1], signals.second)
    };
  }

  std::pair<float, float> getLevels() const {
    return levels;
  }
};

extern LevelAnalyzer level_analyzer;
