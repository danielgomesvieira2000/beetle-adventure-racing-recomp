// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern void *gUvModelExports;
extern void *gMiscExports;
extern void *gAiExports;
extern u8 D_race_00404710;
extern u8 D_race_00404718;
extern u8 D_race_00404724;
extern u8 D_race_00404730;
extern u8 D_race_00404740;
extern s32 D_race_00404AA0;
extern s32 D_race_00404AA4;
extern s32 D_race_00404AA8;
extern s32 D_race_00404AAC;
extern s32 D_race_00404AB0;
extern s32 D_race_004049E4;
extern s32 *D_race_004049E8;
extern s32 D_race_004049EC;
extern u8 D_race_004049C4[];
extern s32 D_race_004049F0;
extern void *D_race_00404A04;
void func_race_00400920();
void func_race_00400AB4();
void func_race_00400CC0();
void func_race_00400EB0();
void func_race_004011BC();
void func_race_00401B3C();
void func_race_00401E04();
void func_race_004024D0();
void func_race_004026D0();
void func_race_00402864();
void func_race_00402AAC();
void func_race_00402CE0();
void func_race_00402E98();
void func_race_004030D0();
void func_race_00403268();
void func_race_00403358();
s32 func_race_0040348C();
void func_race_004034C4();
void func_race_004035C8();
void func_race_0040367C();
void func_race_004036C8();
void func_race_004036E4();
void func_race_00403734();
void func_race_004037DC();
void func_race_004038B0();
void func_race_00403B6C();
void func_race_00404044();
void func_race_00404194();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/__entrypoint_func_race_400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00400920.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00400AB4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00400CC0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00400EB0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004011BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00401B3C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00401E04.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004024D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004026D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00402864.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00402AAC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00402CE0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00402E98.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004030D0.s")

void func_race_00403268(void) {
    (*(void (**)(void *, s32, s32))((u8 *)gMiscExports + 0xFC))((u8 *)&D_race_00404710, D_race_00404AA0, 0x96);
    (*(void (**)(void *, s32, s32))((u8 *)gMiscExports + 0xFC))((u8 *)&D_race_00404718, D_race_00404AA4, 0x8C);
    (*(void (**)(void *, s32, s32))((u8 *)gMiscExports + 0xFC))((u8 *)&D_race_00404724, D_race_00404AA8, 0x82);
    (*(void (**)(void *, s32, s32))((u8 *)gMiscExports + 0xFC))((u8 *)&D_race_00404730, D_race_00404AAC, 0x78);
    (*(void (**)(void *, s32, s32))((u8 *)gMiscExports + 0xFC))((u8 *)&D_race_00404740, D_race_00404AB0, 0x6E);
    if (gAiExports != 0) {
        (*(void (**)(void))((u8 *)gAiExports + 0x10))();
    }
}

void func_race_00403350(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00403358.s")

s32 func_race_0040348C(s32 arg0) {
    if ((arg0 < 0) || (arg0 >= *(s32 *)((u8 *)&D_race_004049F0 + 0x10))) {
        return 0;
    }
    return *(s32 *)((u8 *)&D_race_004049F0 + arg0 * 4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004034C4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004035C8.s")

void func_race_0040367C(s32 arg0) {
    uvLoadFile(0x55564D44, arg0);
    (*(void (**)(s32, void *))((u8 *)gUvModelExports + 0x4C))(arg0, func_race_004037DC);
}

void func_race_004036C8(s32 a0, s32 a1, s32 a2) {
    D_race_004049E4 = a0;
    D_race_004049E8 = (s32 *)a1;
    D_race_004049EC = a2;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004036E4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00403734.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004037DC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_004038B0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00403B6C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/race/func_race_00404044.s")

void func_race_00404194(void) {
    (*(void (**)(s32))((u8 *)D_race_00404A04 + 0x20))(D_race_004049F0);
}

