// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

void uvUnloadModule(s32 tag);                                 /* extern */

void uvModuleCleanup(void);
void uvVec2FAdd(Vec2F *vdst, Vec2F *va, Vec2F *vb);
void uvVec2FAdd(Vec2F *vdst, Vec2F *va, Vec2F *vb);
void uvVec2FAddScaled(Vec2F *vdst, Vec2F *va, f32 sb, Vec2F *vb);
void uvVec2FAddScaled(Vec2F *vdst, Vec2F *va, f32 sb, Vec2F *vb);
void uvVec2FCopy(Vec2F *va, Vec2F *vb);
void uvVec2FCopy(Vec2F *va, Vec2F *vb);
f32 uvVec2FDot(Vec2F *va, Vec2F *vb);
f32 uvVec2FDot(Vec2F *va, Vec2F *vb);
s32 uvVec2FIsZero(Vec2F *v);
f32 uvVec2FLen(Vec2F *v);
f32 uvVec2FLenghtSquared(Vec2F *v);
f32 uvVec2FLenghtSquared(Vec2F *arg0);
void uvVec2FNegate(Vec2F *vdst, Vec2F *vsrc);
s32 uvVec2FNormalize(Vec2F *va, Vec2F *vb);
void uvVec2FReflect(Vec2F *va, Vec2F *vb, Vec2F *vc, f32 factor);
f32 uvVec2FScalarProj(Vec2F *arg0, Vec2F *arg1);
void uvVec2FScale(Vec2F *vdst, Vec2F *va, f32 sb);
void uvVec2FScale(Vec2F *vdst, Vec2F *va, f32 sb);
void uvVec2FSet(Vec2F *vd, f32 x, f32 y);
void uvVec2FSub(Vec2F *vdst, Vec2F *va, Vec2F *vb);
void uvVec2FSub(Vec2F *vdst, Vec2F *va, Vec2F *vb);
void uvVec3FAdd(Vec3F *vdst, Vec3F *va, Vec3F *vb);
void uvVec3FAdd(Vec3F *vdst, Vec3F *va, Vec3F *vb);
void uvVec3FAddScaled(Vec3F *vdst, Vec3F *va, f32 sb, Vec3F *vb);
void uvVec3FAddScaled(Vec3F *vdst, Vec3F *va, f32 sb, Vec3F *vb);
void uvVec3FCopy(Vec3F *vdst, Vec3F *vsrc);
void uvVec3FCross(Vec3F *vd, Vec3F *va, Vec3F *vb);
void uvVec3FCross(Vec3F *arg0, Vec3F *arg1, Vec3F *arg2);
f32 uvVec3FDistance(Vec3F *va, Vec3F *vb);
f32 uvVec3FDistance(Vec3F *arg0, Vec3F *arg1);
f32 uvVec3FDot(Vec3F *va, Vec3F *vb);
s32 uvVec3FIsZero(Vec3F *v);
f32 uvVec3FLen(Vec3F *v);
f32 uvVec3FLenghtSquared(Vec3F *v);
f32 uvVec3FLenghtSquared(Vec3F *vec);
void uvVec3FMult(Vec3F *vdst, Vec3F *va, Vec3F *vb);
void uvVec3FMult(Vec3F *vdst, Vec3F *va, Vec3F *vb);
void uvVec3FNegate(Vec3F *va, Vec3F *vb);
s32 uvVec3FNormalize(Vec3F *vd, Vec3F *vs);
s32 uvVec3FNormalize(Vec3F *vd, Vec3F *vs);
f32 uvVec3FScalarProj(Vec3F *arg0, Vec3F *arg1);
void uvVec3FScale(Vec3F *vdst, Vec3F *va, f32 scale);
void uvVec3FSet(Vec3F *vec, f32 x, f32 y, f32 z);
void uvVec3FSphereAngles(f32 *azimuth, f32 *elevation, Vec3F *v);
void uvVec3FSub(Vec3F *vdst, Vec3F *va, Vec3F *vb);
void uvVec3FSub(Vec3F *vdst, Vec3F *va, Vec3F *vb);
void uvVec3fFromSpherical(Vec3F *arg0, f32 arg1, f32 arg2);

