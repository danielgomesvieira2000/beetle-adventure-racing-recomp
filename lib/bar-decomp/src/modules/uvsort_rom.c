// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "stdarg.h"
#include "module.h"

typedef struct UnkStruct_00400C14_1_s {
    void *(*unk0)(s32, s32);
    s32 unk4;
    f32 unk8;
    u8 unkC;
} UnkStruct_00400C14_1;

typedef struct UnkStruct_00400C14_s {
    f32 unk0;
    f32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    UnkStruct_00400C14_1 *unkC;
    u16 unk10;
} UnkStruct_00400C14;

typedef struct UnkStruct_00400C04_s {
    s32 pad0;
    void *(*unk4)(s32, s32, s32, s32);
} UnkStruct_00400C04;

typedef struct UnkStruct_00400C08_s {
    char pad[0x10];
    void *(*unk10)(s32);
} UnkStruct_00400C08;

typedef struct UnkStruct_00400C00_s {
    s32 pad0;
    char pad4[0xC];
    void *(*unk10)(s32, s32, s32, s32);
    char pad14[0x4];
    void *(*unk18)(s32, s32, u8);
} UnkStruct_00400C00;

void func_uvsort_rom_00400238(s32 arg0);
void func_uvsort_rom_00400240(void);
s32 func_uvsort_rom_00400308(u8 arg0);
void func_uvsort_rom_00400594(s32 arg0);
void func_uvsort_rom_0040059C(s32 arg0);
void func_uvsort_rom_004006F8(s32 arg0);
void func_uvsort_rom_00400750(s32 arg0);
void func_uvsort_rom_00400834(s32 arg0, void *(*arg1)(s32, s32), s32 arg2, f32 arg3);
void func_uvsort_rom_00400898(UnkStruct_00400C14 *arg0);
void func_uvsort_rom_00400B24(s32 arg0, ...);
void uvUnloadModule(s32 tag); /* extern */

void __entrypoint_func_uvsort_rom_400000(UvSort_Exports *exports);

UnkStruct_00400C00 *D_uvsort_rom_00400C00;
UnkStruct_00400C04 *D_uvsort_rom_00400C04;
UnkStruct_00400C08 *D_uvsort_rom_00400C08;
s32 D_uvsort_rom_00400C0C;
s32 D_uvsort_rom_00400C10;
UnkStruct_00400C14 *D_uvsort_rom_00400C14;
s32 D_uvsort_rom_00400C18;
s32 D_uvsort_rom_00400C1C;
extern s32 D_uvsort_rom_00400BF0[];


void __entrypoint_func_uvsort_rom_400000(UvSort_Exports *exports) {
    s32 var_a1;
    s32 var_s0;
    s32 *temp_v0;

    uvUpdateFileAllocPtr(exports);
    exports->func_uvsort_rom_00400B24 = func_uvsort_rom_00400B24;
    exports->func_uvsort_rom_00400240 = func_uvsort_rom_00400240;
    exports->func_uvsort_rom_00400308 = func_uvsort_rom_00400308;
    exports->func_uvsort_rom_00400594 = func_uvsort_rom_00400594;
    exports->func_uvsort_rom_0040059C = func_uvsort_rom_0040059C;
    // clang-format off
    exports->func_uvsort_rom_00400834 = func_uvsort_rom_00400834; D_uvsort_rom_00400C04 = uvLoadModule('CHAN');
    // clang-format on
    D_uvsort_rom_00400C00 = uvLoadModule('CBCK');
    D_uvsort_rom_00400C08 = uvLoadModule('GMGR');
    temp_v0 = uvGetSystemProp(0x10);
    if (temp_v0 == NULL) {
        var_a1 = D_uvsort_rom_00400C10 = 4;
        D_uvsort_rom_00400C0C = 0x32;
    } else {
        if (temp_v0[0] != 0) {
            D_uvsort_rom_00400C10 = temp_v0[0];
        } else {
            D_uvsort_rom_00400C10 = 4;
        }
        var_a1 = D_uvsort_rom_00400C10;
        if (temp_v0[1] != 0) {
            D_uvsort_rom_00400C0C = temp_v0[1];
        } else {
            D_uvsort_rom_00400C0C = 0x32;
        }
    }

    if (D_uvsort_rom_00400C0C) { // FAKE
    }

    D_uvsort_rom_00400C14 = _uvMemAllocAlign8(var_a1 * 0x14);
    for (var_s0 = 0; var_s0 < D_uvsort_rom_00400C10; var_s0++) {
        D_uvsort_rom_00400C14[var_s0].unkC = _uvMemAllocAlign8(D_uvsort_rom_00400C0C * 0x10);
        D_uvsort_rom_00400C14[var_s0].unk9 = 0;
        D_uvsort_rom_00400C14[var_s0].unkB = 0xFF;
    }

    D_uvsort_rom_00400C04->unk4(0, 0xD, (s32) &D_uvsort_rom_00400C1C, 0);
    D_uvsort_rom_00400C00->unk10(D_uvsort_rom_00400C08->unk10(1), (s32) func_uvsort_rom_00400238, 0, 0);
}


