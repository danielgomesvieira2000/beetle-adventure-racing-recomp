// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
extern s32 D_txtview_00400C94;
extern s32 D_txtview_00400C98;
extern UnkStruct_80025C74 *gGameGuiExports;
void func_txtview_00400238(s32);
void func_txtview_00400420(void);
void func_txtview_004000EC();
void func_txtview_00400118();
void func_txtview_00400148();
void func_txtview_00400238();
void func_txtview_00400298();
void func_txtview_00400420();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/txtview/__entrypoint_func_txtview_400000.s")

void func_txtview_004000EC(void) {
    uvUnloadFile(0x55565458, D_txtview_00400C94);
}

void func_txtview_00400118(void) {
    if (D_txtview_00400C98 == 0) {
        func_txtview_00400238(0);
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/txtview/func_txtview_00400148.s")

void func_txtview_00400238(s32 arg0) {
    gGameGuiExports->unk1C((s32) func_txtview_00400420);
    gGameGuiExports->unk10(1);
    D_txtview_00400C98 += 1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/txtview/func_txtview_00400298.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/txtview/func_txtview_00400420.s")

