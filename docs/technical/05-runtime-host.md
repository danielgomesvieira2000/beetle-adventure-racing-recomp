# 05 — The runtime host

The recompiled C is only half a port. The other half is a native host that supplies everything the
N64's hardware and libultra used to: threads, timing, video, audio, input, saves. This chapter
covers what `src/main/` does and, more usefully, the places where the obvious implementation is
wrong.

## The three runtime layers

| Layer | Repo | Responsibility |
|---|---|---|
| **librecomp** | `lib/N64ModernRuntime/librecomp` | The recompiled-CPU side: RDRAM, `recomp_context`, overlay/section bookkeeping, ROM validation, save handling, `recomp::start()` |
| **ultramodern** | `lib/N64ModernRuntime/ultramodern` | libultra reimplemented on modern primitives: threads and the cooperative scheduler, message queues, the VI thread, the AI/audio chain, high-level input |
| **RT64** | `lib/rt64` | The renderer. D3D12 on Windows, Vulkan on Linux/Android, Metal on macOS. Consumes F3DEX2 display lists at HLE |
| **RecompFrontend** | `lib/RecompFrontend` | `recompui` (launcher, settings, controls, mods — built on RmlUi) + `recompinput` (SDL2 input and controller profiles) |

RecompFrontend consumes N64ModernRuntime and RT64 **by path** (`RECOMP_FRONTEND_N64MODERNRUNTIME_PATH`,
`RECOMP_FRONTEND_RT64_PATH`) rather than by CMake target, so it reuses the runtime and renderer the
port already builds instead of duplicating them.

`recomp::start()` takes a single `recomp::Configuration` struct in the pinned runtime; older
Zelda64Recomp-derived examples pass separate arguments. Verify every callback field name against
the vendored headers rather than against a tutorial.

## Boot order, and the two things that must not race

`main()` runs, in this order:

1. **Windows: raise the timer resolution to 1 ms** (`timeBeginPeriod`). This is not a nicety. The
   VI-synced scheduler and the gfx/task threads hand work off via timed waits; at the default
   ~15.6 ms resolution every wake quantises to ~15.6 ms, so one gfx round-trip costs ~2 quanta and a
   menu frame several — which capped the menus at ~10 fps. A race was unaffected (it always has gfx
   queued and never waits), which is exactly why this was misdiagnosed as jitter for a while.
2. **Windows: set DPI awareness before `SDL_Init`.** `SDL_WINDOW_ALLOW_HIGHDPI` alone is not enough:
   unless the *process* is DPI-aware, Windows hands the app a scaled desktop and misreports sizes.
   At 125 % SDL sees 1536×864 of a 1920×1080 display. **`SDL_HINT_WINDOWS_DPI_SCALING` is
   deliberately OFF** — with it on, geometry and mouse positions are reported in logical units while
   the drawable stays in physical pixels, a 1.25× disagreement between where a button draws and
   where a click lands.
3. Resolve the config directory (`%LOCALAPPDATA%\...` on Windows, `~/.config/...` on Linux, or the
   exe directory when a `portable.txt` is present).
4. Load `graphics.json` and `input.json` and push them into the runtime — **before** `recomp::start()`,
   because the renderer reads the graphics config when it constructs the RT64 context and the first
   controller poll must see the input config.