void func_uvsort_rom_00400238(s32 arg0) {
}

void func_uvsort_rom_00400240(void) {
    s32 var_s0;
    s32 var_s1;

    for (var_s0 = 0; var_s0 < D_uvsort_rom_00400C10; var_s0++) {
        _uvMemFree(D_uvsort_rom_00400C14[var_s0].unkC);
    }
    _uvMemFree(D_uvsort_rom_00400C14);
    uvUnloadModule('CHAN');
    D_uvsort_rom_00400C04 = 0;
    uvUnloadModule('CBCK');
    D_uvsort_rom_00400C00 = 0;
    uvUnloadModule('GMGR');
    D_uvsort_rom_00400C08 = 0;
}

s32 func_uvsort_rom_00400308(u8 arg0) {
    s32 i;
    u8 ret;
    u8 temp_v1;
    s32 j;
    s32 sp50;

    ret = 0xFF;
    for (i = 0; i < D_uvsort_rom_00400C10; i++) {
        if (D_uvsort_rom_00400C14[i].unk9 == 0) {
            D_uvsort_rom_00400C14[i].unk9 = 1;
            D_uvsort_rom_00400C14[i].unk8 = arg0;
            D_uvsort_rom_00400C14[i].unk0 = 0.0f;
            D_uvsort_rom_00400C14[i].unk4 = 0.0f;
            D_uvsort_rom_00400C14[i].unkA = 0;
            ret = i;
            break;
        }

        if (arg0 == D_uvsort_rom_00400C14[i].unk8) {
            return -1;
        }
    }

    if (ret == 0xFF) {
        return -1;
    }
    for (i = 0; i < D_uvsort_rom_00400C10; i++) {
        temp_v1 = D_uvsort_rom_00400C14[i].unkB;
        if ((temp_v1 == 0xFF) || (arg0 < D_uvsort_rom_00400C14[temp_v1].unk8)) {
            for (j = D_uvsort_rom_00400C10 - 2; j >= i; j--) {
                D_uvsort_rom_00400C14[j + 1].unkB = D_uvsort_rom_00400C14[j].unkB;
            }
            D_uvsort_rom_00400C14[i].unkB = ret;
            break;
        }
    }

    for (i = 0; i < D_uvsort_rom_00400C1C; i++) {
        if ((D_uvsort_rom_00400C18 << i) & 1) {
            D_uvsort_rom_00400C04->unk4(i, 6, &sp50, 0);
            if (D_uvsort_rom_00400C00->unk18(sp50, func_uvsort_rom_00400750, arg0) != 0) {
                continue;
            }
            D_uvsort_rom_00400C00->unk10(sp50, func_uvsort_rom_00400750, 0, arg0);
        }
    }

    return ret;
}

void func_uvsort_rom_00400594(s32 arg0) {
}

void func_uvsort_rom_0040059C(s32 arg0) {
    s32 sp44;
    s32 var_s0;
    UnkStruct_00400C14 *temp_v0;

    D_uvsort_rom_00400C04->unk4(arg0, 6, &sp44, 0);
    D_uvsort_rom_00400C00->unk10(sp44, func_uvsort_rom_004006F8, 0, 1);
    for (var_s0 = 0; var_s0 < D_uvsort_rom_00400C10; var_s0++) {
        if (D_uvsort_rom_00400C14[var_s0].unk9 != 0) {
            if (D_uvsort_rom_00400C00->unk18(sp44, func_uvsort_rom_00400750, D_uvsort_rom_00400C14[var_s0].unk8) != NULL) {
                continue;
            }
            D_uvsort_rom_00400C00->unk10(sp44, func_uvsort_rom_00400750, 0, D_uvsort_rom_00400C14[var_s0].unk8);
        }
    }
    D_uvsort_rom_00400C18 |= 1 << arg0;
}

