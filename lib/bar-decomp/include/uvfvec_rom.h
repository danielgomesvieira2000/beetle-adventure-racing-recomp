// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef UVFVEC_ROM_H
#define UVFVEC_ROM_H
typedef struct Vec3F_s {
    f32 x;
    f32 y;
    f32 z;
} Vec3F;

typedef struct Vec2F_s {
    f32 x;
    f32 y;
} Vec2F;

typedef struct UvFVec_Rom_Exports_s {
    /* 0x00 */ void (*uvModuleCleanup)();
    /* 0x04 */ void (*uvVec3FSet)(Vec3F *, f32, f32, f32);
    /* 0x08 */ s32  (*uvVec3FNormalize)(Vec3F *, Vec3F *);
    /* 0x0C */ f32  (*uvVec3FLen)(Vec3F *);
    /* 0x10 */ f32  (*uvVec3FLenSquared)(Vec3F *);
    /* 0x14 */ f32  (*uvVec3FDistance)(Vec3F *, Vec3F *);
    /* 0x18 */ f32  (*uvVec3FDot)(Vec3F *, Vec3F *);
    /* 0x1C */ void (*uvVec3FCopy)(Vec3F *, Vec3F *);
    /* 0x20 */ f32  (*uvVec3FScalarProj)(Vec3F *, Vec3F *);
    /* 0x24 */ void (*uvVec3FCross)(Vec3F *, Vec3F *, Vec3F *);
    /* 0x28 */ void (*uvVec3FAdd)(Vec3F *, Vec3F *, Vec3F *);
    /* 0x2C */ void (*uvVec3FMult)(Vec3F *, Vec3F *, Vec3F *);
    /* 0x30 */ void (*uvVec3FAddScaled)(Vec3F *, Vec3F *, f32, Vec3F *);
    /* 0x34 */ void (*uvVec3FSub)(Vec3F *, Vec3F *, Vec3F *);
    /* 0x38 */ void (*uvVec3FScale)(Vec3F *, Vec3F *, f32);
    /* 0x3C */ void (*uvVec3FSphereAngles)(f32 *, f32 *, Vec3F *);
    /* 0x40 */ void (*uvVec3FFromSpherical)(Vec3F *, f32, f32);
    /* 0x44 */ s32  (*uvVec3FIsZero)(Vec3F *);
    /* 0x48 */ void (*uvVec3FNegate)(Vec3F *, Vec3F *);
    /* 0x4C */ void (*uvVec2FSet)(Vec2F *, f32, f32);
    /* 0x50 */ s32  (*uvVec2FNormalize)(Vec2F *, Vec2F *);
    /* 0x54 */ f32  (*uvVec2FLen)(Vec2F *);
    /* 0x58 */ f32  (*uvVec2FLenSquared)(Vec2F *);
    /* 0x5C */ f32  (*uvVec2FDot)(Vec2F *, Vec2F *);
    /* 0x60 */ void (*uvVec2FCopy)(Vec2F *, Vec2F *);
    /* 0x64 */ f32  (*uvVec2FScalarProj)(Vec2F *, Vec2F *);
    /* 0x68 */ void (*uvVec2FAdd)(Vec2F *, Vec2F *, Vec2F *);
    /* 0x6C */ void (*uvVec2FAddScaled)(Vec2F *, Vec2F *, f32, Vec2F *);
    /* 0x70 */ void (*uvVec2FSub)(Vec2F *, Vec2F *, Vec2F *);
    /* 0x74 */ void (*uvVec2FScale)(Vec2F *, Vec2F *, f32);
    /* 0x78 */ s32  (*uvVec2FIsZero)(Vec2F *);
    /* 0x7C */ void (*uvVec2FNegate)(Vec2F *, Vec2F *);
    /* 0x80 */ void (*uvVec2FReflect)(Vec2F *, Vec2F *, Vec2F *, f32);
} UvFVec_Rom_Exports;                                       /* size = 0x84 */

#endif /* UVFVEC_ROM_H */

