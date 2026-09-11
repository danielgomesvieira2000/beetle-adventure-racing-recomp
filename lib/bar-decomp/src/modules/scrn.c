// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
typedef struct UvGeom_Exports {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 unk4;
    /* 0x08 */ s32 unk8;
    /* 0x0C */ s32 unkC;
    /* 0x10 */ void (*unk10)(s16, s16, s16, s16);
} UvGeom_Exports;
extern UvGeom_Exports *gUvGeomExports;
typedef struct ScrnGeomList {
    /* 0x00 */ s16 unk0;
    /* 0x02 */ s16 count;
    /* 0x04 */ u8 pad4[0x22];
    /* 0x26 */ s16 unk26;
    /* 0x28 */ s16 unk28;
    /* 0x2A */ s16 unk2A;
    /* 0x2C */ s16 unk2C;
} ScrnGeomList;
extern s32 D_scrn_004006E8;
extern s16 D_scrn_004006DA;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/scrn/__entrypoint_func_scrn_400000.s")

void func_scrn_00400094(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/scrn/func_scrn_0040009C.s")

void func_scrn_004001DC(void) {
    D_scrn_004006DA = 0;
}

/* Submit screen geometry elements to the UV geometry export manager. */
void func_scrn_004001E8(ScrnGeomList *arg0) {
    s32 var_s1;
    ScrnGeomList *var_s0;

    if (D_scrn_004006E8 != 0) {
        return;
    }
    var_s1 = 0;
    var_s0 = arg0;
    if (arg0->count > 0) {
        do {
            gUvGeomExports->unk10(var_s0->unk26, var_s0->unk2A, var_s0->unk28, var_s0->unk2C);
            var_s1 += 1;
            var_s0 = (ScrnGeomList *)((u8 *)var_s0 + 8);
        } while (var_s1 < arg0->count);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/scrn/func_scrn_0040027C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/scrn/func_scrn_00400398.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/scrn/func_scrn_00400474.s")

