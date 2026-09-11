#ifndef UVVATTR_ROM_H
#define UVVATTR_ROM_H
typedef struct UvVAttr_Exports_s {
    /* 0x00 */ void (*func_uvvattr_rom_00400070)(void);                      /* inferred */
    /* 0x04 */ void (*func_uvvattr_rom_00400094)(u32, u32, ...);         /* inferred */
    /* 0x08 */ void (*func_uvvattr_rom_00400314)(u32, s32, ...);         /* inferred */
    /* 0x0C */ s32 (*func_uvvattr_rom_00400514)(s32, s32 *, s32, ...);  /* inferred */
    /* 0x10 */ void (*func_uvvattr_rom_004011B4)(s32, u32, s32 *, s32 *, s32 *); /* inferred */
} UvVAttr_Exports;                                  /* size = 0x14 */
#endif /* UVVATTR_ROM_H */
