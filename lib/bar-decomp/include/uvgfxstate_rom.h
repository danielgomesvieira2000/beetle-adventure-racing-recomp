#ifndef UVGFXSTATE_ROM_H
#define UVGFXSTATE_ROM_H

typedef struct uvGfxState_s {
    /* 0x00 */ s32 unk0;
    /* 0x04 */ s32 state;
    /* 0x08 */ Gfx *displayList;
    /* 0x0C */ s16 unkC;
    /* 0x0E */ s16 unkE; /* inferred */
    /* 0x10 */ s16 unk10;
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s16 unk14;
} uvGfxState;

typedef struct UvGfxState_Rom_Exports_s {
    /* 0x00 */ void (*func_uvgfxstate_rom_00400390)(void);                      /* inferred */
    /* 0x04 */ void (*func_uvgfxstate_rom_00400440)(u32);                   /* inferred */
    /* 0x08 */ void (*func_uvgfxstate_rom_0040049C)(Mtx4F *, s32);          /* inferred */
    /* 0x0C */ void (*func_uvgfxstate_rom_00401314)(s32);                   /* inferred */
    /* 0x10 */ void (*func_uvgfxstate_rom_00401354)(s32);                  /* inferred */
    /* 0x14 */ void (*func_uvgfxstate_rom_00401398)(s32);                  /* inferred */
    /* 0x18 */ void (*func_uvgfxstate_rom_004013DC)(uvGfxState *, s32);    /* inferred */
    /* 0x1C */ void (*func_uvgfxstate_rom_004013F0)(uvGfxState *, s32);    /* inferred */
    /* 0x20 */ void (*func_uvgfxstate_rom_00401400)(uvGfxState *, s32);    /* inferred */
    /* 0x24 */ void (*func_uvgfxstate_rom_00401418)(uvGfxState *, s32, s32); /* inferred */
    /* 0x28 */ s32 (*func_uvgfxstate_rom_0040143C)(uvGfxState *, s32);     /* inferred */
    /* 0x2C */ void (*func_uvgfxstate_rom_00401460)(s32);                  /* inferred */
    /* 0x30 */ void (*func_uvgfxstate_rom_0040146C)(s32);                  /* inferred */
    /* 0x34 */ void (*func_uvgfxstate_rom_00401498)(uvGfxState *);         /* inferred */
    /* 0x38 */ void (*uvGfxStateDraw)(uvGfxState *);         /* inferred */
    /* 0x3C */ void (*func_uvgfxstate_rom_00401CE8)(void);                     /* inferred */
    /* 0x40 */ void (*func_uvgfxstate_rom_00401E60)(s32);                  /* inferred */
    /* 0x44 */ void (*func_uvgfxstate_rom_00401F54)(f32, f32);             /* inferred */
    /* 0x48 */ void (*func_uvgfxstate_rom_004020DC)(f32 *, f32 *);         /* inferred */
    /* 0x4C */ void (*func_uvgfxstate_rom_004020F8  )(s32, s32);             /* inferred */
    /* 0x50 */ void (*func_uvgfxstate_rom_00402218)(void);                     /* inferred */
    /* 0x54 */ void (*func_uvgfxstate_rom_00402254)(void);                     /* inferred */
    /* 0x58 */ void (*func_uvgfxstate_rom_004022B0)(void);                     /* inferred */
    /* 0x5C */ void (*func_uvgfxstate_rom_00402350)(s32, s32, s32, s32, f32, f32, f32, f32); /* inferred */
    /* 0x60 */ s32 (*func_uvgfxstate_rom_004029B8)(u32);                   /* inferred */
    /* 0x64 */ void (*func_uvgfxstate_rom_00402B14)(s32, s32);             /* inferred */
    /* 0x68 */ void (*func_uvgfxstate_rom_00402B28)(s32, Vec3F);           /* inferred */
    /* 0x6C */ void (*uvGfxSync)(void);                     /* inferred */
    /* 0x70 */ s32 *unk70;                          /* inferred */
    /* 0x74 */ s32 *unk74;                            /* inferred */
    /* 0x78 */ s32 *unk78;                            /* inferred */
} UvGfxState_Rom_Exports;                           /* size = 0x7C */
#endif /* UVGFXSTATE_ROM_H */
