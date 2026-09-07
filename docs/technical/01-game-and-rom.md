# 01 — The game and the ROM

## Cartridge identity

| Property | Value |
|---|---|
| Title (header) | `Beetle Adventure Rac` |
| Cartridge code | `NNSE` (USA) |
| SHA-1 | `e5ab4d226c08d22f68a2edcc48870203e67454b8` |
| MD5 | `cf97c336479ddbf1217e4dde89d9d2d3` |
| CRC1 / CRC2 | `EDF419A8` / `BF1904CC` |
| Size / format | 16 MiB, big-endian `.z64` |
| Entry point | `0x80000400` |
| XXH3-64 (of the `.z64`) | `0x56cfec69d7951f9f` — this is the hash librecomp validates against, **not** the SHA-1 |

Developed by **Paradigm Entertainment**, published by EA (1999). The engine is Paradigm's in-house
**"UV" engine**, shared with their other N64 titles — which is why so many module names are
`uv*_rom`.

No ROM, ELF, or extracted asset may ever be committed. `.gitignore` blocks `*.z64`, `*.n64`,
`*.v64`, `/elf/*.elf` and the generated symbol TOMLs for exactly this reason.

## The entry point, confirmed two ways

`0x80000400`, established independently by:

* ROM header offset `0x8` (the standard N64 header field), and
* the splat `entry` segment vram in the decomp's linker script
  (`linker_scripts/us/beetleadventurerac.ld`, `.entry 0x80000400`).

This is what `beetle-adventure-racing-recomp.toml`'s `[input] entrypoint` is set to. Getting it
wrong does not produce a diagnosable error — the recompiler happily emits a `recomp_entrypoint`
that jumps into the wrong place.

## Memory

* **4 MiB RDRAM. No Expansion Pak.** The game's own heap sizing bakes this in: `_uvMemAllocInit`
  (`0x80002A88`) computes the heap top as `0x80400000 - gMemBlock`, and `func_80005074`
  bounds-checks pointers against the same constant.
* **No TLB-mapped code.** Everything is flat KSEG0, which removes an entire class of problem that
  makes some N64 titles hard to recompile.
