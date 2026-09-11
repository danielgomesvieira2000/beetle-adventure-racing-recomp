#ifndef UVCMIDI_ROM_H
#define UVCMIDI_ROM_H

typedef struct UvCMidi_Exports_s {
    /* 0x00 */ void (*func_uvcmidi_rom_004003B4)(void);
    /* 0x04 */ void (*func_uvcmidi_rom_00400454)(s32);
    /* 0x08 */ void (*func_uvcmidi_rom_0040062C)(s32);
    /* 0x0C */ void (*func_uvcmidi_rom_004006F4)(void);
    /* 0x10 */ void (*func_uvcmidi_rom_0040077C)(u32);
    /* 0x14 */ void (*func_uvcmidi_rom_004007B8)(f32);
    /* 0x18 */ f32 (*func_uvcmidi_rom_00400810)(void);
    /* 0x1C */ void (*func_uvcmidi_rom_00400854)(void);
    /* 0x20 */ void (*func_uvcmidi_rom_00400878)(void);
    /* 0x24 */ void (*func_uvcmidi_rom_004008C0)(void);
    /* 0x28 */ void (*func_uvcmidi_rom_004008E4)(f32);
    /* 0x2C */ void (*func_uvcmidi_rom_00400940)(void);
    /* 0x30 */ f32 (*func_uvcmidi_rom_004009C0)(u8);
    /* 0x34 */ void (*func_uvcmidi_rom_00400A04)(u8, u8);
    /* 0x38 */ void (*func_uvcmidi_rom_00400A38)(u8, u8);
    /* 0x3C */ f32 (*func_uvcmidi_rom_00400A84)(u8);
    /* 0x40 */ void (*func_uvcmidi_rom_00400AC8)(u8, u8);
    /* 0x44 */ void (*func_uvcmidi_rom_00400AFC)(u8, u8);
    /* 0x48 */ f32 (*func_uvcmidi_rom_00400B48)(u8);
    /* 0x4C */ void (*func_uvcmidi_rom_00400B8C)(u8, s32);
    /* 0x50 */ void (*func_uvcmidi_rom_00400BC0)(u8, s32);
    /* 0x54 */ f32 (*func_uvcmidi_rom_00400C0C)(u8);
    /* 0x58 */ void (*func_uvcmidi_rom_00400C3C)(u8, s32);
    /* 0x5C */ void (*func_uvcmidi_rom_00400C70)(u8, s32);
    /* 0x60 */ f32 (*func_uvcmidi_rom_00400CB8)(u8);
    /* 0x64 */ void (*func_uvcmidi_rom_00400CFC)(u8, u8);
    /* 0x68 */ void (*func_uvcmidi_rom_00400D30)(u8, u8);
    /* 0x6C */ void (*func_uvcmidi_rom_00400D7C)(u8, u8, u8);
    /* 0x70 */ void (*func_uvcmidi_rom_00400DC0)(u8, u8, u8);
    /* 0x74 */ void (*func_uvcmidi_rom_00400E10)(u8, u8, u8);
    /* 0x78 */ void (*func_uvcmidi_rom_00400E60)(u8 *, s32, s32 *);
    /* 0x7C */ ALVoiceState *(*func_uvcmidi_rom_00400EC4)(void);
    /* 0x80 */ void (*func_uvcmidi_rom_00400ED4)(ALVoiceState **, ALVoiceState **);
    /* 0x84 */ s32 (*func_uvcmidi_rom_00400EF8)(void);
    /* 0x88 */ void (*func_uvcmidi_rom_00400F04)(u8 *);
    /* 0x8C */ ALSeqPlayer *(*func_uvcmidi_rom_00400FC8)(void);
    /* 0x90 */ s16 (*func_uvcmidi_rom_00400FEC)(void);
} UvCMidi_Exports;
#endif /* UVCMIDI_ROM_H */