void __entrypoint_func_uvfvec_rom_400000(UvFVec_Rom_Exports* exports);

static UvMath_Exports* sUvMathRomExports;

void __entrypoint_func_uvfvec_rom_400000(UvFVec_Rom_Exports* exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->uvVec3FSet = uvVec3FSet;
    exports->uvVec3FNormalize = uvVec3FNormalize;
    exports->uvVec3FLen = uvVec3FLen;
    exports->uvVec3FLenSquared = uvVec3FLenghtSquared;
    exports->uvVec3FDistance = uvVec3FDistance;
    exports->uvVec3FDot = uvVec3FDot;
    exports->uvVec3FCopy = uvVec3FCopy;
    exports->uvVec3FScalarProj = uvVec3FScalarProj;
    exports->uvVec3FCross = uvVec3FCross;
    exports->uvModuleCleanup = uvModuleCleanup;
    exports->uvVec3FAdd = uvVec3FAdd;
    exports->uvVec2FNormalize = uvVec2FNormalize;
    exports->uvVec3FMult = uvVec3FMult;
    exports->uvVec2FIsZero = uvVec2FIsZero;
    exports->uvVec2FLen = uvVec2FLen;
    exports->uvVec3FAddScaled = uvVec3FAddScaled;
    exports->uvVec2FNegate = uvVec2FNegate;
    exports->uvVec2FLenSquared = uvVec2FLenghtSquared;
    exports->uvVec3FSub = uvVec3FSub;
    exports->uvVec2FReflect = uvVec2FReflect;
    exports->uvVec2FDot = uvVec2FDot;
    exports->uvVec3FScale = uvVec3FScale;
    exports->uvVec2FCopy = uvVec2FCopy;
    exports->uvVec3FSphereAngles = uvVec3FSphereAngles;
    exports->uvVec2FScalarProj = uvVec2FScalarProj;
    exports->uvVec3FFromSpherical = uvVec3fFromSpherical;
    exports->uvVec2FAdd = uvVec2FAdd;
    exports->uvVec3FIsZero = uvVec3FIsZero;
    exports->uvVec2FAddScaled = uvVec2FAddScaled;
    exports->uvVec3FNegate = uvVec3FNegate;
    exports->uvVec2FSub = uvVec2FSub;
    exports->uvVec2FSet = uvVec2FSet;
    exports->uvVec2FScale = uvVec2FScale;
    #line 2
    sUvMathRomExports = uvLoadModule('MATH');
}

void uvModuleCleanup(void) {
    uvUnloadModule('MATH'); // Unload math module
}

void uvVec3FSet(Vec3F *vec, f32 x, f32 y, f32 z) {
    vec->x = x;
    vec->y = y;
    vec->z = z;
}

s32 uvVec3FNormalize(Vec3F* vd, Vec3F* vs) {
    f32 ax;
    f32 ay;
    f32 az;
    f32 lenInv;

    ax = vs->x;
    ay = vs->y;
    az = vs->z;
    lenInv = sUvMathRomExports->uvSqrtf(SQ(ax) + SQ(ay) + SQ(az));
    if (lenInv == 0.0f) {
        return FALSE;
    }
    lenInv = 1.0f / lenInv;
    vd->x = ax * lenInv;
    vd->y = ay * lenInv;
    vd->z = az * lenInv;
    return TRUE;
}

f32 uvVec3FLen(Vec3F *v) {
    return sUvMathRomExports->uvSqrtf(uvVec3FLenghtSquared(v));
}

f32 uvVec3FLenghtSquared(Vec3F* v) {
    return SQ(v->x) + SQ(v->y) + SQ(v->z);
}

