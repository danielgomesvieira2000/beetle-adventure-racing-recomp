// src/main/bar_cheats.cpp — see bar_cheats.h.
//
// Each cheat is a host-side write into BAR's RDRAM at a FIXED global address (verified against
// lib/bar-decomp/linker_scripts/us/symbol_addrs.txt + include/structs.h + src/game_init.c). The
// safe surface is the game's own debug-options block inside gGameSettings (0x80025CF0) plus the
// "open count" unlock counters (0x8002CFF0..) and the built-in cheat-unlock flags (0x8002D009..) —
// the engine already reads all of these, so writing them cannot corrupt memory (worst case a value
// is out of range / inert). We intentionally do NOT touch the per-race timer/position/boost state:
// those live in relocatable overlay modules at runtime-allocated heap addresses (no fixed symbol),
// and a literal-address poke there would point into arbitrary heap and crash. See the "bar-cheats"
// project memory for the full vetted list and the discarded (heap-only) candidates.
#include "main/bar_cheats.h"

#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

#include "recomp.h"                 // MEM_W / MEM_B (dereference the local `rdram`)
#include "game/config.hpp"          // bar::config::get_app_config_directory

namespace bar_cheats {
namespace {

// Sign-extend an N64 KSEG0 address the way the MEM_* macros require: (int64_t)(int32_t)addr.
constexpr int64_t se(uint32_t addr) { return (int64_t)(int32_t)addr; }

// Reinterpret an IEEE-754 float as the 32-bit word the game reads back via LWC1/MFC1. RDRAM words
// round-trip host int32 through MEM_W, so writing the float's bit pattern yields the right float.
inline int32_t f32_bits(float f) { int32_t b; std::memcpy(&b, &f, sizeof(b)); return b; }

std::array<std::atomic<bool>, (std::size_t)Id::Count> g_enabled{};   // value-init -> all false

const Info kInfo[(std::size_t)Id::Count] = {
    { "unlock_all",        "Unlock everything",          "All cars, tracks, championships & battle arenas" },
    { "unlock_arenas",     "Unlock Beetle Battle arenas", "All 9 multiplayer battle arenas (Airport .. Woods)" },
    { "reveal_cheat_menu", "Reveal in-game Cheat menu",  "Unlocks BAR's hidden Daisy-box cheats (Options → Cheats)" },
    { "solo_race",         "Solo race (no rivals)",     "Removes the 7 AI opponents — set before a race" },
    { "infinite_laps",     "Infinite laps",             "The race never ends on lap count" },
    { "super_speed",       "Super top speed",           "Raises the car's max-speed cap (2×)" },
    { "steering_assist",   "Steering assist",           "Debug steering aid" },
    { "show_fps",          "Show FPS counter",          "BAR's built-in on-screen framerate display" },
    { "no_glare",          "Disable sun glare",         "Turns off the lens-glare effect" },
};

std::filesystem::path cheats_file() {
    return bar::config::get_app_config_directory() / "cheats.cfg";
}

} // namespace

const Info& info(Id id) { return kInfo[(std::size_t)id]; }

void set(Id id, bool enabled) {
    if ((std::size_t)id < (std::size_t)Id::Count) g_enabled[(std::size_t)id].store(enabled, std::memory_order_relaxed);
}
bool get(Id id) {
    return (std::size_t)id < (std::size_t)Id::Count && g_enabled[(std::size_t)id].load(std::memory_order_relaxed);
}
bool any_enabled() {
    for (const auto& a : g_enabled) if (a.load(std::memory_order_relaxed)) return true;
    return false;
}

void apply_frame(uint8_t* rdram) {
    if (!any_enabled()) return;
    auto on = [](Id id) { return g_enabled[(std::size_t)id].load(std::memory_order_relaxed); };

    // One-time diagnostic: report the active cheat set the first time we apply (lets a headless run
    // confirm load() + the per-frame apply path executed, and which cheats are live).
    {
        static bool logged = false;
        if (!logged) {
            logged = true;
            std::fprintf(stderr, "[beetle-adventure-racing-recomp][cheats] applying:");
            for (int i = 0; i < (int)Id::Count; i++) if (on((Id)i)) std::fprintf(stderr, " %s", kInfo[i].id);
            std::fprintf(stderr, "\n");
            std::fflush(stderr);
        }
    }

    // --- Unlocks: the "open count" gates the selection menus read (safe exact totals) ---
    if (on(Id::UnlockAll)) {
        MEM_W(0, se(0x8002CFF4)) = 11;  // gNumOpenCars    : 3 starter + 8 unlockable Beetles
        MEM_W(0, se(0x8002CFF0)) = 6;   // gNumOpenTracks  : Coventry Cove .. Wicked Woods
        MEM_W(0, se(0x8002CFF8)) = 4;   // gNumOpenTourns  : Novice / Advanced / Pro / Bonus cups
    }

    // --- Beetle Battle arenas. The arena-select menu gates on the 9 per-arena unlock BITS at
    //     0x8002D000..0x8002D008 (=1 each) — NOT on the gNumOpenMtracks *count* (0x8002CFFC), which
    //     only feeds the results-screen "N/9 arenas unlocked" counter. Setting just the count (the
    //     original bug) left the battle maps locked. Verified against BAR's own unlock-everything
    //     routine (func_selection @0x83C099C8, RecompiledFuncs/funcs_38.c:~15768): it writes exactly
    //     these 9 bits (loop bounded by the count) plus the count itself. The upper 6 bits double as
    //     the per-track "collected all 100 boxes" award flags. Also matches the community GameShark
    //     "Unlock <arena> (Multi)" codes 8002D000..8002D008 (Airport..Woods). We set both the bits
    //     (the real gate) and the count (so the menu and the results counter agree). ---
    if (on(Id::UnlockAll) || on(Id::UnlockArenas)) {
        MEM_W(0, se(0x8002CFFC)) = 9;                                     // gNumOpenMtracks (results counter)
        for (int i = 0; i < 9; i++) MEM_B(i, se(0x8002D000)) = (int8_t)1; // per-arena availability bits
    }

    // --- Reveal BAR's hidden cheat menu: set the 18 gOpenCheatFlag bytes (0x8002D009..0x8002D01A) ---
    if (on(Id::RevealCheatMenu)) {
        for (int i = 0; i < 18; i++) MEM_B(i, se(0x8002D009)) = (int8_t)1;
    }

    // --- gGameSettings debug-options block (the engine honors these) ---
    if (on(Id::SoloRace))       MEM_W(0, se(0x80025D18)) = 0;                  // gNumAiCars       (game uses 0 itself)
    if (on(Id::InfiniteLaps))   MEM_W(0, se(0x80025DA4)) = 1;                  // gDebugInfLapsFlag
    if (on(Id::SuperSpeed))     MEM_W(0, se(0x80025D20)) = f32_bits(600.0f);   // gDebugMxSpeed    (default 300.0f)
    if (on(Id::SteeringAssist)) MEM_W(0, se(0x80025D30)) = 1;                  // gDebugSteerAssist
    if (on(Id::ShowFps))        MEM_W(0, se(0x80025DAC)) = 1;                  // gDebugFrateDisp
    if (on(Id::NoGlare))        MEM_W(0, se(0x80025D98)) = 0;                  // gDebugOptsGlare  (default 1 = on)
}

// ---- Persistence: a tiny "id=0/1" text file (no JSON dependency in this always-built TU) ----
void save() {
    try {
        std::ofstream f(cheats_file(), std::ios::trunc);
        if (!f) return;
        for (std::size_t i = 0; i < (std::size_t)Id::Count; i++)
            f << kInfo[i].id << '=' << (g_enabled[i].load(std::memory_order_relaxed) ? 1 : 0) << '\n';
    } catch (...) {}
}

void load() {
    try {
        std::ifstream f(cheats_file());
        if (!f) return;
        std::string line;
        while (std::getline(f, line)) {
            const auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            const std::string key = line.substr(0, eq);
            const bool val = line.find('1', eq) != std::string::npos;
            for (std::size_t i = 0; i < (std::size_t)Id::Count; i++)
                if (key == kInfo[i].id) { g_enabled[i].store(val, std::memory_order_relaxed); break; }
        }
    } catch (...) {}
}

} // namespace bar_cheats
