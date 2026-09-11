// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern f32 D_ai_0040810C;
extern s32 D_ai_00407EA0;
extern void *D_ai_00408E40;
extern void *D_ai_00407FF4;
extern s32 D_ai_00408E48;
extern s32 D_ai_00408E4C;
extern s32 D_ai_00408008;
void func_ai_00400000();
void func_ai_0040019C();
void func_ai_0040089C();
void func_ai_00400A24();
void func_ai_00400E40();
void func_ai_00400F18();
void func_ai_00400FF0();
void func_ai_004017A4();
void func_ai_004019EC();
void func_ai_00401C84();
void func_ai_00401EF0();
void func_ai_00402028();
void func_ai_004021E0();
void func_ai_004022A4();
void func_ai_00402858();
void func_ai_00402974();
void func_ai_00402A10();
void func_ai_00402AE0();
void func_ai_00402C2C();
void func_ai_00402E4C();
void func_ai_004030BC();
void func_ai_00403288();
void func_ai_00403448();
void func_ai_004037B4();
void func_ai_00403994();
void func_ai_00403C80();
void func_ai_00403F4C(s32 arg0);
void func_ai_00403F54();
void func_ai_00404064();
void func_ai_00404150(void *arg0, void *arg1);
void func_ai_00404194();
void func_ai_0040428C();
void func_ai_004042A4();
void func_ai_004044F0();
void func_ai_0040462C();
void func_ai_004048BC();
void func_ai_0040490C();
void func_ai_00404E10();
void func_ai_004051A4();
void func_ai_00405324();
void func_ai_004053A8();
void func_ai_00405754();
void func_ai_004057C0();
void func_ai_00405AC4();
void func_ai_00405B4C();
void func_ai_00405C8C();
void func_ai_00405D18();
void func_ai_00405D50();
void func_ai_00405E88();
void func_ai_00405EF4();
s32 func_ai_00406120();
void func_ai_0040617C();
void func_ai_00406400();
void func_ai_004068A8();
void func_ai_004069CC();
void func_ai_00406D84();
void func_ai_00407054();
s32 func_ai_0040757C();
void func_ai_004075B8();
s32 func_ai_00407724();
extern s32 D_ai_00408E44;
extern s32 D_ai_00407FFC;
extern s32 D_ai_00408000;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_0040019C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_0040089C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00400A24.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/__entrypoint_func_ai_400bbc.s")

void func_ai_00400E38(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00400E40.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00400F18.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00400FF0.s")

void func_ai_0040179C(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004017A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004019EC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00401C84.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00401EF0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402028.s")

void func_ai_004021D8(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004021E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004022A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402858.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402974.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402A10.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402AE0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402C2C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00402E4C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004030BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00403288.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00403448.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004037B4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00403994.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00403C80.s")

void func_ai_00403F44(void) {
}

void func_ai_00403F4C(s32 arg0) {

}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00403F54.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00404064.s")

void func_ai_00404150(void *arg0, void *arg1) {
    s32 idx = D_ai_00407EA0;
    if (idx != -1) {
        *(f32 *)arg0 = *(f32 *)((u8 *)&D_ai_0040810C + idx * 0x1E8);
        *(f32 *)arg1 = 0.0f;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00404194.s")

void func_ai_0040428C(void *arg0) {
    *(u8 *)((u8 *)arg0 + 0xC2) = 0x1;
    *(f32 *)((u8 *)arg0 + 0x84) = 0.0f;
    *(f32 *)((u8 *)arg0 + 0xB0) = 0.0f;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004042A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004044F0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_0040462C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004048BC.s")

void func_ai_00404904(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_0040490C.s")

void func_ai_00404DE8(void) {
}

void func_ai_00404DF0(void *a0, s16 a1) {
    if (D_ai_00407FFC) {
        *(s16 *)((u8 *)a0 + 0xA) = 0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00404E10.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004051A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405324.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004053A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405754.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004057C0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405AC4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405B4C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405C8C.s")

void func_ai_00405D18(void *a0, void *a1, void *a2) {
    *(s32 *)((u8 *)a0 + 0x10) = (s32)a2;
    *(s32 *)((u8 *)a0 + 0xC) = (s32)a1;
    if (a1 == NULL) {
        D_ai_00408E40 = a0;
    } else {
        *(s32 *)((u8 *)a1 + 0x10) = (s32)a0;
    }
    if (a2 == NULL) {
        D_ai_00408E44 = (s32)a0;
        return;
    }
    *(s32 *)((u8 *)a2 + 0xC) = (s32)a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405D50.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405E88.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00405EF4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00406120.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_0040617C.s")

s32 func_ai_004063B4(void) {
    s32 count;
    void *node;

    node = D_ai_00408E44;
    count = 1;
    if (node != NULL) {
loop:
        if (*(s32 *)((u8 *)node + 0x8) != 0) {
            node = *(void **)((u8 *)node + 0xC);
            count += 1;
            if (node != NULL) {
                goto loop;
            }
        }
    }
    if (node == NULL) {
        count = 1;
    }
    D_ai_00407FF4 = node;
    return count;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00406400.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004068A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004069CC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00406D84.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00407054.s")

s32 func_ai_00407564(void) {
    return D_ai_00408E44;
}

void func_ai_00407570(s32 a0) {
    D_ai_00407FFC = a0;
}

s32 func_ai_0040757C(void *arg0) {
    D_ai_00408E48 = 0;
    if (*(s32 *)((u8 *)arg0 + 0xC) != 0) {
        do {
            D_ai_00408E48++;
            arg0 = *(void **)((u8 *)arg0 + 0xC);
        } while (*(s32 *)((u8 *)arg0 + 0xC) != 0);
    }
    return D_ai_00408E48;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_004075B8.s")

void func_ai_00407718(s32 a0) {
    D_ai_00408000 = a0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/ai/func_ai_00407724.s")

