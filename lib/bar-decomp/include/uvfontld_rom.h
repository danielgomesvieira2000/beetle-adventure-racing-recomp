#ifndef UVFONTLD_ROM_H
#define UVFONTLD_ROM_H
typedef struct UvFontLd_Exports_s {
    /* 0x0 */ void (*func_uvfontld_rom_00400048)(void);                       /* inferred */
    /* 0x4 */ ParsedUVFT *(*func_uvfontld_rom_00400050)(u8 *);            /* inferred */
    /* 0x8 */ void (*func_uvfontld_rom_00400324)(ParsedUVFT *);           /* inferred */
} UvFontLd_Exports;                                 /* size = 0xC */
#endif /* UVFONTLD_ROM_H */
