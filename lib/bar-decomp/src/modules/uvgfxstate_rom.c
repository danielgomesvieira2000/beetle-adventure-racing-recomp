// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "uvcback_rom.h"
#include "uvfmtx_rom.h"
#include "uvgfxmgr_rom.h"
#include "uvimtx_rom.h"
#include "uvmath_rom.h"

typedef struct UnkStruct_uvgfxstate_rom_00402D18_s {
    char pad[4];
    void (*unk4)(s32);
} UnkStruct_uvgfxstate_rom_00402D18;

typedef struct UnkStruct_uvgfxstate_rom_00402D14_s {
    char pad[0x10];
    s32 (*unk10)(s32);
    void (*unk14)(void);
} UnkStruct_uvgfxstate_rom_00402D14;

typedef struct UnkStruct_80001724_s {
    /* 0x00 */ char pad0[0x14];
    /* 0x14 */ s32 unk14;                           /* inferred */
    /* 0x18 */ char pad18[4];
    /* 0x1C */ u16 unk1C;                           /* inferred */
    /* 0x1E */ u16 unk1E;                            /* inferred */
} UnkStruct_80001724;                               /* size = 0x1F */

extern s32 D_uvgfxstate_rom_00402C48;

void __entrypoint_func_uvgfxstate_rom_400000(UvGfxState_Rom_Exports* exports);
void uvGfxStateDraw(uvGfxState *); /* extern */
void uvGfxClampLook(LookAt *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
                    f32 arg8, f32 arg9);
void func_uvgfxstate_rom_00400C68(LookAt *arg0, Hilite *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5,
                                  f32 arg6, f32 arg7, f32 arg8, f32 arg9, f32 arg10, f32 arg11,
                                  f32 arg12, f32 arg13, f32 arg14, f32 arg15, f32 arg16, s32 arg17,
                                  s32 arg18);

void func_uvgfxstate_rom_00400390(void);
void func_uvgfxstate_rom_00400440(u32 mode);
void func_uvgfxstate_rom_0040049C(Mtx4F *arg0, s32 arg1);
void func_uvgfxstate_rom_00401314(s32 arg0);
void func_uvgfxstate_rom_00401354(s32 arg0);
void func_uvgfxstate_rom_00401398(s32 arg0);
void func_uvgfxstate_rom_004013DC(uvGfxState *gs, s32 arg1);
void func_uvgfxstate_rom_004013F0(uvGfxState *gs, s32 arg1);
void func_uvgfxstate_rom_00401400(uvGfxState *gs, s32 arg1);
void func_uvgfxstate_rom_00401418(uvGfxState *arg0, s32 arg1, s32 arg2);
s32 func_uvgfxstate_rom_0040143C(uvGfxState *arg0, s32 arg1);
void func_uvgfxstate_rom_00401460(s32 arg0);
void func_uvgfxstate_rom_0040146C(s32 arg0);
void func_uvgfxstate_rom_00401498(uvGfxState *gs);
void func_uvgfxstate_rom_00401CE8(void);
void func_uvgfxstate_rom_00401DD0(s32 arg0);
void func_uvgfxstate_rom_00401E60(s32 arg0);
void func_uvgfxstate_rom_00401F54(f32 arg0, f32 arg1);
void func_uvgfxstate_rom_004020DC(f32 *arg0, f32 *arg1);
void func_uvgfxstate_rom_004020F8(s32 arg0, s32 arg1);
void func_uvgfxstate_rom_00402218(void);
void func_uvgfxstate_rom_00402254(void);
void func_uvgfxstate_rom_004022B0(void);
s32 func_uvgfxstate_rom_004029B8(u32 arg0);
void func_uvgfxstate_rom_00402B14(s32 arg0, s32 arg1);
void func_uvgfxstate_rom_00402B28(s32 arg0, Vec3F arg1);
void uvGfxSync(void); /* extern */
void func_uvgfxstate_rom_00402350(s32 arg0, s32 arg1, s32 arg2, s32 arg3, f32 arg4, f32 arg5, f32 arg6,
                                  f32 arg7);
// .bss
extern UvFMtx_Rom_Exports *sUvFmtxExports;
extern UvGfxMgr_Exports *sUvGfxMgrExports;
extern UvMath_Exports *sUvMathExports;
extern UvImtx_Rom_Exports *sUvImtxExports;

// Not classified yet
extern Vec3F D_uvgfxstate_rom_00402C60[];
extern s32 D_uvgfxstate_rom_00402D30;
extern s32 D_uvgfxstate_rom_00402DEC;
extern s32 D_uvgfxstate_rom_00402DF0;
extern LookAt *D_uvgfxstate_rom_00402DF8[];
extern s32 D_uvgfxstate_rom_00402E00;
extern s32 D_uvgfxstate_rom_00402E04;
extern Hilite *D_uvgfxstate_rom_00402E08[];
extern UvFMtx_Rom_Exports *sUvFmtxExports;
extern UvGfxMgr_Exports *sUvGfxMgrExports;
extern UvMath_Exports *sUvMathExports;
extern UvImtx_Rom_Exports *sUvImtxExports;
extern UvCback_Exports *sUvCbackExports;
extern s32 D_uvgfxstate_rom_00402D00;
extern s32 D_uvgfxstate_rom_00402D30;
extern s32 D_uvgfxstate_rom_00402D60[];
extern s32 D_uvgfxstate_rom_00402C50;
extern s32 D_uvgfxstate_rom_00402C4C;
extern s32 D_uvgfxstate_rom_00402D38[];
extern s32 D_uvgfxstate_rom_00402D40[];
extern s32 D_uvgfxstate_rom_00402D48[];
extern s32 D_uvgfxstate_rom_00402D50[];
extern s32 D_uvgfxstate_rom_00402D58[];
extern s32 D_uvgfxstate_rom_00402C5C;
extern f32 D_uvgfxstate_rom_00402C54;
extern f32 D_uvgfxstate_rom_00402C58;
extern Gfx D_uvgfxstate_rom_00402C78[];
extern UnkStruct_uvgfxstate_rom_00402D14 *D_uvgfxstate_rom_00402D14;
extern UnkStruct_uvgfxstate_rom_00402D18 *D_uvgfxstate_rom_00402D18;
extern s32 D_uvgfxstate_rom_00402D68[];
extern u8 D_uvgfxstate_rom_00402DE8;
extern s32 D_uvgfxstate_rom_00402C40;
extern s32 D_uvgfxstate_rom_00402D2C;

