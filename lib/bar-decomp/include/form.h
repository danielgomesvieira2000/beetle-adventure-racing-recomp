// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef BAR_FORM_H
#define BAR_FORM_H
typedef struct FormFileInfo_s {
    /* 0x00 */ u8* fileData;
    /* 0x04 */ u32 padType; 
    /* 0x08 */ s32 fileSize; // File size couting the first eight bytes
    /* 0x0C */ u32 padTagStart; 
} FormFileInfo;                               /* size = 0x10 */

#endif /* BAR_FORM_H */
