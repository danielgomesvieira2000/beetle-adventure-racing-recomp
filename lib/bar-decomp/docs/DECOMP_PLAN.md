# Decompilation Plan — road to 100%

This document is the strategy. For the operational per‑function loop see
[`AI_WORKFLOW.md`](AI_WORKFLOW.md).

## Definition of "done" (100%)

1. **Zero `GLOBAL_ASM`** — every function exists as C, not an included `.s` stub.
2. **All data typed in C** — no `D_XXXX` raw `.word` blobs; real structs/arrays/enums/strings.
3. **Byte‑exact** — `build/beetleadventurerac.us.z64` SHA‑1 = `e5ab4d22…` on every commit.
4. **Readable** — symbols named, types recovered, key subsystems commented.

## Current state (at fork)

| Metric | Count |
|---|---|
| Functions matched | ~769 (~26%) |
| **Code functions remaining** | **~2,419** (~262k instructions, median 48) |
| **Data/rodata symbols remaining** | **~4,561** |
| Relocatable modules/overlays | 134 |
| Unnamed symbols inside matched code | ~96 `func_*` + ~81 `D_*` |

Difficulty buckets of the remaining functions (from `score_functions.py`):
`~511 leaf+no‑float`, `~491 tiny (≤12 insns)`, `~948 small (≤30)`, `~1,336 no‑float`,
`~1,083 float‑using (hard)`, `~871 large (≥80)`.

## The five work tracks

### Track 1 — Function matching (the bulk)
Decompile each `nonmatching` function to byte‑exact C. Drive selection with `score_functions.py`
(easiest first). Tiered by difficulty → tiered by model (see AI_WORKFLOW). The float‑using (~1,083)
and large (~871) functions are the long tail and dominate effort.

### Track 2 — Data & rodata (~4,561 symbols)
Express data as typed C so IDO emits identical bytes (order, padding, alignment, `.rodata` float
constants, jump tables). Mostly discovered **while** decompiling the functions that consume it —
so Track 2 runs alongside Track 1, not after it.

### Track 3 — Modules / overlays (BAR's distinctive challenge)
Most remaining functions live in the **134 relocatable modules** (`ai`, `race`, `pause`, `gamegui`,
`uvmodel_rom`, …). Matching a module means matching its code **and** preserving its relocation table
and inter‑module export wiring, then re‑packing to the Paradigm `FORM0`/`.uvmo` container via
DaisyBox with a per‑module hash check (see [`build_system.md`](build_system.md)). The per‑module
hash is our **fast checkpoint** (Track‑by‑track verification without a full ROM rebuild).

### Track 4 — Naming & type recovery
Replace `func_*`/`D_*` with meaningful names; recover structs/enums (427 exist; many more needed).
Naming/comments never change codegen → always matching‑safe. Some data typing is *required* to match.

### Track 5 — libultra, asset formats, finalization
`src/libultra` (N64 SDK) can largely be cross‑referenced from existing OoT/SM64 libultra decomps
(same IDO toolchain). Asset/track/model formats (`uvmodel`, `uvterra`, `uvtexture`) get documented
and typed — not required for a matching code build, but needed for a clean PC port.

## Phased execution

The phases overlap; they describe emphasis, not hard gates.

| Phase | Emphasis | Model tier | Exit criterion |
|---|---|---|---|
| **0. Apparatus** | Stand up `score_functions.py`, `progress.py`, `check.sh`, the AI loop, objdiff/ccache | Opus to set up | Loop runs end‑to‑end on 1 function, verified |
| **1. Warm‑up** | ~500 leaf+no‑float + ~491 tiny functions | Haiku/Sonnet write → Opus spot‑audit | ≥40% functions matched |
| **2. Bulk** | ~948 small + remaining no‑float, per‑module | Sonnet write → Opus review | ≥70% functions; first fully‑matched modules |
| **3. Hard tail** | ~1,083 float + ~871 large functions | Opus (m2c+permuter assisted) | ≥95% functions |
| **4. Data & modules** | finish Track 2 typing + Track 3 relocation correctness | Sonnet write → Opus review | all 134 modules hash‑match |
| **5. Polish** | naming, types, comments, libultra, docs | Sonnet write → Opus review | 100% matching, readable |

## Verification apparatus (efficient, checkpoint‑based)

