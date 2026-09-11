// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include <PR/sched.h>

typedef struct UvGfxStruct_s {
    /* 0x00 */ s32 unk0; /* inferred */
    /* 0x04 */ char pad4[4];
    /* 0x08 */ s32 unk8;           /* inferred */
    /* 0x0C */ void *unkC;         /* inferred */
    /* 0x10 */ OSTask task;        /* inferred */
    /* 0x50 */ OSMesgQueue *unk50; /* inferred */
    /* 0x54 */ void *unk54;        /* inferred */
} UvGfxStruct;                     /* size = 0x58 */

typedef struct UvGfxMgr_Rom_00402628_s {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 (*unk4)(u16); /* inferred */
    /* 0x08 */ void (*unk8)(s32);
    /* 0x0C */ void (*unkC)(s32, s32);
} UvGfxMgr_Rom_00402628; /* size = 0x10 */

typedef struct uvGfxViewport_s {
    /* 0x0 */ s16 unk0;
    /* 0x2 */ s16 unk2;
    /* 0x4 */ s16 unk4;
    /* 0x6 */ s16 unk6;
    /* 0x8 */ s16 x0; // left?
    /* 0xA */ s16 x1; // right?
    /* 0xC */ s16 y0; // top?
    /* 0xE */ s16 y1; // bottom?
    /* 010 */ Vp vp;
} uvGfxViewport;

typedef struct Prop1Struct_s {
    /* 0x00 */ s32 unk0[1];  /* inferred */
    /* 0x04 */ char pad4[8]; /* maybe part of unk0[3]? */
    /* 0x0C */ u16 unkC;     /* inferred */
    /* 0x0E */ u16 unkE;     /* inferred */
    /* 0x10 */ u16 unk10;    /* inferred */
    /* 0x12 */ u16 unk12;    /* inferred */
    /* 0x14 */ u16 unk14;    /* inferred */
    /* 0x16 */ u16 unk16;    /* inferred */
    s32 unk18;
    s32 unk1C[1];
} Prop1Struct; /* size = 0x18 */

void func_uvgfxmgr_rom_00402090(void);
extern void *D_uvgfxmgr_rom_00402260;
extern Gfx *sGfxDisplayListHead;
extern f32 D_uvgfxmgr_rom_00402420[];
extern u16 D_uvgfxmgr_rom_0040242C;
extern s32 D_uvgfxmgr_rom_00402430[];
extern void *D_uvgfxmgr_rom_0040243C;
extern u16 sScreenWidth;
extern u16 sScreenHeight;
extern s32 D_uvgfxmgr_rom_00402450;
extern Gfx *sGfxDisplayListBase[];
extern u16 D_uvgfxmgr_rom_00402460;
extern u16 sGfxFbIndex;
extern u16 D_uvgfxmgr_rom_00402464;
extern u16 D_uvgfxmgr_rom_00402466;
extern s32 D_uvgfxmgr_rom_00402470[];
extern s32 D_uvgfxmgr_rom_00402478;
extern void *D_uvgfxmgr_rom_00402484;
extern UvGfxStruct D_uvgfxmgr_rom_00402488[];
extern void (*D_uvgfxmgr_rom_0040261C)(void *, void *);
extern void (*D_uvgfxmgr_rom_00402618)(s32, s32);
extern s32 D_uvgfxmgr_rom_0040247C;

extern s32 D_uvgfxmgr_rom_00402620;
extern s32 D_uvgfxmgr_rom_00402624;
extern UvGfxMgr_Rom_00402628 *D_uvgfxmgr_rom_00402628;
extern s32 D_uvgfxmgr_rom_00402264;
extern OSMesgQueue D_uvgfxmgr_rom_004025B8;
extern s16 D_uvgfxmgr_rom_00402408;
extern s16 D_uvgfxmgr_rom_0040240A;
extern s16 D_uvgfxmgr_rom_0040240C;
extern s16 D_uvgfxmgr_rom_0040240E;
extern Gfx D_uvgfxmgr_rom_00402278[];
extern s32 D_uvgfxmgr_rom_00402410;
extern void *D_uvgfxmgr_rom_00402434;
extern u16 D_uvgfxmgr_rom_00402440;
extern u16 D_uvgfxmgr_rom_00402446;
extern u16 D_uvgfxmgr_rom_00402448;
extern s32 D_uvgfxmgr_rom_0040244C;
extern void *D_uvgfxmgr_rom_00402590;
extern void *D_uvgfxmgr_rom_004025D0;
extern OSMesgQueue D_uvgfxmgr_rom_004025F8;
extern OSScClient D_uvgfxmgr_rom_00402610;
extern s32 sGfxDramStack;
extern s32 sGfxTaskOutputBufferStart;
extern s32 sGfxTaskOutputBufferEnd;
extern s32 sGfxYieldData;
extern OSSched *gScheduler;
extern f32 D_uvgfxmgr_rom_00402480;
extern uvGfxViewport D_uvgfxmgr_rom_004022C8[];
extern s32 D_uvgfxmgr_rom_00402268[];
extern s32 sGfxYieldData;
extern s32 sGfxDramStack;
extern s32 sGfxTaskOutputBufferStart;
extern s32 sGfxTaskOutputBufferEnd;
extern s32 sGfxElementCount;

