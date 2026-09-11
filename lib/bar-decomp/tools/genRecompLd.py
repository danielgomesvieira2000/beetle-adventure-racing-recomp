#!/usr/bin/env python3
"""
genRecompLd.py -- generate linker_scripts/us/recomp.ld for the `make recomp` target.

Purpose
-------
Produce build/recomp.elf laid out so that EACH relocatable game module is its own
single ELF section, pulling in the module's *relocatable* object build/partial_<name>.o
(real .text/.rodata/.data/.bss + ELF MIPS relocations, preserved by `ld -r` and the
`--emit-relocs` LDFLAG). That is the form the static recompiler N64Recomp needs to treat
every module as a relocatable overlay (so it emits LOOKUP_FUNC / RELOC_HI16 / RELOC_LO16).

This contrasts with the two unsuitable forms in the matching build:
  * beetleadventurerac.us.elf splits each module into many per-file blob sub-sections
    (.UVBT_0, .UVAN_0, ...) via splat find_file_boundaries -- raw archived bytes, not code.
  * build/bin/us/<name>.o is a single packed .uvmo binary blob (one .data section, no
    .text, no usable relocations).

Design (mirrors tools/genKernelLd.py)
-------------------------------------
1. Copy the base/kernel layout from the splat-generated base linker script
   (linker_scripts/us/beetleadventurerac.ld, passed as argv[1]) up to and including the
   line containing FORM0_VRAM_END. That keeps the entire base game + FORM0 region exactly
   as the matching link lays it out, and drops the per-module uvmo/blob sections that
   follow (so there is no section/symbol collision with our overlays).
2. Append one output section `.<module>` per module, each at a distinct nominal VRAM,
   pulling in build/partial_<module>.o, with __recomp_<module>_* boundary symbols. The
   modules are relocatable, so the absolute VRAM is irrelevant to N64Recomp; distinct
   windows simply avoid ld overlapping-section errors.
3. Close with /DISCARD/ { *(*); } and the SECTIONS brace, like genKernelLd.py.

The module list is derived the same way the Makefile derives MODULE_NAMES (the union of
asm/us/data/modules/*.data.s and *.bss.s basenames), so it matches exactly the set of
build/partial_<name>.o that `make` builds. FourCC tags (comments only) come from
tools/convPartialModule.py if importable.

!!! UNTESTED !!!
This has not been run through `make recomp` (no toolchain/.venv/ROM at authoring time).
Treat it as a first draft. Open questions to verify -- see docs/recomp.md:
  * distinct per-module VRAMs (this script) vs. all modules at the same 0x400000 base via
    OVERLAY -- relocatable sections make the absolute VMA irrelevant, so distinct is used;
  * that build/partial_<name>.o contains only .text/.rodata/.data/.bss and that its MIPS
    relocations survive into recomp.elf;
  * that FORM0_VRAM_END is the correct truncation boundary (keep base+FORM0, drop blobs).

Usage:  python tools/genRecompLd.py <base_ld>     # e.g. linker_scripts/us/beetleadventurerac.ld
Output: linker_scripts/us/recomp.ld
"""
import glob
import os
import sys

OUT_PATH = "linker_scripts/us/recomp.ld"
BUILD_DIR = "build"
MODULE_DATA_DIR = "asm/us/data/modules"      # matches Makefile MODULE_DATA_DIR (VERSION=us)
TRUNCATE_MARKER = "FORM0_VRAM_END"
# Modules are relocatable; placed in a high window above FORM0_VRAM_END so
# jal (R_MIPS_26, 256MB-windowed) can still reach engine code at 0x800xxxxx. (see
# linker_scripts/us/module_files/<name>_symbol_addrs.txt). They are position-independent,
# so recomp.elf just gives each its own non-overlapping window; N64Recomp relocates them.
MODULE_VRAM_BASE = 0x80800000
MODULE_VRAM_STRIDE = 0x00100000              # 1 MiB per module; far exceeds any module size


def module_names():
    names = set()
    for suffix in (".data.s", ".bss.s"):
        for path in glob.glob(os.path.join(MODULE_DATA_DIR, "*" + suffix)):
            names.add(os.path.basename(path)[: -len(suffix)])
    if not names:
        raise SystemExit(
            f"genRecompLd: no modules found under {MODULE_DATA_DIR} "
            "(run after extraction so the *.data.s/*.bss.s exist)"
        )
    return sorted(names)


def module_tags():
    """Optional name -> FourCC tag map (for comments only)."""
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    try:
        from convPartialModule import partial_to_module
    except Exception:
        return {}
    tags = {}
    for key, tag in partial_to_module.items():
        name = key[len("partial_"):] if key.startswith("partial_") else key
        name = name[:-2] if name.endswith(".o") else name
        tags[name] = tag
    return tags


def base_lines(base_ld_path):
    out, found = [], False
    with open(base_ld_path, "r") as f:
        for line in f:
            out.append(line.rstrip("\n"))
            if TRUNCATE_MARKER in line:
                found = True
                break
    if not found:
        raise SystemExit(f"genRecompLd: marker {TRUNCATE_MARKER!r} not found in {base_ld_path}")
    return out


def module_section(name, tag, vram):
    obj = f"{BUILD_DIR}/partial_{name}.o"
    tag_comment = f" (tag {tag})" if tag else ""
    return [
        f"    /* module {name}{tag_comment} -- relocatable overlay (nominal VRAM 0x{vram:08X}) */",
        f"    .{name} 0x{vram:08X} : SUBALIGN(16)",
        "    {",
        f"        __recomp_{name}_start = .;",
        f"        {obj}(.text .text.*)",
        f"        __recomp_{name}_text_end = .;",
        f"        {obj}(.rodata .rodata.*)",
        f"        {obj}(.data .data.*)",
        f"        __recomp_{name}_end = .;",
        "    }",
        f"    .{name}.bss (NOLOAD) : SUBALIGN(16)",
        "    {",
        f"        __recomp_{name}_bss_start = .;",
        f"        {obj}(.bss .bss.* COMMON)",
        f"        __recomp_{name}_bss_end = .;",
        "    }",
        "",
    ]


def main():
    if len(sys.argv) < 2:
        raise SystemExit(
            "Usage: genRecompLd.py <base_ld>  (e.g. linker_scripts/us/beetleadventurerac.ld)"
        )
    lines = ["/* Autogenerated by tools/genRecompLd.py -- DO NOT EDIT. See docs/recomp.md. */"]
    lines += base_lines(sys.argv[1])
    lines.append("")
    lines.append("    /* ==== relocatable module overlays (one section per module, for N64Recomp) ==== */")
    tags = module_tags()
    names = module_names()
    vram = MODULE_VRAM_BASE
    for name in names:
        lines += module_section(name, tags.get(name), vram)
        vram += MODULE_VRAM_STRIDE
    lines += ["    /DISCARD/ :", "    {", "        *(*);", "    }", "}"]

    with open(OUT_PATH, "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")
    print(f"genRecompLd: wrote {OUT_PATH} ({len(names)} module sections)")


if __name__ == "__main__":
    main()
