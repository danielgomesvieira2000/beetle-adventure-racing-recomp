// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "stdarg.h"

typedef struct UnkStruct_8002D1B0_s {
    /* 0x00 */ f64 unk0;                            /* inferred */
    /* 0x08 */ f64 unk8;
    /* 0x10 */ f64 unk10;
    /* 0x18 */ f64 unk18;                           /* inferred */
    /* 0x20 */ s32 unk20;                           /* inferred */
    /* 0x24 */ s32 unk24;                           /* inferred */
    /* 0x28 */ u8 unk28;
    /* 0x29 */ u8 unk29;                            /* inferred */
    /* 0x2A */ char pad2A[6];                       /* maybe part of unk29[7]? */
} UnkStruct_8002D1B0;                               /* size = 0x30 */


extern UnkStruct_8002D1B0 D_8002D1B0[];
extern u32 D_8001F770;
extern s32 D_8002D930;
extern s32 gDebugProfilingState;

void func_80001040(s32, s32);                          /* extern */
void func_80000E78(s32);                               /* extern */
void func_80000EC8(s32, ...);

s32 func_80000BE0(char* debugString) {
    UnkStruct_8002D1B0* ptr;
    s32 var_v1;
    s32 i;

    for (i = 0; i < 40; i++) {
        ptr = &D_8002D1B0[i];
        if (ptr->unk29 == 0) {
            ptr->unk29 = 1;
            ptr->unk20 = debugString;
            return i;
        }
    }
    return -1;
}

void func_80000C84(s32 arg0) {
    func_80000E78(arg0);
}

void func_80000CA4(s32 arg0) {
    if (gDebugProfilingState != 0) {
        if (D_8002D1B0[arg0].unk28 == 0) {
            D_8002D1B0[arg0].unk29 = 1;
            // if Profiling is set to 2 ("Non Int"), everything speeds up
            if (gDebugProfilingState == 2) {
                if (D_8001F770 == 1) {
                    D_8001F770 = osSetIntMask(1);
                    D_8002D930 = 0; 
                } else {
                    D_8002D930++;
                }
            }
            D_8002D1B0[arg0].unk8 = uvClkGetSec(0x6B);
        }
    }
}

void func_80000D58(s32 arg0) {
    f64 temp_fa0;
    f64 temp_fv0;
    f64 temp_fv1;

    if (gDebugProfilingState != 0) {
        temp_fa0 = uvClkGetSec(0x6B);
        if (D_8002D930 > 0) {
            D_8002D930 -= 1;
        } else if (D_8001F770 != 1) {
            osSetIntMask(D_8001F770);
            D_8001F770 = 1;
        }
        if (D_8002D1B0[arg0].unk28 == 0) {
            if (D_8002D1B0[arg0].unk8 == -1.0) {
                if (D_8002D1B0[arg0].unk20 != 0) {

                }
            } else {
                temp_fv0 = temp_fa0 - D_8002D1B0[arg0].unk8;
                D_8002D1B0[arg0].unk8 = -1.0;
                D_8002D1B0[arg0].unk0 = temp_fv0;
                D_8002D1B0[arg0].unk24 += 1;
                D_8002D1B0[arg0].unk18 += temp_fv0;
            }
        }
    }
}

void func_80000E50(s32 arg0, s32 arg1) {
    D_8002D1B0[arg0].unk24 += arg1;
}

void func_80000E78(s32 arg0) {
    D_8002D1B0[arg0].unk24 = 0;
    D_8002D1B0[arg0].unk28 = 0;
    D_8002D1B0[arg0].unk29 = 0;
    D_8002D1B0[arg0].unk20 = 0;
    D_8002D1B0[arg0].unk18 = 0.0;
    D_8002D1B0[arg0].unk0 = 0.0;
    D_8002D1B0[arg0].unk10 = 0.0;
    D_8002D1B0[arg0].unk8 = -1.0;
}

void func_80000EC8(s32 arg0, ...) {
    UnkStruct_8002D1B0 *temp_v0;
    f64 *temp_a0_2;
    u32 temp_a0;
    s32* ptr;
    va_list args;

    temp_v0 = &D_8002D1B0[arg0];
    va_start(args, arg0);
    while (TRUE) {
        temp_a0 = va_arg(args, int);
        switch (temp_a0) {
            case 0:
                break;
            case 1:
                temp_a0_2 = va_arg(args, double*);
                if (temp_v0->unk24 != 0) {
                    *temp_a0_2 = temp_v0->unk18 / (f64) temp_v0->unk24;
                } else {
                    *temp_a0_2  = 0.0;
                }
                continue;
            case 2:
                *va_arg(args, double*) = temp_v0->unk18;
                continue;
            case 3:
                *va_arg(args, double*) = temp_v0->unk0;
                continue;
            case 4:
                *va_arg(args, u32*) = temp_v0->unk24;
               continue;
        }
        break;
    }
}

void func_80000FC8(void) {
    s32 i;

    if (D_8001F770 != 1) {
        osSetIntMask(D_8001F770);
        D_8001F770 = 1;
        D_8002D930 = 0;
    }
    for (i = 0; i < 40; i++) {
        func_80000E78(i);
    }
    uvClkReset(0x6B);
}

void func_80001038(s32 arg0) {

}

void func_80001040(s32 arg0, s32 arg1) {    
    f64 temp;
    s32 pad;

    if (D_8002D1B0[arg0].unk8 != -1.0) {        
        if ((arg1 != 0) && (D_8002D1B0[arg0].unk28 == 0)) {            
            temp = uvClkGetSec(0x6B);
            D_8002D1B0[arg0].unk10 = temp;
            
        }
        if ((arg1 == 0) && (D_8002D1B0[arg0].unk28 != 0)) {
            temp = uvClkGetSec(0x6B);
            D_8002D1B0[arg0].unk8 += temp - D_8002D1B0[arg0].unk10;
        }
    }
    D_8002D1B0[arg0].unk28 = arg1;
}

void func_80001104(void) {
}

void func_8000110C(s32 arg0) {
    s32 i;

    for (i = 0; i < 0x28; i++) {
        func_80001040(i, arg0);
    }
}