void func_uvgfxmgr_rom_00400794(void);
void func_uvgfxmgr_rom_004007F8(void);
Gfx **uvGetDisplayListHead(void);
void uvGfxDisplayList(Gfx* dl);
s32 func_uvgfxmgr_rom_00400AB8(s32 arg0);
void (*func_uvgfxmgr_rom_00400AF0(void (*arg0)(void *, void *)))(void *, void *);
s32 func_uvgfxmgr_rom_00400B04(s32 arg0);
void func_uvgfxmgr_rom_00400B18(s32 arg0);
void func_uvgfxmgr_rom_00400B24(void);
void func_uvgfxmgr_rom_00400F20(u16 arg0);
void func_uvgfxmgr_rom_00400F20(u16 arg0);
void func_uvgfxmgr_rom_00400F58(s32 arg0);
s32 func_uvgfxmgr_rom_00400F64(void);
void func_uvgfxmgr_rom_00400FF8(f32 arg0);
f32 func_uvgfxmgr_rom_00401004(void);
void uvGfxEnableGamma(s32 enable);
void func_uvgfxmgr_rom_0040107C(f32 red, f32 green, f32 blue, f32 alpha);
void func_uvgfxmgr_rom_00401370(s8 red, s8 green, s8 blue, s8 alpha);
void func_uvgfxmgr_rom_00401430(f32 red, f32 green, f32 blue, f32 alpha);
void func_uvgfxmgr_rom_004016AC(s8 r, s8 g, s8 b, s8 a);
void func_uvgfxmgr_rom_00401714(s32 arg0, s32 arg1, u32 arg2, s32 arg3, s32 arg4, s32 arg5);
void func_uvgfxmgr_rom_00401788(u8 red, u8 green, u8 alpha);
void func_uvgfxmgr_rom_00401914(void);
void uvGfxClipRect(uvGfxViewport *vp, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_uvgfxmgr_rom_00401BD4(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void func_uvgfxmgr_rom_00401C5C(s32 vp_id);
void func_uvgfxmgr_rom_00401C5C(s32); /* extern */
void func_uvgfxmgr_rom_00401D94(s16 persp);
void func_uvgfxmgr_rom_00401DC4(void);
void *func_uvgfxmgr_rom_0040204C(void);
s32 func_uvgfxmgr_rom_00402058(void);
s32 func_uvgfxmgr_rom_00402074(void);
void func_uvgfxmgr_rom_00402090(void);
void func_uvgfxmgr_rom_00402090(void);
void uvCopyFrameBuf(s32 arg0);
void uvSetScreenHeight(s32 height);
void uvSetScreenWidth(s32 arg0);
s32 uvGetScreenHeight(void);
s32 uvGetScreenWidth(void);
void func_uvgfxmgr_rom_004021C8(void);
void uvGfxSetDepthSource(s32 usePrimitiveDepth);
void uvGfxPrimDepth(s32 z);

#if 0
void __entrypoint_func_uvgfxmgr_rom_400000(UvGfxMgr_Exports *arg0) {
    Prop1Struct *temp_v0;
    s32 temp_lo;
    u32 sp54;
    u32 sp50;
    s32 *var_v1;
    s32 i;
    static s32 D_uvgfxmgr_rom_00402434;

    uvUpdateFileAllocPtr((s32) arg0);
    arg0->func_uvgfxmgr_rom_004007F8 = func_uvgfxmgr_rom_004007F8;
    arg0->uvGetDisplayListHead = uvGetDisplayListHead;
    arg0->uvGfxDisplayList = uvGfxDisplayList;
    arg0->func_uvgfxmgr_rom_00400AB8 = func_uvgfxmgr_rom_00400AB8;
    arg0->func_uvgfxmgr_rom_00400AF0 = func_uvgfxmgr_rom_00400AF0;
    arg0->func_uvgfxmgr_rom_00400B04 = func_uvgfxmgr_rom_00400B04;
    arg0->func_uvgfxmgr_rom_00400B18 = func_uvgfxmgr_rom_00400B18;
    arg0->func_uvgfxmgr_rom_00400B24 = func_uvgfxmgr_rom_00400B24;
    arg0->func_uvgfxmgr_rom_00400F20 = func_uvgfxmgr_rom_00400F20;
    arg0->func_uvgfxmgr_rom_00400794 = func_uvgfxmgr_rom_00400794;
    arg0->func_uvgfxmgr_rom_00400F58 = func_uvgfxmgr_rom_00400F58;
    arg0->func_uvgfxmgr_rom_00401788 = func_uvgfxmgr_rom_00401788;
    arg0->func_uvgfxmgr_rom_00400F64 = func_uvgfxmgr_rom_00400F64;
    arg0->uvCopyFrameBuf = uvCopyFrameBuf;
    arg0->func_uvgfxmgr_rom_00401914 = func_uvgfxmgr_rom_00401914;
    arg0->func_uvgfxmgr_rom_00400FF8 = func_uvgfxmgr_rom_00400FF8;
    arg0->uvSetScreenHeight = uvSetScreenHeight;
    arg0->func_uvgfxmgr_rom_00401BD4 = func_uvgfxmgr_rom_00401BD4;
    arg0->func_uvgfxmgr_rom_00401004 = func_uvgfxmgr_rom_00401004;
    arg0->uvSetScreenWidth = uvSetScreenWidth;
    arg0->func_uvgfxmgr_rom_00401C5C = func_uvgfxmgr_rom_00401C5C;
    arg0->uvGfxEnableGamma = uvGfxEnableGamma;
    arg0->uvGetScreenHeight = uvGetScreenHeight;
    arg0->func_uvgfxmgr_rom_00401D94 = func_uvgfxmgr_rom_00401D94;
    arg0->func_uvgfxmgr_rom_0040107C = func_uvgfxmgr_rom_0040107C;
    arg0->uvGetScreenWidth = uvGetScreenWidth;
    arg0->func_uvgfxmgr_rom_00401DC4 = func_uvgfxmgr_rom_00401DC4;
    arg0->func_uvgfxmgr_rom_00401370 = func_uvgfxmgr_rom_00401370;
    arg0->func_uvgfxmgr_rom_004021C8 = func_uvgfxmgr_rom_004021C8;
    arg0->func_uvgfxmgr_rom_0040204C = func_uvgfxmgr_rom_0040204C;
    arg0->func_uvgfxmgr_rom_00401430 = func_uvgfxmgr_rom_00401430;
    arg0->uvGfxSetDepthSource = uvGfxSetDepthSource;
    arg0->func_uvgfxmgr_rom_00402058 = func_uvgfxmgr_rom_00402058;
    arg0->func_uvgfxmgr_rom_004016AC = func_uvgfxmgr_rom_004016AC;
    arg0->uvGfxPrimDepth = uvGfxPrimDepth;
    arg0->func_uvgfxmgr_rom_00402074 = func_uvgfxmgr_rom_00402074;
    arg0->func_uvgfxmgr_rom_00401714 = func_uvgfxmgr_rom_00401714;
    arg0->func_uvgfxmgr_rom_00402090 = func_uvgfxmgr_rom_00402090;
#line 70
    sGfxDramStack = _uvMemAlloc(0x400U, 0x10U);
    D_uvgfxmgr_rom_00402410 = 0x800FDF80;
    func_80002CD0(D_uvgfxmgr_rom_00402410, 0x2080, &sp54, &sp50);
    sGfxYieldData = _uvMemAlloc(0xC00U, 0x10U);
    temp_v0 = uvGetSystemProp(1);
    if (temp_v0 == NULL) {
        sScreenWidth = 0x140;
        sScreenHeight = 0xF0;
        D_uvgfxmgr_rom_00402446 = 0xA;
        D_uvgfxmgr_rom_00402448 = 0xA;
        D_uvgfxmgr_rom_0040242C = 2;
        D_uvgfxmgr_rom_00402430[0] = 0;
        D_uvgfxmgr_rom_00402430[1] = 0;
        D_uvgfxmgr_rom_00402440 = 0;
        D_uvgfxmgr_rom_0040244C = 0x1068;
        sGfxDisplayListBase[0] = NULL;
        sGfxDisplayListBase[1] = NULL;
    } else {
        if (temp_v0->unk10 != 0) {
            sScreenWidth = temp_v0->unk10;
        } else {
            sScreenWidth = 0x140;
        }
        if (temp_v0->unk12 != 0) {
            sScreenHeight = temp_v0->unk12;
        } else {
            sScreenHeight = 0xF0;
        }
        if (temp_v0->unk14 != 0) {
            D_uvgfxmgr_rom_00402446 = temp_v0->unk14;
        } else {
            D_uvgfxmgr_rom_00402446 = 0xA;
        }
        if (temp_v0->unk16 != 0) {
            D_uvgfxmgr_rom_00402448 = temp_v0->unk16;
        } else {
            D_uvgfxmgr_rom_00402448 = 0xA;
        }
        if (temp_v0->unkE != 0) {
            D_uvgfxmgr_rom_0040242C = temp_v0->unkE;
        } else {
            D_uvgfxmgr_rom_0040242C = 2;
        }
        if (temp_v0->unkC != 0) {
            D_uvgfxmgr_rom_00402440 = temp_v0->unkC;
        } else {
            D_uvgfxmgr_rom_00402440 = 0;
        }
        if (temp_v0->unk18 != 0) {
            D_uvgfxmgr_rom_0040244C = temp_v0->unk18;
        } else {
            D_uvgfxmgr_rom_0040244C = 0x1068;
        }

        for (i = 0; i < D_uvgfxmgr_rom_0040242C; i++) {
            if (temp_v0->unk0[i] != 0) {
                D_uvgfxmgr_rom_00402430[i] = temp_v0->unk0[i];
            } else {
                D_uvgfxmgr_rom_00402430[i] = 0;
            }
        }

        for (i = 0; i < 2; i++) {
            if (temp_v0->unk1C[i] != 0) {
                sGfxDisplayListBase[i] = temp_v0->unk1C[i];
            } else {
                sGfxDisplayListBase[i] = 0;
            }
        }
    }
    D_uvgfxmgr_rom_00402430[0] = 0x80200000 - (sScreenWidth * 2 * sScreenHeight);
    D_uvgfxmgr_rom_00402430[1] = 0x80200000;
    func_80002CD0((u32) D_uvgfxmgr_rom_00402430[0], sScreenWidth * 4 * sScreenHeight, &sp54, &sp50);
    if (D_uvgfxmgr_rom_00402440 == 0) {
        temp_lo = sScreenWidth * 2 * sScreenHeight;
        D_uvgfxmgr_rom_00402260 = 0x80400000 - temp_lo;
        func_80002CD0((u32) D_uvgfxmgr_rom_00402260, temp_lo, &sp54, &sp50);
    }
    for (i = 0; i < 2; i++) {
        if (sGfxDisplayListBase[i] == NULL) {
            sGfxDisplayListBase[0] = (Gfx *) 0x80300000;
            sGfxDisplayListBase[1] = (Gfx *) sGfxDisplayListBase[0] + D_uvgfxmgr_rom_0040244C;
            func_80002CD0(sGfxDisplayListBase[0], D_uvgfxmgr_rom_0040244C * 8, &sp54, &sp50);
            func_80002CD0(sGfxDisplayListBase[1], D_uvgfxmgr_rom_0040244C * 8, &sp54, &sp50);
        }
    }

    D_uvgfxmgr_rom_00402628 = uvLoadModule('CBCK');
    D_uvgfxmgr_rom_00402620 = D_uvgfxmgr_rom_00402628->unk4(D_uvgfxmgr_rom_00402446);
    D_uvgfxmgr_rom_00402624 = D_uvgfxmgr_rom_00402628->unk4(D_uvgfxmgr_rom_00402448);
    sGfxFbIndex = 0;
    D_uvgfxmgr_rom_00402460 = 1;
    sGfxDisplayListHead = sGfxDisplayListBase[sGfxFbIndex];
    D_uvgfxmgr_rom_00402484 = &D_uvgfxmgr_rom_00402488[sGfxFbIndex];
    D_uvgfxmgr_rom_00402470[0] = 0;
    D_uvgfxmgr_rom_00402470[1] = 0;
    D_uvgfxmgr_rom_00402478 = 0;
    D_uvgfxmgr_rom_00402420[2] = 0.0f;
    D_uvgfxmgr_rom_00402420[1] = (f32) D_uvgfxmgr_rom_00402420[2];
    D_uvgfxmgr_rom_00402420[0] = D_uvgfxmgr_rom_00402420[2];
    osCreateMesgQueue(&D_uvgfxmgr_rom_004025B8, &D_uvgfxmgr_rom_00402590, 0xA);
    osCreateMesgQueue(&D_uvgfxmgr_rom_004025F8, &D_uvgfxmgr_rom_004025D0, 0xA);
    _uvScAddClient((OSSched *) gScheduler, &D_uvgfxmgr_rom_00402610, &D_uvgfxmgr_rom_004025F8);
    if (D_uvgfxmgr_rom_00402410 & 0xF) {
        sGfxTaskOutputBufferStart = D_uvgfxmgr_rom_00402410 + 8;
    } else {
        sGfxTaskOutputBufferStart = D_uvgfxmgr_rom_00402410;
    }
    sGfxTaskOutputBufferEnd = sGfxTaskOutputBufferStart + 0x2080;
    if (osViGetCurrentFramebuffer() == (void *) 0x100000) {
        uvMemSet(D_uvgfxmgr_rom_00402434, 0U, sScreenWidth * 2 * sScreenHeight);
        osViBlack(FALSE);
        osViSwapBuffer(D_uvgfxmgr_rom_00402434);
        D_uvgfxmgr_rom_00402466 = 0;
        D_uvgfxmgr_rom_00402464 = 1;
    } else if (osViGetCurrentFramebuffer() == (void *) D_uvgfxmgr_rom_00402430[0]) {
        D_uvgfxmgr_rom_00402466 = 1;
        D_uvgfxmgr_rom_00402464 = 0;
    } else {
        D_uvgfxmgr_rom_00402466 = 0;
        D_uvgfxmgr_rom_00402464 = 1;
    }
    D_uvgfxmgr_rom_0040243C = (void *) D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
    D_uvgfxmgr_rom_00402264 = 0;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgfxmgr_rom/__entrypoint_func_uvgfxmgr_rom_400000.s")
#endif

void func_uvgfxmgr_rom_00400794(void) {
    func_uvgfxmgr_rom_00402090();
    D_uvgfxmgr_rom_00402628->unk8(D_uvgfxmgr_rom_00402620);
    D_uvgfxmgr_rom_00402628->unk8(D_uvgfxmgr_rom_00402624);
    uvUnloadModule('CBCK');
}

void func_uvgfxmgr_rom_004007F8(void) {
    s32 temp_a0;

    if (D_uvgfxmgr_rom_00402478 == 1) {
        temp_a0 = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402464];
        if (D_uvgfxmgr_rom_0040261C != NULL) {
            D_uvgfxmgr_rom_0040261C(temp_a0, D_uvgfxmgr_rom_00402260);
            osWritebackDCache(temp_a0, sScreenWidth * sScreenHeight * 2);
        }
        D_uvgfxmgr_rom_00402420[D_uvgfxmgr_rom_00402466] = uvClkGetSec(103);
        uvClkReset(103);
        func_80004958(0, 42);
        D_uvgfxmgr_rom_00402460 = sGfxFbIndex++;
        if (sGfxFbIndex >= 2) {
            sGfxFbIndex = 0;
        }
        D_uvgfxmgr_rom_00402464 = D_uvgfxmgr_rom_00402466++;

        if (D_uvgfxmgr_rom_00402466 >= D_uvgfxmgr_rom_0040242C) {
            D_uvgfxmgr_rom_00402466 = 0;
        }
        D_uvgfxmgr_rom_00402470[sGfxFbIndex] = 1;
        D_uvgfxmgr_rom_00402484 = &D_uvgfxmgr_rom_00402488[sGfxFbIndex];
        D_uvgfxmgr_rom_0040243C = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
        sGfxDisplayListHead = sGfxDisplayListBase[sGfxFbIndex];
    }
    gSPSegment(sGfxDisplayListHead++, 0, 0);
    gDPPipeSync(sGfxDisplayListHead++);
    gDPSetColorImage(sGfxDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, sScreenWidth,
                     osVirtualToPhysical(D_uvgfxmgr_rom_0040243C));
    gDPSetDepthImage(sGfxDisplayListHead++, osVirtualToPhysical(D_uvgfxmgr_rom_00402260));
    gDPPipeSync(sGfxDisplayListHead++);
    D_uvgfxmgr_rom_00402450 = 0;
    if (D_uvgfxmgr_rom_00402620 != 0) {
        D_uvgfxmgr_rom_00402628->unkC(D_uvgfxmgr_rom_00402620, 0);
    }
}

Gfx **uvGetDisplayListHead(void) {
    return &sGfxDisplayListHead;
}

void uvGfxDisplayList(Gfx* dl) {
    gSPDisplayList(sGfxDisplayListHead++, dl);
}

s32 func_uvgfxmgr_rom_00400AB8(s32 arg0) {
    switch (arg0) { /* irregular */
        case 1:
            return D_uvgfxmgr_rom_00402620;
        case 2:
            return D_uvgfxmgr_rom_00402624;
        default:
            return 0;
    }
}

void (*func_uvgfxmgr_rom_00400AF0(void (*arg0)(void *, void *)))(void *, void *) {
    void (*temp_v0)(void *, void *);

    temp_v0 = D_uvgfxmgr_rom_0040261C;
    D_uvgfxmgr_rom_0040261C = arg0;
    return temp_v0;
}

s32 func_uvgfxmgr_rom_00400B04(s32 arg0) {
    s32 temp_v0;

    temp_v0 = D_uvgfxmgr_rom_00402618;
    D_uvgfxmgr_rom_00402618 = arg0;
    return temp_v0;
}

void func_uvgfxmgr_rom_00400B18(s32 arg0) {
    D_uvgfxmgr_rom_00402264 = arg0;
}


void func_uvgfxmgr_rom_00400B24(void) {
    s32 pad;
    UvGfxStruct *temp_a3;
    s32 pad2[2];
    s32 temp_a0;

    gDPFullSync(sGfxDisplayListHead++);
    gSPEndDisplayList(sGfxDisplayListHead++);
    sGfxElementCount = sGfxDisplayListHead - sGfxDisplayListBase[sGfxFbIndex];
    temp_a3 = &D_uvgfxmgr_rom_00402488[sGfxFbIndex];
    if (D_uvgfxmgr_rom_00402264 == 1) {
        temp_a3->unkC = D_uvgfxmgr_rom_00402260;
    } else {
        temp_a3->unkC = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
    }
    temp_a3->unk0 = 0;
    temp_a3->unk50 = &D_uvgfxmgr_rom_004025B8;
    temp_a3->unk54 = &D_uvgfxmgr_rom_00402268[D_uvgfxmgr_rom_00402466];
    temp_a3->unk8 = 0x53;
    temp_a3->task.t.type = M_GFXTASK;
    temp_a3->task.t.flags = 0;
    temp_a3->task.t.ucode_boot = (u64 *) rspBootTextStart;
    temp_a3->task.t.ucode_boot_size = rspBootTextEnd - rspBootTextStart;
    temp_a3->task.t.ucode_size = 0x1000;
    temp_a3->task.t.ucode_data_size = 0x800;
    temp_a3->task.t.dram_stack = sGfxDramStack;
    temp_a3->task.t.dram_stack_size = 0x400;
    temp_a3->task.t.data_ptr = (u64 *) sGfxDisplayListBase[sGfxFbIndex];
    temp_a3->task.t.data_size = sGfxElementCount * sizeof(Gfx);
    temp_a3->task.t.yield_data_ptr = sGfxYieldData;
    temp_a3->task.t.yield_data_size = 0xC00;
    temp_a3->task.t.ucode = (u64 *) gspF3DEX2_fifoTextStart;
    temp_a3->task.t.ucode_data = gspF3DEX2_fifoDataStart;
    temp_a3->task.t.output_buff = sGfxTaskOutputBufferStart;
    temp_a3->task.t.output_buff_size = sGfxTaskOutputBufferEnd;
    func_80004958(0, 0x2B);
    if (D_uvgfxmgr_rom_00402618 != NULL) {
        D_uvgfxmgr_rom_00402618(D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466],
                                D_uvgfxmgr_rom_00402260);
        osWritebackDCache((void *) D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466],
                          sScreenWidth * sScreenHeight * 2);
    }
    osWritebackDCacheAll();
    osSendMesg(_uvScGetCmdQ(gScheduler), temp_a3, 1);
    if (D_uvgfxmgr_rom_00402624 != 0) {
        D_uvgfxmgr_rom_00402628->unkC(D_uvgfxmgr_rom_00402624, 0);
    }
    if (D_uvgfxmgr_rom_00402478 == 0) {
        if (D_uvgfxmgr_rom_0040247C != 0) {
            func_uvgfxmgr_rom_00400F20(0U);
        }
        D_uvgfxmgr_rom_0040247C = 1;
        temp_a0 = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402464];
        if (D_uvgfxmgr_rom_0040261C != NULL) {
            D_uvgfxmgr_rom_0040261C((void *) temp_a0, D_uvgfxmgr_rom_00402260);
            osWritebackDCache((void *) temp_a0, sScreenWidth * sScreenHeight * 2);
        }
        D_uvgfxmgr_rom_00402420[D_uvgfxmgr_rom_00402466] = (f32) uvClkGetSec(0x67);
        uvClkReset(0x67);
        func_80004958(0, 0x2A);
        D_uvgfxmgr_rom_00402460 = sGfxFbIndex++;
        if (sGfxFbIndex >= 2) {
            sGfxFbIndex = 0;
        }
        D_uvgfxmgr_rom_00402464 = D_uvgfxmgr_rom_00402466++;
        if (D_uvgfxmgr_rom_00402466 >= (s32) D_uvgfxmgr_rom_0040242C) {
            D_uvgfxmgr_rom_00402466 = 0;
        }
        D_uvgfxmgr_rom_0040243C = (void *) D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
        sGfxDisplayListHead = (Gfx *) sGfxDisplayListBase[sGfxFbIndex];
    }
}

