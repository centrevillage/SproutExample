#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <functional>

#include "periph/tim.hpp"

struct ClockSync {
  // 1/8
  double mul_rate = 2.0 / 1.0;

  void setMulDiv(uint16_t mul, uint16_t div) {
    mul_rate = (double)mul / (double)div;
  }

  uint32_t min_tick = sec_to_tick(0.01); // 10 msec
  uint32_t max_tick = sec_to_tick(2.0); // 2 sec

  enum class ActiveState : uint8_t {
    inactive = 0,
    ready,
    active
  };

  ActiveState _active = ActiveState::inactive;
  float _cycle_sec = 0.0f;
  uint32_t _prev_tick = 0;
  uint32_t _interval_tick = 0;

  bool isActive() const { return _active == ActiveState::active; }

  void init() {
    _active = ActiveState::inactive;
  }

  // assume to receive the triggers for each 1/16 timing.
  bool trigger(uint32_t tick) {
    if (_active == ActiveState::inactive) {
      _prev_tick = tick;
      _active = ActiveState::ready;
      return false;
    }

    uint32_t diff_tick = tick - _prev_tick;
    _prev_tick = tick;
    if (diff_tick < min_tick) {
      return false;
    }
    if (diff_tick > max_tick) {
      return false;
    }

    _interval_tick = diff_tick;
    _cycle_sec = (float)(((double)tick_to_sec(_interval_tick) * mul_rate));

    _active = ActiveState::active;

    return true;
  }

  float cycleSec() {
    return _cycle_sec;
  }

  void process(uint32_t tick) {
    uint32_t diff_tick = tick - _prev_tick;
    if (diff_tick > max_tick) {
      _active = ActiveState::inactive;
    }
  }
};

extern ClockSync clock_sync;