5. Register the game with librecomp (`recomp::GameEntry`: hash, entrypoint, save type).
6. `bar_register_overlays()` ([04](04-static-recompilation.md#the-section-count-trap)).
7. Resolve the ROM: a previously validated ROM cached in the config dir > an explicit path argument >
   a native file picker.
8. Start the cooperative-preemption timer.
9. Register program identity and the primary font with recompui — **before** `recomp::start()`, which
   brings up the renderer, during which recompui builds its menus and throws if no primary font has
   been registered.
10. `recomp::start(config)` — blocks until the game exits.

### The deferred game start

The game thread blocks in `wait_for_game_started()` until `start_game()` runs. The VI thread only
seeds a dummy `OSViMode` while `!is_game_started()`, so starting before its first tick makes
`update_vi()` dereference a null mode. The auto-start path therefore waits on `g_bar_vi_ticked`
(set by the render context after its first present) on a detached thread, because `recomp::start()`
blocks the main thread.

**With RecompFrontend this auto-start must be disabled**, for two independent reasons. Correctness:
recompui's `add_start_game_or_load_rom_option()` calls `recomp::start_game()` itself once librecomp
reports a valid ROM, so the launcher owns that decision and auto-starting races it. Safety:
`g_bar_vi_ticked` is set by `src/main/rt64_render_context.cpp`, which the frontend **replaces** with
its own context — so nothing sets it, the wait falls through its full 5 s timeout, and the game
starts before the VI thread has ticked. That was a silent crash about nine seconds after launch.

## Threading and the audio starvation problem

**ultramodern has no preemption.** N64 threads run cooperatively: a thread holds the run token until
it does an OS message operation (`osRecvMesg` / `osSendMesg`) or blocks. On real hardware the audio
thread is preemptive — priority plus timer interrupts — so it always gets to run.

The consequence in a port: a long stretch of straight-line game-thread compute (a heavy
physics/AI/collision/display-list frame) never yields, the higher-priority audio manager never runs,
no new PCM is produced, and the host audio queue drains to empty. That is underrun crackle, and it
is worst exactly where the game is busiest.

### Cooperative preemption

`src/main/bar_preempt.cpp` plus `fix-recompiled.sh` rule (F):

* A dedicated host thread raises a global "should yield" flag every **2 ms** (~500 Hz). 60 Hz is far
  too coarse to keep a one-VI audio buffer fed across a multi-VI compute frame.
* A one-line poll is injected at **every recompiled-function prologue** — 3,147 sites — anchored on
  the `int c1cs = 0;` line that appears once in every function body:

```c
{ extern int bar_consume_yield(uint8_t* rdram);
  extern void yield_self_1ms(uint8_t* rdram);
  if (bar_consume_yield(rdram)) yield_self_1ms(rdram); }
```

* `bar_consume_yield` is a single relaxed atomic load on the hot path (the flag is clear ~99.99 % of
  calls). Only on the set path does it check that this is a game thread, that the game has started,
  and that the thread's priority is below 100 — so the audio manager (~110) and scheduler (127)
  never yield on their own work — then consumes the flag with one compare-exchange.

**Why not real preemption.** `SuspendThread`-style forcible preemption would resume the scheduler
while the game thread is mid-mutation of the lockless running queue, can deadlock on the CRT/heap
lock, and can tear RDRAM message-queue writes. Because a cooperative yield only ever fires at a
point the game thread *chose* — a function entry — exactly one N64 thread is ever on the token, and
single-writer-to-RDRAM determinism is preserved by construction.

`BAR_NO_PREEMPT=1` disables it, which is the first thing to try when bisecting a timing regression.

### The targeted case that came first

Rule (E) is the same idea aimed at one function. `func_uvcmidi_rom_00400940` stops the MIDI sequence
player then **spins** until it reports stopped, or 2.0 s. The player's state only advances when the
audio thread runs, and the spin never yields — so under a cooperative scheduler it burns the full
2 s timeout on *every* state transition while the host audio queue drains. A 1 ms yield injected at
the loop top fixes it.

Both rules are catalogued in [07](07-codegen-fixups-and-patches.md).

### The SI event-queue busy-spin

A second, unrelated starvation source, fixed by configuration rather than injection. BAR's SI
(controller) event queue (`main.c:311`, `D_8002F828`) has capacity **3**, and the port's low-level
`__osSiRawStartDma` posts an SI completion per call. The game does not drain them all during the
menus, so the queue fills with stale completions; with ultramodern's default `requeue_si = true`
every new completion **bounces** off the full queue in a ~15 M/sec busy-spin that starves the
cooperative scheduler and throttled the menus to ~10 fps.

SI completions are disposable — live input is re-read from PIF RAM every frame — so the port sets
`requeue_si = false` and drops the excess. `BAR_REQUEUE_SI=1` restores stock behaviour, which is
useful as a control: with it on the menu becomes choppy but *animation speed is unchanged*, which is
the signature of correctly delta-timed content.

## Audio

The chain is `libultra → ultramodern` (`librecomp/src/ai.cpp` → `ultramodern/src/audio.cpp`) →
the host callback, delivering `count` interleaved-stereo `int16` samples pointing straight into
RDRAM with no swapping done upstream.

**The sample transform was established empirically, not assumed**: captured BAR menu music was
analysed offline by autocorrelation. The result is counter-intuitive and worth stating plainly —
librecomp's RDRAM word swizzle already yields **correct host-order `int16`** on a raw read, so
byteswapping produces white-noise screeching. The swizzle does deliver the pair as `[R, L]`, so the
host channel-swaps to `[L, R]`.

`BAR_AUDIO_XFORM` selects among the four combinations: `0` as-is, `1` byteswap only, `2`
channel-swap only (**default, correct**), `3` both. `BAR_AUDIO_CAPTURE=<path>` dumps the raw
untransformed stream so the transform can be verified offline **without opening an audio device** —
which is how this was settled without listening to noise.

`BAR_NO_AUDIO` disables both the device and the recompiled ucode.

See also [04 — the RSP audio microcode](04-static-recompilation.md#the-rsp-audio-microcode) for the
2.0I-vs-2.0J jump-target problem, and
[04 — hardware-register stubs](04-static-recompilation.md#hardware-register-stubs) for why
`osAiGetLength` returning zero silently destroys audio latency.

## Input — the low-level SI/PIF path

BAR does **not** use the high-level `osContStartReadData` that ultramodern reimplements. It goes:

```
osContStartReadData()
  → __osPackReadData()          fills PIF RAM with button = 0xFFFF placeholders
  → __osSiRawStartDma(OS_WRITE)
  → osRecvMesg(SI queue)        waits for SI completion
  → __osSiRawStartDma(OS_READ)
  → parse
```

ultramodern stubs `__osSiRawStartDma`, so with a stub the game (a) deadlocks waiting for the SI
event and (b) reads the `0xFFFF` placeholders as every button held. `src/main/os_unimpl_stubs.cpp`
implements it: it posts the SI event, and on the READ DMA writes a real controller response into
PIF RAM.

PIF RAM per controller is `__OSContReadFormat`, 8 bytes:
`dummy, txsize, rxsize@2, cmd, button@4 (BE u16), stick_x@6, stick_y@7`. The top bits of `rxsize`
carry the channel-error flags (`0x04` = 4 bytes, no error).

Three defects found here, each of which produced a symptom nowhere near its cause:

* **`__osMaxControllers` (byte at `0x80032231`) is left at 0** by BAR's controller init, so
  `__osPackReadData` packs *zero* read-button commands — the PIF is just `CONT_CMD_END` and every
  read returns an empty buffer. The host writes `4` there on every `__osSiRawStartDma`, which happens
  during `osContInit`'s SI operations, before the first read-button pack. (An earlier
  `fix-recompiled.sh` rule clamped the same byte inside the generated code; it was redundant and was
  removed. The host-side write binds to a fixed data address, which is strictly better than an
  instruction-address anchor.)
* **BAR sometimes calls `__osSiRawStartDma` with a corrupt `a1`** — observed `pifram = 0x????21F0`,
  the low bits of the real PIF RAM with a garbage high half. `MEM_BU` on that faults. The handler
  only touches the buffer when `a1` is a sane KSEG0 8 MiB address, and otherwise just posts the SI
  event so the game does not deadlock.
* The SI queue depth issue described above.

## Controller Pak and Rumble Pak

BAR reads and writes its saves, and drives the Rumble Pak, over the **same** `__osSiRawStartDma`
path as button reads, via `READ_PAK` (cmd 2) and `WRITE_PAK` (cmd 3). The wire format and CRC are
ported from `lib/bar-decomp/tools/ultralib/src/io/{crc.c,contramread.c,contramwrite.c,motor.c}` —
specifically the **2.0I** (`BUILD_VERSION < VERSION_J`) branch of `__osContDataCrc`.

A pak command is a single-channel transaction: `channel` filler bytes (`0x00`) then one
`__OSContRamReadFormat` block —
`dummy, txsize, rxsize, cmd, address@4 (BE u16), data[32]@6, datacrc@0x26`.

Address decoding, by 32-byte block:

| Block | Address | Meaning | Served by |
|---|---|---|---|
| `< 0x400` | `< 0x8000` | Controller Pak data | The 32 KiB per-port save store (`mempak_pN.pak`) |
| `0x400` | `0x8000` | Bank select / identify | A per-port RAM echo cell |
| `0x600` | `0xC000` | Rumble motor on/off | Host rumble |

**One port presents both accessories at once**, which is a deliberate deviation from hardware — the
real slot holds one accessory and BAR even prompts the player to swap. They do not collide in the
address space, and the bank-select echo satisfies both (BAR writes a bank number and reads it back;
a Rumble Pak identify writes `0x80` and reads `0x80` back), so serving both from the same stream is
safe and removes the swap prompt as a practical concern.

**Controller Pak support needs two independent halves, and neither works alone** — verified by
reverting each:

1. **`input_get_device_info` must report a pak.** ultramodern sets
   `OSContStatus.status = (connected_pak != Pak::None)`, and BAR gates its *entire* pak path on that
   status — with `None` it never issues an SI pak query at all. `Pak::ControllerPak` was added to
   the ultramodern fork for this: it reports the accessory as present without `osMotorInit`
   accepting the port as a rumble device (which only accepts `RumblePak`).
2. **The SI stub must answer the query and serve the transactions**, including the **short-format**
   (`txsize, rxsize, cmd`) pak status query — the original stub understood only one of the two PIF
   command layouts and silently skipped BAR's, leaving the `0xFF` placeholders untouched.

`BAR_DBG_PAK=1` traces every pak transaction and every distinct SI frame shape in both directions.
It is what diagnosed both halves and is worth keeping.

## Presentation and the render context

`src/main/rt64_render_context.cpp` implements the `RendererContext` ultramodern drives: `send_dl()`
per graphics task, `update_screen()` per VI interrupt.

**BAR uses RT64's `Console` presentation mode — present strictly from the VI origin, at VI time.**
This is not a preference; the main-menu film-roll transition requires it. That transition **pans the
VI origin** with `osViSwapBuffer` across a pre-rendered tall framebuffer without redrawing anything:

* `PresentEarly` presents only freshly *rendered* content, so it drops the VI-origin pan entirely
  and the transition collapses to an instant swap.
* `SkipBuffering` presents the just-rendered framebuffer, so when setup renders the destination page
  it flashes that page for one frame *before* the pan begins.
* `Console` presents exactly the VI-origin framebuffer each frame, so the roll scrolls cleanly.

The full investigation is [`../R6_FILMROLL_FINDINGS.md`](../R6_FILMROLL_FINDINGS.md).

Related: **pre-game presentation depends on ultramodern alternating a dummy VI framebuffer address
each frame.** A merge resolution that kept BAR's `0x100000` base but dropped the alternation made
the launcher render once and then ignore all input — events reached recompinput and queued, but
recompui's UI loop lives in RT64's `draw_hook`, and the renderer had stopped presenting because a
constant address looks like the same frame forever. Console mode makes this especially sharp.

**MSAA and internal resolution are applied only through RT64's validated setup path.** Changing
them live is fragile and crashes: RT64's runtime `updateMultisampling()` skips the device
sample-count check that setup performs, and live render-target reallocation is brittle. The context
snapshots the values RT64 actually configured after setup (setup may have downgraded MSAA if the
device does not support the requested count) and re-asserts them on every runtime config update, so
those two settings change only across a restart. They still persist to `graphics.json` and take
effect on the next launch. Every other setting — refresh rate, aspect ratio, HUD ratio, window mode,
HDR — applies live.

## SDL event ownership

**recompinput must be the sole `SDL_PollEvent` caller.** A second loop in the host races it and each
swallows events the other needs.

It is pumped from `update_gfx`, **not** from `input_poll`. ultramodern does not call `input_poll`
until a game is running, and this port sits at the launcher with no game started — pumping only from
`input_poll` left nothing draining SDL's queue at the launcher, so the window stopped answering
Windows and went "Not Responding".

A consequence still open: with recompinput owning the queue, the host never receives
`SDL_CONTROLLERDEVICEADDED`, so pads plugged in after launch are not seen. Pads present at startup
work. The reference port solves this with a periodic re-scan.

## Small things that cost time

* **`SDL_ShowSimpleMessageBox` works with no window.** The release build is `/SUBSYSTEM:WINDOWS`, so
  `stderr` goes nowhere a player will look and shell redirection captures nothing. Fatal
  startup errors — missing Vulkan loader, no compatible GPU — must reach a real dialog or the player
  gets a silent black window. `BAR_DBG_UI=1` writes frontend diagnostics to `bar_ui_trace.log` for
  the same reason.
* **RT64's backend is per-platform and the SDL window flags differ.** D3D12 binds to the raw HWND and
  needs no graphics-API flag; Vulkan and Metal create their swapchain surface *through* SDL and
  require the matching window flag, without which SDL fails the surface call and RT64 then
  dereferences the null surface and segfaults. The flag is deliberately omitted on Windows.
* **Font family names are matched from inside the file.** `LatoLatin-Regular.ttf` declares itself
  `LatoLatin`, not `Lato`. Registering the wrong name fails *silently*: every element lays out and
  draws in the right place, with no text in any of them.
* **"Restart Game" restarts the process.** The recompiled game cannot be cleanly re-entered
  in-process, so the pause menu's restart relaunches the executable with the same arguments and
  quits the current instance.
