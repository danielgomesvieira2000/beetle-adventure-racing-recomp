# Recompilation ELF (`build/recomp.elf`)

This documents the `make recomp` path: producing an ELF laid out for **static
recompilation** with [N64Recomp](https://github.com/N64Recomp/N64Recomp), consumed by the
**BeetleRecomp** PC port (https://github.com/bryankruman/BeetleRecomp).

> **Status: BUILDS & STRUCTURALLY VERIFIED (2026-06-28).** `make recomp` produces
> `build/recomp.elf` (133 per-module sections, 135 `.rel.*` sections, 665 `__recomp_*`
> boundary symbols, ~48k preserved relocations: R_MIPS_26 + HI16/LO16). The default
> matching ROM is unaffected (still `e5ab4d22`). Fixes applied over the first draft:
> - module overlays placed at a **high VRAM window (`0x80800000`, 1 MiB stride)** instead of
>   `0x400000`, so `jal` (R_MIPS_26, 256 MiB-windowed) reaches engine code at `0x800xxxxx`
>   and the location counter never moves backwards past `FORM0_VRAM_END` (~`0x80033770`);
> - `--allow-multiple-definition` on the link — each module overlay carries its own copy of
>   shared entrypoints (`uvModuleCleanup`, `sUvGfxMgrExports`, …); the per-section relocations
>   N64Recomp consumes are preserved regardless of first-wins symbol resolution;
> - `-T linker_scripts/us/hw_syms.txt` so HW-register symbols (`D_80000318`, `osTvType`),
>   exposed by the relocatable `partial_uvdbg_rom.o`, resolve.
>
> The one item still requiring the external toolchain is N64Recomp actually ingesting the ELF
> (last checklist box) — that needs the BeetleRecomp repo.

## Why a separate ELF

The matching build is unsuitable as N64Recomp input:

- `beetleadventurerac.us.elf` (`find_file_boundaries`) splits each module into many per-file
  blob sub-sections (`.UVBT_0`, `.UVAN_0`, …) — raw archived bytes, not relocatable code.
- `build/bin/us/<module>.o` is a single packed `.uvmo` binary blob (one `.data` section, no
  `.text`, no usable relocations).

N64Recomp needs **one relocatable section per module**, with real `.text`/`.rodata`/`.data`/
`.bss` and ELF MIPS relocations. Those live in **`build/partial_<module>.o`** (produced by
`ld -r`; relocations preserved by the `--emit-relocs` LDFLAG).

## How it works

`make recomp`:
1. runs `pre-partial-link` (builds `FORM0.o`, the partial module objects, and
   `kernel_link_scripts_syms.txt`, which supplies `main_VRAM`, `FORM0_ROM_START`,
   `MODULE_FILES_START`, `UVTS_25_ROM_END`, … to the link);
2. runs `tools/genRecompLd.py linker_scripts/us/beetleadventurerac.ld`, which
   - copies the base script up to `FORM0_VRAM_END` (base game + FORM0 region, exactly as
     the matching link), then
   - appends one `.<module>` output section per module pulling `build/partial_<module>.o`
     at a distinct nominal VRAM, with `__recomp_<module>_*` boundary symbols;
3. links `build/recomp.elf` with `recomp.ld` + the auto undefined-stub scripts +
   `kernel_link_scripts_syms.txt`.

`recomp.ld` is generated and git-ignored (like `kernel.ld`).

## Feeding it to BeetleRecomp

In the recomp repo:
- point `BeetleRecomp.toml` `elf_path` at this `build/recomp.elf`;
- list the `.<module>` section names in `overlays.us.txt` (regenerate with
  `scripts/gen-overlays.sh`).

## Testing checklist

- [x] `make recomp` completes and produces `build/recomp.elf`.
- [x] No duplicate-symbol or overlapping-section errors from `ld` (duplicates allowed
      first-wins via `--allow-multiple-definition`; overlaps avoided by the high-VRAM window).
- [x] `readelf -r build/recomp.elf` shows MIPS relocations surviving from the partial objects
      (5976 R_MIPS_26, 21194 R_MIPS_HI16, 21288 R_MIPS_LO16 across 135 `.rel.*` sections).
- [x] `FORM0_VRAM_END` truncation keeps base + FORM0 and drops the per-file blob sections.
- [x] Distinct high-VRAM windows chosen over `OVERLAY@0x400000` (relocatable sections make the
      absolute VMA irrelevant to N64Recomp; high window keeps `jal` in range).
- [ ] N64Recomp ingests `recomp.elf` and emits per-module relocations (LOOKUP_FUNC/RELOC_*).
      *(Requires the BeetleRecomp repo + N64Recomp toolchain — not available here.)*
