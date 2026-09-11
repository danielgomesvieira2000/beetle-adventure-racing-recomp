// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

typedef struct UnkStruct_uvchannel_rom_00401CD0_s {
    /* 0x000 */ u8 unk0;
    /* 0x001 */ u8 unk1;
    /* 0x002 */ u8 unk2;
    /* 0x003 */ u8 unused;
    /* 0x004 */ Mtx4F unk4;
    /* 0x044 */ Mtx4F unk44;
    /* 0x084 */ Mtx4F unk84;
    /* 0x0C4 */ Vec3F unkC4;
    /* 0x0D0 */ Vec3F unkD0;
    /* 0x0DC */ f32 unkDC;
    /* 0x0E0 */ f32 unkE0;
    /* 0x0E4 */ f32 unkE4;
    /* 0x0E8 */ f32 unkE8;
    /* 0x0EC */ f32 unkEC;
    /* 0x0F0 */ f32 unkF0;
    /* 0x0F4 */ Vec3F unkF4[6];
    /* 0x13C */ Vec3F unk13C[6];
    /* 0x184 */ Vec3F unk184[4];
    /* 0x1B4 */ Vec3F unk1B4[2];
    /* 0x1CC */ Vec3F unk1CC[6];
    /* 0x214 */ u16 unk214;
    /* 0x216 */ u16 unk216;
    /* 0x218 */ u16 unk218;
    /* 0x21A */ u16 unk21A;
    /* 0x21C */ s32 unk21C;
    /* 0x220 */ s32 unk220;
    /* 0x224 */ s32 unk224;
} UnkStruct_uvchannel_rom_00401CD0; /* size = 0x228 */

typedef struct UnkStruct_Geom_Exports_s {
    char pad[0xC];
    void (*unkC)(f32, f32, f32, f32);
    void (*unk10)(s32, s32, s32, s32);
} UnkStruct_Geom_Exports;

extern UnkStruct_uvchannel_rom_00401CD0 *D_uvchannel_rom_00401CD0;
extern s32 D_uvchannel_rom_00401CD4;
extern UvCback_Exports *D_uvchannel_rom_00401CDC;
extern UvMath_Exports *D_uvchannel_rom_00401CE8;
extern UvFVec_Rom_Exports *D_uvchannel_rom_00401CE4;
extern UvFMtx_Rom_Exports *D_uvchannel_rom_00401CE0;
extern UvGfxMgr_Exports *D_uvchannel_rom_00401CEC;
extern UvGfxState_Rom_Exports *D_uvchannel_rom_00401CF0;
extern UvDGeom_Rom_Exports *D_uvchannel_rom_00401CF4;
extern s32 D_uvchannel_rom_00401CD8;

// geom exports
extern UnkStruct_Geom_Exports *D_uvchannel_rom_00401CF8;

extern f32 D_uvchannel_rom_00401CB0;
extern f32 D_uvchannel_rom_00401CB4;

