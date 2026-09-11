// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "os.h"

extern UnkStruct_uvear_rom_0040085 *D_uvear_rom_00400854;
extern u16 D_uvear_rom_00400850;
extern s32 D_uvear_rom_00400860[];
extern UvFVec_Rom_Exports *sUvFvecExports;
extern f64 D_uvear_rom_00400858;

void __entrypoint_func_uvear_rom_400000(UvEar_Rom_Exports *exports);
void func_uvear_rom_0040018C(void);
void func_uvear_rom_004001BC(u16 arg0);
void func_uvear_rom_00400230(u16 arg0, Vec3F vec);
void func_uvear_rom_00400290(u16 arg0, Vec3F *arg1);
void func_uvear_rom_004002E0(u16 arg0, Vec3F arg1);
void func_uvear_rom_00400340(u16 arg0, Vec3F *arg1);
void func_uvear_rom_00400390(u16 arg0, UnkStruct_uvear_rom_0040085 *arg1);
void func_uvear_rom_00400424(u16 arg0, Vec3F arg1);
void func_uvear_rom_00400494(u16 arg0, Vec3F *arg1);
void func_uvear_rom_004004E4(u16 arg0);
f32 func_uvear_rom_004006D4(Vec3F *v1, Vec3F *v2);
s32 func_uvear_rom_0040077C(s32 arg0, Vec3F *arg1);

void __entrypoint_func_uvear_rom_400000(UvEar_Rom_Exports *exports) {
    u16 i;
    u16 *size;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvear_rom_0040018C = func_uvear_rom_0040018C;
    exports->func_uvear_rom_00400230 = func_uvear_rom_00400230;
    exports->func_uvear_rom_00400290 = func_uvear_rom_00400290;
    exports->func_uvear_rom_004002E0 = func_uvear_rom_004002E0;
    exports->func_uvear_rom_00400340 = func_uvear_rom_00400340;
    exports->func_uvear_rom_00400390 = func_uvear_rom_00400390;
    exports->func_uvear_rom_00400424 = func_uvear_rom_00400424;
    exports->func_uvear_rom_00400494 = func_uvear_rom_00400494;
    exports->func_uvear_rom_004004E4 = func_uvear_rom_004004E4;
    exports->func_uvear_rom_0040077C = func_uvear_rom_0040077C;
#line 33
    size = uvGetSystemProp(5);
    if (size == NULL) {
        D_uvear_rom_00400850 = 4;
    } else {
        if (*size != 0) {
            D_uvear_rom_00400850 = *size;
        } else {
            D_uvear_rom_00400850 = 4;
        }
    }
    D_uvear_rom_00400854 =
        _uvMemAllocAlign8(D_uvear_rom_00400850 * sizeof(UnkStruct_uvear_rom_0040085));
    for (i = 0; i < D_uvear_rom_00400850; i++) {
        func_uvear_rom_004001BC(i);
        D_uvear_rom_00400860[i] = 1;
    }

    uvClkReset(0x6A);
    D_uvear_rom_00400858 = uvClkGetSec(0x6A);
    sUvFvecExports = uvLoadModule('FVEC');
}

void func_uvear_rom_0040018C(void) {
    _uvMemFree(D_uvear_rom_00400854);
    uvUnloadModule('FVEC');
}

void func_uvear_rom_004001BC(u16 arg0) {
    UnkStruct_uvear_rom_0040085 *temp_v0;

    temp_v0 = &D_uvear_rom_00400854[arg0];
    temp_v0->vec.x = 0.0f;
    temp_v0->vec.y = 0.0f;
    temp_v0->vec.z = 0.0f;
    temp_v0->unkC.x = 0.0f;
    temp_v0->unkC.y = 0.0f;
    temp_v0->unkC.z = 0.0f;
    temp_v0->unk18 = 0.0f;
    temp_v0->unk1C = 0.0f;
    temp_v0->unk20 = 0.0f;
    temp_v0->unk24 = 0.0f;
    temp_v0->unk28 = 0.0f;
    temp_v0->unk2C = 0.0f;
    temp_v0->unk30.x = 0.0f;
    temp_v0->unk30.y = 0.0f;
    temp_v0->unk30.z = 0.0f;
    temp_v0->unk40 = -1.0;
}

void func_uvear_rom_00400230(u16 arg0, Vec3F vec) {
    if (arg0 < D_uvear_rom_00400850) {
        D_uvear_rom_00400854[arg0].vec = vec;
    }
}

void func_uvear_rom_00400290(u16 arg0, Vec3F *arg1) {
    if (arg0 < D_uvear_rom_00400850) {
        *arg1 = D_uvear_rom_00400854[arg0].vec;
    }
}

