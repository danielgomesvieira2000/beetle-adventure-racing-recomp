// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

void uvUnloadModule(s32 tag);                                 /* extern */
extern void* D_uvgeom_rom_00401754;
extern u16 D_uvgeom_rom_00401760;
extern Gfx* D_uvgeom_rom_0040175C; 

typedef struct UvGeom_Rom_0040019C_s {
    /* 0x00 */ s32 unk0;                            /* inferred */
    /* 0x04 */ s32 unk4;                            /* inferred */
    /* 0x08 */ Gfx* unk8;                           /* inferred */
    /* 0x0C */ s16 unkC;                            /* inferred */
    /* 0x0E */ s16 unkE;                            /* inferred */
    /* 0x10 */ s16 unk10;                           /* inferred */
    /* 0x12 */ s16 unk12;                           /* inferred */
} UvGeom_Rom_0040019C;                              /* size = 0x14 */


#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgeom_rom/__entrypoint_func_uvgeom_rom_400000.s")

void func_uvgeom_rom_00400124(void) {
    uvUnloadModule('GMGR');
    if (D_uvgeom_rom_00401760 & 1) {
        _uvMemFree(D_uvgeom_rom_00401754);
    }
}

void func_uvgeom_rom_00400168(s32 arg0, Gfx** gdlh) {
    if (arg0 == 0) {
        gSPEndDisplayList(gdlh[0]++);
        D_uvgeom_rom_0040175C = *gdlh;
    }
}

s32 func_uvgeom_rom_0040019C(s32 arg0, s32 arg1, UvGeom_Rom_0040019C* arg2, Gfx** arg3) {
    s32 var_v1;

    if (*arg3 == NULL) {
        *arg3 = D_uvgeom_rom_0040175C;
        var_v1 = 0;
    } else {
        var_v1 = 1;
    }
    arg2->unk4 = 0x160FFF;
    arg2->unk10 = 0;
    arg2->unk12 = 0;
    arg2->unkC = -1;
    arg2->unkE = arg0;
    arg2->unk0 = arg1;
    arg2->unk8 = *arg3;
    return var_v1;
}


void func_uvgeom_rom_004001F0(s32 arg0, s32 arg1, UvGeom_Rom_0040019C *arg2, Gfx **arg3) {
    s16 var_a2;
    s16 i; // a1
    s16 var_t0;    
    s16 sp20;
    Vtx *var_t2;
    
    if (arg0 < 3) {
        return;
    }
        
    var_t0 = FALSE;
    sp20 = func_uvgeom_rom_0040019C(arg0, arg1, arg2, arg3);
    var_t2 = arg2->unk0;
    while (arg0 >= 3) {
        if (arg0 < 16) {
            var_a2 = arg0;
        } else {
            var_a2 = 16;
        }

        arg2->unk10 += var_a2;
        
        gSPVertex(arg3[0]++, OS_PHYSICAL_TO_K0(var_t2), var_a2, 0);        
        var_a2 -= 2;
        arg0 -= var_a2;
        var_t2 += var_a2;
        
        for (i = 0; i < var_a2; i++) {
            arg2->unk12++;
            if (var_t0) {
                gSP1Triangle(arg3[0]++, i, i + 2, i + 1, 0);
            } else {
                gSP1Triangle(arg3[0]++, i, i + 1, i + 2, 0);
            }
            var_t0 ^= TRUE;
        }
    }
    func_uvgeom_rom_00400168((s32) sp20, arg3);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgeom_rom/func_uvgeom_rom_00400398.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgeom_rom/func_uvgeom_rom_00400760.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgeom_rom/func_uvgeom_rom_00400F40.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgeom_rom/func_uvgeom_rom_00401270.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvgeom_rom/func_uvgeom_rom_00401518.s")