void func_uvgfxmgr_rom_00400F20(u16 arg0) {
    if (!(arg0)) {
        osRecvMesg(&D_uvgfxmgr_rom_004025B8, NULL, 1);
    }
}

void func_uvgfxmgr_rom_00400F58(s32 arg0) {
    D_uvgfxmgr_rom_00402478 = arg0;
}

s32 func_uvgfxmgr_rom_00400F64(void) {
    if ((D_uvgfxmgr_rom_00402470[D_uvgfxmgr_rom_00402460] != 0)
        && (osRecvMesg(&D_uvgfxmgr_rom_004025B8, NULL, 0) == 0)) {
        D_uvgfxmgr_rom_00402470[D_uvgfxmgr_rom_00402460] = 0;
    }
    D_uvgfxmgr_rom_0040247C = 1;
    return D_uvgfxmgr_rom_00402470[D_uvgfxmgr_rom_00402460] == 0;
}

void func_uvgfxmgr_rom_00400FF8(f32 arg0) {
    D_uvgfxmgr_rom_00402480 = arg0;
}

f32 func_uvgfxmgr_rom_00401004(void) {
    if (D_uvgfxmgr_rom_00402480 > 0.0f) {
        return D_uvgfxmgr_rom_00402480;
    }
    return D_uvgfxmgr_rom_00402420[D_uvgfxmgr_rom_00402460];
}

