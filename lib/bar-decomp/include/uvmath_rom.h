// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef UVMATH_ROM_H
#define UVMATH_ROM_H

typedef struct UvMath_Exports_s {
    /* 0x00 */ void (*unk0)(void);
    /* 0x04 */ f32 (*uvSqrtf)(f32);
    /* 0x08 */ f32 (*uvSinF)(f32);
    /* 0x0C */ f32 (*uvCosF)(f32);
    /* 0x10 */ f32 (*uvTanF)(f32);
    /* 0x14 */ f32 (*uvLength2D)(f32 x, f32 y);
    /* 0x18 */ f32 (*uvLength3D)(f32 x, f32 y, f32 z);
    /* 0x1C */ f32 (*uvArcSinF)(f32);
    /* 0x20 */ f32 (*uvArcCosF)(f32);
    /* 0x24 */ f32 (*uvAtan2F)(f32, f32);
    /* 0x28 */ s32 (*uvAbs)(s32);
    /* 0x2C */ f32 (*uvFabs)(f32);
    /* 0x30 */ void (*uvSrand)(s32);
    /* 0x34 */ f32 (*uvRandFLcg)(void);
    /* 0x38 */ void *unk38;
    /* 0x3C */ s32 (*uvGetRandSeed)(void);
    /* 0x40 */ f32 (*uvClampF)(f32, f32, f32);
    /* 0x44 */ f32 (*uvNormalizeAngle)(f32);
    /* 0x48 */ void *unk48;
} UvMath_Exports; // size = 0x4C

#endif /* UVMATH_ROM_H */

