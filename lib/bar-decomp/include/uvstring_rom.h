// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef UVSTRING_ROM_H
#define UVSTRING_ROM_H

typedef struct UvString_Exports_s {
    /* 0x00 */ void (*uvStrStub)();
    /* 0x04 */ char *(*uvStrChr)(const char *, const char);
    /* 0x08 */ u32 (*uvStrlen)(const char *);
    /* 0x0C */ int (*uvStrcmp)(const char *, const char *);
    /* 0x10 */ void (*uvSprintf)(char *, const char *, ...);
    /* 0x14 */ u8 *(*uvStrcpy)(u8 *, u8 *);
} UvString_Exports;

#endif /* UVSTRING_ROM_H */