void func_uvchannel_rom_004001D4(void);
void func_uvchannel_rom_00400288(s32 arg0, ...);
void func_uvchannel_rom_004005DC(s32 arg0, ...);
void func_uvchannel_rom_00400AD4(s32 arg0, s32 arg1);
void func_uvchannel_rom_00400AFC(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6);
void func_uvchannel_rom_00400B70(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
void func_uvchannel_rom_00400BB8(s32 arg0, s32 arg1, s32 arg2);
s32 func_uvchannel_rom_00400CCC(void);
void func_uvchannel_rom_00400E7C(s32 arg0);
void func_uvchannel_rom_00400EFC(s32 arg0);
void func_uvchannel_rom_00401278(s32 arg0);
void func_uvchannel_rom_00401414(s32 arg0, Mtx4F *arg1);
s32 func_uvchannel_rom_004014E8(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4);
void func_uvchannel_rom_00401658(s32 arg0);
void func_uvchannel_rom_004018D8(s32 arg0);
void func_uvchannel_rom_00401A4C(s32 arg0);

void __entrypoint_func_uvchannel_rom_400000(UvChannelExports *exports) {
    s32 *temp_v0;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvchannel_rom_004001D4 = func_uvchannel_rom_004001D4;
    exports->func_uvchannel_rom_00401414 = func_uvchannel_rom_00401414;
    exports->func_uvchannel_rom_00400288 = func_uvchannel_rom_00400288;
    exports->func_uvchannel_rom_004014E8 = func_uvchannel_rom_004014E8;
    exports->func_uvchannel_rom_004005DC = func_uvchannel_rom_004005DC;
    exports->func_uvchannel_rom_00400AD4 = func_uvchannel_rom_00400AD4;
    exports->func_uvchannel_rom_00400AFC = func_uvchannel_rom_00400AFC;
    exports->func_uvchannel_rom_00400B70 = func_uvchannel_rom_00400B70;
    exports->func_uvchannel_rom_00400BB8 = func_uvchannel_rom_00400BB8;
    exports->func_uvchannel_rom_00400CCC = func_uvchannel_rom_00400CCC;
    exports->func_uvchannel_rom_00400E7C = func_uvchannel_rom_00400E7C;
    exports->func_uvchannel_rom_00401278 = func_uvchannel_rom_00401278;
#line 81
    D_uvchannel_rom_00401CDC = uvLoadModule('CBCK');
    D_uvchannel_rom_00401CE8 = uvLoadModule('MATH');
    D_uvchannel_rom_00401CE4 = uvLoadModule('FVEC');
    D_uvchannel_rom_00401CE0 = uvLoadModule('FMTX');
    D_uvchannel_rom_00401CEC = uvLoadModule('GMGR');
    D_uvchannel_rom_00401CF0 = uvLoadModule('STAT');
    D_uvchannel_rom_00401CF4 = uvLoadModule('DGEO');
    D_uvchannel_rom_00401CF8 = uvLoadModule('GEOM');
    temp_v0 = uvGetSystemProp(0xB);
    if (temp_v0 != NULL) {
        D_uvchannel_rom_00401CD4 = *temp_v0;
    } else {
        D_uvchannel_rom_00401CD4 = 4;
    }
    D_uvchannel_rom_00401CD0 =
        _uvMemAllocAlign8(D_uvchannel_rom_00401CD4 * sizeof(UnkStruct_uvchannel_rom_00401CD0));
    uvMemSet(D_uvchannel_rom_00401CD0, 0U,
             D_uvchannel_rom_00401CD4 * sizeof(UnkStruct_uvchannel_rom_00401CD0));
    D_uvchannel_rom_00401CD8 = 0;
}

void func_uvchannel_rom_004001D4(void) {
    uvUnloadModule('CBCK');
    D_uvchannel_rom_00401CDC = NULL;
    uvUnloadModule('MATH');
    D_uvchannel_rom_00401CE8 = NULL;
    uvUnloadModule('FVEC');
    D_uvchannel_rom_00401CE4 = NULL;
    uvUnloadModule('FMTX');
    D_uvchannel_rom_00401CE0 = NULL;
    uvUnloadModule('GMGR');
    D_uvchannel_rom_00401CEC = NULL;
    uvUnloadModule('STAT');
    D_uvchannel_rom_00401CF0 = NULL;
    uvUnloadModule('DGEO');
    D_uvchannel_rom_00401CF4 = NULL;
    uvUnloadModule('GEOM');
    D_uvchannel_rom_00401CF8 = NULL;
}

#ifdef NEEDS_RODATA
void func_uvchannel_rom_00400288(s32 arg0, ...) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_s2;
    int prop;
    va_list args;
    void *arg;

    if ((arg0 < 0) || (arg0 >= D_uvchannel_rom_00401CD4)) {
        return;
    }

    va_start(args, arg0);
    temp_s2 = &D_uvchannel_rom_00401CD0[arg0];
    while ((prop = va_arg(args, s32)) != 0) {
        switch (prop) {
            case 1:
                D_uvchannel_rom_00401CE0->func_00400504(va_arg(args, Mtx4F *), &temp_s2->unk4);
                break;
            case 2:
                D_uvchannel_rom_00401CE0->func_00400504(va_arg(args, Mtx4F *), &temp_s2->unk84);
                break;
            case 3:
                D_uvchannel_rom_00401CE0->func_00400504(va_arg(args, Mtx4F *), &temp_s2->unk44);
                break;
            case 4:
                *(f32 *) (arg = va_arg(args, f32 *)) = temp_s2->unkDC;
                *(f32 *) (arg = va_arg(args, f32 *)) = temp_s2->unkE0;
                *(f32 *) (arg = va_arg(args, f32 *)) = temp_s2->unkE4;
                *(f32 *) (arg = va_arg(args, f32 *)) = temp_s2->unkE8;
                *(f32 *) (arg = va_arg(args, f32 *)) = temp_s2->unkEC;
                *(f32 *) (arg = va_arg(args, f32 *)) = temp_s2->unkF0;
                break;
            case 5:
                *(s32 *) (arg = va_arg(args, s32 *)) = temp_s2->unk214;
                *(s32 *) (arg = va_arg(args, s32 *)) = temp_s2->unk216;
                *(s32 *) (arg = va_arg(args, s32 *)) = temp_s2->unk218;
                *(s32 *) (arg = va_arg(args, s32 *)) = temp_s2->unk21A;
                break;
            case 6:
                *(s32 *) (arg = va_arg(args, s32 *)) = temp_s2->unk21C;
                break;
            case 7:
                D_uvchannel_rom_00401CE4->uvVec3FCopy(va_arg(args, Vec3F *), &temp_s2->unkC4);
                break;
            case 8:
                D_uvchannel_rom_00401CE4->uvVec3FCopy(va_arg(args, Vec3F *), &temp_s2->unkD0);
                break;
            case 9:
                *(Vec3F **) (arg = va_arg(args, Vec3F **)) = temp_s2->unkF4;
                break;
            case 10:
                *(Vec3F **) (arg = va_arg(args, Vec3F **)) = temp_s2->unk13C;
                break;
            case 11:
                *(Vec3F **) (arg = va_arg(args, Vec3F **)) = temp_s2->unk184;
                break;
            case 12:
                *(Vec3F **) (arg = va_arg(args, Vec3F **)) = temp_s2->unk1CC;
                break;
            case 14:
                *(s32 *) (arg = va_arg(args, s32 *)) = temp_s2->unk1;
                break;
            case 13:
                *(s32 *) (arg = va_arg(args, s32 *)) = D_uvchannel_rom_00401CD4;
                break;
        }
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvchannel_rom/func_uvchannel_rom_00400288.s")
#endif

// Non matching
#if 0
void func_uvchannel_rom_004005DC(s32 arg0, ...) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_s2;
    s32 temp_s3;
    va_list args;
    UvCback_Rom_004000F0 *sp60;
    int prop;

    if ((arg0 < 0) || (arg0 >= D_uvchannel_rom_00401CD4)) {
        return;
    }

    va_start(args, arg0);
    temp_s2 = &D_uvchannel_rom_00401CD0[arg0];
    while ((prop = va_arg(args, s32)) != 0) {
        switch (prop) {
            case 1:
                D_uvchannel_rom_00401CE0->func_00400504(&temp_s2->unk4, &va_arg(args, Mtx4F));
                break;
            case 2:
                D_uvchannel_rom_00401CE0->func_00400504(&temp_s2->unk84, &va_arg(args, Mtx4F));
                break;
            case 3:
                D_uvchannel_rom_00401CE0->func_00400504(&temp_s2->unk44, &va_arg(args, Mtx4F));
                break;
            case 4:
                temp_s2->unkDC = va_arg(args, f64);
                temp_s2->unkE0 = va_arg(args, f64);
                temp_s2->unkE4 = va_arg(args, f64);
                temp_s2->unkE8 = va_arg(args, f64);
                temp_s2->unkEC = va_arg(args, f64);
                temp_s2->unkF0 = va_arg(args, f64);
                D_uvchannel_rom_00401CE0->func_00401F74(&temp_s2->unk4, temp_s2->unkDC, temp_s2->unkE0,
                                                        temp_s2->unkE4, temp_s2->unkE8, temp_s2->unkEC,
                                                        temp_s2->unkF0);
                func_uvchannel_rom_00401658(arg0);
                break;
            case 5:
                temp_s2->unk214 = va_arg(args, s32);
                temp_s2->unk216 = va_arg(args, s32);
                temp_s2->unk218 = va_arg(args, s32);
                temp_s2->unk21A = va_arg(args, s32);
                break;
            case 15:
                temp_s2->unk2 = va_arg(args, s32);
                if (temp_s2->unk2 >= 7) {
                    temp_s2->unk2 = 6;
                }
                if (temp_s2->unk2 <= 0) {
                    temp_s2->unk2 = 1;
                }
                break;
            case 14:
                temp_s2->unk1 = va_arg(args, s32);
                break;
            case 16:
                func_uvchannel_rom_00400288(arg0, 6, &sp60, 0);
                temp_s3 = va_arg(args, s32);
                if (sp60 == NULL) {
                    break;
                }


                if ((D_uvchannel_rom_00401CD8 == 0) && (temp_s3 != 0)) {

                    D_uvchannel_rom_00401CDC->func_uvcback_rom_0040016C(
                        sp60, &func_uvchannel_rom_004018D8, 0, 0x19);
                    D_uvchannel_rom_00401CDC->func_uvcback_rom_0040016C(
                        sp60, &func_uvchannel_rom_00401A4C, 0, 0xFA);
                } else if ((D_uvchannel_rom_00401CD8 != 0) && (temp_s3 == 0)) {
                    D_uvchannel_rom_00401CDC->func_uvcback_rom_00400320(
                        sp60, (s32) &func_uvchannel_rom_004018D8);
                    D_uvchannel_rom_00401CDC->func_uvcback_rom_00400320(
                        sp60, (s32) &func_uvchannel_rom_00401A4C);
                }
                D_uvchannel_rom_00401CD8 = temp_s3;
                break;
        }
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvchannel_rom/func_uvchannel_rom_004005DC.s")
#endif

void func_uvchannel_rom_00400AD4(s32 arg0, s32 arg1) {
    func_uvchannel_rom_004005DC(arg0, 0xE, arg1, 0);
}

void func_uvchannel_rom_00400AFC(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    func_uvchannel_rom_004005DC(arg0, 4, (f64) arg1, (f64) arg2, (f64) arg3, (f64) arg4, (f64) arg5,
                                (f64) arg6, 0);
}

void func_uvchannel_rom_00400B70(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    func_uvchannel_rom_004005DC(arg0, 5, arg1, arg2, arg3, arg4, 0);
}

void func_uvchannel_rom_00400BB8(s32 arg0, s32 arg1, s32 arg2) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_s0;

    temp_s0 = &D_uvchannel_rom_00401CD0[arg0];
    switch (arg1) { /* irregular */
        case 0:
            if (arg2 != 0) {
                D_uvchannel_rom_00401CDC->func_uvcback_rom_0040016C(
                    (UvCback_Rom_004000F0 *) temp_s0->unk21C, arg2, 0, 1);
                temp_s0->unk220 = arg2;
                return;
            }
            if (temp_s0->unk220 != 0) {
                D_uvchannel_rom_00401CDC->func_uvcback_rom_00400320(
                    (UvCback_Rom_004000F0 *) temp_s0->unk21C, temp_s0->unk220);
                temp_s0->unk220 = 0;
                return;
            }
            return;
        case 1:
            if (arg2 != 0) {
                D_uvchannel_rom_00401CDC->func_uvcback_rom_0040016C(
                    (UvCback_Rom_004000F0 *) temp_s0->unk21C, arg2, 0, 0xFF);
                temp_s0->unk224 = arg2;
                return;
            }
            if (temp_s0->unk224 != 0) {
                D_uvchannel_rom_00401CDC->func_uvcback_rom_00400320(
                    (UvCback_Rom_004000F0 *) temp_s0->unk21C, temp_s0->unk224);
                temp_s0->unk224 = 0;
            }
            break;
    }
}

s32 func_uvchannel_rom_00400CCC(void) {
    s32 *sp20;
    s32 var_a0_2;
    s32 *temp_v0;
    UnkStruct_uvchannel_rom_00401CD0 *var_s1;
    s32 i;

    var_s1 = NULL;
    temp_v0 = uvGetSystemProp(0xB);

    for (i = 0; i < D_uvchannel_rom_00401CD4; i++) {
        if (!(D_uvchannel_rom_00401CD0[i].unk0 & 1)) {
            D_uvchannel_rom_00401CD0[i].unk0 = 1;
            var_s1 = &D_uvchannel_rom_00401CD0[i];
            break;
        }
    }

    if (var_s1 == NULL) {
        return -1;
    }
    if (temp_v0 != NULL) {
        var_a0_2 = temp_v0[1];
    } else {
        var_a0_2 = 0x14;
    }
    var_s1->unk21C = D_uvchannel_rom_00401CDC->func_uvcback_rom_00400080(var_a0_2);
    var_s1->unk214 = 0;
    var_s1->unk216 = (s16) (D_uvchannel_rom_00401CEC->uvGetScreenWidth() - 1);
    var_s1->unk218 = 0;
    var_s1->unk21A = (s16) (D_uvchannel_rom_00401CEC->uvGetScreenHeight() - 1);
    var_s1->unk2 = 2;
    D_uvchannel_rom_00401CE0->func_00400B68(&var_s1->unk84);
    D_uvchannel_rom_00401CE0->func_00400B68(&var_s1->unk44);
    D_uvchannel_rom_00401CE0->func_00400B68(&var_s1->unk4);
    // TODO: Find a better match
    var_a0_2 = var_s1->unk21C;
    if (var_a0_2 == NULL) {
        var_s1->unk1 = 0;
        var_a0_2 = *(void **) 0x21C;
        var_s1 = NULL;
    }
    D_uvchannel_rom_00401CDC->func_uvcback_rom_0040016C(var_a0_2, (s32) &func_uvchannel_rom_00400EFC, 0,
                                                        0x1E);
    D_uvchannel_rom_00401CDC->func_uvcback_rom_0040016C(var_s1->unk21C,
                                                        (s32) &func_uvchannel_rom_00400EFC, 0, 0xC8);
    return i;
}

void func_uvchannel_rom_00400E7C(s32 arg0) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_v0;
    s32 temp_a1;

    if ((arg0 < 0) || (arg0 >= D_uvchannel_rom_00401CD4)) {
        return;
    }

    temp_v0 = &D_uvchannel_rom_00401CD0[arg0];
    temp_a1 = temp_v0->unk21C;
    temp_v0->unk1 = 0;
    temp_v0->unk0 = 0;
    if (temp_a1 != 0) {
        D_uvchannel_rom_00401CDC->func_uvcback_rom_004000D0((void *) temp_a1);
        temp_v0->unk21C = 0;
    }
}

#ifdef NEEDS_RODATA
void func_uvchannel_rom_00400EFC(s32 arg0) {
    Gfx **gdl;

    gdl = D_uvchannel_rom_00401CEC->uvGetDisplayListHead();
    D_uvchannel_rom_00401CF0->func_uvgfxstate_rom_00401CE8();
    switch (D_uvchannel_rom_00401CD0[arg0].unk2) {
        case 1:
            gSPClipRatio(gdl[0]++, FRUSTRATIO_1);
            break;
        case 2:
            gSPClipRatio(gdl[0]++, FRUSTRATIO_2);
            break;
        case 3:
            gSPClipRatio(gdl[0]++, FRUSTRATIO_3);
            break;
        case 4:
            gSPClipRatio(gdl[0]++, FRUSTRATIO_4);
            break;
        case 5:
            gSPClipRatio(gdl[0]++, FRUSTRATIO_5);
            break;
        case 6:
            gSPClipRatio(gdl[0]++, FRUSTRATIO_6);
            break;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvchannel_rom/func_uvchannel_rom_00400EFC.s")
#endif

void func_uvchannel_rom_00401278(s32 arg0) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_s0;
    Mtx4F sp64;
    Mtx4F sp24;

    if ((arg0 < 0) || (arg0 >= D_uvchannel_rom_00401CD4)) {
        return;
    }

    temp_s0 = &D_uvchannel_rom_00401CD0[arg0];
    if (temp_s0->unk1 & 2) {
        D_uvchannel_rom_00401CEC->func_uvgfxmgr_rom_00401BD4(
            (s32) temp_s0->unk214, (s32) temp_s0->unk216, (s32) temp_s0->unk218, (s32) temp_s0->unk21A);
        D_uvchannel_rom_00401CE0->func_00402908(&temp_s0->unk4);
        D_uvchannel_rom_00401CE0->func_0040034C(temp_s0->unk44.m[3][0], temp_s0->unk44.m[3][1],
                                                temp_s0->unk44.m[3][2]);
        D_uvchannel_rom_00401CE0->func_00400370(&sp64, &temp_s0->unk44);
        D_uvchannel_rom_00401CE0->func_00401604(&sp24, &sp64);
        D_uvchannel_rom_00401CE0->func_004029DC(&sp24);
        D_uvchannel_rom_00401CEC->func_uvgfxmgr_rom_00401D94(
            (s16) (s32) (131072.0f / (temp_s0->unkF0 + temp_s0->unkEC)));
        if (!(temp_s0->unk1 & 4)) {
            D_uvchannel_rom_00401CEC->func_uvgfxmgr_rom_00401DC4();
        }
        D_uvchannel_rom_00401CDC->func_uvcback_rom_004000F0((UvCback_Rom_004000F0 *) temp_s0->unk21C,
                                                            arg0);
        D_uvchannel_rom_00401CE0->uvGfxMtxViewPop();
    }
}

void func_uvchannel_rom_00401414(s32 arg0, Mtx4F *arg1) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_v0;

    if ((arg0 < 0) || (arg0 >= D_uvchannel_rom_00401CD4)) {
        return;
    }

    temp_v0 = &D_uvchannel_rom_00401CD0[arg0];
    D_uvchannel_rom_00401CE0->func_00400504(&temp_v0->unk44, arg1);
    D_uvchannel_rom_00401CE0->func_00401604(&temp_v0->unk84, arg1);
    temp_v0->unkC4.x = arg1->m[3][0];
    temp_v0->unkC4.y = arg1->m[3][1];
    temp_v0->unkC4.z = arg1->m[3][2];
    temp_v0->unkD0.x = arg1->m[1][0];
    temp_v0->unkD0.y = arg1->m[1][1];
    temp_v0->unkD0.z = arg1->m[1][2];
    func_uvchannel_rom_00401658(arg0);
}

s32 func_uvchannel_rom_004014E8(s32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_v0;
    f32 temp_fv0;
    s32 i;

    if ((arg0 >= 0) && (arg0 < D_uvchannel_rom_00401CD4)) {
        temp_v0 = &D_uvchannel_rom_00401CD0[arg0];

        for (i = 0; i < 6; i += 2) {
            Vec3F *a3 = &temp_v0->unk1CC[i];
            Vec3F *v1 = &temp_v0->unk1CC[i + 1];

            // never true
            if (i == 5) {
                arg1 += a3->x * temp_v0->unkF0;
                arg2 += a3->y * temp_v0->unkF0;
                arg3 += a3->z * temp_v0->unkF0;
            }

            temp_fv0 = (a3->x * arg1) + (a3->y * arg2) + (a3->z * arg3);
            if ((temp_fv0 + arg4) < 0.0f) {
                return 0;
            }

            if (i == 4) {
                arg1 += v1->x * temp_v0->unkF0;
                arg2 += v1->y * temp_v0->unkF0;
                arg3 += v1->z * temp_v0->unkF0;
            }
            temp_fv0 = (v1->x * arg1) + (v1->y * arg2) + (v1->z * arg3);
            if ((temp_fv0 + arg4) < 0.0f) {
                return 0;
            }
        }
    }

    return 1;
}

void func_uvchannel_rom_00401658(s32 arg0) {
    f32 temp_fa0;
    f32 temp_fa1;
    f32 temp_ft4;
    f32 temp_ft5;
    f32 temp_fv0;
    s32 temp_s1;
    s32 var_v0;
    s32 i;
    UnkStruct_uvchannel_rom_00401CD0 *temp_s3;
    Vec3F *temp_s0;
    Mtx4F sp50;

    temp_s3 = &D_uvchannel_rom_00401CD0[arg0];
    temp_s0 = &temp_s3->unkF4[2];
    temp_fv0 = temp_s3->unkF0 / temp_s3->unkEC;
    temp_s3->unkF4[0].y = temp_s3->unkF0;
    temp_s3->unkF4[1].y = temp_s3->unkF0;

    temp_fa1 = temp_s3->unkDC * temp_fv0;
    temp_fa0 = temp_s3->unkE0 * temp_fv0;
    temp_ft4 = temp_s3->unkE4 * temp_fv0;
    temp_ft5 = temp_s3->unkE8 * temp_fv0;
    temp_s3->unkF4[0].x = temp_fa0;
    temp_s3->unkF4[1].x = temp_fa0;
    temp_s3->unkF4[0].z = temp_ft4;
    temp_s3->unkF4[1].z = temp_ft5;
    if (0)
        ; // FAKE
    temp_s0->x = temp_fa1;
    temp_s0->y = temp_s3->unkF0;
    temp_s0->z = temp_ft5;
    temp_s3->unkF4[3].z = temp_ft4;
    temp_s3->unkF4[3].x = temp_fa1;
    temp_s3->unkF4[4].z = 0.0f;
    temp_s3->unkF4[4].x = 0.0f;
    temp_s3->unkF4[5].z = 0.0f;
    temp_s3->unkF4[5].y = 0.0f;
    temp_s3->unkF4[5].x = 0.0f;
    temp_s3->unkF4[3].y = temp_s3->unkF0;
    temp_s3->unkF4[4].y = temp_s3->unkF0;

    for (i = 0; i < 4; i++) {
        Vec3F *temp_s2 = &temp_s3->unkF4[i];
        D_uvchannel_rom_00401CE4->uvVec3FCross(&temp_s3->unk184[i], &temp_s3->unkF4[(i + 1) % 4],
                                               temp_s2);
        D_uvchannel_rom_00401CE4->uvVec3FNormalize(&temp_s3->unk184[i], &temp_s3->unk184[i]);
    }
    temp_s0 = temp_s3->unk1B4;
    D_uvchannel_rom_00401CE4->uvVec3FCopy(temp_s0, (Vec3F *) &temp_s3->unkF4[4].x);
    D_uvchannel_rom_00401CE4->uvVec3FNormalize(temp_s0, temp_s0);
    temp_s3->unk1B4[1].x = -temp_s0->x;
    temp_s3->unk1B4[1].y = -temp_s0->y;
    temp_s3->unk1B4[1].z = -temp_s0->z;
    D_uvchannel_rom_00401CE0->func_00400504(&sp50, &temp_s3->unk44);

    for (i = 0; i < 6; i++) {
        D_uvchannel_rom_00401CE0->func_00401D0C(&sp50, &temp_s3->unk13C[i], &temp_s3->unkF4[i]);
    }
    sp50.m[3][0] = sp50.m[3][1] = sp50.m[3][2] = 0.0f;

    for (i = 0; i < 6; i++) {
        D_uvchannel_rom_00401CE0->func_00401D0C(&sp50, &temp_s3->unk1CC[i], &temp_s3->unk184[i]);
    }
}

void func_uvchannel_rom_004018D8(s32 arg0) {
    Mtx4F sp80;
    Mtx4F sp40;
    UnkStruct_uvchannel_rom_00401CD0 *temp_v0;
    f32 temp_fv1;
    f32 temp_fv0;

    temp_v0 = &D_uvchannel_rom_00401CD0[arg0];
    temp_fv1 = 2.0f * temp_v0->unkF0;
    D_uvchannel_rom_00401CF8->unkC(1.0f, 1.0f, 1.0f, 1.0f);
    D_uvchannel_rom_00401CF8->unk10(0, 0, 0x140, 0xF0);
    temp_fv0 = -temp_fv1;
    D_uvchannel_rom_00401CE0->func_00402038(&sp80, temp_fv0, temp_fv1, temp_fv0, temp_fv1,
                                            D_uvchannel_rom_00401CB0, D_uvchannel_rom_00401CB4);
    D_uvchannel_rom_00401CE0->func_00401000(&sp80, 1.5707963f, 0x78);
    D_uvchannel_rom_00401CE0->func_00402908(&sp80);
    D_uvchannel_rom_00401CE0->func_00400B68(&sp40);
    sp40.m[3][0] = -temp_v0->unk44.m[3][0];
    sp40.m[3][1] = -temp_v0->unk44.m[3][1];
    D_uvchannel_rom_00401CE0->func_004029DC(&sp40);
    D_uvchannel_rom_00401CE0->func_0040034C(0.0f, 0.0f, 0.0f);
}

void func_uvchannel_rom_00401A4C(s32 arg0) {
    UnkStruct_uvchannel_rom_00401CD0 *temp_s0;

    temp_s0 = &D_uvchannel_rom_00401CD0[arg0];
    D_uvchannel_rom_00401CEC->func_uvgfxmgr_rom_00401DC4();
    D_uvchannel_rom_00401CF0->func_uvgfxstate_rom_00401F54(0.0f, 0.0f);
    D_uvchannel_rom_00401CF0->func_uvgfxstate_rom_00402218();
    D_uvchannel_rom_00401CF0->func_uvgfxstate_rom_00401314(0x820FFF);
    D_uvchannel_rom_00401CF0->func_uvgfxstate_rom_00401354(0xB27C0000);
    D_uvchannel_rom_00401CF4->uvVtxBeginPoly();
    D_uvchannel_rom_00401CF4->uvVtx((s32) temp_s0->unk13C[5].x, (s32) temp_s0->unk13C[5].y, 0x3E8, 0, 0, 0xFF, 0, 0, 0x80);
    D_uvchannel_rom_00401CF4->uvVtx((s32) temp_s0->unk13C[1].x, (s32) temp_s0->unk13C[1].y, 0x3E8, 0, 0, 0xFF, 0, 0, 0x80);
    D_uvchannel_rom_00401CF4->uvVtx((s32) temp_s0->unk13C[2].x, (s32) temp_s0->unk13C[2].y, 0x3E8, 0, 0, 0xFF, 0, 0, 0x80);
    D_uvchannel_rom_00401CF4->uvVtxEndPoly();
    D_uvchannel_rom_00401CF0->func_uvgfxstate_rom_00402254();
}