f32 uvVec3FDistance(Vec3F *va, Vec3F *vb) {
    Vec3F v;

    uvVec3FSub(&v, va, vb);
    return sUvMathRomExports->uvSqrtf(uvVec3FLenghtSquared(&v));
}

f32 uvVec3FDot(Vec3F* va, Vec3F* vb) {
    return (va->x * vb->x) + (va->y * vb->y) + (va->z * vb->z);
}

void uvVec3FCopy(Vec3F *vdst, Vec3F *vsrc) {
    vdst->x = vsrc->x;
    vdst->y = vsrc->y;
    vdst->z = vsrc->z;
}

f32 uvVec3FScalarProj(Vec3F* arg0, Vec3F* arg1) {
    f32 sp1C;
    f32 sp18;

    sp18 = uvVec3FDot(arg0, arg1);
    sp1C = uvVec3FLen(arg0);
    return sp18 / (uvVec3FLen(arg1) * sp1C);
}

void uvVec3FCross(Vec3F* vd, Vec3F* va, Vec3F* vb) {
    f32 ax = va->x, ay = va->y, az = va->z;
    f32 bx = vb->x, by = vb->y, bz = vb->z;

    vd->x = ((ay * bz) - (az * by));
    vd->y = -((ax * bz) - (az * bx));
    vd->z = ((ax * by) - (ay * bx));
}

void uvVec3FAdd(Vec3F *vdst, Vec3F *va, Vec3F *vb) {
    vdst->x = va->x + vb->x;
    vdst->y = va->y + vb->y;
    vdst->z = va->z + vb->z;
}

void uvVec3FMult(Vec3F *vdst, Vec3F *va, Vec3F *vb) {
    vdst->x = va->x * vb->x;
    vdst->y = va->y * vb->y;
    vdst->z = va->z * vb->z;
}

void uvVec3FAddScaled(Vec3F *vdst, Vec3F *va, f32 sb, Vec3F *vb) {
    vdst->x = va->x + (sb * vb->x);
    vdst->y = va->y + (sb * vb->y);
    vdst->z = va->z + (sb * vb->z);
}

void uvVec3FSub(Vec3F *vdst, Vec3F *va, Vec3F *vb) {
    s32 pad;
    vdst->x = va->x - vb->x;
    vdst->y = va->y - vb->y;
    vdst->z = va->z - vb->z;
}

void uvVec3FScale(Vec3F *vdst, Vec3F *va, f32 scale) {
    vdst->x = va->x * scale;
    vdst->y = va->y * scale;
    vdst->z = va->z * scale;
}

void uvVec3FSphereAngles(f32 *azimuth, f32 *elevation, Vec3F *v) {
    uvVec3FNormalize(v, v);
    *azimuth = sUvMathRomExports->uvAtan2F(-v->x, v->y);
    *elevation = sUvMathRomExports->uvArcSinF(v->z);
}

void uvVec3fFromSpherical(Vec3F* v, f32 azimuth, f32 elevation) {
    f32 x;
    f32 y;
    s32 pad1C;
    f32 cosf;
    f32 ang;
    f32 z;

    if (elevation > 0.0f) {
        ang = elevation;
    } else {
        ang = -elevation;
    }
    if (ang < 1.5707963f) {
        cosf = sUvMathRomExports->uvCosF(elevation);
        x = -sUvMathRomExports->uvSinF(azimuth) * cosf;
        y = sUvMathRomExports->uvCosF(azimuth) * cosf;
        z = sUvMathRomExports->uvSinF(elevation);
    } else if (1.5707963f < ang) {
        while (elevation > 3.1415927f) {
            elevation -= 6.2831855f;
        }

        while (elevation < -3.1415927f) {
            elevation += 6.2831855f;
        }

        if (1.5707963f < elevation) {
            elevation = 1.5707963f - elevation;
        }
        if (elevation < -1.5707963f) {
            elevation = -1.5707963f - elevation;
        }

        cosf = sUvMathRomExports->uvCosF(elevation);
        x = sUvMathRomExports->uvSinF(azimuth) * cosf;
        y = -sUvMathRomExports->uvCosF(azimuth) * cosf;
        z = sUvMathRomExports->uvSinF(elevation);
    } else {
        y = 0;
        x = 0;
        if (elevation > 0) {
            z = 1.0f;
        } else {
            z = -1.0f;
        }
    }
    uvVec3FSet(v, x, y, z);
}

