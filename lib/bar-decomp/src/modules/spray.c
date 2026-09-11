// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
void uvUnloadFile(s32 formFileTag, s32 fileId);
typedef struct UnkCbckExports_s { char pad[0x10]; void (*unk10)(s32, void *, s32, s32); } UnkCbckExports;
typedef struct UnkChanExports_s { char pad[0x4]; void (*unk4)(s32, s32, void *, s32); } UnkChanExports;
extern UnkCbckExports *gUvCbckExports;
extern UnkChanExports *gUvChanExports;
void func_spray_0040055C(s32 arg0);
void func_spray_004000A8();
void func_spray_004000E0();
void func_spray_00400138();
void func_spray_00400240();
void func_spray_004002A8();
void func_spray_0040055C();
void func_spray_0040064C();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/spray/__entrypoint_func_spray_400000.s")

void func_spray_004000A8(void) {
    uvUnloadFile(0x55565458, 0x7C);
    uvUnloadFile(0x55565458, 0x7B);
}

void func_spray_004000E0(s32 arg0) {
    s32 sp1C;
    gUvChanExports->unk4(arg0, 6, &sp1C, 0);
    gUvCbckExports->unk10(sp1C, func_spray_0040055C, 0, 0xC8);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/spray/func_spray_00400138.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/spray/func_spray_00400240.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/spray/func_spray_004002A8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/spray/func_spray_0040055C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/spray/func_spray_0040064C.s")

