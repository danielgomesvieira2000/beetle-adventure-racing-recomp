#ifndef UVTSEQ_ROM_H
#define UVTSEQ_ROM_H
typedef struct UvTSeq_Exports_s {
    /* 0x00 */ void (*func_uvtseq_rom_0040026C)(void);                      /* inferred */
    /* 0x04 */ void (*func_uvtseq_rom_00400378)(s32, s32);              /* inferred */
    /* 0x08 */ void (*func_uvtseq_rom_00400454)(s32, ...);              /* inferred */
    /* 0x0C */ s32 (*func_uvtseq_rom_004005BC)(void);                       /* inferred */
    /* 0x10 */ void (*func_uvtseq_rom_00400620)(s32);                  /* inferred */
    /* 0x14 */ u16 (*func_uvtseq_rom_0040093C)(s32);                   /* inferred */
    /* 0x18 */ u8 (*func_uvtseq_rom_0040096C)(s32);                    /* inferred */
    /* 0x1C */ u16 (*func_uvtseq_rom_0040098C)(s32, s32);              /* inferred */
    /* 0x20 */ u8 (*func_uvtseq_rom_004009F0)(s32);                    /* inferred */
    /* 0x24 */ f32 (*func_uvtseq_rom_00400A10)(s32);                   /* inferred */
} UvTSeq_Exports;                                   /* size = 0x28 */
#endif /* UVTSEQ_ROM_H */