We do **not** full‑rebuild after every function. Cadence:

- **Per function:** `./diff.py -mwo <Func>` (or objdiff) — instant, no full build.
- **Per chunk (e.g. 5–15 functions in one module):** `scripts/check.sh --module <name>` — incremental
  `make` rebuilds only changed objects and re‑verifies that **one module's** hash. Seconds.
- **Per session / before commit:** `scripts/check.sh` — incremental full `make` + final ROM SHA‑1 +
  `progress.py` report.
- **Milestones:** commit, update progress, optionally publish to a decomp.dev‑style dashboard.

`scripts/check.sh` is the single entry point; `progress.py` reports matched/unmatched by module so we
can pick the next high‑value chunk.

## Tooling improvements adopted (vs. a vanilla setup)

Researched current best practice; adopting:

Status legend: ✅ done & tested · 🟡 partial · ⛔ deferred (with reason).

1. ✅ **m2c first‑pass** (vendored submodule, verified) — seeds the writer model and sometimes matches
   outright. Run before hand/AI work on each function.
2. ✅ **decomp‑permuter** (vendored, deps installed, imports OK) as a background stage — especially
   effective when seeded with the AI's output (empirically +7 matches when permuting AI code vs cold).
3. ✅ **`score_functions.py`** difficulty ranker — work the cheapest‑to‑match targets first and route
   functions to the right model tier.
4. ✅ **`tools/progress.py` (per‑module) + `mapfile_parser progress`/`objdiff_report` (byte‑accurate)
   + per‑module hash checkpoints** for fast, frequent verification.
5. ✅ **decomp.dev report** via `scripts/gen_report.sh` (objdiff‑report format) — **full coverage**:
   main segment + all 133 overlay modules, aggregated by `tools/aggregate_report.py` with a per‑module
   breakdown. See [`DECOMP_DEV.md`](DECOMP_DEV.md).
6. ✅ **Model‑tiered AI loop** (Kappa/Mizuchi‑style: m2c → compile → asm‑differ → permuter → AI → review).
7. 🟡 **Local diffing**: we use **asm‑differ** (`./diff.py`, vendored, works against the ROM+map).
   The **objdiff** GUI/CLI needs per‑object "expected" objects, which BAR doesn't currently emit —
   adopting it fully is a future task; its *report format* is already used for decomp.dev (#5).
8. ⛔ **ccache**: installed + configured, and auto‑accelerates the gcc C‑tool builds via
   `/usr/lib/ccache`. **Not** wired into the matching compile path: that path is
   `asm‑processor (Python) → IDO cc → as`, so ccache (a compiler wrapper) doesn't slot in cleanly, and
   incremental `make` already skips unchanged files — so the per‑function loop wouldn't benefit. Revisit
   only if full‑rebuild time becomes a pain point (then consider sccache or asm‑processor‑level caching).

## Effort & risk

- **Scale:** ~2,419 functions + ~4,561 data symbols + 134 modules. The easy/medium ~1,400 functions
  go fast with the loop; the ~1,950 hard functions, data typing, and module‑relocation correctness
  are the long tail. Realistically **many months** of focused work even with AI acceleration.
- **Top risks:** (a) float register allocation on IDO is the hardest match class — budget permuter +
  Opus time; (b) module relocation/export correctness is BAR‑specific and underdocumented — lean on
  `bar-notes` and `build_system.md`; (c) "cheating" matches that hit bytes but misrepresent intent —
  the Opus review pass exists to catch these.

## Relationship to a PC port
A matching decomp (this plan) unlocks a **decomp‑native** port (Ship‑of‑Harkinian style, the only
route that restores BAR's 4‑player Battle mode). A **static‑recompilation** port (N64Recomp + RT64)
needs *no* decomp and can proceed in parallel as a faster path to a playable single‑player build.
Tracking both is a goal; neither blocks the other.

## Cross-cutting: the knowledge base (Track 0)
Underpinning all tracks is a shared, in-repo knowledge base (typed headers + `docs/SYMBOLS.md`) that
agents READ before decompiling and SCRIBE discoveries back into. Code is commented. This is what
turns the hard logic/float tail from "every function re-derives the context and guesses wrong" into
a compounding effort where match rate climbs. See [`KNOWLEDGE_BASE.md`](KNOWLEDGE_BASE.md).
