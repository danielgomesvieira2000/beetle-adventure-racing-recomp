# 03 — Decompiling BAR

This chapter is for someone starting or resuming the **matching decompilation**. It covers the
toolchain, the module build, the operating strategy that has actually produced results here, and —
importantly for porters — exactly which parts of the decomp the recompilation consumes.

## Lineage

| Project | What it is |
|---|---|
| [`synamaxmusic/bar-notes`](https://github.com/synamaxmusic/bar-notes) | The original reverse-engineering notes |
| [`synamaxmusic/bar-decomp`](https://github.com/synamaxmusic/bar-decomp) | The original decompilation: splat configuration, module/overlay build system, symbol map, IDO setup. Module-system work by **LLONSIT** |
| [`bryankruman/BeetleDecomp`](https://github.com/bryankruman/BeetleDecomp) | The fork this port consumes, vendored here at `lib/bar-decomp` |

Everything downstream — this port included — depends on that splat configuration and symbol map.

## Toolchain

| Component | Value | Note |
|---|---|---|
| Host | Linux, or **WSL2**. The decomp does **not** build natively on Windows | Ubuntu 24.04 is upstream's recommendation; this project builds on 26.04 |
| Compiler | **IDO 5.3** for modules | See the correction below — this was documented wrong for a long time |
| Splat | Segment/asset extraction and the linker scripts | |
| ROM | `baserom.us.z64` in the decomp root | Never committed |
| Targets | `make extract`, `make -j6` (must print `build/beetleadventurerac.us.z64: OK`), `make recomp` → `build/recomp.elf` | |

### The IDO version correction

The modules compile with **IDO 5.3, not 7.1.** The decomp `Makefile:412` overrides `CC` to
asm-processor + `$(IDO)` = `build/5.3/out/cc`; the 7.1 assignment at line 82 is dead for modules.
Every seed prompt, `permuter_settings.toml` and workflow doc that said 7.1 has been corrected. The
byte gate never let a wrong match through because of it, but it mattered for reasoning about
compiler idioms — and a prompt that lies to a model costs matches.

### Binutils

Upstream warns that distributions older than 22.04 (binutils 2.34) miscompile the ELF→ROM step on
the TLS `.tdata` section. Newer is fine; if a plausible-looking build fails its final checksum,
binutils is nonetheless a reasonable suspect.

## Ground rules

* **A C function body means byte-matched.** A `#pragma GLOBAL_ASM(...)` line means still raw
  assembly. The pragma scan is the **only** source of truth for match state — ledgers and issue
  lists drift, and did: `nomatch.json` once listed 92 functions of which 5 had already been banked,
  and a session note still described a long-banked function as "the near-miss target".
* **A win does not exist until it survives the module hash.** A permuter "score 0" is judged in its
  own flattened translation unit; the module build can disagree, because different visible
  declarations produce different codegen. Historically only about half of permuter wins transfer.
* **Changes never go in the decomp.** Keep it a faithful mirror of the original. Modifications
  belong in the recomp's `patches/` and `src/` layers — see [07](07-codegen-fixups-and-patches.md).

## The three-phase strategy

This is the canon, adopted after a review of the campaign data. The operational runbook lives in the
decomp repo (`docs/GRIND_PLAYBOOK.md`), with current state in `RESUME_GRIND.md`; the review that
produced it is [`../DECOMP_STRATEGY.md`](../DECOMP_STRATEGY.md).

### Phase 1 — MATCH, cheaply, at volume

```
refresh pools from live #pragma GLOBAL_ASM scan
→ gen_cards2.py        context-aware m2c cards
→ cheap-model seed wave (placeholder conventions, medium effort)
→ consolidate2.py      per-seed byte gate, accumulating
→ capture_builderr → fix_wave   (cheap model with the exact error; one expensive escalation, then park)
→ permute_campaign2.py fresh imports, budget ladder, gate on every zero, rehunt transfer-fails
→ bank_wins.py         accumulation gate + full-ROM SHA + ledger/pool refresh + commit
→ sweep_wins.py        safety net after any permuter activity
```

Placeholder conventions matter more than they look: `unk<hexoffset>` shims and `Node_<ADDR>` structs
hit roughly **25% direct match on functions ≤200 bytes**, against 7–10% for a
polish-while-matching workflow, at far fewer tokens per function.

### Phase 2 — SCRIBE, per module

Promote verified structs and signatures into shared headers, dedupe shim typedefs, name locals,
comment behaviour.

**This is not cosmetic, which is why it interleaves rather than waiting.** Correct struct and
prototype context *changes codegen* — a wrong prototype produces wrong call-site codegen, e.g. float
promotion — so scribing a mostly-matched module raises the match rate on its remaining tail. Since
m2c now consumes the module context automatically, phase-2 work feeds directly back into phase-1
seed quality.

Priority order: `uvgfxmgr_rom` → `uvchannel_rom` → `uvsprt_rom` → the central export tables →
per-screen modules.

### Phase 3 — NAME, last

Semantic renames of `func_*` / `D_*`, repo-wide across C, headers, remaining `.s`, the ledger and
`SYMBOLS.md`, with a full-ROM SHA per batch. Renames never change codegen but touch everything,
which is exactly why they go last — when the fewest `.s` references remain.

## Permuter economics

The permuter is **CPU, not tokens**, and that shapes everything:

* It **cannot invent control flow.** It mutates a compiling, structurally-close seed. "Lean into the
  permuter" therefore means: cheap seed waves exist to *feed* it, it is the primary matcher, and
  expensive models are reserved for (a) fixing compile errors and (b) the few functions the permuter
  *proves* are close-but-stuck.
* **Budgets should be a ladder, not uniform.** The data says it cracks mostly `base < 400` and that
  long budgets only pay on near-misses. Every function gets a cheap round (300 s); survivors are
  ranked by best score and halved each round (900 s → 1800 s → …). Overnight compute is free;
  tokens are not.
* **Sweep for unbanked wins.** A report-only sweep of 392 permuter import directories found 41
  score-0 outputs — 27 already banked, 2 build errors from context drift, 4 genuine transfer fails,
  and **8 fully bankable functions sitting on disk**, some for days. Run `sweep_wins.py` after any
  permuter activity.
* **Before escalating to an expensive model**, try a PERM-macro annotation pass (`PERM_GENERAL`,
  `PERM_VAR`, …) to direct the permuter's search. That is cheap-model work. When you do escalate,
  always include the permuter's best-output diff in the prompt.

## What the recompilation actually takes from the decomp

Worth being precise about, because the relationship is widely misunderstood:

**The recomp does not compile the decomp's C.** Static recompilation translates the original ROM's
MIPS *machine code* into C automatically. The decomp's hand-written C is never built into the port.

| From the decomp | Used for |
|---|---|
| The **symbol-rich ELF** that rebuilds the ROM byte-for-byte (`make recomp` → `build/recomp.elf`) | The input to N64Recomp |
| **Headers and symbol tables** | Readable names and typed interfaces for generated code and for MIPS patches |
| **Segmentation / module + reloc layout** | Overlay handling for the ~133 relocatable modules |

Because the decomp is byte-matching, **matching progress does not change the port**: recompiling a
function produces identical output whether it started as hand-written C or raw assembly. What flows
downstream from decomp work is better **names and types** — which is readability of generated code
and of patches — and the **module layout** that makes the port tractable at all.

Practical consequence: you do **not** need a finished decompilation to build a port. You need the
ELF and the module layout. A porter can therefore start today and let the decomp's naming catch up.

## The recomp ELF

`make recomp` produces a per-module relocatable ELF: the base game, plus one relocatable section per
module at `0x80800000 + id × 0x100000`, plus MIPS relocations. As of the recorded baseline it
carries **133 module sections, 135 `.rel.*` sections and 665 `__recomp_*` symbols**, and N64Recomp
turns it into **22,246 functions**.

Bring it into the port with `scripts/fetch-elf.sh` (or `.ps1`), which streams it out of the WSL
checkout. Override the defaults for your environment:

```bash
WSL_DISTRO=Ubuntu DECOMP_DIR=/home/you/projects/bar-decomp scripts/fetch-elf.sh
```

## Where to look next

* Module inventory with per-function match state and modification hooks:
  [`../HUD_MENU_SYSTEM_MAP.md`](../HUD_MENU_SYSTEM_MAP.md)
* The writing workflow (scribe track and matching track):
  [`../HUD_MENU_WORKFLOW.md`](../HUD_MENU_WORKFLOW.md)
* The strategy review with the evidence behind the rules above:
  [`../DECOMP_STRATEGY.md`](../DECOMP_STRATEGY.md)
