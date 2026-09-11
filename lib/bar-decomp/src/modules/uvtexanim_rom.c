// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern void *D_uvtexanim_rom_0040098C;
extern s32 D_uvtexanim_rom_00400990;
void func_uvtexanim_rom_004000DC();
void func_uvtexanim_rom_00400118();
void func_uvtexanim_rom_004002A0();
void func_uvtexanim_rom_004002DC();
void func_uvtexanim_rom_00400640();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexanim_rom/__entrypoint_func_uvtexanim_rom_400000.s")

void func_uvtexanim_rom_004000DC(void) {
    uvUnloadModule('GMGR');
    uvUnloadModule('CBCK');
    uvUnloadModule('TEXT');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexanim_rom/func_uvtexanim_rom_00400118.s")

void func_uvtexanim_rom_004002A0(void) {
    if (D_uvtexanim_rom_0040098C != NULL) {
        _uvMemFree(D_uvtexanim_rom_0040098C);
    }
    D_uvtexanim_rom_0040098C = NULL;
    D_uvtexanim_rom_00400990 = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexanim_rom/func_uvtexanim_rom_004002DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtexanim_rom/func_uvtexanim_rom_00400640.s")

