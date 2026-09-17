#pragma once
// The Rumble Pak motor BAR pulses, modelled on the host. See bar_rumble.cpp.

#include <cstdint>

namespace bar::rumble {

constexpr int kPorts = 4;

// BAR_RUMBLE_RAW=1: bypass the model and hand the on/off flag to recompinput as before (A/B).
bool raw_mode();

// A motor start (on) or stop (off) the game just wrote to `port`'s Rumble Pak. Any thread.
void motor(int port, bool on);

// Once per frame on the main thread. Advances every port's motor model and returns, in `out`, the
// strength to send to each port's pad (0..0xFFFF), already scaled by `slider` (0..100) and zero when
// `silenced`. `send[port]` is true when that value should be (re)issued now: it changed, or a held
// level needs refreshing before the 250 ms SDL duration runs out.
void step(double slider, bool silenced, uint16_t out[kPorts], bool send[kPorts]);

// How long, in milliseconds, one issued strength is asked to last.
constexpr uint32_t kSendDurationMs = 250;

} // namespace bar::rumble
