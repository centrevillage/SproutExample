#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include "app_param_id.hpp"
#include <igb_util/serializer.hpp>
#include <array>

struct ParamRange {
  int16_t min = 0;
  int16_t max = 1023;
  float zero_spacing = 0.0;
  float default_value = 0.5f;
};

struct Param {
  const AppParamId id;

  float value = 0.0f;
  int16_t _cache_value = 0;

  inline void setRaw(float raw_value /** 0.0 ~ 1.0 */) {
    value = raw_value;
  }

  inline float getRaw() const {
    return value;
  }

  int16_t get() const {
    return _cache_value;
  }

  void updateValue(const ParamRange& pr) {
    double v = value;
    if (pr.zero_spacing > 0.0) {
      if (pr.min < 0) {
        if (std::abs(v - 0.5) < pr.zero_spacing) {
          _cache_value = 0;
          return;
        }
        if (v > 0.5) {
          v = 0.5 + (v - 0.5 - pr.zero_spacing) * (0.5 / (0.5 - pr.zero_spacing));
        } else {
          v = v * (0.5 / (0.5 - pr.zero_spacing));
        }
      } else {
        if (v < pr.zero_spacing) {
          _cache_value = pr.min;
          return;
        }
        v = (v - pr.zero_spacing) * (0.5 / (0.5 - pr.zero_spacing));
      }
    }

    double value_range = (double)(pr.max - pr.min);
    v = (v * value_range) + (double)pr.min;

    _cache_value = std::clamp((int16_t)v, pr.min, pr.max);
  }

  constexpr static uint32_t serialized_buf_size = 2;
  size_t serialize(uint8_t* out_buffer, size_t size) const {
    uint16_t v =(uint16_t)(std::clamp(value * 65536.0f, 0.0f, 65535.0f));
    out_buffer += igb::Serializer::serialize(out_buffer, v);
    return serialized_buf_size;
  }

  size_t deserialize(uint8_t* in_buffer, size_t size) {
    uint16_t v;
    in_buffer += igb::Serializer::deserialize(in_buffer, v);
    value = (float)v / 65536.0f;
    return serialized_buf_size;
  }
};

struct AppParams {
  std::array<Param, param_size> params = {
    Param { .id = AppParamId::time },
    Param { .id = AppParamId::sun },
    Param { .id = AppParamId::wet },
    Param { .id = AppParamId::time2 },
    Param { .id = AppParamId::sun2 },
    Param { .id = AppParamId::wet2 },
    Param { .id = AppParamId::cv },
  };

  IGB_FAST_INLINE Param& getParam(AppParamId id) {
    return params[static_cast<size_t>(id)];
  }

  constexpr static size_t serialized_buf_size = Param::serialized_buf_size * 6;
  size_t serialize(uint8_t* out_buffer, size_t size) const;
  size_t deserialize(uint8_t* in_buffer, size_t size);
};

extern AppParams app_params;

