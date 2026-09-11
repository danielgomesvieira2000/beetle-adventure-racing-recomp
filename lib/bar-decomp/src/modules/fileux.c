// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "global_exports.h"


typedef struct UnkStruct_fileux_004005A0_s {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} UnkStruct_fileux_004005A0;

void __entrypoint_func_fileux_400000(FileUx_Exports* exports);
void func_fileux_00400060(void);
void func_fileux_00400068(void);
s32 func_fileux_00400070(u8* s);
void func_fileux_004000A0(u8** d, u8* s, s32 len);
void func_fileux_00400138(s32 arg0, s32 arg1);
s32 func_fileux_00400144(void);
s32 func_fileux_0040014C(s32 prop, ...);
s32 func_fileux_0040014C(s32, ...); /* extern */
s32 func_fileux_004005A0(s32 arg0, u8* arg1, s32 arg2);
s32 func_fileux_00400768(s32 arg0, u8* arg1, s32 arg2);
s32 func_fileux_00400970(s32 arg0, s32 arg1, s32 arg2);
s32 func_fileux_004009EC(s32 arg0);

extern UnkStruct_fileux_004005A0* D_fileux_00400EF8[];

void __entrypoint_func_fileux_400000(FileUx_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_fileux_00400060 = func_fileux_00400060;
    exports->func_fileux_004005A0 = func_fileux_004005A0;
    exports->func_fileux_00400768 = func_fileux_00400768;
    exports->func_fileux_00400970 = func_fileux_00400970;
    exports->func_fileux_004009EC = func_fileux_004009EC;
}

void func_fileux_00400060(void) {

}

void func_fileux_00400068(void) {

}

s32 func_fileux_00400070(u8* s) {
    s32 len;
    u8* str;

    len = 0;
    str = s;
    while (*str != '\0') {
        len++;
        str++;
    }

    return len;
}

void func_fileux_004000A0(u8** d, u8* s, s32 len) {
    s32 i;
    u8* src = s;

    for (i = 0; i < len; i++) {
        *(*d + i) = src[i];
    }

    *d += len;
}

void func_fileux_00400138(s32 arg0, s32 arg1) {

}

s32 func_fileux_00400144(void) {
    return 1;
}

#ifdef _NON_MATCHING
extern u8 D_fileux_00400CF8[0x100];

