// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "os_internal.h"
#include "stdarg.h"

void func_80000FC8(void);          /* extern */
void func_800011A8(void);          /* extern */
void _uvMemAllocInitStartUp(void); /* extern */
void func_80002EAC(s32);           /* extern */
void uvModuleInit(void);          /* extern */
void uvSetVideoMode(void);         /* extern */
void _uvScInitClientList(void);    /* extern */
void Thread_Kernel(void *);        /* extern */
s32 func_80003A14(u32 arg0, s32 *arg1);
void func_800051B4(void *);                 /* extern */
void func_80005284(void *);                 /* extern */
void func_80007190(s32, s32 *, s32 *, s32); /* extern */
void func_80000450(void *);                 /* extern */

extern s32 *gModuleExportsSize;
extern s32 D_8002F7C8; // kernel thread stack
extern s32 gCartDMATransferFlag;
extern s32 D_8002F8C8;
extern OSMesgQueue D_8002F800;
extern OSThread sKernelThread;
extern OSIoMesg D_8002F7E0;
extern u16 D_8001F800[];
extern u16 D_8001F7E0[];
extern u8 D_8001F820;
extern void *D_8002F818;
extern OSMesgQueue D_8002F828;
extern OSContStatus D_8002FC30;
extern s32 D_8001F630;
extern u8 D_80022BD8[];
extern s32 D_8002F570;
extern OSThread D_8002F5A0;
extern void *D_8002F7F8;
extern OSMesg D_8002F840;
extern OSMesgQueue D_8002F8B0;
extern OSThread D_8002F8D0;
extern s32 D_8001F7D4;
extern s32 D_8001F7D8;
extern OSMesgQueue D_8002F580;
extern void *D_8002F598;

void func_80004CC0(s32 arg0, s32 arg1, s32 arg2, u16 arg3, u16 arg4);

void uvSysInit(void) {
    // init for power up
    if (gCartDMATransferFlag != 0) {
        _uvMemAllocInitStartUp();
        uvSetVideoMode();
        uvClkInit();
        func_80000FC8();
        uvLoadFormFiles();
        uvModuleInit();
        gCartDMATransferFlag = 0;
        if (D_8002F8C8 != 0) {
            *(volatile s32 *) 0 = 0;
        }
        // init during game
    } else {
        _uvScInitClientList();
        func_80002EAC(0);
        uvLoadFormFiles();
        uvModuleInit();
        func_80000FC8();
    }
}

static void unusedFunction() {
}

void uvWaitForMesg(u8 msg) {
    if (!msg) {
        osRecvMesg(&D_8002F800, NULL, 1);
    }
}

void bootproc(void *arg0) {
    u32 devAddr;
    s32 i;
    u32 sp38[16];

    osInitialize();
    osAiSetFrequency(0x5622U);
#ifdef ISPRINT
    ISViewer_Init();
#endif
    devAddr = 0xFFB000;
    for (i = 0; i < 16; i++, devAddr += 4) {
        __osPiRawReadIo(devAddr, &sp38[i]);
    }
    osCreateThread(&sKernelThread, 1, Thread_Kernel, &sp38, &D_8002F7C8, 0xC);
    osStartThread(&sKernelThread);
}

extern u16 D_80200000[];
extern u16 D_801DA800[];

void func_80004CC0(s32 arg0, s32 arg1, s32 arg2, u16 arg3, u16 arg4) {
    u16 var_s6;
    s16* var_fp;
    s32 temp_t8;
    s32 var_a1;
    s32 i; // row
    s32 j; // column
    s32 l; // out "scaled pixel" row
    s32 k; // out "scaled pixel" column
    s32 var_t1;
    s16* var_ra;
    s32 temp;
    s32 temp2;

    var_t1 = 0;
    for (i = 0; i < 7; i++) {
        for (j = 0; j < 5; j++) {
            if ((j == 0) || (i == 0) || (j == 4) || (i == 6)) {
                // Border color
                var_s6 = arg4;
            } else {
                if ((arg2 & (1 << (15 - var_t1)))) {
                    var_s6 = arg3;
                } else {
                    var_s6 = arg4;
                }
                var_t1++;
            }

            // Framebuffers have to use raw values otherwise addiu instructions are used
            var_fp = 0x80200000;
            var_ra = 0x801DA800;

            // k and l represent row/columns of scaled out "pixels", i and j are multiplied by this scale
            for (k = 0; k < 2; k++) {
                for (l = 0; l < 2; l++) {
                    s32 index = (k + (j * 2 + arg0)) + ((l + (i * 2 + arg1)) * 320);
                    var_fp[index] = var_s6;
                    var_ra[index] = var_s6;
                }
            }
        }
    }
}

void func_80004E18(s32 arg0, s32 arg1, s32 arg2, u16 arg3, u16 arg4) {
    func_80004CC0(arg0, arg1, D_8001F7E0[arg2], arg3, arg4);
}

void func_80004E5C(s32 arg0, s32 arg1) {
    s16 var_s0;
    s16 i;

    var_s0 = arg0;
    for (i = 0; i < 13; i++) {
        var_s0 += 0xA;
        if (D_8001F800[i] == 0) {
            var_s0 = arg0;
            arg1 += 0x10;
        } else {
            func_80004CC0(var_s0, (s16) arg1, D_8001F800[i], 0xFFFFU, 1U);
        }
    }
}

void func_80004F20(u32 arg0, s32 arg1, s32 arg2) {
    s16 i;

    for (i = 0; i < 8; i++) {
        func_80004E18((s16) ((i * 0xA) + arg1), (s16) arg2, (arg0 >> (0x1C - (i * 4))) & 0xF,
                      0xFFFFU, 1U);
    }
}

