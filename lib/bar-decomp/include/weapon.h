#ifndef BAR_WEAPON_H
#define BAR_WEAPON_H
typedef struct Weapon_Exports_s {
    /* 0x00 */ void (*func_weapon_004000E0)(void);
} Weapon_Exports;

typedef struct UnkStruct_weapon_00400184_s {
    s32 unk0;
    s8 unk4;
    f32 unk8;
    char padC[0x8];
    s32 unk14;
    char pad18[0xC];
} UnkStruct_weapon_00400184;
#endif /* BAR_WEAPON_H */
