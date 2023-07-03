#pragma once

#include <algorithm>
#include <igb_util/macro.hpp>
#include "periph/memory_manager.hpp"

constexpr static size_t sdram_buf_size = 67108864 / sizeof(float);
extern float sdram_buf[sdram_buf_size]; // 64MB

extern MemoryManager memory_manager;

IGB_FAST_INLINE float* getFloatPtr(const MemoryArea& area) {
  return (sdram_buf + area.pos);
}

IGB_FAST_INLINE void clearSdram() {
  std::fill(&sdram_buf[0], &sdram_buf[sdram_buf_size], 0.0f);
}

