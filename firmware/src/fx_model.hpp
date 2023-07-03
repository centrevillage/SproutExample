#pragma once

#include <cstddef>
#include <cstdint>
#include "fx_model_select.hpp"

#define SPR_FX_MODEL_EACH_CASE(model_name, idx) case idx: model.emplace<idx>(); break;
#define SPR_FX_MODEL_LIST(default_model, ...) \
  typedef std::variant<default_model __VA_OPT__(,) __VA_ARGS__> Model; \
  Model model = default_model {}; \
  constexpr static uint8_t model_count = IGB_ARG_COUNT(default_model, __VA_ARGS__); \
  constexpr bool setModelByIdx(uint8_t _model_idx) { \
    if (_model_idx >= model_count) { return false; } \
    switch (_model_idx) { \
      IGB_FOR_EACH_WITH_IDX(SPR_FX_MODEL_EACH_CASE, default_model, __VA_ARGS__) \
      default: break; \
    } \
    model_idx = _model_idx; \
    return true; \
  }

struct FxModel {
  uint8_t model_idx = 0;

  SPR_FX_MODEL_SELECT

  void init(bool reset = true) {
    if (reset) {
      resetValues();
    } else {
      updateAllParams();
    }
    std::visit([=](auto& m) {
      m.init();
    }, model);
  }

  void gatein(bool on) {
    std::visit([=](auto& m) {
      m.gatein(on);
    }, model);
  }

  void resetValues() {
    std::visit([=](auto& m) {
      for (uint8_t idx = 0; idx < param_size; ++idx) {
        auto param_id = static_cast<AppParamId>(idx);
        auto& param = app_params.getParam(param_id);
        param.setRaw(m.param_ranges[idx].default_value);
        param.updateValue(m.param_ranges[idx]);
        int16_t v = param.get();
        m.updateParam(param_id, v);
      }
      system_memory.writeParams(app_params, [](){});
    }, model);
  }

  uint8_t modelIdx() const {
    return model_idx;
  }

  void nextModel() {
    changeModel((model_idx + 1) % model_count);
  }

  bool changeModel(uint8_t new_model_idx) {
    if (model_idx == new_model_idx) {
      return false;
    }

    std::visit([=](auto& m) {
      m.deinit();
    }, model);

    if (!setModelByIdx(new_model_idx)) {
      return false;
    }

    std::visit([=](auto& m) {
      m.init();
    }, model);

    system_memory.writeModelIdx(*this, [](){});

    return true;
  }

  IGB_FAST_INLINE void updateParam(AppParamId param_id) {
    std::visit([=](auto& m) {
      size_t idx = static_cast<size_t>(param_id);
      auto& param = app_params.getParam(param_id);
      int16_t prev_v = param.get();
      param.updateValue(m.param_ranges[idx]);
      int16_t v = param.get();
      if (prev_v != v) {
        m.updateParam(param_id, v);
        if (param_id != AppParamId::cv) {
          system_memory.writeParam(param);
        }
      }
    }, model);
  }

  IGB_FAST_INLINE void updateAllParams() {
    for (uint8_t i = 0; i < param_size; ++i) {
      updateParam(static_cast<AppParamId>(i));
    }
  }

  IGB_FAST_INLINE void process(const float* in_buf, float* out_buf, size_t count) {
    std::visit([=](auto& m) {
      m.process(in_buf, out_buf, count);
    }, model);
  }

  IGB_FAST_INLINE void tapTempo(uint16_t tap_tempo_msec) {
    std::visit([=](auto& m) {
      m.tapTempo(tap_tempo_msec);
    }, model);
  }
};

extern FxModel fx_model;

