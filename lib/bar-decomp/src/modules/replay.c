// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern u8 D_replay_00401AC0;
extern u8 D_replay_00401AC8;
extern u8 D_replay_00401AD4;
extern u8 D_replay_00401ADC;
extern u8 D_replay_00401B50;
extern u8 D_replay_00401B54;
extern u8 D_replay_00401B58;
extern void *gUvFvecExports;
extern void *D_replay_00401B64;
extern f32 D_replay_00401BA0;
void func_replay_0040018C();
void func_replay_004002B4();
void func_replay_00400328();
void func_replay_00400748();
void func_replay_0040097C();
void func_replay_00400C50();
void func_replay_00400D84();
s32 func_replay_00400F2C();
s32 func_replay_00400FB0();
f32 func_replay_00400FE8(s32 arg0, s32 arg1);
void func_replay_0040100C();
void func_replay_0040148C();
void func_replay_00401840();
void func_replay_00401968();
extern u8 D_replay_00401B7C;
extern u8 D_replay_00401B78;
extern s32 D_replay_00401B98;
extern s32 D_replay_00401B9C;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/__entrypoint_func_replay_400000.s")

void func_replay_0040017C(void) {
}

void func_replay_00400184(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_0040018C.s")

void func_replay_00400230(f32 arg0, f32 arg1, s32 arg2, f32 arg3) {
    void *p = _uvMemAllocAlign8(0x14);
    ((void (*)(void *, f32, f32, s32))(*(s32 *)((u8 *)gUvFvecExports + 0x4)))(p, arg0, arg1, arg2);
    *(f32 *)((u8 *)p + 0xC) = arg3 * arg3;
    *(s32 *)((u8 *)p + 0x10) = (s32)D_replay_00401B64;
    D_replay_00401B64 = p;
}

void func_replay_004002A8(s32 a0) {
    D_replay_00401B7C = (u8)a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_004002B4.s")

void func_replay_0040031C(void) {
    D_replay_00401B78 = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00400328.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00400748.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_0040097C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00400C50.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00400D84.s")

s32 func_replay_00400F2C(s32 a0, s32 a1) {
    u8 *p = (u8 *)&D_replay_00401B98 + a0 * 20;
    if (*(s32 *)p & a1 && !(*(s32 *)(p + 4) & a1)) {
        return 1;
    }
    return 0;
}

s32 func_replay_00400F78(s32 a0) {
    return *(s32*)((u8*)&D_replay_00401B98 + a0 * 20);
}

s32 func_replay_00400F94(s32 a0) {
    return *(s32*)((u8*)&D_replay_00401B9C + a0 * 20);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00400FB0.s")

f32 func_replay_00400FE8(s32 arg0, s32 arg1) {
    return *(f32 *)((u8 *)&D_replay_00401BA0 + (arg0 * 0x14) + (arg1 * 4));
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_0040100C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_0040148C.s")

s32 func_replay_0040162C(s32 arg0) {
    f32 sp[4];
    void *node;

    node = D_replay_00401B64;
    if (node != 0) {
        do {
            (*(void (**)(void *, s32, void *))((u8 *)gUvFvecExports + 0x34))(&sp[0], arg0, node);
            if (sp[0] * sp[0] + sp[1] * sp[1] + sp[2] * sp[2] < *(f32 *)((u8 *)node + 0xC)) {
                return 1;
            }
            node = (void *)*(s32 *)((u8 *)node + 0x10);
        } while (node != 0);
    }
    return 0;
}

void func_replay_004016E8(s32 arg0) {
    typedef struct {
        char pad0[0x84];
        void (*unk84)(s32, s32);  /* 0x84 */
        char pad88[0x94 - 0x88];
        s32  (*unk94)(void);      /* 0x94 */
        s32  (*unk98)(s32);       /* 0x98 */
        void (*unk9C)(s32, u8 *); /* 0x9C */
    } ReplayGuiExports;
    typedef struct {
        char pad0[0x104];
        void (*unk104)(s32, u8 *, s32, f32, f32, u8 *, s32); /* 0x104 */
    } ReplayMiscExports;
    extern ReplayGuiExports *gUvGuiExports;
    extern ReplayMiscExports *gMiscExports;
    extern u8 D_replay_00401AC0;
    extern u8 D_replay_00401AC8;
    extern u8 D_replay_00401AD4;
    extern u8 D_replay_00401ADC;
    extern u8 D_replay_00401B50;
    extern u8 D_replay_00401B54;
    extern u8 D_replay_00401B58;
    s32 temp;

    temp = gUvGuiExports->unk98((s16)gUvGuiExports->unk94());
    gUvGuiExports->unk9C(temp, &D_replay_00401AC0);
    gUvGuiExports->unk84(arg0, temp);
    gMiscExports->unk104(temp, &D_replay_00401AC8, 0x11, 0.0f, 100.0f, &D_replay_00401B58, 0);
    gMiscExports->unk104(temp, &D_replay_00401AD4, 0x11, -50.0f, 50.0f, &D_replay_00401B50, 0);
    gMiscExports->unk104(temp, &D_replay_00401ADC, 0x11, -50.0f, 50.0f, &D_replay_00401B54, 0);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00401840.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/replay/func_replay_00401968.s")

