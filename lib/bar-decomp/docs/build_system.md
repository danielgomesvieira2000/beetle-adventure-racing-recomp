Originally written by LLONSIT, taken from [PR# 18](https://github.com/synamaxmusic/bar-decomp/pull/18) 

## Build System

This document explains how the build system works to extract and decompile the module files / overlays of the game.

## Changes to the build system

To recompile the module files the build system follows this workflow:

Module Patching -> disassembly -> partial linked object -> DaisyBox 

### Module Patching
   - Use `moduleTool` to patch the extracted module files with a `.uvmo` extension (see `tools/splat_ext/uvmo.py`). The patched files are stored in `patched_modules` this step is necessary to ensure that all module relocations are applied correctly so we can have a proper disassembly from `spimdisasm` 

### Module disassembly
After patching, the build system uses `moduleExtract` to disassemble all module files.

- `moduleExtract` reads a yaml file named: `config/us/modules.yaml` which allows minimal configuration such as migrating `.rodata` to functions or marking sections already migrated. To see an example of this format see the file mentioned above (`config/us/modules.yaml`)

Example disassembly output paths:
- `asm/us/nonmatchings/module_non_matchigs`
- `asm/us/data/modules/module.data` → if the module has `.data`
- `asm/us/data/modules/module.rodata` → if the module has `.rodata`
- `src/modules/module.c` → only created if the C file doesn’t exist

### Partial Linked Object
**Pre partial link**
    The Makefile uses a linker script named kernel.ld to link all main segment code into an elf file named: `kernel.elf`, this is done with the purpose of generating a map file that can be later used by Daisybox

**Partial Linking** 
    After the pre-partial link step we use the assembled files (`module.c.o` `module.rodata.o` `module.data.o` `module.bss.o`) and merge them into a larger ELF relocatable object which will be used by Daisybox in the final step

### ELF → UVMO & Hash Verification
This is probably the most important part of this process, in this step we basically convert the Partial ELF file into the evil custom RIFF format used by Paradigm Entertainment Inc. To do that we use a very nice tool named `Daisybox` which will do the conversion process.
A hash verification ensures that the generated .uvmo file matches the original file byte-for-byte

```
✓ SUCCESS!
Hash verification passed.
Your beetle passed the safety checks and is ready to race in Mount Mayhem!
```

```
✗ MISMATCH!
Hash verification failed.
Bad news buddy, your beetle just forgot how to beetle and cannot race in Coventry Cove!
```
"SUCCESS": Means you are good to go!
"MISMATCH" Means that there are byte differences in the generated uvmo file
