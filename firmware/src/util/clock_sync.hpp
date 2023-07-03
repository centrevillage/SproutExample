#pragma once

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <functional>

struct ClockSync {
  // 1/8
  uint32_t mul = 2;
  uint32_t div = 1;

  uint32_t min_usec = 10UL * 1000UL; // 10 msec
  uint32_t max_usec = 2000UL * 1000UL; // 2 sec

  enum class ActiveState : uint8_t {
    inactive = 0,
    ready,
    active
  };

  ActiveState _active = ActiveState::inactive;
  float _cycle_sec = 0.0f;
  uint32_t _prev_usec = 0;
  uint32_t _interval_usec = 0;

  bool isActive() const { return _active == ActiveState::active; }

  void init() {
    _active = ActiveState::inactive;
  }

  // assume to receive the triggers for each 1/16 timing.
  bool trigger(uint32_t usec) {
    if (_active == ActiveState::inactive) {
      _prev_usec = usec;
      _active = ActiveState::ready;
      return false;
    }

    uint32_t diff_usec = usec - _prev_usec;
    _prev_usec = usec;
    if (diff_usec < min_usec) {
      return false;
    }
    if (diff_usec > max_usec) {
      _active = ActiveState::inactive;
      return false;
    }

    _interval_usec = diff_usec;
    _cycle_sec = (float)(((double)_interval_usec * (double)mul / (double)div) / 1000000.0);

    _active = ActiveState::active;

    return true;
  }

  float cycleSec() {
    return _cycle_sec;
  }

  void process(uint32_t usec) {
    uint32_t diff_usec = usec - _prev_usec;
    if (diff_usec > max_usec) {
      _active = ActiveState::inactive;
    }
  }
};

extern ClockSync clock_sync;
