// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

extern u32 D_uvmath_rom_00400AF0;

//void uvUpdateFileAllocPtr(void*);
void __entrypoint_func_uvmath_rom_400000(UvMath_Exports* exports);
void func_uvmath_rom_00400108(void);
f32 uvSqrtF(f32 arg0);
f32 uvSinF(f32);
f32 uvCosF(f32);
f32 func_uvmath_rom_0040049C(f32);
f32 func_uvmath_rom_004004D0(f32, f32);
f32 func_uvmath_rom_00400504(f32, f32, f32);
f32 func_uvmath_rom_004006F8(f32, f32);

// Needs bss migration?
#if FALSE
extern s32 func_uvmath_rom_00400544;
extern s32 func_uvmath_rom_00400630;
extern s32 func_uvmath_rom_004006F8;
extern s32 func_uvmath_rom_00400838;
extern s32 func_uvmath_rom_00400850;
extern s32 func_uvmath_rom_00400880;
extern s32 func_uvmath_rom_0040088C;
extern s32 func_uvmath_rom_004008E4;
extern s32 func_uvmath_rom_0040092C;
extern s32 func_uvmath_rom_00400938;
extern s32 func_uvmath_rom_00400978;
extern s32 func_uvmath_rom_004009EC;

void __entrypoint_func_uvmath_rom_400000(UvMath_Exports* exports) {
    uvUpdateFileAllocPtr(exports);
    exports->unk0 = func_uvmath_rom_00400108;
    exports->unk28 = &func_uvmath_rom_00400838;
    exports->uvSqrtf = uvSqrtF;
    exports->unk2C = &func_uvmath_rom_00400850;
    exports->uvSinF = &uvSinF;
    exports->unk30 = &func_uvmath_rom_00400880;
    exports->uvCosF = &uvCosF;
    exports->unk34 = &func_uvmath_rom_0040088C;
    exports->uvTanF = &func_uvmath_rom_0040049C;
    exports->unk38 = &func_uvmath_rom_004008E4;
    exports->uvLength2D = &func_uvmath_rom_004004D0;
    exports->unk3C = &func_uvmath_rom_0040092C;
    exports->uvLength3D = &func_uvmath_rom_00400504;
    exports->unk40 = &func_uvmath_rom_00400938;
    exports->unk1C = &func_uvmath_rom_00400544;
    exports->unk44 = &func_uvmath_rom_00400978;
    exports->unk20 = &func_uvmath_rom_00400630;
    exports->unk48 = &func_uvmath_rom_004009EC;
    exports->uvAtan2F = &func_uvmath_rom_004006F8;
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmath_rom/__entrypoint_func_uvmath_rom_400000.s")
#endif

void func_uvmath_rom_00400108(void) {
}

f32 uvSqrtF(f32 arg0) {
    return arg0 < 0.0f ? 0.0f : sqrtf(arg0);
}

// Already matched but needs rodata migration
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmath_rom/uvSinF.s")

// Already matched but needs rodata migration
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmath_rom/uvCosF.s")

// uvTanF
f32 func_uvmath_rom_0040049C(f32 x) {
    return uvSinF(x) / uvCosF(x);
}

// uvLength2D
f32 func_uvmath_rom_004004D0(f32 x, f32 y) {
   return uvSqrtF(SQ(x) + SQ(y));
}

// uvLength3D
f32 func_uvmath_rom_00400504(f32 x, f32 y, f32 z) {
    return uvSqrtF(SQ(x) + SQ(y) + SQ(z));
}

f32 func_uvmath_rom_00400544(f32 arg0) {
    f32 temp_fv0;
    f32 var_fv1;

    if (arg0 > 1.0f) {
        var_fv1 = 1.0f;
        arg0 = 1.0f;
    } else if (arg0 < -1.0f) {
        var_fv1 = 1.0f;
        arg0 = -1.0f;
    } else if (arg0 > 0.0f) {
        var_fv1 = arg0;
    } else {
        var_fv1 = -arg0;
    }
    if (var_fv1 < 0.5f) {
        return func_uvmath_rom_004006F8(arg0, uvSqrtF(1.0f - (arg0 * arg0)));
    }
    temp_fv0 = 1.0f - var_fv1;
    return func_uvmath_rom_004006F8(arg0, uvSqrtF((2.0f * temp_fv0) - SQ(temp_fv0)));
}

extern f32 D_uvmath_rom_00400AAC;
extern f32 D_uvmath_rom_00400AB0;

f32 func_uvmath_rom_00400630(f32 arg0) {
    f32 temp_fv1;
    f32 var_f0;

    temp_fv1 = arg0 + 1.0f;
    if (temp_fv1 > 0.0f) {
        var_f0 = temp_fv1;
    } else {
        var_f0 = -temp_fv1;
    }
    if (var_f0 < D_uvmath_rom_00400AAC) {
        return D_uvmath_rom_00400AB0;
    }
    if (arg0 > 1.0f) {
        var_f0 = arg0 - 1.0f;
    } else {
        var_f0 = -(arg0 - 1.0f);
    }
    if (var_f0 < D_uvmath_rom_00400AAC) {
        return 0.0f;
    }
    return 2.0f * func_uvmath_rom_004006F8(uvSqrtF((1.0f - arg0) / temp_fv1), 1.0f);
}

// uvAtan2F
#ifdef NEEDS_RODATA
f32 func_uvmath_rom_004006F8(f32 y, f32 x) {
    s32 flag;
    f32 ret;
    f32 absY;
    f32 absX;
    f32 tmp;
    f32 absdiff;

    if ((x == 0.0f) && (y == 0.0f)) {
        return 0.0f;
    }
    absX = FABS(x);
    absY = FABS(y);
    flag = 0;
    if (absX < absY) {
        tmp = absX;
        absX = absY;
        absY = tmp;
        flag = 1;
    }
    tmp = (absY / absX);
    absdiff = FABS(absY - absX);
    ret = (PW_PI_4 * (absY / absX)) + (0.309f * absdiff * tmp);

    tmp = 0; // fake store to prevent emitting additional mov instr.
    if (x >= 0.0f) {
        if (flag) {
            ret = PW_PI_2 - ret;
        }
    } else if (flag) {
        ret = PW_PI_2 + ret;
    } else {
        ret = PW_PI - ret;
    }

    if (y >= 0.0f) {
        return ret;
    } else {
        return -ret;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmath_rom/func_uvmath_rom_004006F8.s")
#endif

// uvAbs
s32 func_uvmath_rom_00400838(s32 x) {
    return x < 0 ? -x : x;
}

// uvFabs
f32 func_uvmath_rom_00400850(f32 x) {
    return x < 0.0f ? -x : x;
}

// uvSrand
void func_uvmath_rom_00400880(s32 arg0) {
    D_uvmath_rom_00400AF0 = arg0;
}

// uvRandFLcg
f32 func_uvmath_rom_0040088C(void) {
    u32 val;
    D_uvmath_rom_00400AF0 = (D_uvmath_rom_00400AF0 * 1103515245) + 12345;
    val = (D_uvmath_rom_00400AF0 >> 16) & 0x7FFF;
    return (f32)val / 0x8000;
}

extern f32 D_uvmath_rom_00400AC8;

f32 func_uvmath_rom_004008E4(void) {
    return (f32) (osGetCount() & 0xFFFF) / D_uvmath_rom_00400AC8;
}

s32 func_uvmath_rom_0040092C(void) {
    return D_uvmath_rom_00400AF0;
}

f32 func_uvmath_rom_00400938(f32 arg0, f32 arg1, f32 arg2) {
    if (arg1 < arg0) {
        return arg1;
    }
    if (arg0 < arg2) {
        return arg2;
    }
    return arg0;
}

// Matched but eeds rodata migration
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmath_rom/func_uvmath_rom_00400978.s")

// Matched but eeds rodata migration
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvmath_rom/func_uvmath_rom_004009EC.s")
