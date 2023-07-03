#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <array>
#include <optional>

struct MemoryArea {
  size_t pos = 0;
  size_t size = 0;
};

struct MemoryManager {
  const size_t memory_size;

  constexpr static size_t areas_size = 16;
  constexpr static size_t free_areas_size = areas_size+1;

  size_t free_area_count = 1;
  std::array<MemoryArea, free_areas_size> free_areas;

  void clear() {
    free_areas[0] = MemoryArea {0, memory_size};
    free_area_count = 1;
  }

  void init() {
    clear();
  }

  void removeZeroArea() {
    std::array<MemoryArea, free_areas_size> new_free_areas;
    size_t new_free_areas_count = 0;

    for (size_t i = 0; i < free_area_count; ++i) {
      if (free_areas[i].size != 0) {
        new_free_areas[new_free_areas_count] = free_areas[i];
        ++new_free_areas_count;
      }
    }

    for (size_t i = 0; i < new_free_areas_count; ++i) {
      free_areas[i] = new_free_areas[i];
    }
    
    free_area_count = new_free_areas_count;
  }

  void refreshFreeAreas() {
    removeZeroArea();

    std::sort(free_areas.begin(), free_areas.begin() + free_area_count, [](const MemoryArea& a, const MemoryArea& b) {
      return a.pos < b.pos;
    });

    bool is_merged = false;
    do {
      is_merged = false;
      for (size_t i = 0; i < free_area_count-1; ++i) {
        if ((free_areas[i].pos + free_areas[i].size) == free_areas[i+1].pos) {
          // merge
          free_areas[i].size += free_areas[i+1].size; 
          for (size_t j = i+1; j < free_area_count-1; ++j) {
            // shift data
            free_areas[j] = free_areas[j+1];
          }
          --free_area_count;
          is_merged = true;
          break;
        }
      }
    } while (is_merged);
  }

  std::optional<MemoryArea> assignArea(size_t size) {
    for (size_t i = 0; i < free_areas_size; ++i) {
      if (size <= free_areas[i].size) {
        MemoryArea new_area {
          .pos = free_areas[i].pos,
          .size = size
        };

        free_areas[i].pos += size;
        free_areas[i].size -= size;

        refreshFreeAreas();

        return new_area;
      }
    }
    return std::nullopt;
  }

  void releaseArea(const MemoryArea& area) {
    free_areas[free_area_count] = area;
    ++free_area_count;

    refreshFreeAreas();
  }
};

