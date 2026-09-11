// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "os.h"
#include <PR/sched.h>

typedef struct UnkStruct_8022B6CC {
    s32 unk0;
    s32 unk4;
} UnkStruct_8022B6CC_t;

typedef struct UnkStruct_8002DAE0_inner_s {
    double clockSec;
    u32 unk8;
    u32 unkC;
} UnkStruct_8002DAE0_inner;

typedef struct UnkStruct_8002DAE0_s {
    UnkStruct_8002DAE0_inner vals[30];
} UnkStruct_8002DAE0;

#define VIDEO_MSG 666
#define RSP_DONE_MSG 667
#define RDP_DONE_MSG 668
#define PRE_NMI_MSG 669

extern OSViMode D_8024B7D0[];
extern s32 D_802B9C58;
extern OSScTask *D_8002EE00[];
extern OSScTask *D_8002EDF8;
extern u8 D_8002F250;
extern u8 gSchedRspStatus;
extern u8 gSchedRdpStatus;
extern u8 D_8002F253;
extern u8 D_8002F254;
extern u8 D_8002F255;
extern u8 D_8002F256;
extern u8 D_8002F257;
extern u8 D_8002F258;
extern s32 D_8002F25C;
extern s32 D_8002F260;
extern s32 D_8002F264;
extern s32 D_8002F268;
extern s32 gNmiAsserted;
extern OSSched *gScheduler;
extern s32 gSchedRingIdx;
extern s32 D_8001F7C0;
extern s32 D_8001F7C4;
extern OSScTask *D_8002EE08;
extern s32 D_8002F26C;
extern OSMesgQueue D_8002EE10;
extern OSMesg D_8002EE28[];
extern s32 gSchedStack[];
extern OSSched D_8002F278[];
extern OSSched *gScheduler;
extern f64 D_8002EDD0[];
extern s32 D_8001F7C0;
extern s32 D_8002EDA0[];
extern s32 D_8002EDB8[];

extern UnkStruct_8002DAE0 D_8002DAE0[];
extern UnkStruct_8002DAE0 D_8002E440[];

void _uvScMain(void *);
void func_800048DC(void);
void _uvScRunGfx(void);
void func_80004958(u8, s32);
void func_800048E4(void);
void _uvScCreateScheduler(OSSched *sc, void *stack, s32 priority, u8 mode, u8 numFields);
void _uvScHandleRetrace(void);
void _uvScHandleRSP(void);
void _uvScHandleRDP(void);
void _uvScHandleNMI(void);

void uvSetVideoMode(void) {
    s32 viMode;
    OSSched *temp;

    osCreateMesgQueue(&D_8002EE10, D_8002EE28, 0xA);

    switch (osTvType) {
        case OS_TV_PAL:
            viMode = OS_VI_PAL_LAN1;
            break;
        case OS_TV_NTSC:
            viMode = OS_VI_NTSC_LAN1;
            break;
        case OS_TV_MPAL:
            viMode = OS_VI_MPAL_LAN1;
            break;
        default:
            viMode = OS_VI_PAL_LAN1;
            break;
    }

    temp = gScheduler = D_8002F278;
    _uvScCreateScheduler(temp, gSchedStack, 0x7F, viMode, 1);
}

void func_80003C1C(void) {
    OSMesg sp2C;
    int temp;
    while (osRecvMesg(&gScheduler->interruptQ, &sp2C, OS_MESG_NOBLOCK) != -1) {
    }

    temp = D_8002F257 = 0;
    D_8002F258 = temp;
    D_8002EE00[1] = D_8002EDF8 = NULL;
    D_8002EE00[0] = NULL;
    gSchedRspStatus = gSchedRdpStatus = D_8002F254 = D_8002F253 = 0;
}

void _uvScDoneGfx(void) {
    OSScTask *temp_a3;

    temp_a3 = D_8002EE00[D_8002F256];
    if (!gNmiAsserted) {
        if (temp_a3 == NULL) {
            func_800048DC();
            return;
        }
        if ((D_8002F254 == 0) && (gSchedRspStatus != 0x67) && (gSchedRdpStatus == 0)) {
            func_80004958(FALSE, 0x32);
            osSendMesg(temp_a3->msgQ, temp_a3->msg, 1);
            if (D_8001F7C4 < D_8001F7C0) {
                D_8002F250 = (D_8001F7C0 - D_8001F7C4) + 1;
            } else {
                D_8002F250 = 1;
                if (D_8002F26C == 0) {
                    osViSwapBuffer(temp_a3->framebuffer);
                }
            }
            D_8002EE08 = D_8002EE00[D_8002F256];
            D_8002EE00[D_8002F256] = NULL;
        }
    }
}

