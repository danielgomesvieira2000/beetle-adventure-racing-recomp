// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void func_uvintersect_rom_00400114();
void func_uvintersect_rom_00400144();
s32 func_uvintersect_rom_00400460(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6);
s32 func_uvintersect_rom_004004C4(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7);
void func_uvintersect_rom_00400530();
void func_uvintersect_rom_00400700();
void func_uvintersect_rom_00400804();
void func_uvintersect_rom_0040093C();
void func_uvintersect_rom_00400E60();
void func_uvintersect_rom_00400FA8();
void func_uvintersect_rom_004010F0();
void func_uvintersect_rom_00401318();
void func_uvintersect_rom_0040191C();
void func_uvintersect_rom_00401A2C();
void func_uvintersect_rom_00401CB0();
void func_uvintersect_rom_004020A0();
void func_uvintersect_rom_004021A0();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/__entrypoint_func_uvintersect_rom_400000.s")

void func_uvintersect_rom_00400114(void) {
    uvUnloadModule('MATH');
    uvUnloadModule('FVEC');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00400144.s")

s32 func_uvintersect_rom_00400460(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    f32 dx;
    f32 dy;
    f32 dz;

    dx = arg0 - arg3;
    dy = arg1 - arg4;
    dz = arg2 - arg5;
    if (((dx * dx) + (dy * dy) + (dz * dz)) <= (arg6 * arg6)) {
        return 1;
    }
    return 0;
}

s32 func_uvintersect_rom_004004C4(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7)
{
  f32 dx;
  f32 dy;
  f32 dz;
  f32 r;
  r = arg7 + arg3;
  dx = arg0 - arg4;
  dy = arg1 - arg5;
  r = dx;
  dz = arg2 - arg6;
  if ((((dx * dx) + (dy * dy)) + (dz * dz)) <= ((arg7 + arg3) * (arg7 + arg3)))
  {
    return 1;
    if (((!dx) && (!r)) & 0xFFFFFFFFFFFFFFFFu)
    {
    }
  }
  return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00400530.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00400700.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00400804.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_0040093C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00400E60.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00400FA8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_004010F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00401318.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_0040191C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00401A2C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_00401CB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_004020A0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvintersect_rom/func_uvintersect_rom_004021A0.s")

