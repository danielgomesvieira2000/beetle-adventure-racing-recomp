// os_unimpl_stubs.cpp — native no-op stubs for low-level libultra OS functions.
//
// N64Recomp lists these in `ignored_funcs` (so it does NOT recompile them), and the runtime
// (librecomp/ultramodern) does NOT provide them either — normally a game never reaches them
// because the runtime reimplements the higher-level callers. BAR's recompiled code DOES call
// them, so we provide empty stubs here so the port links. This mirrors how librecomp stubs
// similar low-level access functions (e.g. __osPiGetAccess_recomp is an empty body).
//
// !!! NO-OPs — sufficient to LINK, NOT correct for running. These touch hardware/threading the
// runtime models differently:
//   __osEnqueueThread / __osPopThread        N64 thread queue (ultramodern uses native threads)
//   __osViSwapContext                        VI framebuffer swap (RT64 owns presentation)
//   __osSiGetAccess/RelAccess/RawStartDma    controller (SI) bus + DMA
//   osPiReadIo / __osPiRawReadIo             cartridge (PI) IO
//   __osTimerInterrupt                       timer/scheduler tick
// TODO(BAR): during runtime bring-up, give these real behavior (or reimplement the higher-level
// callers so these are never reached). See docs/recomp notes.
#include "recomp.h"
#include <ultramodern/ultramodern.hpp>
#include <cstdlib>
#include <chrono>   // R6 diagnostic: BAR_DBG_FPS loop-rate counter (remove after)
#include <cstdio>   // R6 diagnostic
#include <cstring>  // BAR_BURST_ON_ROLL: memcpy/strchr/strcmp for the burst-capture spec parse

#include "main/bar_cheats.h"   // bar_cheats::apply_frame (host-side RDRAM cheat pokes)
#include "main/bar_input.hpp"  // bar::input::{port_connected, port_pak} (per-port SI responses)

// Per-port input resolution lives in main.cpp / bar_input (they own the Win32 window / focus + SDL).
extern "C" uint16_t bar_poll_keyboard(int port, int8_t* stick_x, int8_t* stick_y);

// R6 diagnostic (env-gated BAR_DBG_SLIDE): trace the menu page-transition slide. Called from the
// recompiled transition-start (func_selection_00402E98) and slide-draw (func_selection_00418800) to
// see, per navigation, whether the slide draws over many frames or just 1-2 (instant). Remove after R6.
extern "C" void bar_rt64_start_burst(const char *dir, int count);

// Widescreen HUD anchoring (RT64 fork, src/hle/rt64_bar_hud.cpp): RT64 has to know when the game is
// racing, because anchoring the HUD to the edges of the widened frame is right for the race HUD and
// wrong for the menus, which are authored as whole 4:3 compositions. Only this side can read the
// game's state out of RDRAM, so it pushes the answer in.
extern "C" void bar_rt64_set_hud_anchor(int racing);
extern "C" void bar_dbg_slide(const char* tag) {
    // Deterministic burst-capture trigger for the film-roll transition: when the animation loop
    // (func_filmroll_00400170) is entered, tell RT64 to capture the next N presents to dir/fNNNN.png.
    // BAR_BURST_ON_ROLL="dir:count". Fires regardless of BAR_DBG_SLIDE; robust to boot-timing variance.
    {
        static bool parsed = false, fired = false;
        static char burst_dir[256] = {0};
        static int  burst_count = 0;
        static char burst_tag[64] = "func_filmroll_00400170";   // default trigger: the pan loop
        if (!parsed) {
            parsed = true;
            if (const char* spec = std::getenv("BAR_BURST_ON_ROLL")) {   // "dir:count[:tag]"
                if (const char* c1 = std::strchr(spec, ':')) {
                    size_t dlen = (size_t)(c1 - spec);
                    if (dlen >= sizeof(burst_dir)) dlen = sizeof(burst_dir) - 1;
                    std::memcpy(burst_dir, spec, dlen); burst_dir[dlen] = '\0';
                    burst_count = std::atoi(c1 + 1);
                    if (const char* c2 = std::strchr(c1 + 1, ':')) {     // optional trigger-tag override
                        std::strncpy(burst_tag, c2 + 1, sizeof(burst_tag) - 1);
                        burst_tag[sizeof(burst_tag) - 1] = '\0';
                    }
                }
            }
        }
        if (!fired && burst_count > 0 && std::strcmp(tag, burst_tag) == 0) {
            fired = true;
            bar_rt64_start_burst(burst_dir, burst_count);
            std::fprintf(stderr, "[beetle-adventure-racing-recomp] BURST_ON_ROLL -> %s x%d @%s\n", burst_dir, burst_count, burst_tag);
            std::fflush(stderr);
        }
    }
    static const bool on = std::getenv("BAR_DBG_SLIDE") != nullptr;
    if (!on) return;
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::steady_clock::now().time_since_epoch()).count();
    static long long t0 = 0; if (t0 == 0) t0 = ms;
    static unsigned long n = 0;
    std::fprintf(stderr, "[BAR_DBG_SLIDE] %-9s #%lu t+%lldms\n", tag, ++n, ms - t0);
}
extern "C" void bar_dbg_caller(const char* tag, unsigned addr) {
    static const bool on = std::getenv("BAR_DBG_SLIDE") != nullptr;
    if (!on) return;
    std::fprintf(stderr, "[BAR_DBG_SLIDE] %s caller-ra=0x%08X\n", tag, addr);
}

