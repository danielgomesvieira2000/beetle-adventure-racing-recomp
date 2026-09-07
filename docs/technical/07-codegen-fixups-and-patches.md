# 07 — Codegen fixups and MIPS patches

There are exactly two places a change to the game's behaviour may live, and choosing the wrong one is
the most consequential decision in this part of the codebase.

| Layer | Binds to | Rots when | Use for |
|---|---|---|---|
| **`scripts/fix-recompiled.sh`** | N64 **instruction addresses** in generated C | The decomp ELF moves | Things that cannot be expressed as a named function override, and bring-up shortcuts |
| **`patches/*.c`** (`RECOMP_PATCH` / `RECOMP_HOOK`) | **Symbol names** | Never | Everything else |

Patches are strictly better and the remaining fixup rules should migrate to them where they can. Two
cannot: rule (F) applies to *every* recompiled function body rather than a named function, and rule
(C) targets RSPRecomp's output rather than the game.

---

## Part 1 — `scripts/fix-recompiled.sh`

Run **after** `./N64Recomp beetle-adventure-racing-recomp.toml` and **before** building. It is not
optional and it is not idempotent by accident — it is idempotent by design, which is exactly why it
needs the verification described below.

### Why it verifies itself

An anchor that stops matching does not fail loudly. The rule quietly applies to nothing, the build
succeeds, and the game misbehaves in a way that **looks nothing like "a patch went missing"** — an
out-of-memory on one track, dead controllers, audio crackle.

Counting changes cannot detect this, because the rules are idempotent: a second run legitimately
changes nothing, so "changed 0 files" is not a failure signal. **Every rule therefore asserts its
sentinel is present in the tree afterwards**, which holds whether it applied this run or a previous
one. A failed assertion stops the build and names the anchor that went missing.

`FIXRECOMP_STRICT=0` downgrades failures to warnings. That is for deliberate regeneration against a
changed decomp, never for a normal build.

### Codegen fixups

These are mechanical and address-independent; they cannot rot the same way.

**`$zero` loads.** A load into `$zero` (`lw $zero, ...`) is emitted as `0 = MEM_W(...)`, which is not
valid C. Rewritten to `(void)MEM_W(...)` so the read still happens for its side effects. Applied to
`RecompiledFuncs/*.c` and, as rule (C), to `rsp/aspMain.cpp`.

**Empty reloc types.** `recomp_overlays.inl` emits some relocations with an empty `.type` field —
MIPS reloc types beyond N64Recomp's `reloc_names` table. librecomp's `RelocEntryType` only covers
0..7 and the recompiled code resolves addresses inline, so they are inert for running the base game.
Rewritten to `R_MIPS_NONE` so the table is valid C++.

**Symbol-ownership renames.** Two wrappers in `src/main/` need to own symbols the generator also
defines, so the generated definition is renamed and the wrapper takes the name every caller binds to:

* `uvDoModuleRelocs` → `uvDoModuleRelocs_orig` (the [overlay bridge](04-static-recompilation.md#the-overlay-bridge))
* `func_8000E460` → `func_8000E460__hwstub_orig` (the [hardware-register stub](04-static-recompilation.md#hardware-register-stubs))

To add another hardware stub, add its name to the `for fn in ...` list **and** write the wrapper in
`hw_stubs.cpp` in the same change; the script verifies the rename happened.

### The behavioural rules

#### Rule A — heap cap

`_uvMemAllocInit` (`0x80002A88`, patched at `0x80002AA4`) sizes the heap as `0x80400000 - gMemBlock`,
and `func_80005074` (patched at `0x800050B0`) bounds-checks pointers against the same top. BAR is a
4 MiB game but the recomp backs 8 MiB, so `[0x80400000, 0x80800000)` is unused Expansion-Pak space.
Both constants become `0x80800000`.

Fixes the `_uvMemAlloc` out-of-memory failure in a player race on Coventry Cove. The gfx-manager
framebuffer reserve is left alone, so this only *adds* the upper 4 MiB above the framebuffers.

#### Rule B — removed

Was: clamp `__osMaxControllers` inside `__osPackReadData` (`func_8000E6E0`, at `0x8000E718`).

It is redundant. `src/main/os_unimpl_stubs.cpp` writes `4` to that byte (`0x80032231`) on every
`__osSiRawStartDma`, which happens during `osContInit`'s SI operations — before the first read-button
pack — so the clamp never had anything left to do. Verified by removing the injected line and
rebuilding: the game still takes input and reaches a race.

**Kept here as a worked example of the right trade.** The host-side write binds to a fixed RDRAM
address that is part of the game's data layout; the rule bound to an instruction address in generated
C. One rule fewer, and a more durable binding.

#### Rule C — RSP `$zero` loads

The same codegen quirk in `rsp/aspMain.cpp`, RSPRecomp's output.

#### Rule D — attract intro skip

`func_intro_004005CC` already skips the attract on START (`0x1000`) or A (`0x8000`). The game reads
input at `0x81D00618`; a B (`0x4000`) check is injected right after so B also skips.

#### Rule E — MIDI stop-wait yield

`func_uvcmidi_rom_00400940` stops the MIDI sequence player then spins until it reports stopped, or
2.0 s. The player's state only advances when the **audio thread** runs, and this spin never yields —
so under the cooperative scheduler it burns the full timeout on **every** state transition and the
host audio queue drains. A 1 ms yield is injected at the loop top (label `L_85600984`).

#### Rule F — cooperative preemption

The general form of rule E. A poll of a host-set should-yield flag is injected at **every** function
prologue, anchored on `int c1cs = 0;` — a line emitted once in every recompiled function body,
absent from forward declarations and from `rsp/aspMain.cpp`, so the RSP task thread is never touched.
3,147 injection sites; the rule requires at least 1,000 to pass verification.

Design, rationale and the priority gate are in
[05 — Cooperative preemption](05-runtime-host.md#cooperative-preemption).

**This rule cannot become a patch**, since it applies to every function body rather than to a named
function.

#### Rule G — SP_STATUS hardware-write guard

`_uvScDlistRecover`'s display-list overflow recovery path writes `0x2902` to `SP_STATUS_REG`
(`0xA4040010`) to reset the RSP, at the store at `0x80004024`. Under RT64 HLE there is no real RSP
and the RCP register range is not backed by the RDRAM buffer, so the raw store faults whenever
recovery runs. The store is guarded:

```c
if (((uint32_t)ctx->r15 & 0xFF000000u) != 0xA4000000u) MEM_W(0X10, ctx->r15) = ctx->r14;
```

Normal RDRAM stores through the same instruction are unaffected.

#### Rule H — frustum draw distance and culling width

Calls `bar_frustum_adjust` from the top of `func_uvfmtx_rom_00401F74`, handing it the six arguments
by pointer and writing back what it changed. The argument slots are the MIPS o32 layout the function
is compiled with: `a1..a3` are left, right, top, and bottom/near/far are at `0x10`/`0x14`/`0x18` of
the caller's stack — read from the same places the body is about to load them from.

Two things make this rule notable:

* **It is anchored on the function name, not an instruction address**, so unlike the rules above it
  cannot rot when the ELF moves. If you must add a fixup rule, anchor it this way.
* **It carries a verification that the hook is live, not merely present.** An earlier version
  collapsed the multi-line injection into one line, whose leading `//` then commented out the whole
  hook. It compiled, the sentinel was present, the rule reported `ok`, and the hook did nothing. The
  rule now asserts that the declaration and the call land on **separate lines** — `bar_frustum_adjust`
  must appear at least twice, and the call form specifically at least once.

That failure mode generalises: **a sentinel that can appear inside a comment is not a verification.**

The semantics of what the hook does are in
[06 — The frustum BAR draws and culls against](06-graphics.md#the-frustum-bar-draws-and-culls-against).

---

## Part 2 — MIPS patches

Patches are C cross-compiled to MIPS that override (`RECOMP_PATCH`) or hook (`RECOMP_HOOK`) game
functions **by name**. They are recompiled in single-file mode and linked so that the same-named
function takes priority over the recompiler's output.

### The toolchain, and why it needs WSL on Windows

Patches need an LLVM built with the **MIPS backend**. The Windows LLVM used for the host build does
not have one — `llc --version` lists no MIPS target and `-target mips` fails outright with
"No available targets are compatible with triple mips". That is not a flags problem.

Ubuntu's packaged clang carries every target, so `scripts/build-patches.sh` re-enters WSL and builds
there. **`clang-18` specifically**: `BUILDING.md` pins LLVM 18.1.8 because 19.x is documented as
miscompiling MIPS, and Ubuntu ships precisely 18.1.8 as `clang-18`. Apple Clang cannot target MIPS at
all, so macOS needs Homebrew LLVM 18.x for this step.

```bash
sudo apt install -y clang-18 lld-18      # one-time, inside WSL
scripts/build-patches.sh                 # patches/*.c -> patches/patches.elf
```

Compiler flags mirror the decomp's own:

```
-target mips -mips2 -mabi=32 -O2 -G0 -mno-abicalls -mno-odd-spreg
-mno-check-zero-division -fomit-frame-pointer -ffast-math
-fno-unsafe-math-optimizations -fno-builtin-memset
```

and the include set has to mirror the decomp's too — `include` alone is not enough, because
`ultra64.h` and the libultra headers live under `include/2.0I` and `include/2.0I/PR` and the decomp's
sources include them unqualified.

### The patch linker script

```
.text 0x8F000000 : { *(.text) *(.text.*) }
```

The load address must not collide with anything the game occupies, because **patch symbols keep these
addresses when the recompiler resolves relocations**:

* `0x80000400` — the game entry point
* `0x80800000` upwards — the ~133 relocatable modules

`0x80000000`, which this script originally specified, sits 1 KiB *below* the entry point, so `.text`
grew straight into the game's own code. `0x8F000000` is still KSEG0 and clear of both.

### Reference symbols

The recompiler needs symbol metadata to resolve patch relocations against the game.
`tools/gen_reference_syms.py` derives it from the decomp's `recomp.elf` (135 executable sections /
3,253 functions, plus data symbols):

```bash
wsl -d Ubuntu -- python3 tools/gen_reference_syms.py elf/recomp.elf \
    syms/beetleadventurerac.us.syms.toml syms/beetleadventurerac.us.datasyms.toml
```

**Two shapes are easy to confuse**: executable sections use `functions = [{ name, vram, size }]`;
data sections use `symbols = [{ name, vram }]`.

Custom symbols defined by the patches themselves go in `patches/custom_syms.toml`, which is parsed by
the same reader — so symbols must be grouped into **sections**. A bare list of `[[symbol]]` entries
is rejected with "Invalid symbols array", and so is a file with **no** sections, which is why that
file is currently not referenced from `patches.toml`.

```toml
[[section]]
name    = "custom"
rom     = 0x0
vram    = 0x81000000
size    = 0x1000
symbols = [ { name = "my_custom_global", vram = 0x81000000 } ]
```

### `patches.toml`

```toml
[input]
elf_path                 = "patches/patches.elf"
output_func_path         = "RecompiledPatches"
single_file_output       = true
use_absolute_symbols     = true
func_reference_syms_file = "syms/beetleadventurerac.us.syms.toml"
data_reference_syms_files = [ "syms/beetleadventurerac.us.datasyms.toml" ]
output_binary_path       = "patches/patches.bin"   # so the recompiler can fix MIPS32 relocs itself
unpaired_lo16_warnings   = false                   # clang emits many unpaired LO16s
allow_exports            = true
strict_patch_mode        = true                    # patched symbols must exist; non-patch funcs must not be symbols
```

### The linker trap — how a patch silently fails to replace anything

**This is the single most important thing in this chapter.**

N64Recomp does **not** remove a patched function from `RecompiledFuncs`. The patch simply carries the
same C name and the **linker** has to prefer it. Under `clang-cl`, `RECOMP_FUNC` expands to a **weak**
definition for the originals and the patches alike — and with two weak definitions, the linker keeps
whichever it meets first.

The first attempt built patches into a static `PatchesLib`. **A static library only contributes
members for symbols that are still undefined when it is scanned**, and by then the `RecompiledFuncs`
object holding the original had already been pulled in for some other function. So the original won
every time. The viewport patch linked cleanly, reported no error, and changed nothing — which is how
this was found.

The fix: `RecompiledPatches/patches.c` is compiled **directly into the executable**. An object file's
definitions are seen before any archive is opened, so the patch is the first definition and wins.
`wave-race-64-recomp` does the same thing for the same reason.

CMake wires it conditionally, so a fresh clone still builds before anyone has installed a MIPS
toolchain:

```cmake
if (EXISTS ${CMAKE_SOURCE_DIR}/RecompiledPatches/patches.c)
    set(BEETLE_PATCH_SOURCE ${CMAKE_SOURCE_DIR}/RecompiledPatches/patches.c)
endif()
# ... later, after add_executable():
if (BEETLE_PATCH_SOURCE)
    target_sources(beetle-adventure-racing-recomp PRIVATE ${BEETLE_PATCH_SOURCE})
endif()
```

Note the consequence for iteration: `patches.c` is globbed at configure time, so **re-run
`cmake -S . -B <builddir>` once** after it first appears.

### Full patch build sequence

```bash
scripts/build-patches.sh                 # patches/*.c -> patches/patches.elf (in WSL)
wsl -d Ubuntu -- python3 tools/gen_reference_syms.py elf/recomp.elf \
    syms/beetleadventurerac.us.syms.toml syms/beetleadventurerac.us.datasyms.toml
./N64Recomp patches.toml                 # patches.elf -> RecompiledPatches/patches.c
cmake -S . -B build-frontend             # once, after patches.c first appears
cmake --build build-frontend -j
```

### Rules for writing a patch

From `patches/viewport_patch.c`, the first real one:

* **Avoid data symbols in relocatable modules.** The module is relocatable at runtime, so a patch
  baking absolute addresses could read or write the wrong memory. Reach state through the module's
  own exported getters instead.
* **Copy the original body verbatim and make one change.** It makes the diff reviewable and it makes
  a regression attributable.
* **Leave alone anything a caller will rewrite anyway.** The viewport patch does not write the four
  "last rectangle" globals, because the caller invokes the function that stores the same values a
  moment later.
* **Check the coordinate convention twice.** See the bottom-up Y trap in
  [06](06-graphics.md#the-overscan-inset-and-the-patch-that-removes-it).
