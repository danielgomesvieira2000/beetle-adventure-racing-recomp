// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_uvtextureld_rom_004000C4();
void func_uvtextureld_rom_0040010C();
void func_uvtextureld_rom_004001C8();
void func_uvtextureld_rom_004002E8();
void func_uvtextureld_rom_00400A40();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtextureld_rom/__entrypoint_func_uvtextureld_rom_400000.s")

void func_uvtextureld_rom_004000C4(void) {
    uvUnloadModule('GMGR');
    uvUnloadModule('TEXT');
    uvUnloadModule('TSEQ');
    uvUnloadModule('UVTS');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtextureld_rom/func_uvtextureld_rom_0040010C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtextureld_rom/func_uvtextureld_rom_004001C8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtextureld_rom/func_uvtextureld_rom_004002E8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtextureld_rom/func_uvtextureld_rom_00400A40.s")