extern s32 D_uvgfxstate_rom_00402D04;
extern s32 D_uvgfxstate_rom_00402D08;
extern void* D_uvgfxstate_rom_00402D10;

void __entrypoint_func_uvgfxstate_rom_400000(UvGfxState_Rom_Exports* exports) {
    s32 i;
    s32* temp_v0;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvgfxstate_rom_00400440 = func_uvgfxstate_rom_00400440;
    exports->func_uvgfxstate_rom_0040049C = func_uvgfxstate_rom_0040049C;
    exports->func_uvgfxstate_rom_00401314 = func_uvgfxstate_rom_00401314;
    exports->func_uvgfxstate_rom_00401354 = func_uvgfxstate_rom_00401354;
    exports->func_uvgfxstate_rom_00401398 = func_uvgfxstate_rom_00401398;
    exports->func_uvgfxstate_rom_004013DC = func_uvgfxstate_rom_004013DC;
    exports->func_uvgfxstate_rom_004013F0 = func_uvgfxstate_rom_004013F0;
    exports->func_uvgfxstate_rom_00401400 = func_uvgfxstate_rom_00401400;
    exports->func_uvgfxstate_rom_00401418 = func_uvgfxstate_rom_00401418;
    exports->func_uvgfxstate_rom_00400390 = func_uvgfxstate_rom_00400390;
    exports->func_uvgfxstate_rom_0040143C = func_uvgfxstate_rom_0040143C;
    exports->func_uvgfxstate_rom_00402218 = func_uvgfxstate_rom_00402218;
    exports->func_uvgfxstate_rom_00401460 = func_uvgfxstate_rom_00401460;
    exports->func_uvgfxstate_rom_00402254 = func_uvgfxstate_rom_00402254;
    exports->func_uvgfxstate_rom_0040146C = func_uvgfxstate_rom_0040146C;
    exports->func_uvgfxstate_rom_004022B0 = func_uvgfxstate_rom_004022B0;
    exports->func_uvgfxstate_rom_00401498 = func_uvgfxstate_rom_00401498;
    exports->func_uvgfxstate_rom_00402350 = func_uvgfxstate_rom_00402350;
    exports->uvGfxStateDraw = uvGfxStateDraw;
    exports->func_uvgfxstate_rom_004029B8 = func_uvgfxstate_rom_004029B8;
    exports->func_uvgfxstate_rom_00401CE8 = func_uvgfxstate_rom_00401CE8;
    exports->func_uvgfxstate_rom_00402B14 = func_uvgfxstate_rom_00402B14;
    exports->func_uvgfxstate_rom_00401E60 = func_uvgfxstate_rom_00401E60;
    exports->func_uvgfxstate_rom_00402B28 = func_uvgfxstate_rom_00402B28;
    exports->func_uvgfxstate_rom_00401F54 = func_uvgfxstate_rom_00401F54;
    exports->uvGfxSync = uvGfxSync;
    exports->func_uvgfxstate_rom_004020DC = func_uvgfxstate_rom_004020DC;
    exports->func_uvgfxstate_rom_004020F8 = func_uvgfxstate_rom_004020F8;
    exports->unk70 = &D_uvgfxstate_rom_00402D00;
    exports->unk74 = &D_uvgfxstate_rom_00402D04;
    exports->unk78 = &D_uvgfxstate_rom_00402D08;
    #line 1
    temp_v0 = uvGetSystemProp(0x14);
    #line 45
    if (temp_v0 == NULL) {
        D_uvgfxstate_rom_00402DEC = 30;
        if (D_uvgfxstate_rom_00402E00) {} // FAKE
        D_uvgfxstate_rom_00402E00 = 1;
    } else {
        if (temp_v0[0] != 0) {
            D_uvgfxstate_rom_00402DEC = temp_v0[0];
        } else {
            D_uvgfxstate_rom_00402DEC = 0x1E;
        }

        if (temp_v0[1] != 0) {
            D_uvgfxstate_rom_00402E00 = temp_v0[1];
        } else {
            D_uvgfxstate_rom_00402E00 = 1;
        }
    }

    for (i = 0; i < 2; i++) {
        if (D_uvgfxstate_rom_00402DEC > 0) {
            D_uvgfxstate_rom_00402DF8[i] = _uvMemAlloc(D_uvgfxstate_rom_00402DEC * 0x20, 8U);
        }
        if (D_uvgfxstate_rom_00402E00 > 0) {
            D_uvgfxstate_rom_00402E08[i] = _uvMemAlloc(D_uvgfxstate_rom_00402E00 * 0x10, 8U);
        }
    }

    sUvGfxMgrExports = uvLoadModule('GMGR');
    D_uvgfxstate_rom_00402D10 = uvLoadModule('GEOM');
    D_uvgfxstate_rom_00402D14 = uvLoadModule('TEXT');
    D_uvgfxstate_rom_00402D18 = uvLoadModule('LGHT');
    sUvCbackExports = uvLoadModule('CBCK');
    sUvImtxExports = uvLoadModule('IMTX');
    sUvFmtxExports = uvLoadModule('FMTX');
    sUvMathExports = uvLoadModule('MATH');
    D_uvgfxstate_rom_00402C4C = 0;
    D_uvgfxstate_rom_00402C50 = 0;
    sUvCbackExports->func_uvcback_rom_0040016C(sUvGfxMgrExports->func_uvgfxmgr_rom_00400AB8(1), func_uvgfxstate_rom_00401DD0, 0, 0);
}

