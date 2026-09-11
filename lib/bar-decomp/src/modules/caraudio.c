// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "global_exports.h"
/*__SEEDEXTERNS__*/
extern f32 D_caraudio_004040C0[];
extern f32 D_caraudio_00404254;
typedef struct {
    char pad0[0x58];
    void (*unk58)(void);
    char pad5C[0x14];
    void (*unk70)(void);
    char pad74[0xC];
    void (*unk80)(void);
    void (*unk84)(void *, s32);
    void (*unk88)(s32, s32);
} UnkStruct_caraudio_SndExports;
extern UnkStruct_caraudio_SndExports *gSndExports;
void func_caraudio_00400314();
void func_caraudio_0040034C();
void func_caraudio_004003B0();
void func_caraudio_00400550();
void func_caraudio_00400758();
void func_caraudio_0040090C();
void func_caraudio_00400DB4();
void func_caraudio_004010A4();
void func_caraudio_004015B8();
void func_caraudio_00401698();
void func_caraudio_0040171C();
void func_caraudio_00401A08();
void func_caraudio_00401F24();
void func_caraudio_00401FF4();
void func_caraudio_00402044();
void func_caraudio_00402070();
void func_caraudio_0040218C();
void func_caraudio_004022BC();
void func_caraudio_004027A8();
void func_caraudio_004029F8();
void func_caraudio_00402B20();
void func_caraudio_00402C10();
void func_caraudio_00402CA4();
void func_caraudio_00402E00();
void func_caraudio_00403044();
void func_caraudio_00403170();
void func_caraudio_004031E0();
void func_caraudio_0040336C();
void func_caraudio_0040397C();
void func_caraudio_00403A2C();
extern u8 D_caraudio_004041E0[];
extern s32 gCurrentTrack;
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/__entrypoint_func_caraudio_400000.s")

void func_caraudio_00400314(void) {
    gSndExports->unk58();
    uvUnloadModule('motn');
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_0040034C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004003B0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00400550.s")

void func_caraudio_00400708(void *arg0) {
    D_caraudio_00404254 = D_caraudio_004040C0[*(s32 *)(*(s32 *)((u8 *)arg0 + 0xC) + 0x1EC)];
    func_caraudio_00402B20(*(s32 *)((u8 *)arg0 + 0x14), arg0);
    func_caraudio_00402C10(arg0);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00400758.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_0040090C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00400DB4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004010A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004015B8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00401698.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_0040171C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00401A08.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00401F24.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00401FF4.s")

void func_caraudio_00402044(void) {
    gSndExports->unk70();
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00402070.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_0040218C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004022BC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004027A8.s")

s32 func_caraudio_00402994(void *arg0) {
    s32 *slots;

    slots = (s32 *)((u8 *)*(s32 **)((u8 *)arg0 + 0x4) + 0x2D0);
    if (slots[0] != 0) {
        return 1;
    }
    if (slots[1] != 0) {
        return 1;
    }
    if (slots[2] != 0) {
        return 1;
    }
    if (slots[3] != 0) {
        return 1;
    }
    return 0;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004029F8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00402B20.s")

void func_caraudio_00402C08(void) {
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00402C10.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00402CA4.s")

void func_caraudio_00402DE4(s32 a0) {
    D_caraudio_004041E0[a0] = 0;
}

s32 func_caraudio_00402DF4(void) {
    return gCurrentTrack;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00402E00.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00403044.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00403170.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_004031E0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_0040336C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_0040397C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/caraudio/func_caraudio_00403A2C.s")