void _uvScDoneAud(void) {
    if (D_8002EDF8 != NULL) {
        func_80004958(TRUE, 0x2C);
        if (!gNmiAsserted) {
            osSendMesg(D_8002EDF8->msgQ, D_8002EDF8->msg, 1);
        }
        D_8002EDF8 = NULL;
        if (D_8002EE00[D_8002F256] != NULL) {
            _uvScRunGfx();
        }
    }
}

void _uvScRunAud(void) {
    if ((!gNmiAsserted) && (D_8002EDF8 != NULL)) {
        gSchedRspStatus = 0x61;
        func_80004958(TRUE, 0x29);
        osWritebackDCacheAll();
        osSpTaskLoad(&D_8002EDF8->list);
        osSpTaskStartGo(&D_8002EDF8->list);
    }
}

void _uvScRunGfx(void) {
    OSScTask *scTask;

    scTask = D_8002EE00[D_8002F256];
    if (((gNmiAsserted == 0) || (D_8002F254 != 0)) && (scTask != NULL) && (gSchedRspStatus != 'g')
        && ((D_8002F254 != 0) || (gSchedRdpStatus != 'g'))) {
        if (osViGetCurrentFramebuffer() == osViGetNextFramebuffer()) {
            gSchedRspStatus = 'g';
            if (D_8002F254 == 0) {
                D_8002F25C += 1;
                gSchedRdpStatus = 'g';
            }
            func_80004958(TRUE, 0x2A);
            D_8002F254 = D_8002F253 = 0;
            osWritebackDCacheAll();
            osSpTaskLoad(&scTask->list);
            osSpTaskStartGo(&scTask->list);
        }
    }
}

void _uvScDlistRecover(void) {
    // _uvDebugPrintf("Recovered from a bad display list\n");

    IO_WRITE(SP_STATUS_REG, 0x2902);
    if (gSchedRspStatus != 0) {
        osSendMesg(&gScheduler->interruptQ, (void *) RSP_DONE_MSG, 0);
    }
    if (gSchedRdpStatus != 0) {
        osSendMesg(&gScheduler->interruptQ, (void *) RDP_DONE_MSG, 0);
    }
}

void _uvScCreateScheduler(OSSched *sc, void *stack, s32 priority, u8 mode, u8 numFields) {

    gScheduler = sc;
    D_8002EE00[1] = 0;
    D_8002EE00[0] = 0;
    D_8002EDF8 = 0;
    D_8002F250 = 0;
    gSchedRspStatus = 0;
    gSchedRdpStatus = 0;
    D_8002F253 = 0;
    D_8002F254 = 0;
    D_8002F255 = 0;
    D_8002F256 = 0;
    D_8002F257 = 0;
    D_8002F258 = 0;
    D_8002F25C = 0;
    gSchedRingIdx = 0;
    D_8002F260 = 0;
    gNmiAsserted = FALSE;
    D_8002F264 = 1;
    D_8002F268 = 1;

    if ((sc && sc) != 0) {
    } // fakematch

    sc->clientList = 0;
    sc->curRSPTask = 0;
    sc->curRDPTask = 0;
    sc->frameCount = 0;
    sc->audioListHead = 0;
    sc->gfxListHead = 0;
    sc->audioListTail = (OSScTask *) &sc->audioListHead;
    sc->gfxListTail = (OSScTask *) &sc->gfxListHead;
    sc->retraceMsg.type = OS_SC_RETRACE_MSG;
    sc->prenmiMsg.type = OS_SC_PRE_NMI_MSG;

    osCreateMesgQueue(&sc->interruptQ, sc->intBuf, OS_SC_MAX_MESGS);
    osCreateMesgQueue(&sc->cmdQ, sc->cmdMsgBuf, OS_SC_MAX_MESGS);
    osCreateViManager(OS_PRIORITY_VIMGR);
    osViSetMode(&osViModeTable[mode]);
    osViBlack(TRUE);
    osViSwapBuffer((void *) 0x100000);
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON | OS_VI_GAMMA_DITHER_ON);
    osSetEventMesg(OS_EVENT_SP, &sc->interruptQ, (OSMesg) RSP_DONE_MSG);
    osSetEventMesg(OS_EVENT_DP, &sc->interruptQ, (OSMesg) RDP_DONE_MSG);
    osSetEventMesg(OS_EVENT_PRENMI, &sc->interruptQ, (OSMesg) PRE_NMI_MSG);
    osViSetEvent(&sc->interruptQ, (OSMesg) VIDEO_MSG, (u32) numFields);

    osCreateThread(&sc->thread, 4, _uvScMain, (void *) sc, stack, priority);
    osStartThread(&sc->thread);

    uvClkReset(0x69);
    func_800048E4();
}