void uvGfxEnableGamma(s32 enable) {
    if (enable) {
        osViSetSpecialFeatures(OS_VI_GAMMA_ON);
        return;
    }
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
}

void func_uvgfxmgr_rom_0040107C(f32 red, f32 green, f32 blue, f32 alpha) {
    u32 r;
    u32 g;
    u32 b;
    u32 a;

    gDPPipeSync(sGfxDisplayListHead++);
    r = red * 255.0f;
    g = green * 255.0f;
    b = blue * 255.0f;
    a = alpha * 255.0f;
    gDPSetPrimColor(sGfxDisplayListHead++, 0, 0, r & 0xFF, g & 0xFF, (s32) b & 0xFF, a & 0xFF);

    gDPSetFillColor(sGfxDisplayListHead++,
                    GPACK_RGBA5551(r & 0xFF, g & 0xFF, (s32) b & 0xFF, a & 0xFF) << 16
                        | GPACK_RGBA5551(r & 0xFF, g & 0xFF, (s32) b & 0xFF, a & 0xFF));
}

void func_uvgfxmgr_rom_00401370(s8 red, s8 green, s8 blue, s8 alpha) {
    gDPSetPrimColor(sGfxDisplayListHead++, 0, 0, red, green, blue, alpha);
    gDPSetFillColor(sGfxDisplayListHead++, GPACK_RGBA5551(red, green, blue, alpha) << 16
                                               | GPACK_RGBA5551(red, green, blue, alpha));
}

