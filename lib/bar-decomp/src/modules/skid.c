// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
typedef struct UnkCbckExports_s {
    char pad[0x10];
    void (*unk10)(s32, void *, s32, s32);
} UnkCbckExports;
typedef struct UnkChanExports_s {
    char pad[0x4];
    void (*unk4)(s32, s32, void *, s32);
} UnkChanExports;
extern UnkCbckExports *gUvCbckExports;
extern UnkChanExports *gUvChanExports;
void func_skid_00400788(s32 arg0);
void func_skid_0040010C();
void func_skid_00400134();
void func_skid_0040018C();
void func_skid_004002CC();
void func_skid_004002E0();
void func_skid_00400788();
void func_skid_00400968();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/skid/__entrypoint_func_skid_400000.s")

void func_skid_0040010C(void) {
    uvUnloadFile('UVTX', 0x1B8);
}

void func_skid_00400134(s32 arg0) {
    s32 sp1C;
    gUvChanExports->unk4(arg0, 6, &sp1C, 0);
    gUvCbckExports->unk10(sp1C, func_skid_00400788, 0, 0x2E);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/skid/func_skid_0040018C.s")

void func_skid_004002CC(void *arg0) {
    *(u8 *)((u8 *)arg0 + 0x0) = 0x1;
    *(f32 *)((u8 *)arg0 + 0x8) = 0.0f;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/skid/func_skid_004002E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/skid/func_skid_00400788.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/skid/func_skid_00400968.s")