s32 uvVec3FIsZero(Vec3F *v) {
    if ((v->x == 0.0) && (v->y == 0.0) && (v->z == 0.0)) {
        return TRUE;
    }
    return FALSE;
}

void uvVec3FNegate(Vec3F* va, Vec3F *vb) {
    va->x = -vb->x;
    va->y = -vb->y;
    va->z = -vb->z;
}

void uvVec2FSet(Vec2F *vd, f32 x, f32 y) {
    vd->x = x;
    vd->y = y;
}

s32 uvVec2FNormalize(Vec2F* va, Vec2F* vb) {
    f32 ax;
    f32 ay;
    f32 len;

    ax = vb->x;
    ay = vb->y;
    len = sUvMathRomExports->uvSqrtf(SQ(ax) + SQ(ay));
    if (len == 0.0f) {
        return FALSE;
    }
    va->x = ax / len;
    va->y = ay / len;
    return TRUE;
}

f32 uvVec2FLen(Vec2F *v) {
    return sUvMathRomExports->uvSqrtf(uvVec2FLenghtSquared(v));
}

f32 uvVec2FLenghtSquared(Vec2F *v) {
    return SQ(v->x) + SQ(v->y);
}

f32 uvVec2FDot(Vec2F *va, Vec2F *vb) {
    return (va->x * vb->x) + (va->y * vb->y);
}

void uvVec2FCopy(Vec2F *va, Vec2F *vb) {
    va->x = vb->x;
    va->y = vb->y;
}

f32 uvVec2FScalarProj(Vec2F* arg0, Vec2F* arg1) {
    f32 sp1C;
    f32 sp18;

    sp18 = uvVec2FDot(arg0, arg1);
    sp1C = uvVec2FLen(arg0);
    return sp18 / (uvVec2FLen(arg1) * sp1C);
}

void uvVec2FAdd(Vec2F *vdst, Vec2F *va, Vec2F *vb) {
    vdst->x = va->x + vb->x;
    vdst->y = va->y + vb->y;
}

void uvVec2FAddScaled(Vec2F *vdst, Vec2F *va, f32 sb, Vec2F *vb) {
    vdst->x = va->x + (sb * vb->x);
    vdst->y = va->y + (sb * vb->y);
}

void uvVec2FSub(Vec2F *vdst, Vec2F *va, Vec2F *vb) {
    vdst->x = va->x - vb->x;
    vdst->y = va->y - vb->y;
}

void uvVec2FScale(Vec2F *vdst, Vec2F *va, f32 sb) {
    vdst->x = va->x * sb;
    vdst->y = va->y * sb;
}

s32 uvVec2FIsZero(Vec2F *v) {
    if ((v->x == 0.0) && (v->y == 0.0)) {
        return TRUE;
    }
    return FALSE;
}

void uvVec2FNegate(Vec2F *vdst, Vec2F *vsrc) {
    vdst->x = -vsrc->x;
    vdst->y = -vsrc->y;
}

void uvVec2FReflect(Vec2F* va, Vec2F* vb, Vec2F* vc, f32 factor) {
    UNUSED s32 pad;
    Vec2F dest;

    uvVec2FScale(&dest, vb, uvVec2FDot(vb, vc) * (1.0f + factor));
    uvVec2FSub(va, vc, &dest);
}