void func_uvgfxmgr_rom_00401430(f32 red, f32 green, f32 blue, f32 alpha) {
    u32 r;
    u32 g;
    u32 b;
    u32 a;
    r = red * 255.0f;
    g = green * 255.0f;
    b = blue * 255.0f;
    a = alpha * 255.0f;
    gDPSetEnvColor(sGfxDisplayListHead++, r & 0xff, g & 0xff, b & 0xff, a & 0xff);
}

void func_uvgfxmgr_rom_004016AC(s8 r, s8 g, s8 b, s8 a) {
    gDPSetEnvColor(sGfxDisplayListHead++, r, g, b, a);
}

void func_uvgfxmgr_rom_00401714(s32 arg0, s32 arg1, u32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    gDPSetConvert(sGfxDisplayListHead++, arg0, arg1, arg2, arg3, arg4, arg5);
}

void func_uvgfxmgr_rom_00401788(u8 red, u8 green, u8 alpha) {
    gDPPipeSync(sGfxDisplayListHead++);

    gDPSetCycleType(sGfxDisplayListHead++, G_CYC_FILL);
    gDPSetFillColor(sGfxDisplayListHead++,
                    GPACK_RGBA5551(red, green, alpha, 1) << 16 | GPACK_RGBA5551(red, green, alpha, 1));

    gDPFillRectangle(sGfxDisplayListHead++, D_uvgfxmgr_rom_00402408,
                     sScreenHeight - D_uvgfxmgr_rom_0040240E, D_uvgfxmgr_rom_0040240A,
                     sScreenHeight - D_uvgfxmgr_rom_0040240C);
    gDPPipeSync(sGfxDisplayListHead++);
    gDPSetCycleType(sGfxDisplayListHead++, G_CYC_2CYCLE);
    gDPSetFillColor(sGfxDisplayListHead++, 0x10001);
}