s32 func_fileux_0040014C(s32 prop, ...) {
    u8 sp60[0x100];
    u8* sp5C;
    u8* sp58;
    s32 sp54;
    s32 sp50;
    s32 i; // a3
    s32 temp_v0;
    s32 sp44;
    s32 var_s0;
    s32 sp3C;
    s32 sp38;
    s32 sp34;
    s32 sp30;
    va_list args;
    u8 *v0;

    gUvGfxMgrExports->func_uvgfxmgr_rom_00402090();
    sp5C = sp60;
    for (i = 0; i < 0x100; i++) {
        sp60[i] = i;
    }

    func_fileux_00400138(sp5C, 0x100);
    if (func_fileux_00400144() == 0) {
        return -1;
    }

    for (i = 0; i < 0x100; i++) {
        if (D_fileux_00400CF8[i] == (0xFF - i)) {
            break;
        }
    }

    _uvMediaCopy(sp60, &prop, sizeof(int));
    sp5C += 4;
    va_start(args, prop);
    switch (prop) {
    case 0x4F:
        sp58 = va_arg(args, u8*);
        sp44 = va_arg(args, s32);
        func_fileux_004000A0(&sp5C, sp58, func_fileux_00400070(sp58));
        *sp5C = '\0';
        sp5C += 1;
        func_fileux_004000A0(&sp5C, &sp44, 4);
        if (sp44 & 0x100) {
            i = va_arg(args, s32);
            func_fileux_004000A0(&sp5C, (u8* ) &i, 4);
        } else {
            i = 0;
        }
        func_fileux_00400138(sp60, 0x100);
        func_fileux_00400144();
        return ((s32*)D_fileux_00400CF8)[0];
    case 0x52:
        sp50 = va_arg(args, s32);
        sp54 = va_arg(args, s32);
        sp3C = va_arg(args, s32);
        sp30 = va_arg(args, s32);
        func_fileux_004000A0(&sp5C, &sp50, 4);
        func_fileux_004000A0(&sp5C, &sp3C, 4);
        func_fileux_00400138(sp60, 0x100);
        func_fileux_00400144();
        v0 = sp60;
        if (sp30 != 0) {
            // FAKE
            if (&sp30) {}
            if (&sp30) {}
        }
        v0[0] = 6;
        func_fileux_00400138(v0, 0x100);
        for (i = 0; i < sp3C; ) {
            temp_v0 = sp3C - i;
            if (temp_v0 < 0x100) {
                var_s0 = temp_v0;
            } else {
                var_s0 = 0x100;
            }
            func_fileux_00400144();
            _uvMediaCopy(sp54 + i, D_fileux_00400CF8, var_s0);
            i += var_s0;
        }
        return sp3C;
    case 0x57:
        sp50 = va_arg(args, s32);
        sp54 = va_arg(args, s32);
        sp3C = va_arg(args, s32);
        func_fileux_004000A0(&sp5C, (u8* ) &sp50, 4);
        func_fileux_004000A0(&sp5C, (u8* ) &sp3C, 4);
        func_fileux_00400138(sp60, 0x100);
        func_fileux_00400144();
        i = sp3C;
        if (sp3C & 7) {
            i = (sp3C - (sp3C % 8)) + 8;
        }
        func_fileux_00400138(sp54, i);
        func_fileux_00400144();
        return ((s32*)D_fileux_00400CF8)[0];
    case 0x43:
        sp50 = va_arg(args, s32);
        func_fileux_004000A0(&sp5C, (u8* ) &sp50, 4);
        func_fileux_00400138(sp60, 0x100);
        func_fileux_00400144();
        return ((s32*)D_fileux_00400CF8)[0];
    case 0x4C:
        sp50 = va_arg(args, s32);
        sp38 = va_arg(args, s32);
        sp34 = va_arg(args, s32);
        func_fileux_004000A0(&sp5C, &sp50, 4);
        func_fileux_004000A0(&sp5C, &sp38, 4);
        func_fileux_004000A0(&sp5C, &sp34, 4);
        func_fileux_00400138(sp60, 0x100);
        func_fileux_00400144();
        return ((s32*)D_fileux_00400CF8)[0];
    default:
        return -1;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/fileux/func_fileux_0040014C.s")
#endif

s32 func_fileux_004005A0(s32 arg0, u8* arg1, s32 arg2) {
    s32 var_a3;
    s32 var_s1;
    UnkStruct_fileux_004005A0* var_s3;

    var_s1 = 0;
    if ((arg0 < 0x14) && (arg0 >= 0)) {
        var_s3 = D_fileux_00400EF8[arg0];
    } else {
        var_s3 = NULL;
    }
    if ((var_s3 != NULL) && (((var_s3->unk0 == 0)) || (var_s3->unk8 == 0))) {
        var_s3->unk8 = 0;
        if (var_s3->unk0 > 0) {
            if ((0x100 - var_s3->unk0) < arg2) {
                var_s1 = (0x100 - var_s3->unk0);
            } else {
                var_s1 = arg2;
            }
            _uvMediaCopy((u8*)var_s3 + var_s3->unk0 + 0xC, arg1, var_s1);
            var_s3->unk0 = var_s3->unk0 + var_s1;
            if (var_s3->unk0 == 0x100) {
                func_fileux_0040014C(0x57, arg0, &var_s3->unkC, 0x100);
                var_s3->unk0 = 0;
            }
        }
        if (var_s1 == arg2) {
            return 0;
        }
        if (var_s3->unk0 != 0) {
            return 1;
        }
        while (var_s1 < arg2) {
            if ((arg2 - var_s1) >= 0x101) {
                var_s1 += func_fileux_0040014C(0x57, arg0, arg1 + var_s1, 0x100);
            } else {
                _uvMediaCopy(&var_s3->unkC, arg1 + var_s1, (u32) (arg2 - var_s1));
                var_s3->unk0 = (arg2 - var_s1);
                var_s1 = arg2;
            }
        }
        goto ret;
    }

    while (var_s1 < arg2) {
        if ((arg2 - var_s1) < 0x100) {
            var_a3 = (arg2 - var_s1);
        } else {
            var_a3 = 0x100;
        }
        var_s1 += func_fileux_0040014C(0x57, arg0, arg1 + var_s1, var_a3);
    }

    if (var_s1 != arg2) {
        return 1;
    }
    ret:
    return 0;
}

s32 func_fileux_00400768(s32 arg0, u8* arg1, s32 arg2) {
    UnkStruct_fileux_004005A0* var_s2;
    s32 var_a3;
    s32 var_s1;
    s32 temp_v0_3;

    var_s1 = 0;
    if ((arg0 < 0x14) && (arg0 >= 0)) {
        var_s2 = D_fileux_00400EF8[arg0];
    } else {
        var_s2 = NULL;
    }
    if ((var_s2 != NULL) && (((var_s2->unk0 == 0)) || (var_s2->unk8 != 0))) {
        var_s2->unk8 = 1;
        if (var_s2->unk0 != 0) {
            if (arg2 < (var_s2->unk0 - var_s2->unk4)) {
                var_s1 = arg2;
            } else {
                var_s1 = (var_s2->unk0 - var_s2->unk4);
            }
            _uvMediaCopy(arg1, (u8*)var_s2 + var_s2->unk4 + 0xC, (u32) var_s1);
            var_s2->unk4 += var_s1;
        }
        if (var_s1 == arg2) {
            return 0;
        }
        if ((var_s2->unk0 != 0) && (var_s2->unk0 != var_s2->unk4)) {
            return 1;
        }
        while (var_s1 < arg2) {
            if ((arg2 - var_s1) >= 0x101) {
                var_s1 += func_fileux_0040014C(0x52, arg0, arg1 + var_s1, 0x100, 1);
                continue;
            }
            temp_v0_3 = func_fileux_0040014C(0x52, arg0, &var_s2->unkC, 0x100, /* extra? */ 0);
            var_s2->unk0 = temp_v0_3;
            var_s2->unk4 = (arg2 - var_s1);
            if (temp_v0_3 < (arg2 - var_s1)) {
                return 1;
            }
            _uvMediaCopy(arg1 + var_s1, &var_s2->unkC, (u32) var_s2->unk4);
            var_s1 += var_s2->unk4;
        }

        goto ret;
    }
    while (var_s1 < arg2) {
        if ((arg2 - var_s1) < 0x100) {
            var_a3 = (arg2 - var_s1);
        } else {
            var_a3 = 0x100;
        }
        var_s1 += func_fileux_0040014C(0x52, arg0, arg1 + var_s1, var_a3, /* extra? */ 1);
    }
    if (var_s1 != arg2) {
        return 1;
    }
ret:
    return 0;
}

s32 func_fileux_00400970(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_v0;

    temp_v0 = func_fileux_0040014C(0x4F, arg0, arg1, arg2);
    if (temp_v0 < 0) {
        return temp_v0;
    } else if (temp_v0 < 0x14) {
        D_fileux_00400EF8[temp_v0] = _uvMemAllocAlign8(0x10C);
        if (D_fileux_00400EF8[temp_v0] != NULL) {
            D_fileux_00400EF8[temp_v0]->unk0 = 0;
        }
    }
    return temp_v0;
}

s32 func_fileux_004009EC(s32 arg0) {
    UnkStruct_fileux_004005A0* temp_s0;

    if (arg0 < 0) {
        return -1;
    }
    if (arg0 < 0x14) {
        temp_s0 = D_fileux_00400EF8[arg0];
        if (temp_s0 != NULL) {
            if ((temp_s0->unk0 != 0) && (temp_s0->unk8 == 0)) {
                func_fileux_0040014C(0x57, arg0, &temp_s0->unkC, temp_s0->unk0);
            }
            _uvMemFree(temp_s0);
            D_fileux_00400EF8[arg0] = NULL;
        }
    }
    if (func_fileux_0040014C(0x43, arg0) < 0) {
        return -1;
    }
    return 0;
}
