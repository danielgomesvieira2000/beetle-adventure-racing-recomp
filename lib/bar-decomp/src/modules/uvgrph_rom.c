// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

typedef struct UvGrphInnerStruct_s {
   /* unk0 */ f32 unk0;
   /* unk4 */ f32 unk4;
} UvGrphInnerStruct;

typedef struct UvGrphStruct_s {
    /* 0x0 */ s32 count;
    /* 0x4 */ UvGrphInnerStruct arr[];
} UvGrphStruct;

s32 func_uvgrph_rom_00400194(UvGrphStruct *grph, s32 arg1);
s32 func_uvgrph_rom_004002AC(UvGrphStruct* grph, s32 arg1);
f32 func_uvgrph_rom_00400370(UvGrphStruct *arg0, f32 arg1, f32 arg2, f32 arg3);

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgrph_rom/__entrypoint_func_uvgrph_rom_400000.s")

void func_uvgrph_rom_00400078(void) {

}

f32 func_uvgrph_rom_00400080(UvGrphStruct* arg0, f32 arg1) {
    f32 temp_fv1;
    s32 i;

    if (arg1 <= arg0->arr->unk0) {
        return arg0->arr->unk4;
    }


    if (arg0->arr[arg0->count - 1].unk0 <= arg1) {
        return arg0->arr[arg0->count - 1].unk4;
    }

    for (i = 1; i < arg0->count - 1; i++) {
        if ((arg1 < arg0->arr[i].unk0)) {
            break;
        }
    }

    temp_fv1 = (arg1 - arg0->arr[i - 1].unk0) / (arg0->arr[i].unk0 - arg0->arr[i - 1].unk0);
    return (arg0->arr[i].unk4 * temp_fv1) + (arg0->arr[i - 1].unk4 * (1.0f - temp_fv1));
}

void func_uvgrph_rom_00400148(s32 arg0, s32* arg1) {
    s32 i;

    if (*arg1 < 0) {
        *arg1 = 0;
    } else if (*arg1 >= 0x15) {
        *arg1 = 0x14;
    }
    // @useless loop
    i = 0;
    while (i < *arg1) {
        i++;
    }
}


#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgrph_rom/func_uvgrph_rom_00400194.s")

s32 func_uvgrph_rom_004002AC(UvGrphStruct *grph, s32 arg1) {
    s32 i;

    if (grph->count < 3) {
        return arg1;
    }

    if ((arg1 < 0) || ((grph->count - 1) < arg1)) {
        return arg1;
    }
    for (i = arg1; i < grph->count - 1; i++) {
        grph->arr[i].unk0 = grph->arr[i + 1].unk0;
        grph->arr[i].unk4 = grph->arr[i + 1].unk4;
    }

    arg1--;
    grph->count = grph->count - 1;
    if (arg1 < 0) {
        arg1 = 0;
    }

    return arg1;
}

void func_uvgrph_rom_0040033C(UvGrphStruct* arg0, f32 arg1) {
    func_uvgrph_rom_00400370(arg0, arg1, 0.0f, 0.0f);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgrph_rom/func_uvgrph_rom_00400370.s")
