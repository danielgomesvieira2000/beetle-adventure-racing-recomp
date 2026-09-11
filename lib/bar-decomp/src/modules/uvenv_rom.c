// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 *D_uvenv_rom_00403C08;
extern s32 D_uvenv_rom_00403C0C;
void func_uvenv_rom_004001B4();
void func_uvenv_rom_00400220();
void func_uvenv_rom_00400450();
void func_uvenv_rom_00401954();
void func_uvenv_rom_00401F7C();
void func_uvenv_rom_00401FE0();
void func_uvenv_rom_0040205C();
void func_uvenv_rom_0040253C();
void func_uvenv_rom_00402DD8();
void func_uvenv_rom_00402E68();
void func_uvenv_rom_00402F00();
s32 func_uvenv_rom_00403AFC();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/__entrypoint_func_uvenv_rom_400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_004001B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00400220.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00400450.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00401954.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00401F7C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00401FE0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_0040205C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_0040253C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00402DD8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00402E68.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvenv_rom/func_uvenv_rom_00402F00.s")

s32 func_uvenv_rom_00403AFC(s32 arg0) {
    if (D_uvenv_rom_00403C0C < arg0) {
        return -1;
    }
    return D_uvenv_rom_00403C08[arg0];
}

