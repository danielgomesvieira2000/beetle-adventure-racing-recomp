// bar_config.cpp — beetle-adventure-racing-recomp feature/settings flags.
//
// These are the toggles the recompiled game code (RecompiledFuncs/*.c) queries at runtime. For now they
// are hardcoded / env-driven; the in-game settings menu being built on the feature/settings-menu-and-high-fps
// branch will replace these with persisted setting values. Keep the accessor names/signatures stable so the
// menu can just set the backing storage. All accessors are `extern "C"` so the C recompiled output can call them.
#include <cstdlib>

// Intro skip: let the player skip the attract cinematics (and, when wired, the legal/logo screens) by
// pressing A / B / Start, instead of waiting out their fixed display timers. The game already skips the
// attract on START/A; our hook adds B and (TODO) extends skipping to the boot legal screens. ON by default;
// set BAR_NO_INTRO_SKIP to keep intros unskippable. The controller-pak/save prompt is intentionally NOT
// skippable. TODO(settings-menu): back this with a persisted "Skip intros" setting.
extern "C" int bar_intro_skip(void) {
    static const int on = (std::getenv("BAR_NO_INTRO_SKIP") == nullptr) ? 1 : 0;
    return on;
}

// TEMPORARY (BAR_DBG_TIME=1): report the UV timeline delta-time source.
//
// Intro/menu animation advances as `step -= rate * func_uvgfxmgr_rom_00401004()` (uvtseq_rom.c:243).
// That function returns EITHER a fixed override (D_uvgfxmgr_rom_00402480, when > 0) or a measured
// per-frame delta from uvClkGetSec. If menus are running fast while races are correct, we expect the
// menu to be on the fixed-override path with a value that assumes a lower render rate than we deliver.
// Probe is called from the two return sites of that recompiled function. Remove once diagnosed.
#include <cstdio>
#include <chrono>
extern "C" void bar_dbg_timebase(int path, float v) {
    static const bool on = std::getenv("BAR_DBG_TIME") != nullptr;
    if (!on) return;
    // Aggregate per second: which path was taken, and the value range seen.
    static long long t0 = 0; static unsigned n_over = 0, n_meas = 0;
    static float lo = 1e9f, hi = -1e9f, last = 0.0f;
    const long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now().time_since_epoch()).count();
    if (path == 1) n_over++; else n_meas++;
    if (v < lo) lo = v;
    if (v > hi) hi = v;
    last = v;
    if (t0 == 0) t0 = ms;
    if (ms - t0 >= 1000) {
        std::fprintf(stderr,
            "[BAR_DBG_TIME] calls/sec=%u (override=%u measured=%u) delta last=%.5f min=%.5f max=%.5f"
            "  => implied fps %.1f\n",
            n_over + n_meas, n_over, n_meas, last, lo, hi, last > 0.0f ? 1.0f / last : 0.0f);
        t0 = ms; n_over = n_meas = 0; lo = 1e9f; hi = -1e9f;
    }
}

// TEMPORARY (BAR_TIME_SCALE=<float>): scale the UV timeline delta-time source.
//
// Called from both return sites of the recompiled func_uvgfxmgr_rom_00401004. Timeline sequences
// advance as `step -= rate * delta` (uvtseq_rom.c:243), so the returned delta is directly
// proportional to animation speed: a scale BELOW 1.0 slows everything the timeline drives.
// Default 1.0 (exactly the stock value, so this is inert unless set). The point is empirical: dial
// the intro until it matches a reference emulator running the same ROM, and the factor that lands
// is the size of the discrepancy we have to explain.
extern "C" float bar_timebase(float v) {
    static const float scale = [] {
        const char* s = std::getenv("BAR_TIME_SCALE");
        const float f = (s != nullptr) ? (float)std::atof(s) : 1.0f;
        return (f > 0.0f && f < 100.0f) ? f : 1.0f;
    }();
    bar_dbg_timebase(0, v);
    return v * scale;
}