#define BAR_OS_STUB(name) \
    extern "C" void name(uint8_t* rdram, recomp_context* ctx) { (void)rdram; (void)ctx; }

BAR_OS_STUB(__osViSwapContext_recomp)
BAR_OS_STUB(__osTimerInterrupt_recomp)
BAR_OS_STUB(__osSiGetAccess_recomp)
BAR_OS_STUB(__osSiRelAccess_recomp)

// ----------------------------------------------------------------------------------------
// Controller-accessory (pak) joybus emulation. BAR reads/writes its Controller Pak saves and drives
// the Rumble Pak over the SAME low-level __osSiRawStartDma path as button reads, via READ_PAK (cmd 2)
// and WRITE_PAK (cmd 3). We answer those here: mempak data blocks are backed by bar::input's 32 KiB
// per-port store; the accessory-detect (block 0x400) and rumble (block 0x600) regions implement the
// standard motor-init handshake so a Rumble Pak is detected and its motor toggles. Wire format + CRC
// are ported from lib/bar-decomp tools/ultralib/src/io/{crc.c,contramread.c,contramwrite.c,motor.c}.
//
// A pak command is a SINGLE-channel transaction: `channel` filler bytes (0x00) then one
// __OSContRamReadFormat block (dummy,txsize,rxsize,cmd, address@4 (BE u16), data[32]@6, datacrc@0x26).
// Blocks: CONT_BLOCK_DETECT = 0xC000/0x20... 0x8000/0x20 = 0x400; CONT_BLOCK_RUMBLE = 0xC000/0x20 = 0x600.
namespace {
constexpr int kBlockDetect = 0x400;   // 0x8000 >> 5
constexpr int kBlockRumble = 0x600;   // 0xC000 >> 5

// __osContDataCrc, ported verbatim from crc.c (2.0I / BUILD_VERSION < VERSION_J branch).
uint8_t pak_data_crc(const uint8_t* data) {
    uint8_t temp = 0, temp2;
    for (int i = 0; i <= 32; i++) {
        for (int j = 7; j > -1; j--) {
            temp2 = (temp & 0x80) ? 0x85 : 0;
            temp <<= 1;
            if (i != 32) temp |= ((data[i] & (1 << j)) ? 1 : 0);
            temp ^= temp2;
        }
    }
    return temp;
}

// Per-port RAM-echo cell for a Controller Pak's accessory-detect / bank-select region (block 0x400):
// __osPfsSelectBank writes the bank byte here and reads it back, and the mempak detect is RAM-like.
uint8_t g_detect_cell[4][32] = {};

// TEMPORARY (BAR_DBG_PAK=1): trace Controller Pak detection. The game reports "No Controller Pak
// present" even with a pak configured, so log every SI frame's command triple plus each pak
// transaction, to find where osPfsInit's probe diverges from what we answer. Remove once fixed.
bool bar_dbg_pak() {
    static const bool on = std::getenv("BAR_DBG_PAK") != nullptr;
    return on;
}
}

