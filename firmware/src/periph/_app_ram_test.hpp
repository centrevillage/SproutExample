#include <functional>
#include <igb_sdk/util/mem_accessor.hpp>

using namespace igb;
using namespace igb::sdk;

// test object
struct AppFram {
  constexpr static uint32_t data_size = 32768; // 256kbit
  uint8_t data[data_size];

  void init() {
  }

  void requestRead(uint8_t* buf, uint32_t buf_size, uint32_t read_address, auto&& callback) {
    uint8_t* p = &(data[read_address]);
    for (uint8_t i = 0; i < buf_size; ++i) {
      buf[i] = *p;
      ++p;
    }
    std::function<void(void)> func = callback;
    if (func) { func(); }
  }

  void requestWrite(uint8_t* buf, uint32_t buf_size, uint32_t write_address, auto&& callback) {
    uint8_t* p = &(data[write_address]);
    for (uint8_t i = 0; i < buf_size; ++i) {
      *p = buf[i];
      ++p;
    }
    std::function<void(void)> func = callback;
    if (func) { func(); }
  }

  bool isProcessing() const {
    return false;
  }
  
  void process() {
  };

  inline bool isRequestEmpty() const {
    return true;
  }
};

typedef MemAccessor<AppFram, 256, 32> MemAccessorCls;
typedef MemAccessorCls::ReqInfo RamReqInfo;
typedef MemAccessorCls::ReqType RamReqType;

struct AppRam {
  MemAccessorCls mem;

  void init() {
    mem.init();
  }

  void request(auto&& info) {
    mem.request(info);
  }

  void process() {
    mem.process();
  }

  inline bool isProcessing() const {
    return mem.isProcessing();
  }

  inline bool isRequestEmpty() const {
    return mem.isRequestEmpty();
  }

  void busyWait() {
    while (!isRequestEmpty()) {
      process();
    } // busy wait
  }
};