// Module cleanup function
void func_uvgfxstate_rom_00400390(void) {
    sUvCbackExports->func_uvcback_rom_00400320(sUvGfxMgrExports->func_uvgfxmgr_rom_00400AB8(1),
                                               (s32) func_uvgfxstate_rom_00401DD0);
    uvUnloadModule('GMGR');
    uvUnloadModule('GEOM');
    uvUnloadModule('TEXT');
    uvUnloadModule('CBCK');
    uvUnloadModule('LGHT');
    uvUnloadModule('IMTX');
    uvUnloadModule('FMTX');
    uvUnloadModule('MATH');
}

void func_uvgfxstate_rom_00400440(u32 mode) {
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    uvGfxSync();
    gDPSetColorDither(gdl[0]++, mode);
}

void func_uvgfxstate_rom_0040049C(Mtx4F *arg0, s32 arg1) {
    Mtx4F spC0;
    Mtx4F sp80;
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();

    if (D_uvgfxstate_rom_00402DEC > D_uvgfxstate_rom_00402DF0) {
        sUvFmtxExports->func_004029B8(&spC0);
        sUvFmtxExports->func_00401604(&sp80, &spC0);
        if (arg1 != 0) {
            uvGfxClampLook(
                &D_uvgfxstate_rom_00402DF8[D_uvgfxstate_rom_00402D30][D_uvgfxstate_rom_00402DF0],
                sp80.m[3][0], sp80.m[3][1], sp80.m[3][2], arg0->m[3][0], arg0->m[3][1], arg0->m[3][2],
                sp80.m[1][0], sp80.m[1][1], sp80.m[1][2]);
            goto lookAt; // Required to match
        }
        if (D_uvgfxstate_rom_00402E04 < D_uvgfxstate_rom_00402E00) {
            func_uvgfxstate_rom_00400C68(
                &D_uvgfxstate_rom_00402DF8[D_uvgfxstate_rom_00402D30][D_uvgfxstate_rom_00402DF0],
                &D_uvgfxstate_rom_00402E08[D_uvgfxstate_rom_00402D30][D_uvgfxstate_rom_00402E04], 0.0f,
                0.0f, 500.0f, arg0->m[3][0], arg0->m[3][1], arg0->m[3][2], 0.0f, 1.0f, 0.0f,
                D_uvgfxstate_rom_00402C60->x, D_uvgfxstate_rom_00402C60->y,
                D_uvgfxstate_rom_00402C60->z, D_uvgfxstate_rom_00402C60[1].x,
                D_uvgfxstate_rom_00402C60[1].y, D_uvgfxstate_rom_00402C60[1].z, 32, 32);
            gDPSetHilite1Tile(gdl[0]++, G_TX_RENDERTILE, &D_uvgfxstate_rom_00402E08[D_uvgfxstate_rom_00402D30][D_uvgfxstate_rom_00402E04], 32, 32);
            gDPSetHilite2Tile(gdl[0]++, G_TX_MIRROR, &D_uvgfxstate_rom_00402E08[D_uvgfxstate_rom_00402D30][D_uvgfxstate_rom_00402E04], 32, 32);
            D_uvgfxstate_rom_00402E04++;
        lookAt:
            uvGfxSync();
            gSPLookAt(gdl[0]++,
                      &D_uvgfxstate_rom_00402DF8[D_uvgfxstate_rom_00402D30][D_uvgfxstate_rom_00402DF0])
                D_uvgfxstate_rom_00402DF0++;
        }
    }
}

void uvGfxClampLook(LookAt *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
                    f32 arg8, f32 arg9) {
    f32 len;
    f32 sp58;
    f32 sp54;
    f32 sp50;
    f32 sp4C;
    f32 sp48;
    f32 sp44;

    sp58 = arg4 - arg1;
    sp54 = arg5 - arg2;
    sp50 = arg6 - arg3;

    len = sqrtf(SQ(sp58) + SQ(sp54) + SQ(sp50));
    if (len != 0.0f) {
        len = -1.0f / len;
    } else {
        len = -255.0f;
    }
    sp58 *= len;
    sp54 *= len;
    sp50 *= len;

    sp4C = (arg8 * sp50) - (arg9 * sp54);
    sp48 = (arg9 * sp58) - (arg7 * sp50);
    sp44 = (arg7 * sp54) - (arg8 * sp58);

    len = sqrtf(SQ(sp4C) + SQ(sp48) + SQ(sp44));
    if (len != 0.0f) {
        len = 1.0f / len;
    } else {
        len = 255.0f;
    }
    sp4C *= len;
    sp48 *= len;
    sp44 *= len;

    arg7 = (sp54 * sp44) - (sp50 * sp48);
    arg8 = (sp50 * sp4C) - (sp58 * sp44);
    arg9 = (sp58 * sp48) - (sp54 * sp4C);

    len = sqrtf(SQ(arg7) + SQ(arg8) + SQ(arg9));
    if (len != 0.0f) {
        len = 1.0f / len;
    } else {
        len = 255.0f;
    }
    arg7 *= len;
    arg8 *= len;
    arg9 *= len;

    arg0->l[0].l.dir[0] = (s32) MIN(sp4C * 128.0f, 127.0f) & 0xFF;
    arg0->l[0].l.dir[1] = (s32) MIN(sp48 * 128.0f, 127.0f) & 0xFF;
    arg0->l[0].l.dir[2] = (s32) MIN(sp44 * 128.0f, 127.0f) & 0xFF;

    arg0->l[1].l.dir[0] = (s32) MIN(arg7 * 128.0f, 127.0f) & 0xFF;
    arg0->l[1].l.dir[1] = (s32) MIN(arg8 * 128.0f, 127.0f) & 0xFF;
    arg0->l[1].l.dir[2] = (s32) MIN(arg9 * 128.0f, 127.0f) & 0xFF;
    arg0->l[0].l.col[0] = 0;
    arg0->l[0].l.col[1] = 0;
    arg0->l[0].l.col[2] = 0;
    arg0->l[0].l.pad1 = 0;
    arg0->l[0].l.colc[0] = 0;
    arg0->l[0].l.colc[1] = 0;
    arg0->l[0].l.colc[2] = 0;
    arg0->l[0].l.pad2 = 0;
    arg0->l[1].l.col[0] = 0;
    arg0->l[1].l.col[1] = 0x80;
    arg0->l[1].l.col[2] = 0;
    arg0->l[1].l.pad1 = 0;
    arg0->l[1].l.colc[0] = 0;
    arg0->l[1].l.colc[1] = 0x80;
    arg0->l[1].l.colc[2] = 0;
    arg0->l[1].l.pad2 = 0;
}