void func_uvgfxmgr_rom_00401914(void) {
    gDPPipeSync(sGfxDisplayListHead++);
    gDPSetCycleType(sGfxDisplayListHead++, G_CYC_FILL);
    gDPSetFillColor(sGfxDisplayListHead++, 0x00000000);
    gDPFillRectangle(sGfxDisplayListHead++, 0, sScreenWidth, 0, sScreenHeight);
    gDPPipeSync(sGfxDisplayListHead++);
    gDPSetCycleType(sGfxDisplayListHead++, G_CYC_2CYCLE);
    gDPSetFillColor(sGfxDisplayListHead++, 0x10001);
}

void uvGfxClipRect(uvGfxViewport *vp, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 var_a2;
    s32 var_a3;

    vp->x0 = arg1;
    vp->x1 = arg2;
    vp->y0 = arg3;
    vp->y1 = arg4;
    if (vp->x0 < 0) {
        vp->x0 = 0;
    } else if (vp->x0 > sScreenWidth) {
        vp->x0 = sScreenWidth;
    }

    if (vp->x1 < 0) {
        vp->x1 = 0;
    } else if (vp->x1 > sScreenWidth) {
        vp->x1 = sScreenWidth;
    }

    if (vp->y1 < 0) {
        vp->y1 = 0;
    } else if (vp->y1 > sScreenHeight) {
        vp->y1 = sScreenHeight;
    }

    if (vp->y0 < 0) {
        vp->y0 = 0;
    } else if (vp->y0 > sScreenHeight) {
        vp->y0 = sScreenHeight;
    }

    vp->unk0 = vp->x0;
    if (vp->unk0 < 0) {
        vp->unk0 = 0;
    }
    vp->unk2 = vp->x1;
    if (vp->unk2 > sScreenWidth - 1) {
        vp->unk2 = sScreenWidth - 1;
    }
    vp->unk4 = vp->y0;
    if (vp->unk4 < 0) {
        vp->unk4 = 0;
    }
    vp->unk6 = vp->y1;
    if (vp->unk6 > sScreenHeight - 1) {
        vp->unk6 = sScreenHeight - 1;
    }

    var_a2 = vp->unk2 - vp->unk0;
    var_a3 = vp->unk6 - vp->unk4;

    vp->vp.vp.vscale[0] = (var_a2 << 1);
    vp->vp.vp.vscale[1] = (var_a3 << 1);
    vp->vp.vp.vscale[2] = 0x1FF;
    vp->vp.vp.vscale[3] = 0;
    vp->vp.vp.vtrans[0] = (u16) ((vp->unk0 + (var_a2 >> 1)) & 0xFFFF) << 2;
    vp->vp.vp.vtrans[1] = (u16) (((sScreenHeight - vp->unk4) - (var_a3 >> 1)) & 0xFFFF) << 2;
    vp->vp.vp.vtrans[2] = 0x1FF;
    vp->vp.vp.vtrans[3] = 0;
    D_uvgfxmgr_rom_00402408 = vp->x0;
    D_uvgfxmgr_rom_0040240A = vp->x1;
    D_uvgfxmgr_rom_0040240C = vp->y0;
    D_uvgfxmgr_rom_0040240E = vp->y1;
}

