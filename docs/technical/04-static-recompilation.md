# 04 — Static recompilation

Static recompilation translates the ROM's MIPS machine code into C, once, ahead of time, then links
that C against a modern runtime. It is not emulation and not a manual rewrite. This chapter covers
the [N64Recomp](https://github.com/N64Recomp/N64Recomp) side: configuration, what it emits, and the
two pieces of glue BAR needs that a simpler game would not.

## The pipeline

```
your ROM ─┐
          ├─► BeetleDecomp (WSL) ──► symbol-rich ELF ──► N64Recomp ──► RecompiledFuncs/*.c ─┐
recomp.ld ┘   (per-module layout)     (elf/recomp.elf)   (…-recomp.toml)                    │
                                                                                            ├─► CMake ─► exe
 patches/*.c ─► clang -target mips ─► patches.elf ─► N64Recomp (patches.toml) ─► RecompiledPatches/patches.c ┘
                                                            │
            runtime: librecomp + ultramodern + RT64 ────────┘
```

Ordered build steps, each depending on the last:

1. `git clone` (there are no submodules; every library is committed under `lib/`)
2. Build the recompiler → `N64Recomp.exe`, `RSPRecomp.exe` at the repo root (`scripts/setup.sh` / `.ps1`)
3. (WSL) `make extract && make -j6` in the decomp — must print `build/beetleadventurerac.us.z64: OK`
4. (WSL) `make recomp` → `build/recomp.elf`
5. `scripts/fetch-elf.sh` → `elf/recomp.elf`
6. `./N64Recomp beetle-adventure-racing-recomp.toml` → `RecompiledFuncs/*.c`
7. **`scripts/fix-recompiled.sh` — mandatory, and re-run after every regeneration** ([07](07-codegen-fixups-and-patches.md))
8. `cmake -S . -B build-cmake -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release`
9. `cmake --build build-cmake -j`

`RecompiledFuncs/` and `RecompiledPatches/` are **generated and git-ignored**. Never edit them by
hand — any change belongs in `fix-recompiled.sh` (address-anchored) or in `patches/` (name-anchored).

## The main configuration

`beetle-adventure-racing-recomp.toml`:

```toml
[input]
entrypoint                = 0x80000400        # ROM header +0x8, and the splat `entry` segment vram
elf_path                  = "elf/recomp.elf"  # per-module ELF from the decomp's `make recomp`
output_func_path          = "RecompiledFuncs" # one C file per function
relocatable_sections_path = "overlays.us.txt" # the 133 .<module> sections, one per line

[patches]
stubs   = []   # functions to emit as empty bodies
ignored = []   # symbols that look like functions but are data
```

Three fields deserve comment.

**`relocatable_sections_path`** is what turns the module sections into *overlays* rather than
ordinary code. Order is significant: N64Recomp assigns overlay ids by position in this file, and
`src/main/overlay_bridge.cpp` indexes its `nameTag` table by the same ordering. Regenerate with
`scripts/gen-overlays.sh` and review the output before using it.

**`stubs`** and **`ignored`** are both empty here, deliberately. The functions that should have been
stubbed — libultra routines that poke RCP registers — are instead handled by renaming the generated
definition and wrapping it in `src/main/hw_stubs.cpp`. That was a bring-up shortcut; the cleaner
long-term form is to list them here, or instruction-patch the offending accesses, and regenerate.

**An alternative input** exists and is commented out in the file: a decompressed ROM plus a symbols
TOML (`rom_file_path` + `symbols_file_path`). That is the path to take if you have no decomp at all —
you supply the symbol table yourself.

## What N64Recomp emits

* `RecompiledFuncs/funcs_N.c` — the recompiled functions, chunked. Each function is
  `RECOMP_FUNC void <name>(uint8_t* rdram, recomp_context* ctx)`, with the original instruction
  addresses in `// 0x........:` comments. Those comments are the anchors the fixup rules key on.
* `RecompiledFuncs/recomp_overlays.inl` — the section and overlay tables, as file-static data:

```c
static SectionTableEntry section_table[];   // one entry per CODE section (base + ~130 overlays)
const  size_t num_sections;                 // TOTAL ELF sections INCLUDING data (BAR: 413)
static int overlay_sections_by_index[];     // overlay index -> code-section position
```

### The section-count trap

`num_sections` is **413** while `ARRLEN(section_table)` is the code-section count. librecomp keys its
relocation and section bookkeeping by *section index*, and relocations target indices well past the
code sections, so `init_overlays()` must allocate `section_addresses[total_num_sections]` using
`num_sections`. Getting this wrong produces out-of-range writes during module relocation, which
present as garbage data pointers — not as an obvious index error.

`src/main/register_overlays.cpp` is the entire wiring:

```cpp
recomp::overlays::overlay_section_table_data_t sections{};
sections.code_sections     = section_table;
sections.num_code_sections = ARRLEN(section_table);
sections.total_num_sections = num_sections;      // NOT ARRLEN(section_table)

recomp::overlays::overlays_by_index_t overlays{};
overlays.table = overlay_sections_by_index;
overlays.len   = ARRLEN(overlay_sections_by_index);

recomp::overlays::register_overlays(sections, overlays);
```

Call it from `main()` before `recomp::start()`.

## The overlay bridge

**This is the piece that made the port possible**, and the piece a porter of another
module-heavy title will have to write from scratch.

The problem, restated from [02](02-engine-and-modules.md): BAR loads a module into a heap buffer and
jumps to its entry point. The module's recompiled functions are keyed in `func_map` by their
recomp-ELF VRAM (`0x80800000 + id × 0x100000`), not by the heap address — so the entry-point
`LOOKUP_FUNC` fails with `Failed to find function at 0x...`.

The fix intercepts `uvDoModuleRelocs`, which the game calls with `a0 = ovlStartPtr` and
`a1 = ModuleCommInfo*` immediately before the entry jump:

```cpp
extern "C" void uvDoModuleRelocs(uint8_t* rdram, recomp_context* ctx) {
    const gpr ovl_start = ctx->r4;                              // a0
    const gpr info_addr = ctx->r5;                              // a1
    const uint32_t name_tag = (uint32_t)MEM_W(0x1C, info_addr); // ModuleCommInfo.nameTag

    const int overlay_id = overlay_id_for_tag(name_tag);
    if (overlay_id >= 0) {
        unload_overlay_by_id((uint32_t)overlay_id);             // so address reuse re-registers cleanly
        load_overlay_by_id((uint32_t)overlay_id, (uint32_t)ovl_start);
    }
    uvDoModuleRelocs_orig(rdram, ctx);   // then the game's own byte relocation
}
```

Four things are load-bearing:

* **`load_overlay_by_id` / `unload_overlay_by_id` are `extern "C"` in librecomp's
  `src/overlays.cpp` and are not in the public header.** Declare them yourself.
* **The section base is `ovlStartPtr`**, the code start. Registering at
  `ovlStartPtr - exportsSize` breaks function resolution — established by experiment.
* **Unload before load.** Modules stream continuously and addresses are reused.
* **The original relocation still has to run.** It fixes the data pointers the recompiled code
  reads. The wrapper adds registration; it does not replace anything.

Owning the `uvDoModuleRelocs` symbol requires renaming the generated definition, which
`scripts/fix-recompiled.sh` does (`uvDoModuleRelocs` → `uvDoModuleRelocs_orig`). All callers then
bind to the wrapper.

An unknown nameTag logs a loud unconditional warning — a module that fails to register produces
symptoms nowhere near the cause, so it must not be silent. The per-load success log is gated behind
`BAR_DEBUG_OVERLAYS` because during a race it is a measurable drag.

## Hardware-register stubs

A handful of libultra functions were recompiled raw (the decomp did not symbol-match them, so they
were never listed in `stubs`/`ignored`). They `MEM_W`/`MEM_B` KSEG1 register addresses —
`0xA4xxxxxx` for AI, PI and SP — which are not memory-mapped in the recomp and fault out of bounds.

`fix-recompiled.sh` renames each generated definition to `<name>__hwstub_orig`, and
`src/main/hw_stubs.cpp` owns the symbol every caller binds to. Currently one function is handled
this way, and it is a good illustration that a "stub" is sometimes not a stub at all:

**`func_8000E460` is `osAiGetLength()`**, which reads `AI_LEN_REG` (`0xA4500004`) — the remaining
audio DMA length in bytes. Returning `0` links fine and sounds catastrophic: the audio manager
(`uvaudiomgr_rom`) always believed the AI buffer was empty, synthesised the maximum samples every
video frame, and the host queue filled far faster than it drained, so the sound lagged further behind
the game every second. The stub returns `ultramodern::get_remaining_audio_bytes()`, which is already
in `AI_LEN_REG` byte units (`queued_frames × 4`); the game does `>> 2` to recover frames, so the
units match and the generator self-stabilises to a bounded ~one-VI queue.

**The general lesson**: a hardware register the game *reads* is usually a feedback signal. Stubbing
it to zero does not disable a feature, it removes a control loop's input.

To add another: put the name in the `for fn in ...` list in `fix-recompiled.sh` **and** write the
wrapper in `hw_stubs.cpp`, in the same change. The script verifies that the rename happened and
fails the build if it did not.

## The RSP audio microcode

The graphics microcode is F3DEX2 and RT64 handles it at HLE — nothing is recompiled. The **audio**
microcode is recompiled by RSPRecomp from `rsp/aspMain.bar.us.toml`:

```toml
text_offset  = 0x1F410
text_size    = 0xE20
text_address = 0x04001080
output_function_name = "aspMain"
extra_indirect_branch_targets = [ 0x1118, 0x1470, 0x11DC, 0x1B38, ... ]
```

`extra_indirect_branch_targets` is the interesting field. RSPRecomp needs every jump-table
destination declared, because it cannot discover them statically. This list was **seeded from the
stock libultra `aspMain` jump tables in the Dr. Mario 64 and Majora's Mask reference RSPRecomp
configs** — the stock `aspMain` text is shared across libultra titles, so those configs transfer.

They transfer *mostly*. BAR ships libultra **2.0I** against the references' 2.0J/L, and the
recompiled ucode hits an unhandled indirect-jump target on certain audio tasks — notably Beetle
Battle — returning `RspExitReason::UnhandledJumpTarget`. `run_task()` treats any non-`Broke` exit as
fatal and `quick_exit()`s the whole process, so a single bad audio task would take the game down.
`src/main/main.cpp` wraps `aspMain` so that degrades to silence for that one task:

```cpp
static RspExitReason bar_aspMain_guarded(uint8_t* rdram, uint32_t ucode_addr) {
    RspExitReason r = aspMain(rdram, ucode_addr);
    return (r == RspExitReason::Broke) ? r : /* report a normal break, task silently dropped */;
}
```

`BAR_NO_AUDIO` is the coarse escape hatch: it falls back to a silent stub ucode and opens no audio
device.

The generated `rsp/aspMain.cpp` also needs the `$zero`-load codegen fixup, which is why
`fix-recompiled.sh` rule (C) applies the same rewrite to it.

## Building the recompiler

`scripts/setup.sh` builds N64Recomp and RSPRecomp in
Release, and copies the executables to the repo root. N64Recomp builds natively on Windows now — it
does not need WSL, unlike the decomp and the MIPS patches.

**Pin your recompiler revision and record it.** `docs/PINNED_REVISIONS.md` documents a known
discrepancy: an inherited status doc names `ffb39cd` as verified and warns against `81213c1`, while
this tree pins `a940efa` (one release later). If recompiled output misbehaves in a way that smells
like codegen rather than game logic, checking out the older revision and regenerating is the first
thing to try.
