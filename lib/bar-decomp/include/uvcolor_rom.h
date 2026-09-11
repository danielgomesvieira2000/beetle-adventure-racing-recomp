#ifndef UVCOLOR_ROM_H
#define UVCOLOR_ROM_H
typedef struct UvColor_Exports_s {
    /* 0x00 */ void (*func_uvcolor_rom_00400070)(void);
    /* 0x04 */ void (*func_uvcolor_rom_00400094)(s32, f32, f32, f32);
    /* 0x08 */ void (*func_uvcolor_rom_00400120)(s32);
    /* 0x0C */ void (*func_uvcolor_rom_00400520)(s32, f32, f32, f32);
    /* 0x10 */ void (*func_uvcolor_rom_00400578)(s32, f32, f32, f32, f32);
} UvColor_Exports;
#endif /* UVCOLOR_ROM_H */