void func_80004FD8(s32 arg0, s32 arg1) {
    u32 temp_v0;
    s32 sp20;

    temp_v0 = func_80003A14(arg0, &sp20);
    if (temp_v0 != 0) {
        int temp;
        func_80004F20(temp_v0, 0x14, arg1);
        arg0 -= (s32) uvGetLoadedModule(temp_v0);
        arg0 -= gModuleExportsSize[sp20];
    } else {
        arg0 += 0x7FFFFBB0;
    }
    func_80004F20(arg0, 0x78, arg1);
    osWritebackDCacheAll();
}

#ifdef NEEDS_DATA
extern s32 D_8001F81C;
extern s32 D_8002F750;

UNUSED void func_80005074(s32 arg0, u8 *arg1, s32 arg2) {
    s32 temp_a0;
    s32 temp_v0;
    s32 *var_a1;
    u8 *var_s0;
    s32 var_a3;
    static s32 D_8001F81C = 0;
    s32 temp;

    if (D_8001F81C >= 0xD) {
        return;
    }
    var_a3 = 0;
    func_80004FD8(arg0, arg2);

    for (var_a1 = arg0; (u32) var_a1 < 0x80400000; var_a1++) {
        temp_a0 = (*var_a1 & 0xFFFF0000);
        if (temp_a0 == 0x8FBF0000) {
            var_a3 = *(u32 *) ((s16) (*var_a1 & 0xFFFF) + arg1);
        }
        if (temp_a0 == 0x27BD0000) {
            if ((s16) (*var_a1 & 0xFFFF) < 0) {
                break;
            }
            arg1 += (s16) (*var_a1 & 0xFFFF);
        }
        if (temp_a0 == 0xAFBF0000) {
            break;
        }

        if ((*var_a1 == (temp = 0x03E00008)) && (D_8001F81C == 0) && (!var_a3)) {
            var_a3 = D_8002F750;
        }

        if ((*var_a1 == (temp = 0x42000018))) {
            break;
        }
    }
    if (var_a3 != 0) {
        D_8001F81C++;
        func_80005074(var_a3 - 4, arg1, arg2 + 0x10);
        D_8001F81C--;
    }
}
#else
#pragma GLOBAL_ASM("asm/us/nonmatchings/main/func_80005074.s")
#endif

void Thread_Fault(void *arg0) {
    s32 pad;
    OSMesg mesg = NULL;
    OSThread *thread;

    osCreateMesgQueue(&D_8002F580, &D_8002F598, 1);
    osSetEventMesg(OS_EVENT_FAULT, &D_8002F580, (void *) 0x10);
    while (TRUE) {
        osRecvMesg(&D_8002F580, &mesg, OS_MESG_BLOCK);
        thread = __osGetActiveQueue();
        if (thread->state != OS_STATE_STOPPED) {
            osStopThread(thread);
        }
        D_8001F7D8 = thread->id;
        D_8001F7D4 = 1;
    }
}

void Thread_App(void *arg0) {
    func_80000450(arg0);
}

void Thread_Kernel(void *arg0) {
    osCreatePiManager(0x96, &D_8002F8B0, &D_8002F840, 0x1C);
    osCreateMesgQueue(&D_8002F800, &D_8002F7F8, 1);
    osCreateThread(&D_8002F5A0, 0, Thread_Fault, NULL, &D_8002F570, 0xFA);
    osStartThread(&D_8002F5A0);
    osCreateThread(&D_8002F8D0, 6, Thread_App, arg0, &D_80022BD8[D_8001F630 - 16], 0xA);
    osStartThread(&D_8002F8D0);
    osSetThreadPri(NULL, 0);
    while (TRUE)
        ;
}

void _uvDebugPrintf(char *fmt, ...) {
#ifdef ISPRINT
    va_list arglist;
    va_start(arglist, fmt);
    osSyncPrintf(fmt, arglist);
    va_end(arglist);
#endif
}

void _uvDMA(void *vAddr, u32 devAddr, u32 nbytes) {
    if ((!((s32) vAddr % 8)) && (!((s32) devAddr % 2)) && (nbytes < 0x400001U)) {
        if (nbytes & 1) {
            nbytes = (nbytes + 1) & ~1;
        }
        osWritebackDCache(vAddr, (s32) nbytes);
        osPiStartDma(&D_8002F7E0, 0, 0, devAddr, vAddr, nbytes, &D_8002F800);
        osInvalDCache(vAddr, (s32) nbytes);
        uvWaitForMesg(0U);
    }
}

u8 uvContMesgInit(OSMesgQueue **arg0, OSContStatus **arg1) {
    s32 pad;
    void *sp68;
    OSMesgQueue sp50;
    OSTimer sp30;
    OSMesg mesg;

    *arg1 = &D_8002FC30;
    if (D_8001F820 == 0) {
        osCreateMesgQueue(&sp50, &sp68, 2);
        osSetTimer(&sp30, 0x2710, 0, &sp50, &sp68);
        osRecvMesg(&sp50, &mesg, OS_MESG_BLOCK);
        osCreateMesgQueue(&D_8002F828, &D_8002F818, 3);
        osSetEventMesg(OS_EVENT_SI, &D_8002F828, (void *) 0x33333333);
        osContInit(&D_8002F828, &D_8001F820, *arg1);
    }
    *arg0 = &D_8002F828;
    return D_8001F820;
}

void func_80005570(void) {
}
