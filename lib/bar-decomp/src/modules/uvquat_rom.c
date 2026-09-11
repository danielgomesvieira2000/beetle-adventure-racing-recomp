// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

void __entrypoint_func_uvquat_rom_400000(UvQuat_Exports *exports);
void uvModuleCleanup(void);
void func_uvquat_rom_004000D8(Quat *arg0, Mtx4F *arg1);
void uvQuatFromEuler(Quat *arg0, f32 arg1, f32 arg2, f32 arg3);
void func_uvquat_rom_00400524(Mtx4F *arg0, Quat *arg1);
void func_uvquat_rom_0040070C(Quat *arg0, Quat *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);
void uvQuatMultiply(Quat *arg0, Quat *arg1, Quat *arg2);
void uvQuatCopy(Quat *arg0, Quat *arg1);
void uvQuatCopy(Quat *arg0, Quat *arg1);
void func_uvquat_rom_004009BC(Quat *arg0, Quat *arg1, Quat *arg2, f32 arg3);

// .bss
extern UvMath_Exports *D_uvquat_rom_00400B50;
extern UvFMtx_Rom_Exports *D_uvquat_rom_00400B54;
extern Mtx4F D_uvquat_rom_00400B58;

void __entrypoint_func_uvquat_rom_400000(UvQuat_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->uvModuleCleanup = uvModuleCleanup;
    exports->func_uvquat_rom_004000D8 = func_uvquat_rom_004000D8;
    exports->uvQuatFromEuler = uvQuatFromEuler;
    exports->func_uvquat_rom_00400524 = func_uvquat_rom_00400524;
    exports->func_uvquat_rom_0040070C = func_uvquat_rom_0040070C;
    exports->uvQuatMultiply = uvQuatMultiply;
    exports->uvQuatCopy = uvQuatCopy;
    exports->func_uvquat_rom_004009BC = func_uvquat_rom_004009BC;
#line 1
    D_uvquat_rom_00400B50 = uvLoadModule('MATH');
    D_uvquat_rom_00400B54 = uvLoadModule('FMTX');
}

void uvModuleCleanup(void) {
    uvUnloadModule('MATH');
    uvUnloadModule('FMTX');
}

void func_uvquat_rom_004000D8(Quat* quat, Mtx4F* mtx) {
    f32 temp_fa1;
    f32 temp_ft5;
    f32 temp_fv0_2;
    f32 temp_fv1_2;
    s32 var_v0;

    temp_ft5 = mtx->m[1][1] + mtx->m[0][0];
    temp_fa1 = mtx->m[2][2] + temp_ft5;
    if (temp_fa1 >= 0.0f) {
        temp_fv0_2 = D_uvquat_rom_00400B50->uvSqrtf(temp_fa1 + 1.0f);
        temp_fv1_2 = 0.5f / temp_fv0_2;
        quat->w = temp_fv0_2 * 0.5f;
        quat->x = (mtx->m[2][1] - mtx->m[1][2]) * temp_fv1_2;
        quat->y = (mtx->m[0][2] - mtx->m[2][0]) * temp_fv1_2;
        quat->z = (mtx->m[1][0] - mtx->m[0][1]) * temp_fv1_2;
        return;
    }
    var_v0 = 0;
    if (mtx->m[0][0] < mtx->m[1][1]) {
        var_v0 = 1;
    }
    if (mtx->m[var_v0][var_v0] < mtx->m[2][2]) {
        var_v0 = 2;
    }

    switch (var_v0) {                               /* irregular */
    case 0:
        temp_fv0_2 = D_uvquat_rom_00400B50->uvSqrtf((mtx->m[0][0] - (mtx->m[1][1] + mtx->m[2][2])) + 1.0f);
        temp_fv1_2 = 0.5f / temp_fv0_2;
        quat->x = temp_fv0_2 * 0.5f;
        quat->y = (mtx->m[0][1] + mtx->m[1][0]) * temp_fv1_2;
        quat->z = (mtx->m[2][0] + mtx->m[0][2]) * temp_fv1_2;
        quat->w = (mtx->m[2][1] - mtx->m[1][2]) * temp_fv1_2;
        break;
    case 1:
        temp_fv0_2 = D_uvquat_rom_00400B50->uvSqrtf((mtx->m[1][1] - (mtx->m[2][2] + mtx->m[0][0])) + 1.0f);
        temp_fv1_2 = 0.5f / temp_fv0_2;
        quat->y = temp_fv0_2 * 0.5f;
        quat->z = (mtx->m[1][2] + mtx->m[2][1]) * temp_fv1_2;
        quat->x = (mtx->m[0][1] + mtx->m[1][0]) * temp_fv1_2;
        quat->w = (mtx->m[0][2] - mtx->m[2][0]) * temp_fv1_2;
        break;
    case 2:
        temp_fv0_2 = D_uvquat_rom_00400B50->uvSqrtf((mtx->m[2][2] - temp_ft5) + 1.0f);
        temp_fv1_2 = 0.5f / temp_fv0_2;
        quat->z = temp_fv0_2 * 0.5f;
        quat->x = (mtx->m[2][0] + mtx->m[0][2]) * temp_fv1_2;
        quat->y = (mtx->m[1][2] + mtx->m[2][1]) * temp_fv1_2;
        quat->w = (mtx->m[1][0] - mtx->m[0][1]) * temp_fv1_2;
        break;
    }
}