void func_uvgfxmgr_rom_00401BD4(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    uvGfxViewport *temp_t8;

    temp_t8 = &D_uvgfxmgr_rom_004022C8[D_uvgfxmgr_rom_00402450];
    if (D_uvgfxmgr_rom_00402450 < 0xB) {
        uvGfxClipRect(temp_t8, arg0, arg1, arg2, arg3);
        func_uvgfxmgr_rom_00401C5C(D_uvgfxmgr_rom_00402450);
        D_uvgfxmgr_rom_00402450 += 1;
    }
}

void func_uvgfxmgr_rom_00401C5C(s32 vp_id) {
    uvGfxViewport *vp;
    vp = &D_uvgfxmgr_rom_004022C8[vp_id];

    gSPViewport(sGfxDisplayListHead++, OS_PHYSICAL_TO_K0(&vp->vp));
    gDPSetScissor(sGfxDisplayListHead++, G_SC_NON_INTERLACE, vp->x0, sScreenHeight - vp->y1, vp->x1,
                  sScreenHeight - vp->y0);
    D_uvgfxmgr_rom_00402408 = vp->x0;
    D_uvgfxmgr_rom_0040240A = vp->x1;
    D_uvgfxmgr_rom_0040240C = vp->y0;
    D_uvgfxmgr_rom_0040240E = vp->y1;
}