* The port backs **8 MiB** of RDRAM regardless, so `[0x80400000, 0x80800000)` is unused
  Expansion-Pak space. Raising both of the constants above to `0x80800000` is the single change
  that fixes the `_uvMemAlloc` out-of-memory failure on Coventry Cove in a player race — see
  [07 — rule A](07-codegen-fixups-and-patches.md#rule-a--heap-cap).
* Relocatable modules are placed by the decomp's recomp ELF at **`0x80800000 + id × 0x100000`**.
  That window is above both the game image and the extended heap, which is why the patch linker
  script ([07](07-codegen-fixups-and-patches.md#the-patch-linker-script)) has to load patches at
  `0x8F000000` rather than anywhere near `0x80000000`.

## Hardware the port has to model

| Subsystem | What BAR uses | What the port does |
|---|---|---|
| **Graphics microcode** | Stock `gspF3DEX2_fifo` (decomp `gfx_ucode: f3dex2`). *Not* a custom Paradigm ucode. | RT64 handles F3DEX2 at HLE; nothing is recompiled |
| **Audio microcode** | Stock libultra `aspMain`, **version 2.0I** (reference RSPRecomp configs are 2.0J/L) | Recompiled by RSPRecomp into `rsp/aspMain.cpp` |
| **Audio library** | libultra `alAudioFrame` / `alSeqFileNew` / ALSndPlayer, wrapped in Paradigm's UV middleware (`uvaudiomgr_rom`, `uvcmidi_rom`) | Runs recompiled; the host supplies AI register semantics |
| **Saves** | **Controller Pak only** (`osPfs*`). No EEPROM, SRAM or Flash. | Emulated at the joybus level; 32 KiB per-port `mempak_pN.pak` |
| **Rumble** | Rumble Pak over the same joybus path | Served alongside the Controller Pak from one port |
| **Controller** | The **low-level** SI/PIF path — `osContStartReadData` → `__osPackReadData` → `__osSiRawStartDma`, not the high-level API the runtime reimplements | The host implements `__osSiRawStartDma` itself; see [05](05-runtime-host.md#input--the-low-level-sipif-path) |

The controller row is the one that surprises people. Most recomp runtimes assume a game calls
`osContStartReadData` and reimplement it natively. BAR reaches the raw SI DMA, so
`ultramodern`'s stub is reached, and if you leave it a stub the game deadlocks waiting for the
SI-complete event *and* reads the `0xFFFF` placeholder as "every button held".

## The frame clock

BAR carries its own minimum-frame-length control, `D_8001F7C0`, applied in `_uvScDoneGfx`
(`lib/bar-decomp/src/sched.c`):

```c
if (D_8001F7C4 < D_8001F7C0) { D_8002F250 = (D_8001F7C0 - D_8001F7C4) + 1; }  // defer the swap
else                         { D_8002F250 = 1; osViSwapBuffer(...); }         // swap now
```

`D_8001F7C4` counts retraces since the last frame was released. **The field is one less than the
frame length in fields** — it ships as `0`, so a frame lasts one field (60 Hz) and nothing is ever
deferred. Writing `1` gives 30 Hz; `2` gives 20 Hz.

Two properties make this the game's own correct pacing knob, and one property makes it insufficient
on its own:

* It gates **only** the gfx completion path. `_uvScDoneAud` is separate, so audio buffer generation
  and the MIDI sequencer's tempo keep full rate. (Contrast with capping VI retrace delivery, which
  halves the audio engine along with everything else and produces half-tempo music.)
* It defers the **buffer swap**, not the simulation. The game keeps stepping logic at full rate and
  simply presents fewer frames.

That distinction is the heart of the still-open "intro runs too fast" issue: on hardware the
console's heavier RCP gave the attract sequence roughly 30 Hz, and the port completes its tasks fast
enough to give it 60. Neither the retrace divider (slows everything, wrecks audio) nor the frame
limiter (keeps audio, does not slow the simulation) is the right lever. The full investigation,
including eight refuted hypotheses that should not be re-run, is in
[`../KNOWN_ISSUES.md`](../KNOWN_ISSUES.md).

## Screen-flow state

Two globals drive the whole screen state machine: `gGameStateFlag`, and
`gGameSettings[0].gameStateFlag`. `gGameSettings` is at **`0x80025CF0`** (base-game address, plain
RDRAM, directly readable host-side), with these fields measured by instrumenting the SI poll:

| Offset | Field | Notes |
|---|---|---|
| `+0x08` | `gameStateFlag` | Writing this drives the main loop's `uvSetGameState`; `BAR_FORCESTATE` uses it |
| `+0x88` | `raceState` | Read by the game as an **s16**, i.e. the word's *high half* — values arriving host-side as `0x10000`/`0x30000` are phases 1 and 3 |
| `+0xA4` | `currentGameState` | `2` = boot attract, `5` = race (including load screens), `14` = Controller Pak prompts **and** every front-end menu |

`currentGameState == 14` covering both the pak prompts and the menus is a real trap: the state
cannot distinguish them, so headless verification of "did we reach the menu" has to be done by
screenshot, not by state. Likewise `currentGameState == 5` is not "a race" — see
[06 — HUD anchoring](06-graphics.md#anchoring-the-hud-to-a-widened-frame) for the phase table that
actually separates the loading card, the countdown and the race.

## Why BAR is a good recompilation target

Worth stating explicitly, because these are the properties to check before starting a port of any
other title:

* **Stock graphics microcode.** A custom ucode means either extending the renderer's HLE or
  recompiling the ucode with RSPRecomp and running it against an LLE path. F3DEX2 is supported
  directly.
* **No TLB.** Flat KSEG0 addressing means recompiled code can index RDRAM directly.
* **One save medium, and a simple one.** A Controller Pak is 32 KiB of RAM behind a documented
  joybus protocol. Flash and EEPROM emulation would each have been more work.
* **A byte-matching decompilation already exists.** Not a prerequisite for recompilation, but it
  is the source of the symbol-rich ELF and the module layout — see [03](03-decompilation.md).

The one property that made it *hard* is the module system: ~133 relocatable overlays that stream in
and out during a race. That is the subject of [02](02-engine-and-modules.md).
