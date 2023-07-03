#pragma once

#include "config.hpp"
#include <igb_util/serializer.hpp>

struct Env {
  constexpr static uint32_t current_vendor_id = SPR_VENDOR_ID;
  constexpr static uint32_t current_firmware_id = SPR_FIRMWARE_ID;
  constexpr static uint32_t current_firmware_version = SPR_FIRMWARE_VER;

  constexpr static uint32_t unknown_id_ver = 0;
  uint32_t vendor_id = unknown_id_ver;
  uint32_t firmware_id = unknown_id_ver;
  uint32_t firmware_version = unknown_id_ver;

  bool isCompatible() const {
    return vendor_id == current_vendor_id
      && firmware_id == current_firmware_id
      && firmware_version == current_firmware_version;
  }

  void setCurrentVers() {
    vendor_id = current_vendor_id;
    firmware_id = current_firmware_id;
    firmware_version = current_firmware_version;
  }

  void clearVers() {
    vendor_id = unknown_id_ver;
    firmware_id = unknown_id_ver;
    firmware_version = unknown_id_ver;
  }

  constexpr static size_t serialized_buf_size = 3 * sizeof(uint32_t);
  size_t serialize(uint8_t* out_buffer, size_t size) const {
    out_buffer += igb::Serializer::serialize(out_buffer, vendor_id);
    out_buffer += igb::Serializer::serialize(out_buffer, firmware_id);
    out_buffer += igb::Serializer::serialize(out_buffer, firmware_version);
    return serialized_buf_size;
  }
  size_t deserialize(uint8_t* in_buffer, size_t size) {
    in_buffer += igb::Serializer::deserialize(in_buffer, vendor_id);
    in_buffer += igb::Serializer::deserialize(in_buffer, firmware_id);
    in_buffer += igb::Serializer::deserialize(in_buffer, firmware_version);
    return serialized_buf_size;
  }
};

extern Env env;