// Handle a single READ_PAK/WRITE_PAK block for `port` at PIF format base `fb` (sign-extended). Uses the
// MEM_* macros, so it must be called with `rdram` in scope (from __osSiRawStartDma_recomp).
static void bar_handle_pak(uint8_t* rdram, int64_t fb, int port, unsigned cmd) {
    const unsigned addr  = ((unsigned)(uint8_t)MEM_BU(4, fb) << 8) | (uint8_t)MEM_BU(5, fb);
    const int      block = (int)(addr >> 5);
    const bool     is_write = (cmd == 3);
    const bar::input_config::PakType pak = bar::input::port_pak(port);

    uint8_t data[32];
    if (is_write) {
        for (int i = 0; i < 32; i++) data[i] = (uint8_t)MEM_BU(6 + i, fb);   // bytes the game is writing
    } else {
        for (int i = 0; i < 32; i++) data[i] = 0;                            // default read response
    }

    // Serve BOTH accessories from the same transaction stream, rather than branching on the
    // configured pak type. On real hardware the slot holds one accessory and BAR even prompts the
    // player to swap ("Please remove the Rumble Pak and insert the Controller Pak"), but the two do
    // not actually collide in the address space:
    //
    //   block <  0x400 (addr < 0x8000)  Controller Pak data   -> 32 KiB per-port save store
    //   block == 0x400 (addr   0x8000)  bank-select / identify -> RAM echo, which satisfies BOTH
    //                                    (BAR writes a bank number and reads it back; a Rumble Pak
    //                                     identify writes 0x80 and reads 0x80 back)
    //   block == 0x600 (addr   0xC000)  rumble motor on/off    -> host rumble
    //
    // So a single port can present a working Controller Pak and a working Rumble Pak at once, with
    // no manual switching. Deliberate deviation from hardware; the identify semantics are left
    // exactly as the save path already proved, so this changes policy only.
    if (block == kBlockDetect) {
        if (is_write) std::memcpy(g_detect_cell[port], data, 32);
        else          std::memcpy(data, g_detect_cell[port], 32);
    } else if (block < kBlockDetect) {
        if (is_write) bar::input::mempak_write(port, block, data);
        else          bar::input::mempak_read(port, block, data);
    } else if (block == kBlockRumble) {
        if (is_write) {
            bar::input::set_rumble(port, data[0] != 0);
            if (bar_dbg_pak())
                std::fprintf(stderr, "[BAR_DBG_PAK] *** RUMBLE motor write port=%d value=0x%02X ***\n",
                             port, data[0]);
        }
    }
    // Blocks between the data area and the accessory region are out of range: leave zero.
    (void)pak;

    if (bar_dbg_pak()) {
        std::fprintf(stderr, "[BAR_DBG_PAK] %s port=%d addr=0x%04X block=0x%X pak=%d data[0..3]=%02X%02X%02X%02X\n",
                     is_write ? "WRITE" : "READ", port, addr, block, (int)pak,
                     data[0], data[1], data[2], data[3]);
    }

    if (!is_write) {
        for (int i = 0; i < 32; i++) MEM_B(6 + i, fb) = (int8_t)data[i];      // READ_PAK returns 32 data bytes
    }
    // Both READ_PAK and WRITE_PAK echo the data CRC (the game verifies it). rxsize (fb+2) already carries
    // no channel-error bits, so leave it.
    MEM_B(0x26, fb) = (int8_t)pak_data_crc(data);
}

