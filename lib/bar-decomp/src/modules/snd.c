#include "common.h"
/*__SEEDEXTERNS__*/
typedef struct {
    char pad0[0x4];
    s32 (*unk4)(void);
    void (*unk8)(s32);
    void (*unkC)(s32, s32);
    char pad10[0x20];
    void (*unk30)(s32, s32);
    char pad34[0x20];
    void (*unk54)(s32);
    char pad58[0x3C];
    void (*unk94)(s32, s32, s32);
} SndUvEmitterExports;
extern SndUvEmitterExports *gUvEmitterExports;
extern s32 D_snd_0040616C[];
extern u8 D_snd_004064EF;
extern void *D_snd_00406198;
void func_snd_004004F8();
void func_snd_004005C8();
void func_snd_00400750();
void func_snd_00400854();
void func_snd_0040094C();
void func_snd_00400A64();
s32 func_snd_00400B0C();
void func_snd_00400B54();
void func_snd_00400CD8();
void func_snd_00400DDC();
void func_snd_00400EC0();
void func_snd_00401038();
void func_snd_00401304();
void func_snd_004013DC();
void func_snd_00401434();
void func_snd_00401474();
f32 func_snd_004014B4(void);
f32 func_snd_004014C4(void);
void func_snd_004014E0();
void func_snd_00401564();
void func_snd_00401650();
void func_snd_00401694();
void func_snd_00401800();
void func_snd_00401914();
void func_snd_00401A28();
void func_snd_00401AA8();
void func_snd_00401CDC();
void func_snd_00401D14();
void func_snd_00401D54();
void func_snd_00401DA0();
void func_snd_00401E08();
void func_snd_00401E70();
void func_snd_00401EB8();
void func_snd_00401F48();
void func_snd_00401FD8();
void func_snd_00402020();
void func_snd_00402084();
void func_snd_004020D0();
void func_snd_0040211C();
void func_snd_0040216C();
void func_snd_0040221C();
void func_snd_004022D0();
void func_snd_0040231C();
void func_snd_00402368();
s32 func_snd_00402388();
void func_snd_004023A8();
u8 func_snd_004023D4();
s32 func_snd_004023F4();
void func_snd_00402424();
void func_snd_0040252C();
void func_snd_0040260C();
void func_snd_00402660();
void func_snd_004027E8();
void func_snd_0040284C();
void func_snd_00402B40();
void func_snd_00402DC0();
extern u8 D_snd_004063B8[];
extern u8 D_snd_004064B8[];
extern u8 D_snd_00406168;
extern u16 D_snd_00406190;
extern s32 D_snd_004064BC;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/__entrypoint_func_snd_400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004004F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004005C8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400750.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400854.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040094C.s")

/* func_snd_00400A64(u8 arg0) -> s16: allocate an emitter slot via
 * gUvEmitterExports->unk4(); if it returns 0xFF, return 0xFF. Otherwise
 * configure the slot: unkC(slot, arg0); unk30(slot, 0); unk54(slot); return slot.
 * Logic verified; byte-match blocked on IDO reg-alloc (masked slot wants $s0,
 * with andi in the branch delay slot + per-arg re-mask). Needs decomp-permuter. */
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400A64.s")


#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400B0C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400B54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400CD8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400DDC.s")

void func_snd_00400E90(s32 a0) {
    D_snd_004064B8[a0] = 0;
}

void func_snd_00400EA0(s32 a0) {
    D_snd_004064B8[a0] = 1;
}

void func_snd_00400EB4(s32 a0) {
    D_snd_00406168 = a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00400EC0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401038.s")

void func_snd_004012F4(u16 a0) {
    D_snd_00406190 = a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401304.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004013DC.s")

void func_snd_00401434(s32 a0) {
    gUvEmitterExports->unk94(0, D_snd_0040616C[a0], a0);
}

void func_snd_00401474(s32 a0) {
    gUvEmitterExports->unk94(1, D_snd_0040616C[a0], a0);
}

f32 func_snd_004014B4(void) {
    return 1.0f;
}

f32 func_snd_004014C4(void) {
    return 1.0f;
}

void func_snd_004014D4(s32 a0) {
    D_snd_004064BC = a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004014E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401564.s")

void func_snd_00401650(void) {
    s16 i = 1;
    do {
        func_snd_00401564(i & 0xFF);
        i++;
    } while (i < 0x28);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401694.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401800.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401914.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401A28.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401AA8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401CDC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401D14.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401D54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401DA0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401E08.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401E70.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401EB8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401F48.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00401FD8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402020.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402084.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004020D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040211C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040216C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040221C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004022D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040231C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402368.s")

s32 func_snd_00402388(s32 a0) {
    return *(s32 *)((u8 *)D_snd_00406198 + a0 * 0x1C + 4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004023A8.s")

u8 func_snd_004023D4(s32 a0) {
    return *(u8 *)((u8 *)D_snd_00406198 + a0 * 0x1C + 0x18);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004023F4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402424.s")

void func_snd_00402504(void *arg0) {
    if (arg0 == NULL) {
        *(s32 *)0 = 0;
        return;
    }
    *(u8 *)((u8 *)arg0 + 4) = 0xFF;
    *(f32 *)arg0 = -1.0f;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040252C.s")

u8 func_snd_004025EC(s32 a0) {
    return D_snd_004063B8[a0];
}

void func_snd_004025FC(s32 a0) {
    D_snd_004063B8[a0] = 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040260C.s")

void func_snd_0040264C(s32 a0) {
    D_snd_004063B8[a0] = 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402660.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_004027E8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_0040284C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402B40.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/snd/func_snd_00402DC0.s")

void func_snd_00402E1C(void) {
}