void func_uvsort_rom_004006F8(s32 arg0) {
    s32 i;

    D_uvsort_rom_00400BF0[0] = 0;
    for (i = 0; i < D_uvsort_rom_00400C10; i++) {
        D_uvsort_rom_00400C14[i].unk10 = 0;
    }
}

void func_uvsort_rom_00400750(s32 arg0) {
    UnkStruct_00400C14 *temp_s2;
    UnkStruct_00400C14_1 *temp_s3;
    UnkStruct_00400C14_1 *var_s1;
    UnkStruct_00400C14_1 *temp_v0;
    s32 i;

    if (D_uvsort_rom_00400BF0[0] != -1) {
        temp_s2 = &D_uvsort_rom_00400C14[D_uvsort_rom_00400C14[D_uvsort_rom_00400BF0[0]].unkB];
        func_uvsort_rom_00400898(temp_s2);
        temp_s3 = temp_s2->unkC;
        for (i = 0, var_s1 = temp_s3; i < temp_s2->unk10; i++, var_s1++) {
            temp_v0 = &temp_s3[var_s1->unkC];
            temp_v0->unk0(arg0, temp_v0->unk4);
        }
        D_uvsort_rom_00400BF0[0]++;
    }
}

void func_uvsort_rom_00400834(s32 arg0, void *(*arg1)(s32, s32), s32 arg2, f32 arg3) {
    UnkStruct_00400C14 *temp_v0;
    UnkStruct_00400C14_1 *temp_v1;

    temp_v0 = &D_uvsort_rom_00400C14[arg0];
    temp_v1 = &temp_v0->unkC[temp_v0->unk10];
    if (D_uvsort_rom_00400C0C != temp_v0->unk10) {
        temp_v1->unk0 = arg1;
        temp_v1->unk4 = arg2;
        temp_v1->unk8 = arg3;
        temp_v0->unkA &= 0xFFFD;
        temp_v0->unk10 += 1;
    }
}

void func_uvsort_rom_00400898(UnkStruct_00400C14 *arg0) {
    s32 i;
    s32 j;
    f32 var_fv0;
    s32 temp_v1;
    UnkStruct_00400C14_1 *temp_v0;

    temp_v0 = arg0->unkC;
    temp_v1 = arg0->unk10;

    if (arg0->unkA & 2) {
        return;
    }

    if (arg0->unkA & 1) {
        for (i = 0; i < temp_v1; i++) {
            temp_v0[i].unkC = i;
        }
        arg0->unkA |= 2;
        return;
    }

    for (i = 0; i < temp_v1; i++) {
        var_fv0 = -1000000.0f;
        for (j = 0; j < temp_v1; j++) {
            if (var_fv0 < temp_v0[j].unk8) {
                temp_v0[i].unkC = j;
                var_fv0 = temp_v0[j].unk8;
            }
        }
        temp_v0[temp_v0[i].unkC].unk8 = -temp_v0[temp_v0[i].unkC].unk8;
    }

    for (i = 0; i < temp_v1; i++) {
        var_fv0 = temp_v0[i].unk8;
        if (var_fv0 < 0.0f) {
            temp_v0[i].unk8 = -var_fv0;
        }
    }

    arg0->unkA |= 2;
}

void func_uvsort_rom_00400B24(s32 arg0, ...) {
    va_list args;
    s32 var_a0;
    s32 var_v0;

    va_start(args, arg0);

    while (var_a0 = va_arg(args, s32)) {
        if (var_a0 != 1) {
            continue;
        }

        if (va_arg(args, s32)) {
            D_uvsort_rom_00400C14[arg0].unkA |= 1;
        } else {
            D_uvsort_rom_00400C14[arg0].unkA &= ~1;
        }
    }
}
