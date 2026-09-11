// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
typedef struct {
    char pad[0x4];
    void (*unk4)(s32, s32, void *, s32);
} AieditChanExports;
typedef struct {
    char pad[0x10];
    void (*unk10)(s32, void (*)(s32), s32, s32);
} AieditCbckExports;
typedef struct {
    char pad[0x1C];
    void (*unk1C)(s32);
    void (*unk20)(s32);
} AieditTdataExports;
extern AieditChanExports *gUvChanExports;
extern AieditCbckExports *gUvCbckExports;
void func_aiedit_00400944(s32 arg0);
void func_aiedit_00400234();
void func_aiedit_0040084C();
void func_aiedit_00400944();
void func_aiedit_0040098C();
void func_aiedit_004009E4();
void func_aiedit_00400D6C();
void func_aiedit_00400D9C();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/__entrypoint_func_aiedit_400000.s")

void func_aiedit_0040022C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/func_aiedit_00400234.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/func_aiedit_0040084C.s")

void func_aiedit_0040093C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/func_aiedit_00400944.s")

void func_aiedit_0040098C(s32 arg0) {
    s32 sp1C;
    gUvChanExports->unk4(arg0, 6, &sp1C, 0);
    gUvCbckExports->unk10(sp1C, func_aiedit_00400944, 0, 0xC8);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/func_aiedit_004009E4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/func_aiedit_00400D6C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/aiedit/func_aiedit_00400D9C.s")