void _uvScInitClientList(void) {
    gScheduler->clientList = NULL;
}

void _uvScAddClient(OSSched *sc, OSScClient *client, OSMesgQueue *mq) {
    client->msgQ = mq;
    client->next = sc->clientList;
    sc->clientList = (OSScClient *) client;
}

UNUSED void _uvScRemoveClient(OSSched *sc, OSScClient *c) {
    OSScClient *client;
    OSScClient **prevNext;


    for (client = sc->clientList, prevNext = &client; client != NULL; client = client->next) {
        if (client == c) {
            *prevNext = c->next;
            return;
        }
        prevNext = &client->next;
    }
}

s32 _uvScGetCmdQ(OSSched *s) {
    return &s->cmdQ;
}

void _uvScMain(void *arg0) {
    OSMesg msg;
    msg = NULL;

    while (1) {
        osRecvMesg(&gScheduler->interruptQ, &msg, 1);

        switch ((int) msg) {
            case VIDEO_MSG:
                _uvScHandleRetrace();
                break;
            case RSP_DONE_MSG:
                _uvScHandleRSP();
                break;
            case RDP_DONE_MSG:
                _uvScHandleRDP();
                break;
            case PRE_NMI_MSG:
                _uvScHandleNMI();
                break;
            default:
                //_uvDebugPrintf("unknown sched interrupt mesg: 0x%x\n", msg);
                break;
        }
    }
}

void _uvScHandleRetrace(void) {
    OSScTask *sp34;
    OSScClient *var_s0;
    char pad[0x4];

    sp34 = NULL;
    uvClkUpdate();
    if (!gNmiAsserted) {
        D_8002F264 = 1;
        if (gSchedRspStatus != 0) {
            D_8002F258 += 1;
        } else {
            D_8002F258 = 0;
        }
        if (gSchedRdpStatus != 0) {
            D_8002F257 += 1;
        } else {
            D_8002F257 = 0;
        }
        if ((s32) D_8002F258 >= 0x33) {
            func_800048DC();
            _uvScDlistRecover();
            return;
        }
        if ((D_8002F258 + 50) < (s32) D_8002F257) {
            D_8002F257 = 0;
            func_800048DC();
            osSendMesg(&gScheduler->interruptQ, (void *) 0x29C, 0);
            return;
        }
        D_8001F7C4 += 1;
        if (gSchedRspStatus == 'a') {
            return;
        }

        if (D_8002F250) {
            if (D_8002F250 == 2) {
                if (D_8002F26C == 0) {
                    osViSwapBuffer(D_8002EE08->framebuffer);
                }
                D_8002EE08 = NULL;
            }

            if (!(--D_8002F250)) {
                func_800048E4();
                D_8002F256 ^= 1;
                D_8001F7C4 = 0;
            }
        }

        while (osRecvMesg(&gScheduler->cmdQ, (OSMesg)&sp34, 0) != -1) {
            if (sp34 == NULL) {
                break;
            }

            if (sp34->list.t.type == 1) {
                D_8002EE00[D_8002F255] = (OSScTask *) sp34;
                D_8002F255 ^= 1;
            } else if (sp34->list.t.type == 2) {
                D_8002EDF8 = (OSScTask *) sp34;
            }
        }
        if (D_8002EDF8 != NULL) {
            if (gSchedRspStatus == 'g') {
                if (D_8002F253 != 0) {
                    _uvScDlistRecover();
                    return;
                }
                D_8002F253 = 1;
                func_80004958(TRUE, 0x31);
                osSpTaskYield();
            } else {
                _uvScRunAud();
            }
        } else if (D_8002EE00[D_8002F256] != NULL) {
            _uvScRunGfx();
        }
        var_s0 = gScheduler->clientList;
        while (var_s0 != NULL) {
            osSendMesg(var_s0->msgQ, gScheduler, 0);
            var_s0 = var_s0->next;
        }
    }
}