// __osSiRawStartDma: BAR drives the controller (SI/PIF) bus directly through the low-level libultra
// path — osContStartReadData() does __osPackReadData() (which fills the PIF RAM with button=0xFFFF
// placeholders) -> __osSiRawStartDma(OS_WRITE) -> osRecvMesg -> __osSiRawStartDma(OS_READ) -> parse.
// ultramodern stubs this (it expects games to use the reimplemented high-level osContStartReadData),
// so two things must happen here or the game (a) deadlocks waiting for the SI-complete event and
// (b) reads the 0xFFFF placeholders as "every button held":
//   1. Post the SI event so the game's osRecvMesg(SI queue) returns (like send_si_message in the
//      high-level path).
//   2. On the READ DMA, write a real controller-read response into the PIF RAM so the game gets
//      actual (currently neutral) input instead of the 0xFFFF placeholder.
// PIF RAM layout per controller is __OSContReadFormat (8 bytes): dummy,txsize,rxsize@2,cmd,
// button@4(BE u16),stick_x@6,stick_y@7. rxsize top bits = CHNL error (0x04 = 4 bytes, no error).
extern "C" void __osSiRawStartDma_recomp(uint8_t* rdram, recomp_context* ctx) {
    const int32_t direction = (int32_t)ctx->r4;   // OS_READ = 0, OS_WRITE = 1
    const uint32_t pifram = (uint32_t)ctx->r5;     // &__osContPifRam.ramarray
    // __osMaxControllers (byte @ 0x80032231) is left at 0 by BAR's recompiled controller init, so
    // __osPackReadData (func_8000E6E0) packs ZERO read-button commands — the PIF is just CONT_CMD_END
    // and every controller read returns an empty buffer (no input reaches the game). Force it to
    // MAXCONTROLLERS(4) here (runs during osContInit's SI ops, before the first read-button pack) so a
    // real CONT_CMD_READ_BUTTON gets packed and the joybus read carries actual buttons.
    MEM_B(0, (int64_t)(int32_t)0x80032231) = (int8_t)4;
    // BAR sometimes calls __osSiRawStartDma with a corrupt a1 (observed pifram=0x????21F0 — low bits of
    // the real PIF RAM but a garbage high half) -> MEM_BU faults reading it. Only touch the buffer when
    // a1 is a sane RDRAM (KSEG0, 8 MiB) address; otherwise just post the SI event so we never AV.
    const bool valid_pifram = ((pifram - 0x80000000u) < 0x00800000u);

    // TEMPORARY (BAR_DBG_PAK=1): dump every DISTINCT SI frame shape, both directions, before any
    // classification. The pak-frame classifier below may simply be failing to recognise BAR's pak
    // frames; this shows the raw truth. Deduped against the last logged signature so the per-frame
    // button poll doesn't flood the log.
    if (bar_dbg_pak() && valid_pifram) {
        const int64_t pf = (int32_t)ctx->r5;
        char sig[80]; int n = 0;
        n += std::snprintf(sig + n, sizeof(sig) - n, "d%d:", (int)direction);
        for (int k = 0; k < 24 && n < (int)sizeof(sig) - 3; k++)
            n += std::snprintf(sig + n, sizeof(sig) - n, "%02X", (unsigned)MEM_BU(0, pf + k));
        static std::string last;
        if (last != sig) { last = sig; std::fprintf(stderr, "[BAR_DBG_SI] %s\n", sig); }
    }
    // Drive the state machine for testing: BAR_FORCESTATE="frame:state frame:state ..." writes
    // gGameSettings->gameStateFlag (@0x08) once at each listed SI-frame; the game's main loop then
    // calls uvSetGameState(state). Lets us reach later screens / a race without the (unread) buttons.
    { static bool inited = false; static const char* fs = nullptr; static unsigned long long fc = 0;
      if (!inited) { inited = true; fs = std::getenv("BAR_FORCESTATE"); }
      if (fs && *fs) { ++fc; const int64_t GS = (int64_t)(int32_t)0x80025CF0;
        for (const char* p = fs; *p; ) {
            while (*p == ' ') ++p; if (!*p) break;
            char* e = nullptr; long fr = std::strtol(p, &e, 10); long st = 0;
            if (e && *e == ':') st = std::strtol(e + 1, &e, 10);
            p = (e && e != p) ? e : p + 1;
            if ((long)fc == fr) { MEM_W(0X8, GS) = (int32_t)st; }
        } } }
    // Cheats menu: poke the game's RDRAM for any enabled BAR cheats. This hook fires ~once per
    // controller poll (per frame) in both the menus and a race, the right cadence for the
    // "every frame" cheat writes (unlocks, debug-options flags). No-op when nothing is enabled.
    bar_cheats::apply_frame(rdram);
    // R6 diagnostic (env-gated BAR_DBG_FPS): the menu/game main loop polls the controller once per
    // iteration, so this hook's call rate == the loop rate. The page-slide animation advances per loop
    // iteration; if this is >> native 60 Hz the slide completes in ~1 display frame ("disabled"-looking).
    { static const bool dbg = std::getenv("BAR_DBG_FPS") != nullptr;
      if (dbg) {
          static unsigned long n = 0; static long long t0 = 0;
          long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now().time_since_epoch()).count();
          ++n; if (t0 == 0) t0 = ms;
          if (ms - t0 >= 1000) { std::fprintf(stderr, "[BAR_DBG_FPS] %lu SI-polls/sec (== menu loop rate)\n", n); n = 0; t0 = ms; }
      } }
    // TEMPORARY (BAR_DBG_DIV=1): log BAR's whole-frame divider D_8001F7C0 alongside the game state.
    // The timeline system is delta-timed and measured correct (~1/60), so anything running fast must
    // advance a fixed amount per RENDERED frame. If the console runs menus at divider 2 (30 fps) and
    // we render every frame (divider 1), those animations play at double speed. Base-game address, so
    // it is plain RDRAM and readable directly. Remove once diagnosed.
    { static const bool dbg = std::getenv("BAR_DBG_DIV") != nullptr;
      if (dbg) {
          static int32_t last_div = -0x7FFF, last_st = -0x7FFF;
          const int32_t div = (int32_t)MEM_W(0, (int64_t)(int32_t)0x8001F7C0);
          const int32_t st  = (int32_t)MEM_W(0XA4, (int64_t)(int32_t)0x80025CF0);
          if (div != last_div || st != last_st) {
              std::fprintf(stderr, "[BAR_DBG_DIV] frame divider = %d   (currentGameState = %d)\n", div, st);
              last_div = div; last_st = st;
          }
      } }
    // Tell RT64 whether the racing HUD is on screen, so it anchors it to the edges of the widened
    // frame (see the extern above). Two fields of gGameSettings (0x80025CF0) are needed, both
    // measured by running the headless race recipe with BAR_HUD_TRACE=1:
    //
    //   currentGameState (+0xA4) is 14 through the Controller Pak prompts and every front-end menu,
    //   2 for the boot attract sequence, and 5 for a race.
    //
    //   raceState (+0x88) is the race's phase. State 5 alone is not enough: it also covers the
    //   track-loading screen ("Coventry Cove", with its wipe tiles and its record-time text) and the
    //   "Get Ready" card, which are full 4:3 compositions and come apart if pulled to the edges.
    //
    // The game reads raceState as an s16 -- the word's high half, so the values that arrive here as
    // 0x10000 and 0x30000 are phases 1 and 3 -- and func_plyr_00402268 in the decomp runs the player
    // update for phase 0 OR phase 3. That is the game's own grouping for "the car is on the track",
    // and it is exactly the condition wanted here: phase 1 is the loading and "Get Ready" cards with
    // no HUD, phase 3 is the countdown with the HUD already drawn, phase 0 is the race running.
    // Anchoring only phase 0 left the HUD at its 4:3 positions through the whole countdown and then
    // snapped it out to the edges the moment the player got control.
    {
        const int64_t GS = (int64_t)(int32_t)0x80025CF0;
        const int32_t st = (int32_t)MEM_W(0XA4, GS);          // currentGameState
        const int32_t phase = (int32_t)MEM_H(0X88, GS);       // raceState, as the game reads it
        const int32_t paused = (int32_t)MEM_H(0X86, GS);      // pauseFlag
        static const bool trace = std::getenv("BAR_HUD_TRACE") != nullptr;
        if (trace) {
            static int32_t lastS = -0x7FFF, lastR = -0x7FFF, lastP = -0x7FFF;
            if ((st != lastS) || (phase != lastR) || (paused != lastP)) {
                lastS = st; lastR = phase; lastP = paused;
                std::fprintf(stdout, "[hud] state=%d racePhase=%d paused=%d\n", st, phase, paused);
                std::fflush(stdout);
            }
        }
        // Phase 0 is also what the field still reads at the moment currentGameState becomes 5, before
        // the race has set it -- a stale value from before, over the film-roll wipe out of the Rumble
        // Pak prompt. So phase 0 only counts once this race has been seen setting up (phase 1 or 3),
        // which is reset whenever the game state changes.
        static int32_t lastState = -0x7FFF;
        static bool sawSetup = false;
        if (st != lastState) {
            lastState = st;
            sawSetup = false;
        }
        if ((phase == 1) || (phase == 3)) {
            sawSetup = true;
        }

        bar_rt64_set_hud_anchor(((st == 5) && sawSetup && ((phase == 0) || (phase == 3))) ? 1 : 0);

        // Attract/intro frame cap.
        //
        // currentGameState 2 is the boot attract sequence: a canned replay of a race, driven by the
        // game's own race simulation. In this port the demo cars drive too fast and consequently miss
        // their scripted events -- the sequence loses sync with itself rather than merely looking
        // quick. That is the signature of a per-frame-stepped simulation being handed twice the
        // frames it was authored for: an actual race in this port runs its loop at ~30/s, while the
        // attract runs at ~60/s (measured previously, docs/KNOWN_ISSUES.md).
        //
        // So halve the rate at which the game is given VI retraces while state 2 is on screen, which
        // steps the demo's simulation at 30 Hz, and restore 60 everywhere else -- menus, races and the
        // front end are untouched. This changes how often the GAME steps, not how often the host
        // presents, so it is not a drop to 30 fps of output.
        //
        // Two measurements bound what this does and does not fix:
        //
        //   * Car speed: captured at the same wall-clock offset into the attract, the demo car is at
        //     visibly different points on the lap with the cap on versus off -- it drives slower, as
        //     intended.
        //
        //   * Sequence length: unchanged. State 2 lasts 72.45 s stock, 72.76 s capped, and 72.46 s
        //     with the UV timeline delta halved. The attract's LENGTH is on a different clock from
        //     its motion -- the divider only changes how often the game is handed a retrace, while
        //     total_vis, the VI registers and the wall clock keep advancing at 60. So the demo now
        //     drives at the right speed but still gets cut off after the same number of real seconds,
        //     covering less of the lap than before.
        //
        // BAR_ATTRACT_HZ overrides the target (60 disables the cap); BAR_VI_DIVIDER=<n> forces a
        // divider in every state.
        {
            static const int forced = [] {
                const char* e = std::getenv("BAR_VI_DIVIDER");
                return (e != nullptr) ? std::atoi(e) : 0;
            }();
            static const int attract_div = [] {
                const char* e = std::getenv("BAR_ATTRACT_HZ");
                const int hz = (e != nullptr) ? std::atoi(e) : 30;   // 60 == no cap
                return (hz > 0 && hz < 60) ? (60 / hz) : 1;
            }();

            const int want = (forced > 0) ? forced : ((st == 2) ? attract_div : 1);
            static int applied = -1;
            if (want != applied) {
                applied = want;
                ultramodern::set_vi_divider(want);
                if (std::getenv("BAR_DBG_STATE") != nullptr) {
                    std::fprintf(stderr, "[beetle-adventure-racing-recomp] VI divider -> %d "
                                         "(game loop %d Hz) in state %d\n", want, 60 / want, st);
                }
            }
        }
    }
    // R6 tooling (env-gated BAR_DBG_STATE): log currentGameState (gGameSettings+0xA4) transitions so a
    // BAR_AUTOPLAY script can be tuned/verified headlessly (boot -> logos -> intro -> menu=0xE -> race=2).
    { static const bool dbg = std::getenv("BAR_DBG_STATE") != nullptr;
      if (dbg) {
          static int32_t last = -0x7FFF;
          int32_t st = (int32_t)MEM_W(0XA4, (int64_t)(int32_t)0x80025CF0);
          if (st != last) {
              long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch()).count();
              std::fprintf(stderr, "[BAR_DBG_STATE] currentGameState -> %d (0x%X) @%lldms\n", st, st, ms);
          }
          last = st;
      } }
    if (direction == 0 /* OS_READ */ && valid_pifram) {
        // The PIF RAM holds one joybus command block per controller (8 bytes for the status-query
        // and button-read commands BAR uses for the menu). Branch on the command byte (offset 3) so
        // we write the matching response; only port 0 is connected. Leave other commands (Controller
        // Pak / EEPROM) untouched so we don't corrupt their (differently sized) buffers.
        // MEM_* expect a SIGN-EXTENDED N64 address (the macros subtract 0xFFFFFFFF80000000): a plain
        // uint32_t 0x800321F0 zero-extends and underflows that subtraction to a +4 GiB out-of-bounds
        // offset -> access violation. Use a sign-extended gpr-style value, exactly like ctx->rN.
        const int64_t pifram_se = (int32_t)ctx->r5;

        // First, detect a single-channel pak command (READ_PAK cmd 2 / WRITE_PAK cmd 3). These use a
        // different PIF layout than the button/status poll: `channel` filler bytes (0x00) then ONE
        // __OSContRamReadFormat block at offset `channel`. Identify it by the (cmd, txsize, rxsize)
        // triple + all-zero leading filler, then service the pak (mempak / rumble) and skip the button
        // loop (a pak transaction carries no button blocks).
        // SHORT-format single command (libultra's __OSContRequesFormatShort / the pak request path):
        // txsize@0, rxsize@1, cmd@2, payload@3 — i.e. NO leading dummy byte, unlike the 8-byte
        // per-port button/status blocks handled below (dummy@0, txsize@1, rxsize@2, cmd@3).
        //
        // BAR issues its Controller Pak status query in this layout ("01 03 00 FF FF FF FE"). Reading
        // cmd at offset 3 sees 0xFF, so the old code skipped it and left the 0xFF response placeholders
        // untouched — the game then reports the pak unusable ("error creating game note"). Answer it
        // here, and route short-format READ_PAK/WRITE_PAK through the same handler by passing a base
        // one byte earlier, which realigns its dummy-relative offsets (addr@4,5 -> 3,4; data@6 -> 5).
        {
            const unsigned s_tx  = (unsigned)MEM_BU(0, pifram_se);
            const unsigned s_rx  = (unsigned)MEM_BU(1, pifram_se);
            const unsigned s_cmd = (unsigned)MEM_BU(2, pifram_se);
            const bool has_pak = bar::input::port_pak(0) != bar::input_config::PakType::None;

            if (s_cmd == 0 && s_tx == 0x01 && s_rx == 0x03 && bar::input::port_connected(0)) {
                MEM_B(3, pifram_se) = 0x05;                              // typeh
                MEM_B(4, pifram_se) = 0x00;                              // typel -> CONT_TYPE_NORMAL
                MEM_B(5, pifram_se) = has_pak ? (int8_t)0x01 : (int8_t)0x00;   // CONT_CARD_ON
                if (bar_dbg_pak()) std::fprintf(stderr, "[BAR_DBG_PAK] short STATUS -> pak=%d\n", (int)has_pak);
                ultramodern::send_si_message();
                return;
            }
            if (((s_cmd == 2 && s_tx == 0x03 && s_rx == 0x21) ||         // READ_PAK
                 (s_cmd == 3 && s_tx == 0x23 && s_rx == 0x01)) && has_pak) {
                bar_handle_pak(rdram, pifram_se - 1, 0, s_cmd);
                ultramodern::send_si_message();
                return;
            }
        }

        int pak_ch = -1; unsigned pak_cmd = 0;
        for (int ch = 0; ch < 4; ch++) {
            const int64_t fb = pifram_se + ch;
            const unsigned c  = (unsigned)MEM_BU(3, fb);
            const unsigned tx = (unsigned)MEM_BU(1, fb);
            const unsigned rx = (unsigned)MEM_BU(2, fb);
            if ((c == 2 && tx == 3 && rx == 33) || (c == 3 && tx == 35 && rx == 1)) {
                bool filler_ok = true;
                for (int k = 0; k < ch; k++) if ((unsigned)MEM_BU(0, pifram_se + k) != 0x00) { filler_ok = false; break; }
                if (filler_ok) { pak_ch = ch; pak_cmd = c; break; }
            }
        }

        if (bar_dbg_pak()) {
            // Log the raw command triples for all four 8-byte slots plus the first bytes, so a pak
            // frame we FAILED to classify still shows up here (that is the suspected failure mode).
            std::fprintf(stderr, "[BAR_DBG_PAK] pak_ch=%d cmd=%u |", pak_ch, pak_cmd);
            for (int ch = 0; ch < 4; ch++) {
                const int64_t fb = pifram_se + (int64_t)(ch * 8);
                std::fprintf(stderr, " [%d]tx=%02X rx=%02X c=%02X", ch,
                             (unsigned)MEM_BU(1, fb), (unsigned)MEM_BU(2, fb), (unsigned)MEM_BU(3, fb));
            }
            std::fprintf(stderr, " | raw:");
            for (int k = 0; k < 12; k++) std::fprintf(stderr, "%02X", (unsigned)MEM_BU(0, pifram_se + k));
            std::fprintf(stderr, "\n");
        }

        if (pak_ch >= 0) {
            if (bar::input::port_pak(pak_ch) != bar::input_config::PakType::None) {
                bar_handle_pak(rdram, pifram_se + pak_ch, pak_ch, pak_cmd);
            }
        } else {
            // The button/status poll packs one 8-byte block per port (all four, cmd 0/1). Answer each port
            // from its live config (bar::input): a port with a device assigned is "connected" and reports
            // CONT_TYPE_NORMAL + a pak-present status bit; an unplugged/unassigned port replies NO_RESPONSE.
            for (int i = 0; i < 4; i++) {
                const int64_t blk = pifram_se + (int64_t)(i * 8);
                const unsigned cmd = (unsigned)MEM_BU(3, blk);   // CONT_CMD_* the game packed
                if (cmd != 0 && cmd != 1) continue;              // not status/button -> leave as-is
                if (!bar::input::port_connected(i)) {            // unplugged / no device assigned
                    MEM_B(2, blk) = (int8_t)0x80;                // rxsize: CHNL NO_RESPONSE error
                    continue;
                }
                if (cmd == 0) {                  // CONT_CMD_REQUEST_STATUS (controller detect)
                    const bool has_pak = bar::input::port_pak(i) != bar::input_config::PakType::None;
                    MEM_B(2, blk) = 0x03;        // rxsize=3, no channel error
                    MEM_B(4, blk) = 0x05;        // typeh  -> type = typel<<8|typeh = 0x0005 (CONT_TYPE_NORMAL)
                    MEM_B(5, blk) = 0x00;        // typel
                    MEM_B(6, blk) = has_pak ? (int8_t)0x01 : (int8_t)0x00;  // status: CONT_CARD_ON (pak present)
                    MEM_B(7, blk) = 0x00;
                } else {                         // CONT_CMD_READ_BUTTON
                    int8_t sx = 0, sy = 0;
                    const uint16_t button = bar_poll_keyboard(i, &sx, &sy);
                    MEM_B(2, blk) = 0x04;        // rxsize=4, no channel error
                    MEM_B(4, blk) = (int8_t)(uint8_t)(button >> 8);
                    MEM_B(5, blk) = (int8_t)(uint8_t)(button & 0xFF);
                    MEM_B(6, blk) = sx;
                    MEM_B(7, blk) = sy;
                }
            }
        }
    }
    ultramodern::send_si_message();
}

BAR_OS_STUB(osPiReadIo_recomp)
BAR_OS_STUB(__osPiRawReadIo_recomp)
BAR_OS_STUB(__osPopThread_recomp)
BAR_OS_STUB(__osEnqueueThread_recomp)
