#ifndef UVJANIMLD_ROM_H
#define UVJANIMLD_ROM_H

typedef struct UvParsedAN_2_s {
    struct UvParsedAN_2_s *unk0;
    u16 unk4;
    u16 unk6;
    u8 *unk8;
    u16 *unkC;
    Vec3F *unk10;
    Vec3F *unk14;
    Quat *unk18;
    u8 *unk1C;
} UvParsedAN_2;

typedef struct UvParsedAN_s {
    /* 0x00 */ u16 unk0;
    /* 0x02 */ u16 unk2; /* inferred */
    /* 0x04 */ u16 unk4; /* inferred */
    /* 0x06 */ u16 unk6; /* inferred */
    /* 0x08 */ u8 unk8[1];
    /* 0x09 */ u8 unk9;            /* inferred */
    /* 0x0A */ char padA[2];       /* maybe part of unk9[3]? */
    /* 0x0C */ UvParsedAN_2 *unkC; /* inferred */
} UvParsedAN;                      /* size = 0x10 */

typedef struct UvJanimLd_Exports_s {
    /* 0x0 */ void (*stub)(void);                              /* inferred */
    /* 0x4 */ UvParsedAN *(*uvParseAnimationData)(u8 *);       /* inferred */
    /* 0x8 */ void (*uvFreeParsedAnimationData)(UvParsedAN *); /* inferred */
} UvJanimLd_Exports;
#endif /* UVJANIMLD_ROM_H */

