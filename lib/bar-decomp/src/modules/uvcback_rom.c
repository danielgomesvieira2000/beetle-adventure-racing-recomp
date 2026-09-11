// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"


void __entrypoint_func_uvcback_rom_400000(UvCback_Exports* exports);
void func_uvcback_rom_00400078(void);
void* func_uvcback_rom_00400080(s32 arg0);
void func_uvcback_rom_004000D0(void* ptr);
void func_uvcback_rom_004000F0(UvCback_Rom_004000F0* arg0, s32 arg1);
s32 func_uvcback_rom_0040016C(UvCback_Rom_004000F0* arg0, s32 arg1, s32 arg2, s32 arg3);
s32 func_uvcback_rom_00400320(UvCback_Rom_004000F0* arg0, s32 arg1);
s32 func_uvcback_rom_004003C8(UvCback_Rom_004000F0* arg0, s32 arg1, u8 arg2);

void __entrypoint_func_uvcback_rom_400000(UvCback_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvcback_rom_00400320 = func_uvcback_rom_00400320;
    exports->func_uvcback_rom_00400078 = func_uvcback_rom_00400078;
    exports->func_uvcback_rom_004003C8 = func_uvcback_rom_004003C8;
    exports->func_uvcback_rom_00400080 = func_uvcback_rom_00400080;
    exports->func_uvcback_rom_004000D0 = func_uvcback_rom_004000D0;
    exports->func_uvcback_rom_004000F0 = func_uvcback_rom_004000F0;
    exports->func_uvcback_rom_0040016C = func_uvcback_rom_0040016C;
}

void func_uvcback_rom_00400078(void) {

}

void* func_uvcback_rom_00400080(s32 count) {
    UvCback_Rom_004000F0* ptr;
    u32 size;

    size = (count * sizeof(UvCback_Rom_004000F0_Unk4)) + sizeof(UvCback_Rom_004000F0);
    ptr = _uvMemAllocAlign8(size);
    uvMemSet(ptr, 0, size);
    ptr->count = count;
    return ptr;
}

void func_uvcback_rom_004000D0(void* ptr) {
    _uvMemFree(ptr);
}

void func_uvcback_rom_004000F0(UvCback_Rom_004000F0* arg0, s32 arg1) {
    void* (*temp_v0)(s32);
    s32 var_s1;
    s32 count;
    UvCback_Rom_004000F0* var_s0;

    if (arg0 != NULL) {
        count = arg0->count;
        for (var_s1 = 0; var_s1 < count; var_s1++) {
            temp_v0 = (void*)arg0->unk4[var_s1].unk0;
            if (temp_v0 != NULL) {
                temp_v0(arg1);
                count = arg0->count;
            }
        }
    }
}

s32 func_uvcback_rom_0040016C(UvCback_Rom_004000F0* arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 i;
    s32 j;
    UvCback_Rom_004000F0* t2;
    UvCback_Rom_004000F0_Unk4* var_t0;

    if (arg0 == NULL) {
        return -1;
    }

    if (arg0->unk4[arg0->count - 1].unk0 != 0) {
        return -1;
    }
    if ((arg2 == 0) && (arg3 == 0)) {
        for (i = 0; i < arg0->count; i++) {
            if (arg0->unk4[i].unk0 == 0) {
                arg0->unk4[i].unk0 = arg1;
                arg0->unk4[i].unk4 = 0;
                break;
            }
        }
    } else if (arg2 != 0) {
        for (i = 0; i < arg0->count; i++) {
            var_t0 = &arg0->unk4[i];
            if (arg2 == var_t0->unk0) {
                for (j = arg0->count - 2; j >= i; j--) {
                    arg0->unk4[j + 1] = arg0->unk4[j];
                }
                var_t0->unk0 = arg1;
                var_t0->unk4 = 0;
            }
        }
    } else {
        for (i = 0; i < arg0->count; i++) {
            var_t0 = &arg0->unk4[i];
            if ((arg3 < var_t0->unk4) || (var_t0->unk0 == 0)) {
                for (j = arg0->count - 2; j >= i; j--)   {
                    arg0->unk4[j + 1] = arg0->unk4[j];
                }
                var_t0->unk0 = arg1;
                var_t0->unk4 = arg3;
                break;
            }
        }
    }
    return 0;
}

s32 func_uvcback_rom_00400320(UvCback_Rom_004000F0* arg0, s32 arg1) {
    s32 i;
    s32 j;

    if (arg0 == NULL) {
        return -1;
    }
    for (i = 0; i < arg0->count; i++) {
        UvCback_Rom_004000F0_Unk4 *temp = &arg0->unk4[i];
        if (arg1 == temp->unk0) {
            for (j = i + 1; j < arg0->count; j++) {
                arg0->unk4[j - 1] = arg0->unk4[j];
            }
            arg0->unk4[arg0->count - 1].unk0 = 0;
        }
    }

    return 0;
}

s32 func_uvcback_rom_004003C8(UvCback_Rom_004000F0* arg0, s32 arg1, u8 arg2) {
    s32 i;

    if (arg0 == NULL) {
        return -1;
    }

    for (i = 0; i < arg0->count; i++) {
        UvCback_Rom_004000F0_Unk4 *temp = &arg0->unk4[i];
        if ((arg1 == arg0->unk4[i].unk0) && (arg2 == arg0->unk4[i].unk4)) {
            return TRUE;
        }
    }

    return 0;
}
