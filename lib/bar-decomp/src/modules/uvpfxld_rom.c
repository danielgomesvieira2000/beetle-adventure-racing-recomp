// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_uvpfxld_rom_00400EC0;
void func_uvpfxld_rom_00400094();
void func_uvpfxld_rom_004000DC();
void func_uvpfxld_rom_004002DC();
void func_uvpfxld_rom_00400994();
void func_uvpfxld_rom_00400B98();
void func_uvpfxld_rom_00400BD4();
void func_uvpfxld_rom_00400CD0();
extern void *D_uvpfxld_rom_00400EE4;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfxld_rom/__entrypoint_func_uvpfxld_rom_400000.s")

void func_uvpfxld_rom_00400094(void) {
    uvUnloadModule('TSEQ');
    uvUnloadModule('FMTX');
    uvUnloadModule('FVEC');
    uvUnloadModule('UPFX');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfxld_rom/func_uvpfxld_rom_004000DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfxld_rom/func_uvpfxld_rom_004002DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfxld_rom/func_uvpfxld_rom_00400994.s")

void func_uvpfxld_rom_00400B98(void *arg0) {
    _uvMediaCopy(*(s32 *)((u8 *)D_uvpfxld_rom_00400EE4 + 0x30), arg0, D_uvpfxld_rom_00400EC0 * 2);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfxld_rom/func_uvpfxld_rom_00400BD4.s")

void func_uvpfxld_rom_00400CBC(void *a0) {
    *(s16 *)((u8 *)D_uvpfxld_rom_00400EE4 + 0x2) = *(s32 *)a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfxld_rom/func_uvpfxld_rom_00400CD0.s")

