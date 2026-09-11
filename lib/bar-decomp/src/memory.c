// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"

void _uvMediaCopy(void* vAddr, void* devAddr, u32 nbytes) {
    s32 i;
    u8 *alignCeil;
    s32 alignDiff;
    u8 *src;
    u8 buf[16];
    u8 *dst;
    s32 temp_a2;
    s32 temp_v0;

    dst = vAddr;
    src = devAddr;
    if ((u32)src & 0x80000000) {
        for (i = 0; (u32)i < nbytes; i++) {
            dst[i] = src[i];
        }
        return;
    }

    while (nbytes > 0x1000) {
        _uvMediaCopy(dst, src, 0x1000);
        nbytes -= 0x1000;
        dst += 0x1000;
        src += 0x1000;
    }

    if (nbytes != 0) {
        if (((s32)src | (s32) dst | nbytes) & 1) {
            // it appears the devs intended to cause a fault by storing to an unaligned address,
            // but IDO outsmarted them and broke this into two `sb` instructions
            // Debug Print removed..
            *(u16*)(1) = 0;
            return;
        }
        if ((s32)dst & 7) {
            alignCeil = (u8*)((s32)(dst + 7) & ~7);
            alignDiff = alignCeil - dst;
            temp_a2 = (u32)src & 2;

            if ((nbytes != (u32)alignDiff) != 0) {
                _uvDMA(alignCeil, src + alignDiff, nbytes - alignDiff);
            }
            temp_v0 = src - temp_a2;

            osPiReadIo(temp_v0, (u32*)&buf[0]);
            osPiReadIo(temp_v0 + 4, (u32*)&buf[4]);
            if (temp_a2) {
                osPiReadIo(temp_v0 + 8, (u32*)&buf[8]);
            }
            for (i = 0; i < alignDiff && i < nbytes; i++) {
                dst[i] = buf[i + temp_a2];
            }

        } else {
            _uvDMA(dst, src, nbytes);
        }
    }
}

void uvMemSet(void* vAddr, u8 value, u32 nbytes) {
    u32 count;
    u8* dest;

    count = 0;
    if (nbytes != 0) {
        dest = vAddr;
        do {
            count += 1;
            *dest++ = value;
        } while (count < nbytes);
    }
}

u64 uvMemRead(void* vAddr, u32 nbytes) {
    u64 out;
    u64 temp1;
    s64 sp28;
    u8* src;

    src = vAddr;
    if ((nbytes != 1) && (nbytes != 2) && (nbytes != 4) && (nbytes != 8)) {
        out = 0;
    } else if ((s32) vAddr & 0x80000000) {
        out = src[0];
        if (nbytes >= 2U) {
            temp1 = out << 8;
            out = temp1 | src[1];
        }
        if (nbytes >= 3U) {
            temp1 = out << 16;
            out = temp1 | (src[2] << 8) | src[3];
        }
        if (nbytes >= 5U) {
            temp1 = out << 32;
            out = temp1 | (src[4] << 0x18) | (src[5] << 0x10) | (src[6] << 8) | src[7];
        }
    } else {
        _uvMediaCopy(&sp28, vAddr, nbytes);
        out = uvMemRead(&sp28, nbytes);
    }
    return out;
}
