// bar_watchdog.h -- hang detector for the thread that pumps SDL. See bar_watchdog.cpp for why this
// exists and what it writes. Both calls are no-ops off Windows, so callers need no #ifdef.
#pragma once

namespace bar::watchdog {

// Start the monitor thread. Call once from main(), after the app config directory is usable (the
// report is written there) and before the first frame.
void install();

// One relaxed atomic increment. Call once per frame from update_gfx -- the callback that runs on the
// thread owning the window, which is the thread whose stall this watches for.
void heartbeat();

} // namespace bar::watchdog