void func_uvgfxmgr_rom_00401D94(s16 persp) {
    gSPPerspNormalize(sGfxDisplayListHead++, persp);
}

void func_uvgfxmgr_rom_00401DC4(void) {
    uvGfxViewport *vp;

    if ((D_uvgfxmgr_rom_00402260 != NULL) && (D_uvgfxmgr_rom_0040243C != NULL)) {
        vp = &D_uvgfxmgr_rom_004022C8[D_uvgfxmgr_rom_00402450 - 1];

        gDPPipeSync(sGfxDisplayListHead++);
        gDPSetRenderMode(sGfxDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        gDPPipeSync(sGfxDisplayListHead++);
        gDPSetCycleType(sGfxDisplayListHead++, G_CYC_FILL);
        gDPPipeSync(sGfxDisplayListHead++);
        gDPSetFillColor(sGfxDisplayListHead++, 0xFFFCFFFC);
        gDPPipeSync(sGfxDisplayListHead++);
        gDPSetColorImage(sGfxDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, sScreenWidth,
                         osVirtualToPhysical(D_uvgfxmgr_rom_00402260));
        gDPPipeSync(sGfxDisplayListHead++);
        gDPFillRectangle(sGfxDisplayListHead++, vp->x0, sScreenHeight - vp->y1, vp->x1,
                         sScreenHeight - vp->y0);
        gDPPipeSync(sGfxDisplayListHead++);
        gDPSetColorImage(sGfxDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, sScreenWidth,
                         osVirtualToPhysical(D_uvgfxmgr_rom_0040243C));
        gDPPipeSync(sGfxDisplayListHead++);
        gDPSetCycleType(sGfxDisplayListHead++, G_CYC_2CYCLE);
        gDPPipeSync(sGfxDisplayListHead++);
    }
}

void *func_uvgfxmgr_rom_0040204C(void) {
    return D_uvgfxmgr_rom_00402260;
}

s32 func_uvgfxmgr_rom_00402058(void) {
    return D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
}

s32 func_uvgfxmgr_rom_00402074(void) {
    return D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402464];
}

void func_uvgfxmgr_rom_00402090(void) {
    if (D_uvgfxmgr_rom_0040247C != 0) {
        func_uvgfxmgr_rom_00400F20(0U);
        D_uvgfxmgr_rom_0040247C = 0;
    }
}

void uvCopyFrameBuf(s32 arg0) {
    s32 var_a1;
    s32 var_a0;

    if ((arg0 >= 0) && (arg0 < 2)) {
        if (arg0 != 0) {
            var_a1 = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
            var_a0 = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402464];
        } else {
            var_a1 = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402464];
            var_a0 = D_uvgfxmgr_rom_00402430[D_uvgfxmgr_rom_00402466];
        }
        func_uvgfxmgr_rom_00402090();
        _uvMediaCopy((void *) var_a0, (void *) var_a1, sScreenWidth * sScreenHeight * 2);
        osWritebackDCacheAll();
    }
}

void uvSetScreenHeight(s32 height) {
    sScreenHeight = height;
}

void uvSetScreenWidth(s32 width) {
    sScreenWidth = width;
}

s32 uvGetScreenHeight(void) {
    return sScreenHeight;
}

s32 uvGetScreenWidth(void) {
    return sScreenWidth;
}

void func_uvgfxmgr_rom_004021C8(void) {
    gSPDisplayList(sGfxDisplayListHead++, D_uvgfxmgr_rom_00402278);
}

void uvGfxSetDepthSource(s32 usePrimitiveDepth) {
    gDPSetDepthSource(sGfxDisplayListHead++, usePrimitiveDepth ? G_ZS_PRIM : G_ZS_PIXEL);
}

void uvGfxPrimDepth(s32 z) {
    gDPSetPrimDepth(sGfxDisplayListHead++, z, 0);
}
