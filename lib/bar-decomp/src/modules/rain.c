// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
typedef struct { char pad0[0x70]; void (*unk70)(void *); } UnkRainSndExports;
extern UnkRainSndExports *gSndExports;
extern u8 D_rain_004016B8[8];
void func_rain_00400000();
void func_rain_0040010C();
void func_rain_00400140();
void func_rain_00400390(f32 *arg0, f32 arg1, f32 *arg2, f32 arg3);
void func_rain_004003D4();
void func_rain_00400678();
void func_rain_004008FC();
void func_rain_00400B2C();
void func_rain_00400D48();
void func_rain_00400F98();
void func_rain_00401068();
void func_rain_00401200();
void func_rain_00401304();
void func_rain_00401404();
void func_rain_00401478();
void func_rain_0040157C();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/__entrypoint_func_rain_400054.s")

void func_rain_0040010C(void) {
    gSndExports->unk70(D_rain_004016B8);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00400140.s")

void func_rain_00400388(void *arg0, s32 arg1) {
    *(u8 *)((u8 *)arg0 + 0x12D4) = arg1;
}

void func_rain_00400390(f32 *arg0, f32 arg1, f32 *arg2, f32 arg3) {
    if ((*arg0 < arg1) && (arg3 >= 0.0f)) {
        *arg2 = arg3;
        *arg0 = arg1;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_004003D4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00400678.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_004008FC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00400B2C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00400D48.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00400F98.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00401068.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00401200.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00401304.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00401404.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_00401478.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/rain/func_rain_0040157C.s")

