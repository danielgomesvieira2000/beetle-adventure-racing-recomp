// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef UVUFILELD_ROM_H
#define UVUFILELD_ROM_H
typedef struct UvuFileLd_Rom_Exports_s {
    void (*uvuFileldStub)(void);
    void (*uvFileCopy)(u8 *);
    void (*uvFreeFilePtr)(void);
    void (*uvSetCopyFileDataFlag)(s32);
} UvuFileLd_Rom_Exports;
#endif /* UVUFILELD_ROM_H */


