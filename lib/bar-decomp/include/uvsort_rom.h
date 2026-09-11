// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef UVSORT_ROM_H
#define UVSORT_ROM_H

typedef struct UvSort_Exports_s {
    /* 0x00 */ void (*func_uvsort_rom_00400240)();                      /* inferred */
    /* 0x04 */ s32 (*func_uvsort_rom_00400308)(u8);                     /* inferred */
    /* 0x08 */ void (*func_uvsort_rom_00400594)(s32);                   /* inferred */
    /* 0x0C */ void (*func_uvsort_rom_0040059C)(s32);                   /* inferred */
    /* 0x10 */ void (*func_uvsort_rom_00400834)(s32, void *(*)(s32, s32), s32, f32); /* inferred */
    /* 0x14 */ void (*func_uvsort_rom_00400B24)(s32, ...);             /* inferred */
} UvSort_Exports;                                   /* size = 0x18 */

#endif /* UVSORT_ROM_H */

