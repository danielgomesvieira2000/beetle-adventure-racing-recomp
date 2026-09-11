// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
/*__SEEDEXTERNS__*/
extern f32 D_uvpfx_rom_00404F40;
extern f32 D_uvpfx_rom_00404D40;
extern s32 D_uvpfx_rom_00404FA8;
extern void* D_uvpfx_rom_00404FA0;
extern void* D_uvpfx_rom_00405BB0;
void func_uvpfx_rom_00400264();
void func_uvpfx_rom_00400398();
void func_uvpfx_rom_00400488();
void func_uvpfx_rom_004004D0();
void func_uvpfx_rom_004012CC();
void func_uvpfx_rom_00401888();
void func_uvpfx_rom_004019A4();
void func_uvpfx_rom_00401C6C();
void func_uvpfx_rom_00401D88();
void func_uvpfx_rom_00401E7C();
void func_uvpfx_rom_00402008();
void func_uvpfx_rom_0040211C();
void func_uvpfx_rom_004021FC();
void func_uvpfx_rom_0040242C();
void func_uvpfx_rom_0040374C();
void func_uvpfx_rom_00403848();
void func_uvpfx_rom_00403898();
void func_uvpfx_rom_00403AC8();
void func_uvpfx_rom_00404A60();
#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/__entrypoint_func_uvpfx_rom_400000.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00400264.s")

s32 func_uvpfx_rom_004002BC(s32 arg0) {
    s32 found;
    s32 i;
    void *entry;
    void *node;
    void *data;

    found = -1;
    i = 0;
    if (D_uvpfx_rom_00404FA8 > 0) {
        entry = D_uvpfx_rom_00404FA0;
        do {
            node = *(s32 **)((u8 *)entry + 0x0);
            data = *(s32 **)((u8 *)entry + 0x4);
            if (arg0 == *(s16 *)((u8 *)node + 0x2)) {
                if (*(s16 *)((u8 *)data + 0x24) == 0) {
                    *(s16 *)((u8 *)data + 0x24) = -1;
                    found = i;
                    *(f32 *)((u8 *)data + 0xC) = D_uvpfx_rom_00404D40;
                    break;
                }
            }
            i += 1;
            entry = (u8 *)entry + 0xC;
        } while (i < D_uvpfx_rom_00404FA8);
    }
    if (found < 0) {
        return -1;
    }
    node = *(s32 **)((u8 *)D_uvpfx_rom_00404FA0 + (found * 0xC));
    if (*(s16 *)((u8 *)node + 0x4) == 1) {
        func_uvpfx_rom_0040374C(found);
    } else {
        func_uvpfx_rom_0040211C(found);
    }
    return found;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00400398.s")

void func_uvpfx_rom_00400488(s32 arg0) {
    s32 val = *(s32*)((u8*)D_uvpfx_rom_00404FA0 + arg0 * 0xC + 4);
    ((void(*)(s32))(*(s32*)((u8*)D_uvpfx_rom_00405BB0 + 0xC)))(val + 0x80);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_004004D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_004012CC.s")

void func_uvpfx_rom_00401810(void) {
    uvUnloadModule(0x464D5458);
    uvUnloadModule(0x46564543);
    uvUnloadModule(0x4D415448);
    uvUnloadModule(0x4447454F);
    uvUnloadModule(0x53544154);
    uvUnloadModule(0x4348414E);
    uvUnloadModule(0x4342434B);
    uvUnloadModule(0x54534551);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00401888.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_004019A4.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00401C6C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00401D88.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00401E7C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00402008.s")

void func_uvpfx_rom_004020D4(s32 arg0) {
    void *entry;

    entry = ((void **)((u8 *)D_uvpfx_rom_00404FA0 + (arg0 * 0xC)))[1];
    *(f32 *)((u8 *)entry + 0xC) =
        ((D_uvpfx_rom_00404F40 - *(f32 *)((u8 *)entry + 0x10)) + 1.0f) - *(f32 *)((u8 *)entry + 0x4);
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_0040211C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_004021FC.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_0040242C.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_0040374C.s")

void func_uvpfx_rom_0040381C(s32 arg0, f32 arg1) {
    void *obj = ((void **)((u8 *)D_uvpfx_rom_00404FA0 + arg0 * 0xC))[0];
    void *emitter = ((void **)obj)[0xA];
    *(f32 *)((u8 *)emitter + 0x18) = arg1;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00403848.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00403898.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00403AC8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvpfx_rom/func_uvpfx_rom_00404A60.s")

void func_uvpfx_rom_00404D2C(void) {
}

