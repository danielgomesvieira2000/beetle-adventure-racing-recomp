<p align="center">
<img width="650" src="media/bar-decomp_logov2.png" alt="BAR Decomp logo">
</p>

# BAR Decomp

A work‑in‑progress, **matching** decompilation of *Beetle Adventure Racing!* (Nintendo 64, USA).
"Matching" means the C source in this repo compiles **byte‑for‑byte** back to the original ROM
(SHA‑1 `e5ab4d226c08d22f68a2edcc48870203e67454b8`) using the original compiler (IDO 7.1).

> **This is an independent fork.** It is **not** affiliated with, and does **not** aim to contribute
> back to, the upstream project. Unlike upstream, this fork **actively uses AI‑assisted tooling**
> (LLMs) to accelerate decompilation, with a human‑reviewed, byte‑exact verification gate. See
> [`CONTRIBUTING.md`](CONTRIBUTING.md) and [`docs/AI_WORKFLOW.md`](docs/AI_WORKFLOW.md).

## Credits & lineage

This fork is built on the excellent groundwork of the original authors, and we are grateful for it:

- **[synamaxmusic/bar-decomp](https://github.com/synamaxmusic/bar-decomp)** — the original Beetle
  Adventure Racing decompilation (splat config, module/overlay build system, symbol map, IDO setup).
- **[synamaxmusic/bar-notes](https://github.com/synamaxmusic/bar-notes)** — reverse‑engineering notes
  and tooling for the game.
- **LLONSIT** — original `docs/build_system.md` and module‑system work.
- The wider N64 decomp community for the shared toolchain: **splat**, **ido-static-recomp**,
  **asm-differ**/**objdiff**, **m2c**, and **decomp-permuter**.

Because the upstream project is licensed **AGPL‑3.0**, this derivative is **also AGPL‑3.0** (see
[`LICENSE`](LICENSE)). All copyleft and attribution obligations are retained.

## Legal / ROM policy

This repository contains **no copyrighted game data** — no ROM, and no assets extracted from one.
To build, you must supply your **own legally‑dumped** USA cartridge image. The ROM and all build
artifacts are git‑ignored and must never be committed. Your ROM is verified by SHA‑1 only.

---

## Quick start (Ubuntu 24.04, native or under WSL2)

> **Why 24.04?** It ships **binutils 2.42** and **Python 3.12**. Older distros (e.g. Ubuntu 20.04 /
> binutils 2.34) miscompile the final ELF→ROM step on the TLS `.tdata` section and produce a broken,
> multi‑gigabyte ROM. Use 22.04+ — 24.04 recommended.

### 1. Dependencies
```bash
sudo apt update
sudo apt install -y build-essential binutils-mips-linux-gnu python3 python3-pip python3-venv \
  python3-dev ninja-build clang-format clang-tidy libcjson-dev libzstd-dev binutils-dev \
  libpng-dev zlib1g-dev git curl pkg-config
```

### 2. Clone
```bash
git clone --recurse-submodules <your-fork-url> bar-decomp
cd bar-decomp
```

### 3. Toolchain + Python deps (venv)
```bash
python3 -m venv .venv
source ./.venv/bin/activate     # REQUIRED every shell — see note below
make dependencies
```
> ⚠️ **Always `source ./.venv/bin/activate` before building.** The Makefile calls the bare
> `mapfile_parser` console script, which only exists inside the venv. Without it the build fails at
> `pre-partial-link` with `Error 127`.

### 4. Provide your base ROM
Copy your USA dump into the project root as **`baserom.us.z64`** (git‑ignored). It must hash to
`e5ab4d226c08d22f68a2edcc48870203e67454b8`. *(A mislabeled `.n64`/`.v64` byte‑swapped dump can be
converted with `dd conv=swab in.n64 baserom.us.z64`.)*

### 5. Extract & build
```bash
make extract     # one‑time: splat splits the ROM + disassembles all modules
make -j6         # build; modest -j avoids thrashing WSL2
```
Success prints `build/beetleadventurerac.us.z64: OK`. A `FAILED` means either a bad base ROM or
non‑matching code.

### Verifying / tracking progress
```bash
scripts/check.sh            # incremental build + ROM hash + progress summary (use at checkpoints)
scripts/check.sh --module race   # fast: rebuild & hash‑verify just one module after editing it
python3 tools/progress.py   # matched/unmatched function & data breakdown, per module
python3 score_functions.py  # rank the easiest remaining functions to decompile next
```

---

## Project status

Roughly **~26% of functions matched** at the time of forking. Remaining work (see
[`docs/DECOMP_PLAN.md`](docs/DECOMP_PLAN.md) for the full breakdown):

| Track | Remaining |
|---|---|
| Code functions to match | ~2,419 |
| Data/rodata symbols to type into C | ~4,561 |
| Relocatable modules/overlays | 134 (correct relocation + hashes) |

## Documentation
- [`docs/DECOMP_PLAN.md`](docs/DECOMP_PLAN.md) — the roadmap to 100% and how the work is organized.
- [`docs/AI_WORKFLOW.md`](docs/AI_WORKFLOW.md) — the AI‑assisted, model‑tiered matching loop.
- [`docs/build_system.md`](docs/build_system.md) — how the module/overlay build works (DaisyBox/FORM0).
- [`docs/bar-basics.md`](docs/bar-basics.md) — game/engine basics.
- [`CONTRIBUTING.md`](CONTRIBUTING.md) — how to contribute (humans and agents welcome).

## Goals
1. Reach a **100% matching** decompilation (zero `GLOBAL_ASM`, all data typed in C).
2. Keep the code **readable** — byte‑identical output, but named symbols, real types, and comments.
3. Lay the groundwork for a **modern PC port** (decomp‑native and/or N64Recomp). See the plan.
