#include "periph/app_midi.hpp"

AppMidi app_midi;

bool led_flag = false;

uint8_t send_event_bytes[3] = {0, 0, 0};

void AppMidi::onReceive(daisy::MidiEvent m) {
  switch(m.type) {
    case daisy::SystemRealTime:
      {
        switch(m.srt_type) {
          case daisy::TimingClock:
            send_event_bytes[0] = 0xF8; // clock
            midi.SendMessage(send_event_bytes, 1);
            break;
          case daisy::Start:
            send_event_bytes[0] = 0xFA; // start
            midi.SendMessage(send_event_bytes, 1);
            break;
          case daisy::Continue:
            send_event_bytes[0] = 0xFB; // continue
            midi.SendMessage(send_event_bytes, 1);
            break;
          case daisy::Stop:
            send_event_bytes[0] = 0xFC; // stop
            midi.SendMessage(send_event_bytes, 1);
            break;
          default:
            break;
        }
      }
      break;
    default:
      break;
  }
}
