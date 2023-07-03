#include "periph/app_spi.hpp"
#include <igb_sdk/device/fram_mb85r_spi.hpp>
#include <igb_daisy/periph/gpio.hpp>
#include <igb_sdk/util/mem_accessor.hpp>
#include <igb_stm32/periph/fast_gpio.hpp>

using namespace igb;
using namespace igb::stm32;
using namespace igb::sdk;

struct AppProcessor {
  void operator()() {
    process();
  }

  void process() {
    // dummy
  }
};

typedef FramMb85rSPI<AppSpi, FastGpioPin<daisy_pin_to_stm32_pin(DaisyGpioPinType::d7)>, 16 /* address bit size */, AppProcessor> AppFram;
typedef MemAccessor<AppFram, 256 /* data_buf_size */, 32 /* ring_buf_size */> MemAccessorCls;
typedef MemAccessorCls::ReqInfo RamReqInfo;
typedef MemAccessorCls::ReqType RamReqType;

struct AppRam {
  AppSpi spi;
  MemAccessorCls mem;

  inline void init() {
    spi.init(SpiBaudratePrescaler::div2);
    mem.init();
  }

  inline void request(auto&& info) { mem.request(info); }
  inline void process() { mem.process(); }
  inline bool isProcessing() const { return mem.isProcessing(); }
  inline bool isRequestEmpty() const { return mem.isRequestEmpty(); }

  IGB_SIZE_OPTIMIZE void busyWait() {
    while (!isRequestEmpty()) {
      process();
    } // busy wait
  }
};
