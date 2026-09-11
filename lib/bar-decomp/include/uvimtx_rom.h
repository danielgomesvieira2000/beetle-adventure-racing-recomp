#ifndef UVIMTX_ROM_H
#define UVIMTX_ROM_H
typedef struct UvImtx_Rom_Exports_s {
    /* 0x00 */ void (*func_uvimtx_rom_0040018C)();                      /* inferred */
    /* 0x04 */ void (*uvIMtxCopy)(Mtx*, Mtx);             /* inferred */
    /* 0x08 */ void (*uvIMtxSetIdentity)(Mtx*);                  /* inferred */
    /* 0x0C */ void (*uvImtxStub)(s32, s32, s32, s32);    /* inferred */
    /* 0x10 */ void (*func_uvimtx_rom_00400338)(uvMtx*, uvMtx);        /* inferred */
    /* 0x14 */ void (*func_uvimtx_rom_00400410)(Mtx);                  /* inferred */
    /* 0x18 */ void (*uvGfxMtxViewPop)(void);                     /* inferred */
    /* 0x1C */ void (*func_uvimtx_rom_004004D8)(Mtx);                  /* inferred */
    /* 0x20 */ Mtx* (*uvIMtxPush)(Mtx, u16);             /* inferred */
    /* 0x24 */ void (*func_uvimtx_rom_0040062C)(Mtx*, u16);            /* inferred */
    /* 0x28 */ s32 (*func_uvimtx_rom_004006E0)(s32);                   /* inferred */
} UvImtx_Rom_Exports;                               /* size = 0x2C */   
#endif /* UVIMTX_ROM_H */

