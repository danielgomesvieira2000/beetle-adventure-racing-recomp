// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_uvtexture_rom_00400D24;
extern s32 D_uvtexture_rom_00400D28;
#include "module.h"

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/__entrypoint_func_uvtexture_rom_400000.s")

void func_uvtexture_rom_00400134(void) {
    uvUnloadModule('GMGR');
    uvUnloadModule('CBCK');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400164.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_004001C0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400244.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400500.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_004009F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400B10.s")

void func_uvtexture_rom_00400B7C(void) {
    D_uvtexture_rom_00400D24 = 0xFFE;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400B8C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400BBC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexture_rom/func_uvtexture_rom_00400C6C.s")

s32 func_uvtexture_rom_00400CB0(void) {
    return D_uvtexture_rom_00400D28;
}

