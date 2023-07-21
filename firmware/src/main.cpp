#include <stdio.h>
#include <string.h>

#include "daisy_seed.h"
#include "hardware.hpp"
#include "config.hpp"

#include <igb_stm32/base.hpp>
#include <igb_stm32/periph/systick.hpp>
#include <igb_stm32/periph/gpio.hpp>
#include <igb_stm32/periph/spi.hpp>

#include <igb_stm32/periph/flash.hpp>
#include <igb_stm32/periph/nvic.hpp>
#include <igb_stm32/periph/rcc.hpp>
#include <igb_stm32/periph/tim.hpp>
#include <igb_sdk/util/soft_timer.hpp>

#include "periph/led_display.hpp"
#include "periph/app_spi.hpp"
#include "periph/app_ram.hpp"
#include "periph/app_midi.hpp"
#include "periph/app_adc.hpp"
#include "periph/gatein.hpp"
#include "periph/app_buttons.hpp"
#include "periph/sdram.hpp"
#include "periph/system_memory.hpp"
#include "periph/tim.hpp"
#include "app_mode.hpp"
#include "fx_model.hpp"
#include "periph/memory_manager.hpp"
#include "util/level_analyzer.hpp"
#include "util/beat_lfo.hpp"
#include "util/app_knob_mode.hpp"
#include "util/clock_sync.hpp"
#include "env.hpp"

using namespace igb::stm32;
using namespace igb::sdk;

AppSpi spi;

SoftTimer<1> setup_timer; 

// tim2/3/4/5 are used by daisy lib.
Tim<TimType::tim15> hard_timer; // for button process
extern "C" void TIM15_IRQHandler() {
  if (hard_timer.is(TimState::update)) {
    app_buttons.process();
    hard_timer.clear(TimState::update);
  }
}
inline void initHardTimer() {
  hard_timer.init(
    TimConf {
      .prescale = 1,
      .period = 65535,
      .enable_it_update = true,
      .interrupt_priority = 2
    }
  );
  hard_timer.start();
}

void AudioCallback(daisy::AudioHandle::InterleavingInputBuffer  in,
                   daisy::AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size) {
  fx_model.process(in, out, size);
}

static inline void setup() {
  hardware.Configure();
  hardware.Init(false /* boost = true (480MHz) */);

  app_buttons.init();
  gatein.init();
  app_ram.init();
  memory_manager.init();
  beat_lfo.init(current_tick());
  clock_sync.init();

  delay_msec(10);

  led_display.init();

  app_midi.init();

  app_adc.init();

  level_analyzer.init();

  clearSdram();

  system_memory.checkVersionAndRestore();
  app_ram.busyWait();
  app_mode.init();

  setup_timer.oneshotCallback(sec_to_tick(0.01), current_tick(), [](){
    app_knob_mode.changeType(KnobModeType::catching);
    app_mode.refresh();
  });

  initHardTimer();
}

static inline void loop() {
  led_display.process();
  gatein.process();
  //app_buttons.process();
  app_ram.process();
  app_midi.process();
  app_adc.process();
  app_mode.process();
  beat_lfo.process(current_tick());
  setup_timer.process(current_tick());
  clock_sync.process(current_tick());
}

int main(void) {
  setup();

  hardware.SetAudioBlockSize(SPR_AUDIO_BUFFER_SIZE);
  if (SPR_SR == 96000) {
    hardware.SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate::SAI_96KHZ);
  } else {
    hardware.SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);
  }
  hardware.StartAudio(AudioCallback);
  for(;;) {
    loop();
  }
}
