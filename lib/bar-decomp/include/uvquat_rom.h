#ifndef UVQUAT_ROM_H
#define UVQUAT_ROM_H

typedef struct Quat_s {
    /* 0x00 */ f32 x;                /* inferred */
    /* 0x04 */ f32 y;                /* inferred */
    /* 0x08 */ f32 z;                /* inferred */
    /* 0x0C */ f32 w;                /* inferred */
} Quat; /* size = 0x10 */

typedef struct UvQuat_Exports_s {
    /* 0x00 */ void (*uvModuleCleanup)(void);                      /* inferred */
    /* 0x04 */ void (*func_uvquat_rom_004000D8)(Quat*, Mtx4F*); /* inferred */
    /* 0x08 */ void (*uvQuatFromEuler)(Quat*, f32, f32, f32); /* inferred */
    /* 0x0C */ void (*func_uvquat_rom_00400524)(Mtx4F*, Quat*); /* inferred */
    /* 0x10 */ void (*func_uvquat_rom_0040070C)(Quat*, Quat*, f32, f32, f32, f32); /* inferred */
    /* 0x14 */ void (*uvQuatMultiply)(Quat*, Quat*, Quat*); /* inferred */
    /* 0x18 */ void (*uvQuatCopy)(Quat*, Quat*); /* inferred */
    /* 0x1C */ void (*func_uvquat_rom_004009BC)(Quat*, Quat*, Quat*, f32); /* inferred */
} UvQuat_Exports;                                   /* size = 0x20 */

#endif /* UVQUAT_ROM_H */