#if 0
extern f64 D_uvgfxstate_rom_00402C00;
extern f64 D_uvgfxstate_rom_00402C08;

void func_uvgfxstate_rom_00400C68(LookAt* arg0, Hilite* arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9, f32 arg10, f32 arg11, f32 arg12, f32 arg13, f32 arg14, f32 arg15, f32 arg16, s32 arg17, s32 arg18) {
    f32 temp_fa0;
    f32 temp_fa1_3;
    f32 temp_fs1;
    f32 temp_fs2;
    f32 sp74;
    f32 sp70;
    f32 sp6C;
    f32 temp_fs3;
    f32 temp_ft4_3;
    f32 temp_ft5_3;
    s32 pad[4];
    f32 sp4C;
    f32 sp48;
    f32 sp44;
    f32 temp_fv1;

    temp_fs1 = arg5 - arg2;
    temp_fs2 = arg6 - arg3;
    temp_fs3 = arg7 - arg4;

    temp_fv1 = -1.0 / sqrtf(SQ(temp_fs1) + SQ(temp_fs2) + SQ(temp_fs3));
    temp_fs1 *= temp_fv1;
    temp_fs2 *= temp_fv1;
    temp_fs3 *= temp_fv1;

    sp74 = (arg9 * temp_fs3) - (arg10 * temp_fs2);
    sp70 = (arg10 * temp_fs1) - (arg8 * temp_fs3);
    sp6C = (arg8 * temp_fs2) - (arg9 * temp_fs1);

    temp_fv1 = 1.0 / sqrtf(SQ(sp74) + SQ(sp70) + SQ(sp6C));

    sp74 *= temp_fv1;
    sp70 *= temp_fv1;
    sp6C *= temp_fv1;

    arg8 = (temp_fs2 * sp6C) - (temp_fs3 * sp70);
    arg9 = (temp_fs3 * sp74) - (temp_fs1 * sp6C);
    arg10 = (temp_fs1 * sp70) - (temp_fs2 * sp74);

    temp_fv1 = 1.0 / sqrtf(SQ(arg8) + SQ(arg9) + SQ(arg10));
    arg8 *= temp_fv1;
    arg9 *= temp_fv1;
    arg10 *= temp_fv1;

    temp_fv1 = 1.0 / sqrtf(SQ(arg11) + SQ(arg12) + SQ(arg13));
    arg11 *= temp_fv1;
    arg12 *= temp_fv1;
    arg13 *= temp_fv1;

    sp4C = arg11 + temp_fs1;
    sp48 = arg12 + temp_fs2;
    sp44 = arg13 + temp_fs3;

    temp_fv1 = sqrtf(SQ(sp44)+ SQ(sp48) + SQ(sp4C));
    if (D_uvgfxstate_rom_00402C00 < temp_fv1) {
        temp_fa0 = 1.0 / temp_fv1;
        temp_fa1_3 = sp4C * temp_fa0;
        temp_ft4_3 = sp48 * temp_fa0;
        temp_ft5_3 = sp44 * temp_fa0;
        arg1->h.x1 = ((arg17 * 4) + (((temp_fa1_3 * sp74) + (temp_ft4_3 * sp70) + (temp_ft5_3 * sp6C)) * arg17 * 2));
        arg1->h.y1 = ((arg18 * 4) + (((temp_fa1_3 * arg8) + (temp_ft4_3 * arg9) + (temp_ft5_3 * arg10)) * arg18 * 2));
    } else {
        arg1->h.x1 = arg17 * 2;
        arg1->h.y1 = arg18 * 2;
    }
    temp_fv1 = 1.0 / sqrtf(SQ(arg14) + SQ(arg15) + SQ(arg16));

    arg14 *= temp_fv1;
    arg15 *= temp_fv1;
    arg16 *= temp_fv1;

    sp4C = arg14 + temp_fs1;
    sp48 = arg15 + temp_fs2;
    sp44 = arg16 + temp_fs3;

    temp_fv1 = sqrtf(SQ(sp44)+ SQ(sp48) + SQ(sp4C));
    if (D_uvgfxstate_rom_00402C08 < temp_fv1) {
        temp_fa0 = 1.0 / temp_fv1;
        temp_fa1_3 = sp4C * temp_fa0;
        temp_ft4_3 = sp48 * temp_fa0;
        temp_ft5_3 = sp44 * temp_fa0;
        arg1->h.x2 = (arg17 * 4) + (((temp_fa1_3 * sp74) + (temp_ft4_3 * sp70) + (temp_ft5_3 * sp6C)) * arg17 * 2);
        arg1->h.y2 = (arg18 * 4) + (((temp_fa1_3 * arg8) + (temp_ft4_3 * arg9) + (temp_ft5_3 * arg10)) * arg18 * 2);
    } else {
        arg1->h.x2 = arg17 * 2;
        arg1->h.y2 = arg18 * 2;
    }


    arg0->l[0].l.dir[0] = (s32) MIN(sp74 * 128.0f, 127.0f) & 0xFF;
    arg0->l[0].l.dir[1] = (s32) MIN(sp70 * 128.0f, 127.0f) & 0xFF;
    arg0->l[0].l.dir[2] = (s32) MIN(sp6C * 128.0f, 127.0f) & 0xFF;
    arg0->l[1].l.dir[0] = (s32) MIN(arg8 * 128.0f, 127.0f) & 0xFF;
    arg0->l[1].l.dir[1] = (s32) MIN(arg9 * 128.0f, 127.0f) & 0xFF;
    arg0->l[1].l.dir[2] = (s32) MIN(arg10 * 128.0f, 127.0f) & 0xFF;
    arg0->l[0].l.col[0] = 0;
    arg0->l[0].l.col[1] = 0;
    arg0->l[0].l.col[2] = 0;
    arg0->l[0].l.pad1 = 0;
    arg0->l[0].l.colc[0] = 0;
    arg0->l[0].l.colc[1] = 0;
    arg0->l[0].l.colc[2] = 0;
    arg0->l[0].l.pad2 = 0;
    arg0->l[1].l.col[0] = 0;
    arg0->l[1].l.col[1] = 0x80;
    arg0->l[1].l.col[2] = 0;
    arg0->l[1].l.pad1 = 0;
    arg0->l[1].l.colc[0] = 0;
    arg0->l[1].l.colc[1] = 0x80;
    arg0->l[1].l.colc[2] = 0;
    arg0->l[1].l.pad2 = 0;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgfxstate_rom/func_uvgfxstate_rom_00400C68.s")
#endif

void func_uvgfxstate_rom_00401314(s32 arg0) {
    s32 temp_v1;

    temp_v1 = arg0 | D_uvgfxstate_rom_00402C48;
    if (temp_v1 != D_uvgfxstate_rom_00402C48) {
        uvGfxState sp1C;
        sp1C.state = temp_v1;
        sp1C.unkC = -1;
        sp1C.displayList = 0;
        uvGfxStateDraw(&sp1C);
    }
}

void func_uvgfxstate_rom_00401354(s32 arg0) {
    s32 temp_v1;
    uvGfxState sp1C;

    temp_v1 = ~arg0 & D_uvgfxstate_rom_00402C48;
    if (temp_v1 != D_uvgfxstate_rom_00402C48) {
        sp1C.state = temp_v1;
        sp1C.unkC = -1;
        sp1C.displayList = NULL;
        uvGfxStateDraw(&sp1C);
    }
}

void func_uvgfxstate_rom_00401398(s32 arg0) {
    uvGfxState sp18;

    sp18.displayList = NULL;
    sp18.unkC = -1;
    sp18.state = (D_uvgfxstate_rom_00402C48 & ~0xFFF) | arg0;
    uvGfxStateDraw(&sp18);
}

void func_uvgfxstate_rom_004013DC(uvGfxState *gs, s32 arg1) {
    gs->state = gs->state & ~arg1;
}

void func_uvgfxstate_rom_004013F0(uvGfxState *gs, s32 arg1) {
    gs->state |= arg1;
}

void func_uvgfxstate_rom_00401400(uvGfxState *gs, s32 arg1) {
    gs->state = (gs->state & ~0xFFF) | arg1;
}

void func_uvgfxstate_rom_00401418(uvGfxState *arg0, s32 arg1, s32 arg2) {
    if (arg1 == (arg0->state & 0xFFF)) {
        arg0->state = (arg0->state & ~0xFFF) | arg2;
    }
}

s32 func_uvgfxstate_rom_0040143C(uvGfxState *arg0, s32 arg1) {
    if (arg1 == (arg0->state & 0xFFF)) {
        return TRUE;
    }
    return FALSE;
}

void func_uvgfxstate_rom_00401460(s32 arg0) {
    D_uvgfxstate_rom_00402C50 = arg0;
}

void func_uvgfxstate_rom_0040146C(s32 arg0) {
    if (arg0 == -1) {
        D_uvgfxstate_rom_00402C4C = 0;
        D_uvgfxstate_rom_00402C50 = 0;
        return;
    }
    D_uvgfxstate_rom_00402C4C = arg0;
}

void func_uvgfxstate_rom_00401498(uvGfxState *gs) {
    Gfx **gdl;
    s32 sp50;
    void *sp4C;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    sp50 = sUvGfxMgrExports->func_uvgfxmgr_rom_00402058();
    sp4C = sUvGfxMgrExports->func_uvgfxmgr_rom_0040204C();
    gSPClearGeometryMode(gdl[0]++, G_CULL_BACK | G_LIGHTING);
    gSPSetGeometryMode(gdl[0]++, G_ZBUFFER | G_SHADE | G_CULL_FRONT);
    uvGfxSync();
    gDPSetCycleType(gdl[0]++, G_CYC_2CYCLE);
    gDPSetTextureLOD(gdl[0]++, G_TL_TILE);
    gDPSetCombineLERP(gdl[0]++, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED);
    gDPSetAlphaCompare(gdl[0]++, G_AC_NONE);
    uvGfxSync();
    gDPSetRenderMode(gdl[0]++, G_RM_PASS, G_RM_ZB_XLU_SURF2);
    gDPSetColorImage(gdl[0]++, G_IM_FMT_RGBA, G_IM_SIZ_16b, sUvGfxMgrExports->uvGetScreenWidth(),
                     osVirtualToPhysical(sp4C));
    gSPDisplayList(gdl[0]++, gs->displayList);
    uvGfxSync();
    gDPSetColorImage(gdl[0]++, G_IM_FMT_RGBA, G_IM_SIZ_16b, sUvGfxMgrExports->uvGetScreenWidth(),
                     osVirtualToPhysical(sp50));
    gDPSetCombineMode(gdl[0]++, G_CC_SHADE, G_CC_PASS2);
    gDPSetRenderMode(gdl[0]++, G_RM_PASS, G_RM_ZB_XLU_SURF2);
    gSPClearGeometryMode(gdl[0]++, G_CULL_FRONT);
    gSPSetGeometryMode(gdl[0]++, G_CULL_BACK);
    gSPDisplayList(gdl[0]++, gs->displayList);
    D_uvgfxstate_rom_00402D38[D_uvgfxstate_rom_00402D30] += gs->unk10 * 2;
    D_uvgfxstate_rom_00402D50[D_uvgfxstate_rom_00402D30] += gs->unk14 * 2;
    D_uvgfxstate_rom_00402D58[D_uvgfxstate_rom_00402D30] += gs->unkE * 2;
    D_uvgfxstate_rom_00402D40[D_uvgfxstate_rom_00402D30] += gs->unk12 * 2;
    D_uvgfxstate_rom_00402D48[D_uvgfxstate_rom_00402D30] += 2;
    func_uvgfxstate_rom_004022B0();
}

void uvGfxStateDraw(uvGfxState *gs) {
    s32 pad;
    Gfx *gdlh;
    s32 sp54;
    s32 var_v1;
    s32 sp4C;
    s32 state;
    s32 temp_a2;
    Gfx **gdl;
    s32 temp_a0;

    state = gs->state;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    gdlh = *gdl;

    if (state & 0x40000) {
        *gdl = gdlh;
        D_uvgfxstate_rom_00402D18->unk4(gs->unkC);
        gdlh = *gdl;
    }

    if (state != D_uvgfxstate_rom_00402C48) {
        gDPPipeSync(gdlh++);
        temp_a2 = state & 0xFFF;
        sp4C = D_uvgfxstate_rom_00402C48 & 0xFFF;
        if (sp4C != temp_a2) {
            *gdl = gdlh;
            D_uvgfxstate_rom_00402C5C = D_uvgfxstate_rom_00402D14->unk10(state);
            gdlh = *gdl;
        }
        temp_a0 = D_uvgfxstate_rom_00402C5C;
        if (((state & 0x01E00000) != (D_uvgfxstate_rom_00402C48 & 0x01E00000)) || (temp_a2 != sp4C)) {
            if (state & 0x01000000) {
                switch (state & 0xC00000) {
                    case 0x400000:
                        sp54 = 0x112D58;
                        break;
                    case 0xC00000:
                        sp54 = 0x104DD8;
                        break;
                    case 0x800000:
                        sp54 = 0x104E50;
                        break;
                    case 0x0:
                        sp54 = 0x112E10;
                        break;
                }
            } else if (state & 0x800000) {
                switch (state & 0x600000) {
                    case 0x600000:
                        if (temp_a2 == 0xFFF) {
                            sp54 = 0x1045D8;
                        } else if (temp_a0 & 0x10000) {
                            sp54 = 0x105278;
                        } else if ((temp_a0 & 0x8000) || (temp_a0 & 0x4000) || (state & 0x04000000)) {
                            sp54 = 0x1049D8;
                        } else {
                            sp54 = 0x103078;
                        }
                        break;
                    case 0x400000:
                        if (temp_a2 == 0xFFF) {
                            sp54 = 0x1041C8;
                        } else if ((temp_a0 & 0x8000) || (temp_a0 & 0x4000) || (state & 0x04000000)) {
                            sp54 = 0x1041C8;
                        } else {
                            sp54 = 0x103048;
                        }
                        break;
                    case 0x200000:
                        if (temp_a2 == 0xFFF) {
                            sp54 = 0x104B50;
                        } else if (temp_a0 & 0x10000) {
                            sp54 = 0x105278;
                        } else {
                            sp54 = 0x104A50;
                        }
                        break;
                    case 0:
                        if (temp_a2 == 0xFFF) {
                            sp54 = 0x104340;
                        } else {
                            sp54 = 0x104240;
                        }
                        break;
                }
            } else {
                switch (state & 0x600000) {
                    case 0x200000:
                        sp54 = 0x112230;
                        break;
                    case 0x400000:
                        sp54 = 0x102048;
                        break;
                    case 0x600000:
                        sp54 = 0x102078;
                        break;
                    case 0x0:
                        sp54 = 0x03024000;
                        break;
                    default:
                        break;
                }
            }

            gDPSetRenderMode(gdlh++, G_RM_PASS, sp54);
        }
        if ((state & 0x887C0000) != (D_uvgfxstate_rom_00402C48 & 0x887C0000)) {
            sp54 = 0;
            var_v1 = 0;
            if (state & 0x08000000) {
                sp54 |= 0x40000;
            } else {
                var_v1 |= 0x40000;
            }
            if (state & 0x100000) {
                sp54 |= 0x400;
            } else {
                var_v1 |= 0x400;
            }
            if (state & 0x80000) {
                sp54 |= 0x200;
            } else {
                var_v1 |= 0x200;
            }
            if (state & 0x08040000) {
                sp54 |= 0x20000;
            } else {
                var_v1 |= 0x20000;
            }
            if (state & 0x200000) {
                sp54 |= 1;
            } else {
                var_v1 |= 1;
            }
            if (var_v1 != 0) {
                gSPClearGeometryMode(gdlh++, var_v1);
            }
            if (sp54 != 0) {
                gSPSetGeometryMode(gdlh++, sp54);
            }
        }
        D_uvgfxstate_rom_00402C48 = state;
    }
    if (gs->displayList != NULL) {
        gSPDisplayList(gdlh++, gs->displayList);
        D_uvgfxstate_rom_00402D38[D_uvgfxstate_rom_00402D30] += gs->unk10;
        D_uvgfxstate_rom_00402D50[D_uvgfxstate_rom_00402D30] += gs->unk14;
        D_uvgfxstate_rom_00402D58[D_uvgfxstate_rom_00402D30] += gs->unkE;
        D_uvgfxstate_rom_00402D40[D_uvgfxstate_rom_00402D30] += gs->unk12;
        D_uvgfxstate_rom_00402D48[D_uvgfxstate_rom_00402D30] += 1;
    }
    *gdl = gdlh;
}

void func_uvgfxstate_rom_00401CE8(void) {
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    if (D_uvgfxstate_rom_00402C54 > 0.0f) {
        gSPSetGeometryMode(gdl[0]++, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_SHADING_SMOOTH);
    } else {
        gSPSetGeometryMode(gdl[0]++, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH);
    }
    gSPDisplayList(gdl[0]++, D_uvgfxstate_rom_00402C78);
    D_uvgfxstate_rom_00402C48 = 0x500FFF;
    D_uvgfxstate_rom_00402D14->unk14();
    func_uvgfxstate_rom_00401F54(D_uvgfxstate_rom_00402C54, D_uvgfxstate_rom_00402C58);
}

void func_uvgfxstate_rom_00401DD0(s32 arg0) {
    D_uvgfxstate_rom_00402DF0 = 0;
    D_uvgfxstate_rom_00402E04 = 0;
    D_uvgfxstate_rom_00402D30 = D_uvgfxstate_rom_00402D30 ^ 1;
    D_uvgfxstate_rom_00402D38[D_uvgfxstate_rom_00402D30] = 0;
    D_uvgfxstate_rom_00402D50[D_uvgfxstate_rom_00402D30] = 0;
    D_uvgfxstate_rom_00402D58[D_uvgfxstate_rom_00402D30] = 0;
    D_uvgfxstate_rom_00402D60[D_uvgfxstate_rom_00402D30] = 0;
    D_uvgfxstate_rom_00402D40[D_uvgfxstate_rom_00402D30] = 0;
    D_uvgfxstate_rom_00402D48[D_uvgfxstate_rom_00402D30] = 0;
    func_uvgfxstate_rom_00401CE8();
}

void func_uvgfxstate_rom_00401E60(s32 arg0) {
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    if (arg0 != 0) {
        gDPSetRenderMode(gdl[0]++, G_RM_PASS, G_RM_AA_ZB_OPA_TERR2);
        gSPSetGeometryMode(gdl[0]++, G_ZBUFFER);
        D_uvgfxstate_rom_00402C48 |= 0x200000;
    } else {
        gDPSetRenderMode(gdl[0]++, G_RM_PASS, G_RM_AA_OPA_TERR2);
        gSPClearGeometryMode(gdl[0]++, G_ZBUFFER);
        D_uvgfxstate_rom_00402C48 &= 0xFFDFFFFF;
    }
}

#ifdef NEEDS_RODATA
void func_uvgfxstate_rom_00401F54(f32 arg0, f32 arg1) {
    Gfx **gdl;
    f32 var_fv0;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    if (arg0 < 0.0f) {
        arg0 = 0.0f;
    } else if (arg0 > 1.0f) {
        arg0 = 1.0f;
    }
    if (arg1 < 0.0f) {
        arg1 = 0.0f;
    } else if (arg1 > 1.0f) {
        arg1 = 1.0f;
    }
    D_uvgfxstate_rom_00402C54 = arg0;
    D_uvgfxstate_rom_00402C58 = arg1;
    if (arg0 > 0.0f) {
        arg1 *= 255.0f;
        var_fv0 = arg1 / (1.0f - arg0);
        gSPSetGeometryMode(gdl[0]++, G_FOG);

        if (32767.0f < var_fv0) {
            var_fv0 = 32767.0f;
        }
        if (var_fv0 < -32467.0f) {
            var_fv0 = -32467.0f;
        }
        gSPFogFactor(gdl[0]++, ((s32) var_fv0), ((s32) (arg1 - var_fv0)));
    } else {
        gSPClearGeometryMode(gdl[0]++, G_FOG);
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgfxstate_rom/func_uvgfxstate_rom_00401F54.s")
#endif

void func_uvgfxstate_rom_004020DC(f32 *arg0, f32 *arg1) {
    *arg0 = D_uvgfxstate_rom_00402C54;
    *arg1 = D_uvgfxstate_rom_00402C58;
}

void func_uvgfxstate_rom_004020F8(s32 arg0, s32 arg1) {
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    if (arg0 != 0) {
        gSPSetGeometryMode(gdl[0]++, G_CULL_FRONT);
        D_uvgfxstate_rom_00402C48 |= 0x80000;
    } else {
        gSPClearGeometryMode(gdl[0]++, G_CULL_FRONT);
        D_uvgfxstate_rom_00402C48 &= 0xFFF7FFFF;
    }
    if (arg1 != 0) {
        gSPSetGeometryMode(gdl[0]++, G_CULL_BACK);
        D_uvgfxstate_rom_00402C48 |= 0x100000;
    } else {
        gSPClearGeometryMode(gdl[0]++, G_CULL_BACK);
        D_uvgfxstate_rom_00402C48 &= 0xFFEFFFFF;
    }
}

void func_uvgfxstate_rom_00402218(void) {
    if (D_uvgfxstate_rom_00402DE8 < 0x1F) {
        D_uvgfxstate_rom_00402D68[D_uvgfxstate_rom_00402DE8++] = D_uvgfxstate_rom_00402C48;
    }
}

// uvGfxStatePop
void func_uvgfxstate_rom_00402254(void) {
    u32 newState;
    uvGfxState sp1C;

    if (D_uvgfxstate_rom_00402DE8 != 0) {
        D_uvgfxstate_rom_00402DE8--;
        newState = D_uvgfxstate_rom_00402D68[D_uvgfxstate_rom_00402DE8];
        sp1C.unkC = -1;
        sp1C.displayList = NULL;
        sp1C.state = newState;
        uvGfxStateDraw(&sp1C);
    }
}

void func_uvgfxstate_rom_004022B0(void) {
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    gDPSetCycleType(gdl[0]++, G_CYC_2CYCLE);
    gDPSetCombineMode(gdl[0]++, G_CC_SHADE, G_CC_PASS2);
    D_uvgfxstate_rom_00402C48 = (D_uvgfxstate_rom_00402C48 & ~0xFFF) | 0xFFF;
    D_uvgfxstate_rom_00402D14->unk14();
}

void func_uvgfxstate_rom_00402350(s32 arg0, s32 arg1, s32 arg2, s32 arg3, f32 arg4, f32 arg5, f32 arg6,
                                  f32 arg7) {
    f32 temp_fa0;
    f32 temp_ft4;
    f32 temp_fv0;
    s32 sp78;
    s16 sp76;
    s16 sp74;
    s16 sp72;
    s16 sp70;
    UnkStruct_80001724 *temp_v0_2;
    s32 pad;
    f32 var_fv0;
    Gfx **gdl;
    f32 var_fa1;

    sp78 = D_uvgfxstate_rom_00402C48 & 0xFFF;
    gdl = sUvGfxMgrExports->uvGetDisplayListHead();

    if ((arg1 - arg0) <= 0) {
        return;
    }

    if ((arg3 - arg2) <= 0) {
        return;
    }

    if (sp78 < 0xFFE) {
        temp_v0_2 = uvGetLoadedFile('UVTX', sp78);
        temp_fv0 = temp_v0_2->unk1C;
        var_fa1 = temp_v0_2->unk1E;
        arg5 += 1.0f / temp_v0_2->unk1C;
        arg4 += 1.0f / temp_v0_2->unk1C;

        sp76 = temp_v0_2->unk1C * arg4 * 32.0f;
        sp74 = var_fa1 * arg7 * 32.0f;
        if (arg4 <= arg5) {
            var_fv0 = 1.0f;
            var_fv0 += (arg1 - arg0);
        } else {
            var_fv0 = (arg1 - arg0);
        }
        sp72 = (1024.0f * temp_fv0 * (arg5 - arg4)) / var_fv0;
        sp70 = (1024.0f * var_fa1 * (arg7 - arg6)) / (1.0f + (arg3 - arg2));
        uvGfxSync();
        if (!(temp_v0_2->unk14 & 0x1000)) {
            gDPSetCombineMode(gdl[0]++, G_CC_MODULATEIA_PRIM, G_CC_PASS2);
        }
        gDPSetTexturePersp(gdl[0]++, G_TP_NONE);
    } else {
        uvGfxSync();
        gDPSetCombineMode(gdl[0]++, G_CC_PRIMITIVE, G_CC_PASS2);
        sp72 = 0;
        sp70 = 0;
        sp76 = 0;
        sp74 = 0;
    }

    arg2--;
    arg1++;
    gSPScisTextureRectangle(gdl[0]++,
                            arg0 << 2,                                           // xl
                            (sUvGfxMgrExports->uvGetScreenHeight() - arg3) << 2, // yl
                            arg1 << 2,                                           // xh
                            (sUvGfxMgrExports->uvGetScreenHeight() - arg2) << 2, // yh
                            G_TX_MIRROR,                                         // tile
                            sp76,                                                // s
                            sp74,                                                // t
                            sp72,                                                // dsdx
                            -sp70                                                // dtdy
    );
}

#ifdef NEEDS_RODATA
extern s32 D_uvgfxstate_rom_00402D3C[];
extern s32 D_uvgfxstate_rom_00402D44[];
extern s32 D_uvgfxstate_rom_00402D4C[];
extern s32 D_uvgfxstate_rom_00402D54[];
extern s32 D_uvgfxstate_rom_00402D5C[];
extern s32 D_uvgfxstate_rom_00402D64[];
s32 func_uvgfxstate_rom_004029B8(u32 arg0) {
    switch (arg0) {
        case 0:
            return D_uvgfxstate_rom_00402D3C[0 - D_uvgfxstate_rom_00402D30];
        case 1:
            return D_uvgfxstate_rom_00402D44[0 - D_uvgfxstate_rom_00402D30];
        case 5:
            return D_uvgfxstate_rom_00402D4C[0 - D_uvgfxstate_rom_00402D30];
        case 6:
            return D_uvgfxstate_rom_00402D54[0 - D_uvgfxstate_rom_00402D30];
        case 7:
            return D_uvgfxstate_rom_00402D5C[0 - D_uvgfxstate_rom_00402D30];
        case 8:
            return D_uvgfxstate_rom_00402D64[0 - D_uvgfxstate_rom_00402D30];
        case 4:
            return D_uvgfxstate_rom_00402D14->unk18(0);
        case 2:
            return sUvImtxExports->func_uvimtx_rom_004006E0(0);
        case 3:
            return sUvImtxExports->func_uvimtx_rom_004006E0(1);
        default:
            return 0;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgfxstate_rom/func_uvgfxstate_rom_004029B8.s")
#endif

void func_uvgfxstate_rom_00402B14(s32 arg0, s32 arg1) {
    D_uvgfxstate_rom_00402D2C = arg1;
    D_uvgfxstate_rom_00402C40 = arg0;
}

void func_uvgfxstate_rom_00402B28(s32 arg0, Vec3F arg1) {
    if (arg0 < 3) {
        D_uvgfxstate_rom_00402C60[arg0] = arg1;
    }
}

void uvGfxSync(void) {
    Gfx **gdl;

    gdl = sUvGfxMgrExports->uvGetDisplayListHead();
    if (D_uvgfxstate_rom_00402D00 != 2) {
        D_uvgfxstate_rom_00402D60[D_uvgfxstate_rom_00402D30]++;
        gDPPipeSync(gdl[0]++);
    }
}
