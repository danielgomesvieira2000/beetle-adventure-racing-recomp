// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef UVTSEQLD_ROM_H
#define UVTSEQLD_ROM_H
typedef struct UvtSeqLd_Rom_Exports_s {
    /* 0x00 */ void (*func_uvtseqld_rom_00400054)(void);                      /* inferred */
    /* 0x04 */ void *(*func_uvtseqld_rom_0040005C)(u8 *);                 /* inferred */
    /* 0x08 */ void (*func_uvtseqld_rom_0040027C)(void *);                /* inferred */
    /* 0x0C */ s32 (*func_uvtseqld_rom_0040029C)(s32);                    /* inferred */
} UvtSeqLd_Rom_Exports;                             /* size = 0x10 */
#endif /* UVTSEQLD_ROM_H */
