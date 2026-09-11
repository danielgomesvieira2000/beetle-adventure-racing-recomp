// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_uvtrackld_rom_00400050();
void func_uvtrackld_rom_004001E0();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtrackld_rom/__entrypoint_func_uvtrackld_rom_400000.s")

void func_uvtrackld_rom_00400048(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvtrackld_rom/func_uvtrackld_rom_00400050.s")

void func_uvtrackld_rom_004001E0(void *arg0) {
    _uvMemFree(*(void **)((u8 *)arg0 + 0xC));
    _uvMemFree(*(void **)((u8 *)arg0 + 0x8));
    _uvMemFree(arg0);
}

