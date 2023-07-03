#include "app_params.hpp"

AppParams app_params;

size_t AppParams::serialize(uint8_t* out_buffer, size_t size) const {
  for (uint8_t i = 0; i < (param_size - 1); ++i) {
    out_buffer += params[i].serialize(out_buffer, size);
  }
  return serialized_buf_size;
}

size_t AppParams::deserialize(uint8_t* in_buffer, size_t size) {
  for (uint8_t i = 0; i < (param_size - 1); ++i) {
    in_buffer += params[i].deserialize(in_buffer, size);
  }
  return serialized_buf_size;
}