void uvQuatFromEuler(Quat *quat, f32 x, f32 y, f32 z) {
    f32 temp_ft4;
    f32 sp58;
    f32 temp_fa1;
    f32 temp_fs0;
    f32 sp4C;
    f32 sp48;
    f32 temp_fs1;
    f32 temp_fv0;
    f32 temp_fv1;
    f32 pad;
    f32 temp_fa0;

    temp_fs0 = (x + 1.5707963f) * 0.5f;
    temp_fs1 = D_uvquat_rom_00400B50->uvSinF(temp_fs0);
    temp_fs0 = D_uvquat_rom_00400B50->uvCosF(temp_fs0);
    temp_fa0 = -y * 0.5f;
    sp4C = D_uvquat_rom_00400B50->uvSinF(temp_fa0);
    sp58 = D_uvquat_rom_00400B50->uvCosF(temp_fa0);

    sp48 = D_uvquat_rom_00400B50->uvSinF(((z - 3.1415927f) * 0.5f));
    temp_fv0 = D_uvquat_rom_00400B50->uvCosF(((z - 3.1415927f) * 0.5f));
    temp_fv1 = sp4C * sp48;
    temp_fa0 = sp4C * temp_fv0;
    temp_fa1 = sp58 * temp_fv0;
    temp_ft4 = sp58 * sp48;

    quat->x = (temp_fs0 * temp_fa1) + (temp_fs1 * temp_fv1);
    quat->y = (temp_fs0 * temp_ft4) - (temp_fs1 * temp_fa0);
    quat->z = (temp_fs0 * temp_fa0) + (temp_fs1 * temp_ft4);
    quat->w = (-temp_fs0 * temp_fv1) + (temp_fs1 * temp_fa1);
}

void func_uvquat_rom_00400524(Mtx4F *arg0, Quat *quat) {
    f32 sp44;
    f32 sp40;
    f32 temp_ft5;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    f32 sp28;
    f32 sp24;
    f32 sp20;
    static s32 firstTime = FALSE;

    if (!firstTime) {
        firstTime = TRUE;
        D_uvquat_rom_00400B54->func_00402B30(&D_uvquat_rom_00400B58, -1.5707964f, 0.0f, 3.1415927f);
    }

    sp44 = SQ(quat->x);
    sp40 = SQ(quat->y);
    temp_ft5 = SQ(quat->z);
    sp38 = SQ(quat->w);
    sp30 = quat->y * quat->x;
    sp24 = quat->z * quat->x;
    sp34 = quat->w * quat->x;
    sp2C = quat->z * quat->y;
    sp28 = quat->w * quat->y;
    sp20 = quat->w * quat->z;

    arg0->m[0][0] = ((sp44 + sp40) - temp_ft5) - sp38;
    arg0->m[0][1] = 2.0f * (sp2C + sp34);
    arg0->m[0][2] = 2.0f * (sp28 - sp24);
    arg0->m[0][3] = 0.0f;
    arg0->m[1][0] = 2.0f * (sp2C - sp34);
    arg0->m[1][1] = ((sp44 - sp40) + temp_ft5) - sp38;
    arg0->m[1][2] = 2.0f * (sp20 + sp30);
    arg0->m[1][3] = 0.0f;
    arg0->m[2][0] = 2.0f * (sp24 + sp28);
    arg0->m[2][1] = 2.0f * (sp20 - sp30);
    arg0->m[2][2] = ((sp44 - sp40) - temp_ft5) + sp38;
    arg0->m[2][3] = 0.0f;
    arg0->m[3][0] = 0.0f;
    arg0->m[3][1] = 0.0f;
    arg0->m[3][2] = 0.0f;
    arg0->m[3][3] = 1.0f;
    D_uvquat_rom_00400B54->func_00400BB8(arg0, arg0, &D_uvquat_rom_00400B58);
}

