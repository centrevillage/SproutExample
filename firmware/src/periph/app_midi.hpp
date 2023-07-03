#pragma once

#include "daisy_seed.h"
#include <igb_util/macro.hpp>

struct AppMidi {
  daisy::MidiUartHandler midi;

  void init() {
    daisy::MidiUartHandler::Config midi_config;
    midi.Init(midi_config);
    midi.StartReceive();
  }

  IGB_FAST_INLINE void process() {
    midi.Listen();
    while(midi.HasEvents()) {
      onReceive(midi.PopEvent());
    }
  }

  void onReceive(daisy::MidiEvent m);
};

extern AppMidi app_midi;