void _uvScHandleRSP(void) {
    if (gSchedRspStatus != 0) {
        D_8002F258 = 0;
        if (gSchedRspStatus == 'a') {
            gSchedRspStatus = 0;
            _uvScDoneAud();
            return;
        }
        gSchedRspStatus = 0;
        if (D_8002F253 != 0) {
            if (osSpTaskYielded(&D_8002EE00[D_8002F256]->list) != 0) {
                D_8002F254 = 1;
                func_80004958(TRUE, 0x2D);
                if (gNmiAsserted) {
                    D_8002EDF8 = NULL;
                    D_8002F253 = 0;
                    _uvScRunGfx();
                    return;
                }
            } else {
                func_80004958(TRUE, 0x2B);
            }
            D_8002F253 = 0;
            if (D_8002EDF8 != NULL) {
                _uvScRunAud();
            }
        } else {
            func_80004958(TRUE, 0x2B);
        }
        if ((gSchedRspStatus != 'g') && (gSchedRdpStatus == 0)) {
            _uvScDoneGfx();
        }
    }
}

void _uvScHandleRDP(void) {
    gSchedRdpStatus = 0;
    D_8002F257 = 0;
    func_80004958(TRUE, 0x30);
    if ((gSchedRspStatus != 'g') && (D_8002F254 == 0)) {
        _uvScDoneGfx();
    }
}

void _uvScHandleNMI(void) {
    UnkStruct_8002D1A4 *temp_v0;

    gNmiAsserted = TRUE;
    osViBlack(TRUE);
    temp_v0 = uvGetLoadedModule('CONT');
    if (temp_v0 != NULL) {
        temp_v0->unk68();
    }
}

void func_800048C8(s32 arg0) {
    D_8002F268 = arg0;
}

void func_800048D4(s32 arg0) {
}

void func_800048DC(void) {
}

void func_800048E4(void) {
    if (++gSchedRingIdx >= 5) {
        gSchedRingIdx = 0;
    }
    D_8002EDD0[gSchedRingIdx] = uvClkGetSec(0x69);
    D_8002EDB8[gSchedRingIdx] = 0;
    D_8002EDA0[gSchedRingIdx] = 0;
}

void func_80004958(u8 arg0, s32 arg1) {
    UnkStruct_8002DAE0_inner *var_v0;
    u32 idx;

    if (D_8002F268) {
        idx = gSchedRingIdx;
        if ((D_8002EDA0[idx] < 0x1E) && (D_8002EDB8[idx] < 0x1E)) {
            if (!arg0) {
                var_v0 = D_8002DAE0[idx].vals;
                var_v0 = &var_v0[D_8002EDA0[idx]++];
            } else {
                var_v0 = D_8002E440[idx].vals;
                var_v0 = &var_v0[D_8002EDB8[idx]++];
            }
            var_v0->clockSec = uvClkGetSec(0x69);
            var_v0->unk8 = arg1;
            var_v0->unkC =
                (gSchedRspStatus << 0x18) | (gSchedRdpStatus << 0x10) | (D_8002F253 << 1) | D_8002F254;
        }
    }
}

void func_80004A78(s32 arg0) {
    D_8001F7C0 = arg0;
}

f64 func_80004A84(void) {
    int idx1;
    int idx2;
    idx1 = (gSchedRingIdx + 1) % 5;
    idx2 = (gSchedRingIdx + 2) % 5;
    return D_8002EDD0[idx2] - D_8002EDD0[idx1];
}

void func_80004B1C(s32 arg0, s32 arg1, s32 arg2) {
}

void func_80004B2C(s32 arg0) {
    D_8002F26C = arg0;
}
