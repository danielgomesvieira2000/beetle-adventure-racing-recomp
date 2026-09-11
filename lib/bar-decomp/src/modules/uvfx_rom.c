// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"

/* Local export-table stubs: only the members accessed in this module.
   Shapes mirror the build-verified spray/flag modules. */
typedef struct UnkCbckExports_s {
    char pad[0x10];
    void (*unk10)(s32, void *, s32, s32); /* 0x10 - register callback */
} UnkCbckExports;
typedef struct UnkChanExports_s {
    char pad[0x4];
    void (*unk4)(s32, s32, void *, s32);  /* 0x04 - open callback list */
} UnkChanExports;
typedef struct UnkFmtxExports_s {
    char pad[0xC];
    void (*unkC)(void *);                 /* 0x0C - Mtx cleanup (single ptr arg) */
} UnkFmtxExports;

extern UnkChanExports *D_uvfx_rom_0040170C; /* CHAN module exports */
extern UnkCbckExports *D_uvfx_rom_00401708; /* CBCK module exports */
extern UnkFmtxExports *D_uvfx_rom_004016EC; /* FMTX module exports */

extern u8  D_uvfx_rom_00401120;             /* effects-slot array base (10 slots x 0x94) */
extern s32 D_uvfx_rom_004011A8;             /* entry[0] field +0x88 (s32) */
extern s16 D_uvfx_rom_004011AC;             /* entry[0] field +0x8C (s16) */
extern u8  D_uvfx_rom_004011AF;             /* entry[0] field +0x8F (u8) */
extern s32 D_uvfx_rom_0040123C;             /* entry[1] field +0x11C (s32) */
extern s16 D_uvfx_rom_00401240;             /* entry[1] field +0x120 (s16) */
extern u8  D_uvfx_rom_00401243;             /* entry[1] field +0x123 (u8) */
extern u8  D_uvfx_rom_00401248;             /* entry[2] base - loop start for batch clear */
extern u8  D_uvfx_rom_004016E8;             /* one-past-end sentinel for batch-clear loop */

void func_uvfx_rom_00400C90(void); /* effect-update callback - forward decl */

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/__entrypoint_func_uvfx_rom_400000.s")

// Register the per-frame FX-update callback into the CBCK callback list.
// Opens a CHAN callback container (type 6) for arg0, then registers
// func_uvfx_rom_00400C90 into that container with priority 0x41.
void func_uvfx_rom_004001CC(s32 arg0) {
    s32 sp1C;

    D_uvfx_rom_0040170C->unk4(arg0, 6, &sp1C, 0);
    D_uvfx_rom_00401708->unk10(sp1C, func_uvfx_rom_00400C90, 0, 0x41);
}

// Unload all modules that uvfx_rom depends on (called at module teardown).
// Four-char tags spell: GMGR STAT CBCK CHAN FMTX IMTX MATH DGEO ISCT TSEQ TERR.
void func_uvfx_rom_00400224(void) {
    uvUnloadModule(0x474D4752); /* GMGR */
    uvUnloadModule(0x53544154); /* STAT */
    uvUnloadModule(0x4342434B); /* CBCK */
    uvUnloadModule(0x4348414E); /* CHAN */
    uvUnloadModule(0x464D5458); /* FMTX */
    uvUnloadModule(0x494D5458); /* IMTX */
    uvUnloadModule(0x4D415448); /* MATH */
    uvUnloadModule(0x4447454F); /* DGEO */
    uvUnloadModule(0x49534354); /* ISCT */
    uvUnloadModule(0x54534551); /* TSEQ */
    uvUnloadModule(0x54455252); /* TERR */
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/func_uvfx_rom_004002C0.s")

// Return the channel/type byte (entry[arg0].unk1) of effect slot arg0,
// or 0xFF if the slot is inactive (entry[0x8E] == 0).
u8 func_uvfx_rom_0040033C(s32 arg0) {
    u8 *entry = (u8 *)&D_uvfx_rom_00401120 + arg0 * 0x94;

    if (entry[0x8E] != 0) {
        return entry[1]; /* unk1: channel/type id */
    }
    return 0xFF;
}

// Destroy the FMTX transform associated with effect slot arg0 (0xFF = no-op).
// Calls the FMTX-module matrix-cleanup function on the Mtx4F at offset +0x48.
void func_uvfx_rom_0040037C(s32 arg0) {
    if (arg0 != 0xFF) {
        D_uvfx_rom_004016EC->unkC(
            (void *)((u8 *)&D_uvfx_rom_00401120 + arg0 * 0x94 + 0x48));
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/func_uvfx_rom_004003D0.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/func_uvfx_rom_004006C8.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/func_uvfx_rom_00400838.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/func_uvfx_rom_00400C90.s")

#pragma GLOBAL_ASM("asm/us/nonmatchings/modules/uvfx_rom/func_uvfx_rom_00400E90.s")

// Find the first free (inactive, entry[0x8E] == 0) effect slot among the 10
// slots. Returns the slot index [0..9], or 0xFF if all are occupied.
// arg0 is passed by the caller but unused here (IDO still spills it).
// NOTE: `i = 0; do` kept on one line - IDO O2 scheduling is line-sensitive here
// (found via decomp-permuter; splitting the line shifts the prologue schedule).
s32 func_uvfx_rom_0040104C(s32 arg0) {
    s32 i;
    s32 stride = 0x94; /* slot size; kept in a register for multu */

    i = 0; do {
        if (*((u8 *)&D_uvfx_rom_00401120 + (u32)i * stride + 0x8E) == 0) {
            return i; /* found an empty slot */
        }
        i = (i + 1) & 0xFFFF;
    } while (i < 0xA);
    return 0xFF; /* all 10 slots occupied */
}

