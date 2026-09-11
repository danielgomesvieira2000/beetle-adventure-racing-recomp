#ifndef UVCHANNEL_ROM_H
#define UVCHANNEL_ROM_H
typedef struct UvChannelExports_s {
    /* 0x00 */ void (*func_uvchannel_rom_004001D4)(void);                      /* inferred */
    /* 0x04 */ void (*func_uvchannel_rom_00400288)(s32, ...);              /* inferred */
    /* 0x08 */ void (*func_uvchannel_rom_004005DC)(s32, ...);              /* inferred */
    /* 0x0C */ void (*func_uvchannel_rom_00400AD4)(s32, s32);              /* inferred */
    /* 0x10 */ void (*func_uvchannel_rom_00400AFC)(s32, f32, f32, f32, f32, f32, f32); /* inferred */
    /* 0x14 */ void (*func_uvchannel_rom_00400B70)(s32, s32, s32, s32, s32); /* inferred */
    /* 0x18 */ void (*func_uvchannel_rom_00400BB8)(s32, s32, s32);        /* inferred */
    /* 0x1C */ s32 (*func_uvchannel_rom_00400CCC)(void);                      /* inferred */
    /* 0x20 */ void (*func_uvchannel_rom_00400E7C)(s32);                  /* inferred */
    /* 0x24 */ void (*func_uvchannel_rom_00401278)(s32);                  /* inferred */
    /* 0x28 */ void (*func_uvchannel_rom_00401414)(s32, Mtx4F*);          /* inferred */
    /* 0x2C */ s32 (*func_uvchannel_rom_004014E8)(s32, f32, f32, f32, f32); /* inferred */
} UvChannelExports;                                 /* size = 0x30 */
#endif /* UVCHANNEL_ROM_H */
