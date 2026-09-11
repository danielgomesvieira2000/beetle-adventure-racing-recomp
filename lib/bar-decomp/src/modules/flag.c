// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern void *D_flag_00401728;
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
void func_flag_00400080();
void func_flag_004004A4();
void func_flag_00400520();
void func_flag_004006BC();
void func_flag_004008F4();
void func_flag_00400D10();
void func_flag_00400D54();
void func_flag_00401198();
void func_flag_004012FC();
void func_flag_00401354();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/__entrypoint_func_flag_400000.s")

void func_flag_00400078(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_00400080.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_004004A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_00400520.s")

void func_flag_004006B4(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_004006BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_004008F4.s")

void func_flag_00400D10(void) {
    void *s0;

    s0 = D_flag_00401728;
    if (s0 != NULL) {
        do {
            func_flag_004008F4(s0);
            s0 = *(void **)((u8 *)s0 + 0x48);
        } while (s0 != NULL);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_00400D54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_00401198.s")

void func_flag_004012FC(s32 a0) {
    s32 sp1C;

    gUvChanExports->unk4(a0, 6, &sp1C, 0);
    gUvCbckExports->unk10(sp1C, func_flag_00401198, 0, 0x3D);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/flag/func_flag_00401354.s")

