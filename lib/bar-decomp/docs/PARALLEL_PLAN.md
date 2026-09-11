# Parallelized decompilation — wave plan (workflow-executed)

Decompilation parallelized into **waves**, each run as one `Workflow`. Maximizes concurrent
cheap-model work and routes by difficulty to the cheapest capable model.

## The constraint that shapes the design
- **Writing C is embarrassingly parallel** — different modules touch different files.
- **Building is a single shared serialization point** — one `make` verifies the whole ROM, and
  reports a pass/fail **per module** (each overlay has its own hash check). So we never build in
  parallel; we fan out writers, then do **one** build per wave that verifies everyone at once.

## Unit of parallel work = a MODULE
Each module is an independent `src/modules/<mod>.c` with its own hash check. Per-module agents can
build up a shared struct/externs (clean code) and never collide with each other.

## Per-wave workflow shape (3 phases)
1. **Decompile** (PARALLEL, small model — Haiku/Sonnet): one writer agent per module. It reads the
   module's unmatched `.s` (read-only), writes clean matching C (named structs/typed externs,
   register-width params per `docs/AI_WORKFLOW.md`), and returns a structured result. **Writers do
   not build** — zero shared-state conflict, so all run concurrently (cap ~16).
2. **Integrate** (SERIAL, one agent): applies every module's C, runs a single `make`, parses
   per-module hash + compile results, **reverts any module that fails to compile or match** (keeps
   the repo byte-matching/green), deletes matched `.s`, commits. Returns `{matched, failed}`.
3. **Repair** (PARALLEL, Opus): one agent per *failed* module — diffs with `objdump`/asm-differ,
   fixes the C, hands back to a final integrate. Hard cases get parked with notes.

Net: N modules written concurrently by cheap models, 1 build, failures repaired by Opus in parallel.

## Model tiering (cheapest capable, per our policy)
| Phase | Model | Why |
|---|---|---|
| Decompile (trivial: empties/return-const) | **Haiku 4.5** | pattern-level |
| Decompile (simple/medium) | **Sonnet 4.6** | ~74% first-pass match rate |
| Integrate | Sonnet (or scripted) | mechanical apply+build+revert |
| Repair / review | **Opus 4.8** | the hard tail + correctness audit |

## Waves (easiest first; each = one Workflow run, parameterized by module list + max-instructions)
- **Wave 0** — trivial patterns (empties, `return const`). *Mostly done (~125 fns).* Haiku/scripted.
- **Wave 1** — simple accessors (<=6 insn, no float) across all modules. Haiku/Sonnet. ~hundreds.
- **Wave 2** — small functions (<=20 insn, no float). Sonnet.
- **Wave 3** — medium (<=50 insn). Sonnet write -> Opus review.
- **Wave 4** — hard: float-using + large (>=80 insn). Opus + decomp-permuter.
- **Wave 5** — data/rodata symbols (Track 2). Sonnet.
- **Wave 6** — module relocation / overlay correctness (Track 3). Opus.
- **Wave 7** — polish: naming, struct recovery, libultra, comments (Tracks 4-5). Sonnet -> Opus.

## Safety invariants
- The repo is **always left byte-matching** after a wave (Integrate reverts anything that isn't).
- Every kept function is verified by the per-module hash + final ROM SHA-1.
- Each wave commits its matched modules; failures roll forward to the next wave/repair.

## How to run
`Workflow` with the `bar-decomp-wave` script, `args = { modules: [...], maxInsns: N, writerModel: "sonnet" }`.
Scout the wave's module list first with `python3 score_functions.py` + `tools/progress.py`.