void func_uvear_rom_004002E0(u16 arg0, Vec3F arg1) {
    UnkStruct_uvear_rom_0040085 *temp_t0;

    if (arg0 < D_uvear_rom_00400850) {
        D_uvear_rom_00400854[arg0].unkC = arg1;
    }
}

void func_uvear_rom_00400340(u16 arg0, Vec3F *arg1) {
    if (arg0 < D_uvear_rom_00400850) {
        *arg1 = D_uvear_rom_00400854[arg0].unkC;
    }
}

void func_uvear_rom_00400390(u16 arg0, UnkStruct_uvear_rom_0040085 *arg1) {
    if (arg0 < D_uvear_rom_00400850) {
        D_uvear_rom_00400854[arg0].unkC.x = arg1->vec.x;
        D_uvear_rom_00400854[arg0].unkC.y = arg1->vec.y;
        D_uvear_rom_00400854[arg0].unkC.z = arg1->vec.z;
        D_uvear_rom_00400854[arg0].vec.x = arg1->unk30.x;
        D_uvear_rom_00400854[arg0].vec.y = arg1->unk30.y;
        D_uvear_rom_00400854[arg0].vec.z = arg1->unk30.z;
    }
}

void func_uvear_rom_00400424(u16 arg0, Vec3F arg1) {
    if (arg0 < D_uvear_rom_00400850) {
        D_uvear_rom_00400860[arg0] = 0;
        D_uvear_rom_00400854[arg0].unk30 = arg1;
    }
}

void func_uvear_rom_00400494(u16 arg0, Vec3F *arg1) {
    if (arg0 < D_uvear_rom_00400850) {
        *arg1 = D_uvear_rom_00400854[arg0].unk30;
    }
}

void func_uvear_rom_004004E4(u16 arg0) {
    Vec3F sp4C;
    Vec3F sp40;
    Vec3F sp34;
    f32 sp30;

    if (!(arg0 < D_uvear_rom_00400850)) {
        return;
    }

    D_uvear_rom_00400858 = uvClkGetSec(0x6A);
    if (D_uvear_rom_00400858 != D_uvear_rom_00400854[arg0].unk40) {
        sUvFvecExports->uvVec3FCopy(&sp4C, &D_uvear_rom_00400854[arg0].vec);
        sUvFvecExports->uvVec3FCopy(&sp40, &D_uvear_rom_00400854[arg0].unkC);
        if (D_uvear_rom_00400860[arg0] != 0) {
            sp30 = (f32) (D_uvear_rom_00400858 - D_uvear_rom_00400854[arg0].unk40);
            sUvFvecExports->uvVec3FSub(&sp34, &sp4C, (Vec3F *) &D_uvear_rom_00400854[arg0].unk18);
            sUvFvecExports->uvVec3FScale(&sp34, &sp34, 1.0f / sp30);
        } else {
            sUvFvecExports->uvVec3FCopy(&sp34, &D_uvear_rom_00400854[arg0].unk30);
        }
        sUvFvecExports->uvVec3FCopy((Vec3F *) &D_uvear_rom_00400854[arg0].unk18, &sp4C);
        sUvFvecExports->uvVec3FCopy((Vec3F *) &D_uvear_rom_00400854[arg0].unk24, &sp40);
        sUvFvecExports->uvVec3FCopy(&D_uvear_rom_00400854[arg0].unk30, &sp34);
        D_uvear_rom_00400854[arg0].unk40 = D_uvear_rom_00400858;
    }
}

f32 func_uvear_rom_004006D4(Vec3F *v1, Vec3F *v2) {
    f32 var_fa0;
    f32 var_fa1;

    if (v2->x < v1->x) {
        var_fa0 = v1->x - v2->x;
    } else {
        var_fa0 = -(v1->x - v2->x);
    }
    if (v2->y < v1->y) {
        var_fa1 = v1->y - v2->y;
    } else {
        var_fa1 = -(v1->y - v2->y);
    }
    if (!(var_fa1 < var_fa0)) {
        var_fa0 = var_fa1;
    }
    if (v2->z < v1->z) {
        var_fa1 = v1->z - v2->z;
    } else {
        var_fa1 = -(v1->z - v2->z);
    }
    if (!(var_fa1 < var_fa0)) {
        var_fa0 = var_fa1;
    }
    return var_fa0;
}

s32 func_uvear_rom_0040077C(s32 arg0, Vec3F *arg1) {
    f32 temp_fv0;
    f32 var_fs0;
    s32 var_s0;
    s32 i;
    s32 found;

    var_fs0 = 200000.0f;
    found = 0;

    for (i = 0; i < arg0; i++) {
        temp_fv0 = func_uvear_rom_004006D4(arg1, &D_uvear_rom_00400854[i].vec);
        if (temp_fv0 < var_fs0) {
            var_fs0 = temp_fv0;
            found = i;
        }
    }

    return found;
}
