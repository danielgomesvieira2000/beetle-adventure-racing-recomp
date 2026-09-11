#ifndef UVDBG_ROM_H
#define UVDBG_ROM_H

typedef struct UvDbgRom_Exports_s {
    /* 0x00 */ void (*func_uvdbg_rom_00400490)(void);                      /* inferred */
    /* 0x04 */ void (*func_uvdbg_rom_00401D20)(u8, u8, u8);            /* inferred */
    /* 0x08 */ void (*func_uvdbg_rom_00401D94)(s32, s32);              /* inferred */
    /* 0x0C */ void (*func_uvdbg_rom_00401DE8)(s32);                   /* inferred */
    /* 0x10 */ void (*func_uvdbg_rom_00402128)(void);                     /* inferred */
    /* 0x14 */ void (*func_uvdbg_rom_00402490)(void);                     /* inferred */
    /* 0x18 */ void (*func_uvdbg_rom_004029D4)(s32);                  /* inferred */
    /* 0x1C */ void (*func_uvdbg_rom_004029E0)(void);                     /* inferred */
    /* 0x20 */ void (*func_uvdbg_rom_00402A14)(s32);                  /* inferred */
    /* 0x24 */ void (*func_uvdbg_rom_00402CB8)(u8, u8);               /* inferred */
    /* 0x28 */ void (*func_uvdbg_rom_00402D00)(void);                     /* inferred */
} UvDbgRom_Exports;                                 /* size = 0x2C */

#endif /* UVDBG_ROM_H */