static s32 unused[] = { 0x00200000, &__entrypoint_func_uvquat_rom_400000, 0 };

void func_uvquat_rom_0040070C(Quat *arg0, Quat *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 temp_ft4;

    temp_ft4 = (1.0f - (SQ(arg1->w) + (SQ(arg1->x) + SQ(arg1->y) + SQ(arg1->z)))) * (0.99f / arg5);
    arg0->x = (arg1->x * temp_ft4) + (-0.5f * ((arg1->y * arg4) + (arg1->z * arg3) + (arg1->w * arg2)));
    arg0->y =
        (arg1->y * temp_ft4) + (0.5f * (((arg1->x * arg4) - (arg1->w * arg3)) + (arg1->z * arg2)));
    arg0->z =
        (arg1->z * temp_ft4) + (0.5f * (((arg1->w * arg4) + (arg1->x * arg3)) - (arg1->y * arg2)));
    arg0->w = (arg1->w * temp_ft4) + (0.5f * ((-arg1->z * arg4) + (arg1->y * arg3) + (arg1->x * arg2)));
}

void uvQuatMultiply(Quat *dest, Quat *q1, Quat *q2) {
    Quat quat;

    quat.x = ((q1->w * q2->x) + (q1->x * q2->w) + (q1->y * q2->z)) - (q2->y * q1->z);
    quat.y = ((q1->w * q2->y) + (q1->y * q2->w) + (q1->z * q2->x)) - (q2->z * q1->x);
    quat.z = ((q1->w * q2->z) + (q1->z * q2->w) + (q1->x * q2->y)) - (q2->x * q1->y);
    quat.w = (((q1->w * q2->w) - (q1->x * q2->x)) - (q1->y * q2->y)) - (q2->x * q1->z);
    uvQuatCopy(dest, &quat);
}

void uvQuatCopy(Quat *q1, Quat *q2) {
    q1->x = q2->x;
    q1->y = q2->y;
    q1->z = q2->z;
    q1->w = q2->w;
}

void func_uvquat_rom_004009BC(Quat *arg0, Quat *arg1, Quat *arg2, f32 arg3) {
    f32 temp_fv0;
    f32 var_fv1;

    var_fv1 = ((arg2->w * arg1->w) + ((arg1->x * arg2->x) + (arg1->y * arg2->y) + (arg1->z * arg2->z)));
    temp_fv0 = (1.0f - arg3);
    if (var_fv1 < 0) {
        arg3 = -arg3;
    }

    arg0->x = (f32) ((arg2->x * arg3) + (arg1->x * temp_fv0));
    arg0->y = (f32) ((arg2->y * arg3) + (arg1->y * temp_fv0));
    arg0->z = (f32) ((arg2->z * arg3) + (arg1->z * temp_fv0));
    arg0->w = (f32) ((arg2->w * arg3) + (arg1->w * temp_fv0));

    temp_fv0 = D_uvquat_rom_00400B50->uvSqrtf(SQ(arg0->w) + (SQ(arg0->x) + SQ(arg0->y) + SQ(arg0->z)));
    if (temp_fv0 == 0.0f) {
        return;
    }

    arg0->x /= temp_fv0;
    arg0->y /= temp_fv0;
    arg0->z /= temp_fv0;
    arg0->w /= temp_fv0;
}
