#pragma once

#include "periph/app_ram.hpp"
#include "env.hpp"
#include "app_params.hpp"

#include <igb_util/serializer.hpp>

// memory structure
// env[256]:
//  env_version[4]
//  firmware_id[4]
//  firmware_versoin[4]
//  reserved[]
// reserved[]
// tmp[256]:
//  - firmware save region
struct SystemMemory {
  constexpr static size_t env_address = 0;
  constexpr static size_t tmp_address = 256;
  constexpr static size_t model_idx_address = tmp_address;
  constexpr static size_t params_address = model_idx_address + 1;

  enum class State {
    none = 0,
    restore,
    reset
  };
  State state = State::none;

  IGB_SIZE_OPTIMIZE void write(size_t address, const auto& v, auto&& on_complete) {
    app_ram.request(RamReqInfo {
      .req_type = RamReqType::write,
      .mem_address = address,
      .buf_size = igb::Serializer::size(v),
      .serialize_func = [&v](uint8_t* buf, size_t size) {
        if (size < igb::Serializer::size(v)) { return (size_t)0; }
        return igb::Serializer::serialize(buf, v);
      },
      .complete_func = on_complete
    });
  }

  IGB_SIZE_OPTIMIZE void read(size_t address, auto& v, auto&& on_complete) {
    app_ram.request(RamReqInfo {
      .req_type = RamReqType::read,
      .mem_address = address,
      .buf_size = igb::Serializer::size(v),
      .serialize_func = [&v](uint8_t* buf, size_t size) {
        if (size < igb::Serializer::size(v)) { return (size_t)0; }
        return igb::Serializer::deserialize(buf, v);
      },
      .complete_func = on_complete
    });
  }

  void writeEnv(const Env& _env, auto&& on_complete) {
    write(env_address, _env, on_complete);
  }

  void readEnv(Env& _env, auto&& on_complete) {
    read(env_address, _env, on_complete);
  }

  void writeParams(const auto& _params, auto&& on_complete) {
    write(params_address, _params, on_complete);
  }

  void readParams(auto& _params, auto&& on_complete) {
    read(params_address, _params, on_complete);
  }

  void writeParam(const auto& param) {
    size_t param_offset = static_cast<size_t>(param.id) * Param::serialized_buf_size;
    write(params_address + param_offset, param, [](){});
  }

  void readParam(auto& param) {
    size_t param_offset = static_cast<size_t>(param.id) * Param::serialized_buf_size;
    read(params_address + param_offset, param, [](){});
  }


  void writeModelIdx(const auto& _fx_model, auto&& on_complete) {
    app_ram.request(RamReqInfo {
      .req_type = RamReqType::write,
      .mem_address = model_idx_address,
      .buf_size = 1,
      .serialize_func = [&_fx_model](uint8_t* buf, size_t size) {
        buf[0] = _fx_model.modelIdx();
        return 1;
      },
      .complete_func = on_complete
    });
  }

  void readModelIdx(auto& _fx_model, auto&& on_complete) {
    app_ram.request(RamReqInfo {
      .req_type = RamReqType::read,
      .mem_address = model_idx_address,
      .buf_size = 1,
      .serialize_func = [&_fx_model](uint8_t* buf, size_t size) {
        _fx_model.setModelByIdx(buf[0]);
        return 1;
      },
      .complete_func = on_complete
    });
  }

  void restore();
  void reset();

  void checkVersionAndRestore();

  void busyWait() {
    app_ram.busyWait();
  }
};

extern SystemMemory system_memory;
