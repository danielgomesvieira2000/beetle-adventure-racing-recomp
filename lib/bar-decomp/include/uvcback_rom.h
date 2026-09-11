#ifndef UVCBACK_ROM_H
#define UVCBACK_ROM_H

typedef struct UvCback_Rom_004000F0_Unk4 {
    s32 unk0;
    s32 unk4;
} UvCback_Rom_004000F0_Unk4;

typedef struct UvCback_Rom_004000F0_s {
    s32 count;
    UvCback_Rom_004000F0_Unk4 unk4[];
} UvCback_Rom_004000F0;

typedef struct UvCback_Exports_s {
    /* 0x00 */ void (*func_uvcback_rom_00400078)(void);                      /* inferred */
    /* 0x04 */ void *(*func_uvcback_rom_00400080)(s32);                  /* inferred */
    /* 0x08 */ void (*func_uvcback_rom_004000D0)(void *);                /* inferred */
    /* 0x0C */ void (*func_uvcback_rom_004000F0)(UvCback_Rom_004000F0 *, s32); /* inferred */
    /* 0x10 */ s32 (*func_uvcback_rom_0040016C)(UvCback_Rom_004000F0 *, s32, s32, s32); /* inferred */
    /* 0x14 */ s32 (*func_uvcback_rom_00400320)(UvCback_Rom_004000F0 *, s32); /* inferred */
    /* 0x18 */ s32 (*func_uvcback_rom_004003C8)(UvCback_Rom_004000F0 *, s32, u8); /* inferred */
} UvCback_Exports;
#endif /* UVCBACK_ROM_H */
