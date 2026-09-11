// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "libaudio.h"
#include "module.h"

#define AUDIO_DEFAULT_SAMPLE_RATE 0x5622
#define AUDIO_DEFAULT_MAX_VOICES 0x10
#define AUDIO_DEFAULT_DMA_BUF_COUNT 0x10
#define AUDIO_DEFAULT_DMA_BUF_SIZE 0x800
#define AUDIO_DEFAULT_CMD_LIST_COUNT 0x3E8
#define AUDIO_DEFAULT_MAX_UPDATES 0x100

typedef struct {
    ALLink node;
    u32 startAddr;
    u32 lastFrame;
    char *ptr;
} AMDMABuffer;

typedef struct {
    u8 initialized;
    AMDMABuffer *firstUsed;
    AMDMABuffer *firstFree;
} AMDMAState;

typedef union {
    struct {
        short type;
    } gen;
    struct {
        short type;
        struct AudioInfo *info;
    } done;
    OSScMsg app;
} AudioMsg;

typedef struct AudioInfo {
    s16 *data;
    s16 frameSamples;
    OSScTask task;
    AudioMsg msg;
} AudioInfo; // size 0x80

typedef struct AMAudioMgr {
    Acmd *ACMDList[2];
    AudioInfo *audioInfo[3];
    OSThread thread;
    OSMesgQueue audioFrameMsgQ;
    OSMesg audioFrameMsgBuf[8];
    OSMesgQueue audioReplyMsgQ;
    OSMesg audioReplyMsgBuf[8];
    ALGlobals g;
} AMAudioMgr;

typedef struct AudioMgr_Settings_s {
    /* 0x00 */ u16 dmaBufCount;   /* inferred */
    /* 0x02 */ u16 dmaBufSize;    /* inferred */
    /* 0x04 */ u16 aCmdListCount; /* inferred */
    /* 0x06 */ char pad6[2];
    /* 0x08 */ s32 unk8;       /* inferred */
    /* 0x0C */ void *unkC;     /* inferred */
    /* 0x10 */ u16 maxVoices;  /* inferred */
    /* 0x12 */ u16 maxUpdates; /* inferred */
    /* 0x14 */ u16 unk14;      /* inferred */
    /* 0x16 */ char pad16[2];
    /* 0x18 */ u32 sampleRate; /* inferred */
    /* 0x1C */ u16 unk1C;      /* inferred */
    /* 0x1E */ u16 unk1E;      /* inferred */
    /* 0x20 */ char pad20[2];
} AudioMgr_Settings; /* size = 0x22 */

typedef struct UnkStruct_uvaudiomgr_rom_0040130C_s {
    char pad[0x8];
    s32 unk8[1];
} UnkStruct_uvaudiomgr_rom_0040130C;

extern s32 D_uvaudiomgr_rom_00401304;
extern s32 D_uvaudiomgr_rom_00405748;
extern s32 D_uvaudiomgr_rom_0040571C;
extern AMDMABuffer *D_uvaudiomgr_rom_004056BC;
extern AMAudioMgr sAudioMgrInfo;
extern UnkStruct_uvaudiomgr_rom_0040130C *D_uvaudiomgr_rom_0040130C;
extern s32 D_uvaudiomgr_rom_00401310;
extern s32 D_uvaudiomgr_rom_00405418;
extern OSMesgQueue D_uvaudiomgr_rom_004055F0;
extern void *D_uvaudiomgr_rom_00405608;
extern OSMesgQueue D_uvaudiomgr_rom_00405628;
extern void *D_uvaudiomgr_rom_00405640;
extern ALGlobals D_uvaudiomgr_rom_00405660;
extern s32 D_uvaudiomgr_rom_004056CC;
extern OSMesgQueue D_uvaudiomgr_rom_004056D8;
extern void **D_uvaudiomgr_rom_004056F0;
extern OSMesgQueue D_uvaudiomgr_rom_004056F8;
extern void *D_uvaudiomgr_rom_00405710;
extern u16 sAudioDmaBufCount;
extern u16 D_uvaudiomgr_rom_00405716;
extern u16 sAudioDmaBufSize;
extern u16 sAudioCmdListCount;
extern u16 sAudioMaxVoices;
extern u16 sAudioMaxUpdates;
extern u32 sAudioSampleRate;
extern UvCback_Exports *sUvCbackExports;
extern OSIoMesg *D_uvaudiomgr_rom_004056D0;
extern void *D_uvaudiomgr_rom_00405720;
extern u16 D_uvaudiomgr_rom_0040577C;
extern s32 D_uvaudiomgr_rom_00405780;
extern void *D_uvaudiomgr_rom_00405784;
extern void *D_uvaudiomgr_rom_00405788;
extern s32 D_uvaudiomgr_rom_004056C4;
extern s32 D_uvaudiomgr_rom_004056C8;
extern OSScClient sSchedulerClient;
extern s32 D_uvaudiomgr_rom_00405430[];
extern u32 sAudioFrameCount;
extern OSSched *gScheduler;
extern s32 D_uvaudiomgr_rom_00401300;
extern AMDMAState sAudioDmaState;
extern u16 D_uvaudiomgr_rom_00405728; // unused
extern u16 D_uvaudiomgr_rom_00405730;
extern u16 D_uvaudiomgr_rom_00405732;
extern f32 D_uvaudiomgr_rom_00405734;
extern f32 D_uvaudiomgr_rom_00405738;
extern s32 D_uvaudiomgr_rom_0040573C;
extern ALSynConfig D_uvaudiomgr_rom_00405758;
extern f32 D_uvaudiomgr_rom_00405740;

s32 func_uvaudiomgr_rom_00400000(void);
void uvCreateAudioMgr(ALSynConfig *arg0, s32 pri);
void func_uvaudiomgr_rom_004008C0(void);
void func_uvaudiomgr_rom_00400980(void);
void uvAudioMgrThreadFunc(void *arg0);
s32 uvAudioMgrHandleFrameMesg(AudioInfo *info, AudioInfo *previousInfo);
void uvAudioMgrHandleDoneMesg(s32 arg0);
s32 uvAudioMgrDma(s32 addr, s32 len, UNUSED void *state);
s32 (*uvAudioMgrDmaNew(AMDMAState **state))(s32, s32, void *);
void uvAudioMgrClearDma(void);
void func_uvaudiomgr_rom_00401160(void);
void func_uvaudiomgr_rom_00401168(void);
void *func_uvaudiomgr_rom_00401170(s32 arg0);
void func_uvaudiomgr_rom_004011A8(f32 arg0);
void func_uvaudiomgr_rom_004011B4(f32 *arg0);
void func_uvaudiomgr_rom_004011C4(f32 arg0);
void func_uvaudiomgr_rom_004011D0(f32 *arg0);
void func_uvaudiomgr_rom_004011E0(s32 arg0);
void func_uvaudiomgr_rom_004011EC(s32 *arg0);
void func_uvaudiomgr_rom_004011FC(f32 arg0);
void func_uvaudiomgr_rom_00401208(f32 *arg0);
s32 func_uvaudiomgr_rom_00401218(void);
void func_uvaudiomgr_rom_00401228(s32 arg0, s32 arg1, s32 arg2);
s32 func_uvaudiomgr_rom_0040129C(s32 arg0, s32 arg1);
void func_uvaudiomgr_rom_004012E4(void);
void __entrypoint_func_uvaudiomgr_rom_40000c(UvAudioMgr_Exports *exports);

s32 func_uvaudiomgr_rom_00400000(void) {
    return D_uvaudiomgr_rom_0040571C;
}

void __entrypoint_func_uvaudiomgr_rom_40000c(UvAudioMgr_Exports *exports) {
    AudioMgr_Settings *settings;
    s32 var_a0;

    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvaudiomgr_rom_004008C0 = func_uvaudiomgr_rom_004008C0;
    exports->func_uvaudiomgr_rom_004011EC = func_uvaudiomgr_rom_004011EC;
    exports->func_uvaudiomgr_rom_00400000 = func_uvaudiomgr_rom_00400000;
    exports->func_uvaudiomgr_rom_004011FC = func_uvaudiomgr_rom_004011FC;
    exports->func_uvaudiomgr_rom_00401160 = func_uvaudiomgr_rom_00401160;
    exports->func_uvaudiomgr_rom_00401208 = func_uvaudiomgr_rom_00401208;
    exports->func_uvaudiomgr_rom_00401168 = func_uvaudiomgr_rom_00401168;
    exports->func_uvaudiomgr_rom_00401218 = func_uvaudiomgr_rom_00401218;
    exports->func_uvaudiomgr_rom_00401170 = func_uvaudiomgr_rom_00401170;
    exports->func_uvaudiomgr_rom_00401228 = func_uvaudiomgr_rom_00401228;
    exports->func_uvaudiomgr_rom_004011A8 = func_uvaudiomgr_rom_004011A8;
    exports->func_uvaudiomgr_rom_0040129C = func_uvaudiomgr_rom_0040129C;
    exports->func_uvaudiomgr_rom_004011B4 = func_uvaudiomgr_rom_004011B4;
    exports->func_uvaudiomgr_rom_004012E4 = func_uvaudiomgr_rom_004012E4;
    exports->func_uvaudiomgr_rom_004011C4 = func_uvaudiomgr_rom_004011C4;
    exports->func_uvaudiomgr_rom_004011D0 = func_uvaudiomgr_rom_004011D0;
    exports->func_uvaudiomgr_rom_004011E0 = func_uvaudiomgr_rom_004011E0;
    D_uvaudiomgr_rom_00405734 = 1.0f;
    D_uvaudiomgr_rom_00405738 = 1.0f;
    D_uvaudiomgr_rom_0040573C = 2;
    D_uvaudiomgr_rom_00405740 = 0.0f;
    settings = uvGetSystemProp(SYSTEM_PROPID_AUDIOMGR_SETTINGS);
    if (settings == NULL) {
        sAudioDmaBufCount = AUDIO_DEFAULT_DMA_BUF_COUNT;
        sAudioDmaBufSize = 0x800;
        sAudioCmdListCount = 0x3E8;
        sAudioMaxUpdates = 0x100;
        D_uvaudiomgr_rom_00405728 = 0x100;
        sAudioMaxVoices = AUDIO_DEFAULT_MAX_VOICES;
        sAudioSampleRate = AUDIO_DEFAULT_SAMPLE_RATE;
        D_uvaudiomgr_rom_00405730 = 0xA;
        D_uvaudiomgr_rom_00405732 = 0xA;
        D_uvaudiomgr_rom_0040571C = 0;
        D_uvaudiomgr_rom_00405720 = NULL;
    } else {
        if (settings->dmaBufCount != 0) {
            sAudioDmaBufCount = settings->dmaBufCount;
        } else {
            sAudioDmaBufCount = AUDIO_DEFAULT_DMA_BUF_COUNT;
        }
        if (settings->dmaBufSize != 0) {
            sAudioDmaBufSize = settings->dmaBufSize;
        } else {
            sAudioDmaBufSize = AUDIO_DEFAULT_DMA_BUF_SIZE;
        }
        if (settings->aCmdListCount != 0) {
            sAudioCmdListCount = settings->aCmdListCount;
        } else {
            sAudioCmdListCount = AUDIO_DEFAULT_CMD_LIST_COUNT;
        }
        if (settings->maxUpdates != 0) {
            sAudioMaxUpdates = settings->maxUpdates;
        } else {
            sAudioMaxUpdates = AUDIO_DEFAULT_MAX_UPDATES;
        }
        if (settings->unk14 != 0) {
            D_uvaudiomgr_rom_00405728 = settings->unk14;
        } else {
            D_uvaudiomgr_rom_00405728 = 0x100;
        }
        if (settings->maxVoices != 0) {
            sAudioMaxVoices = settings->maxVoices;
        } else {
            sAudioMaxVoices = AUDIO_DEFAULT_MAX_VOICES;
        }
        if (settings->sampleRate != 0) {
            sAudioSampleRate = settings->sampleRate;
        } else {
            sAudioSampleRate = AUDIO_DEFAULT_SAMPLE_RATE;
        }
        if (settings->unk1C != 0) {
            D_uvaudiomgr_rom_00405730 = settings->unk1C;
        } else {
            D_uvaudiomgr_rom_00405730 = 0xA;
        }
        if (settings->unk1E != 0) {
            D_uvaudiomgr_rom_00405732 = settings->unk1E;
        } else {
            D_uvaudiomgr_rom_00405732 = 0xA;
        }
        D_uvaudiomgr_rom_0040571C = settings->unk8;
        D_uvaudiomgr_rom_00405720 = settings->unkC;
    }
    func_uvaudiomgr_rom_00400980();
    if (D_uvaudiomgr_rom_00405720 == NULL) {
        var_a0 = D_uvaudiomgr_rom_0040571C;
        if (var_a0 == 0) {
            var_a0 = D_uvaudiomgr_rom_0040571C =
                ((sAudioDmaBufCount) * (sAudioDmaBufSize)) + ((sAudioCmdListCount & 0xFFFF) * 0x10)
                + (D_uvaudiomgr_rom_004056CC * 0xC) + (sAudioMaxUpdates * 0x1C)
                + (sAudioMaxVoices * 0x214) + 0x322C;
        }
        D_uvaudiomgr_rom_0040577C |= 1;
        D_uvaudiomgr_rom_00405720 = _uvMemAllocAlign8((u32) var_a0);
    }

    D_uvaudiomgr_rom_00405716 = (u16) (s32) (sAudioDmaBufCount * 1.5);
    D_uvaudiomgr_rom_004056BC = _uvMemAllocAlign8(sAudioDmaBufCount * 0x14);
    uvMemSet(D_uvaudiomgr_rom_004056BC, 0U, sAudioDmaBufCount * 0x14);
    D_uvaudiomgr_rom_004056D0 = _uvMemAllocAlign8(D_uvaudiomgr_rom_00405716 * 0x18);
    uvMemSet(D_uvaudiomgr_rom_004056D0, 0U, D_uvaudiomgr_rom_00405716 * 0x18);
    D_uvaudiomgr_rom_004056F0 = _uvMemAllocAlign8(D_uvaudiomgr_rom_00405716 * 4);
    uvMemSet(D_uvaudiomgr_rom_004056F0, 0U, D_uvaudiomgr_rom_00405716 * 4);
    uvMemSet(D_uvaudiomgr_rom_00405720, 0U, (u32) D_uvaudiomgr_rom_0040571C);
    alHeapInit((ALHeap *) &D_uvaudiomgr_rom_00405748, (u8 *) D_uvaudiomgr_rom_00405720,
               D_uvaudiomgr_rom_0040571C);
    D_uvaudiomgr_rom_00405758.heap = (ALHeap *) &D_uvaudiomgr_rom_00405748;
    uvCreateAudioMgr(&D_uvaudiomgr_rom_00405758, 0x6E);
    osSendMesg(&D_uvaudiomgr_rom_004056F8, NULL, 1);
    if (((s32) D_uvaudiomgr_rom_00405730 > 0) || ((s32) D_uvaudiomgr_rom_00405732 > 0)) {
        sUvCbackExports = uvLoadModule('CBCK');
        D_uvaudiomgr_rom_00405784 =
            sUvCbackExports->func_uvcback_rom_00400080((s32) D_uvaudiomgr_rom_00405730);
        D_uvaudiomgr_rom_00405788 =
            sUvCbackExports->func_uvcback_rom_00400080((s32) D_uvaudiomgr_rom_00405732);
    }
}

void uvCreateAudioMgr(ALSynConfig *arg0, s32 pri) {
    s32 i;
    s32 j;
    AMDMABuffer *temp_a1;

    arg0->maxVVoices = sAudioMaxVoices;
    arg0->maxPVoices = sAudioMaxVoices;
    arg0->maxUpdates = sAudioMaxUpdates;
    arg0->dmaproc = NULL;
    arg0->maxFXbusses = 8;
    arg0->fxType = 6;
    arg0->outputRate = 0;

    // @redundant check
    if (arg0->fxType == 6) {
        arg0->params = &D_uvaudiomgr_rom_00401310;
    }

    arg0->dmaproc = &uvAudioMgrDmaNew;
    arg0->outputRate = osAiSetFrequency(sAudioSampleRate);
    alInit(&D_uvaudiomgr_rom_00405660, arg0);
    D_uvaudiomgr_rom_0040130C = alSynGetFXRef(&D_uvaudiomgr_rom_00405660.drvr, 0, 0);
    D_uvaudiomgr_rom_004056BC->node.prev = NULL;
    D_uvaudiomgr_rom_004056BC->node.next = NULL;

    for (i = 0; i < sAudioDmaBufCount - 1; i++) {
        alLink(&D_uvaudiomgr_rom_004056BC[i + 1].node, &D_uvaudiomgr_rom_004056BC[i].node);
        D_uvaudiomgr_rom_004056BC[i].ptr = alHeapDBAlloc(NULL, 0, arg0->heap, 1, sAudioDmaBufSize);
    }

    D_uvaudiomgr_rom_004056BC[i].ptr = alHeapDBAlloc(NULL, 0, arg0->heap, 1, (s32) sAudioDmaBufSize);

    for (i = 0; i < 2; i++) {
        sAudioMgrInfo.ACMDList[i] =
            alHeapDBAlloc(NULL, 0, arg0->heap, 1, sAudioCmdListCount * 8);
    }

    for (i = 0; i < 3; i++) {
        sAudioMgrInfo.audioInfo[i] = alHeapDBAlloc(NULL, 0, arg0->heap, 1, 0x80);
        sAudioMgrInfo.audioInfo[i]->msg.done.type = 2;
        sAudioMgrInfo.audioInfo[i]->msg.done.info = sAudioMgrInfo.audioInfo[i];
        sAudioMgrInfo.audioInfo[i]->data =
            alHeapDBAlloc(NULL, 0, arg0->heap, 1, D_uvaudiomgr_rom_004056CC * 4);
    }

    osCreateMesgQueue(&D_uvaudiomgr_rom_00405628, &D_uvaudiomgr_rom_00405640, 8);
    osCreateMesgQueue(&D_uvaudiomgr_rom_004055F0, &D_uvaudiomgr_rom_00405608, 8);
    osCreateMesgQueue(&D_uvaudiomgr_rom_004056D8, D_uvaudiomgr_rom_004056F0,
                      (s32) D_uvaudiomgr_rom_00405716);
    osCreateMesgQueue(&D_uvaudiomgr_rom_004056F8, &D_uvaudiomgr_rom_00405710, 1);
    osCreateThread(&sAudioMgrInfo.thread, 3, uvAudioMgrThreadFunc, NULL, &D_uvaudiomgr_rom_00405418, pri);
    osStartThread(&sAudioMgrInfo.thread);
}

void func_uvaudiomgr_rom_004008C0(void) {
    osDestroyThread(&sAudioMgrInfo.thread);
    alClose(&D_uvaudiomgr_rom_00405660);
    if (D_uvaudiomgr_rom_0040577C & 1) {
        _uvMemFree(D_uvaudiomgr_rom_00405720);
    }
    _uvMemFree(D_uvaudiomgr_rom_004056BC);
    _uvMemFree(D_uvaudiomgr_rom_004056D0);
    _uvMemFree(D_uvaudiomgr_rom_004056F0);
    sUvCbackExports->func_uvcback_rom_004000D0(D_uvaudiomgr_rom_00405784);
    sUvCbackExports->func_uvcback_rom_004000D0(D_uvaudiomgr_rom_00405788);
    uvUnloadModule(0x4342434B);
    D_uvaudiomgr_rom_00405780 = 1;
}

void func_uvaudiomgr_rom_00400980(void) {
    s32 temp_v0;
    f32 var_fv0;
    s32 temp_v1;

    temp_v0 = osAiSetFrequency(sAudioSampleRate);
    switch (osTvType) { /* irregular */
        case OS_TV_PAL:
            var_fv0 = ((f32) temp_v0 * 1) / 50.0f;
            break;
        case OS_TV_NTSC:
        case OS_TV_MPAL:
            var_fv0 = ((f32) temp_v0 * 1) / 60.0f;
            break;
        default:
            break;
    }

    D_uvaudiomgr_rom_004056C8 = var_fv0;

    if (D_uvaudiomgr_rom_004056C8 < var_fv0) {
        D_uvaudiomgr_rom_004056C8++;
    }
    if (D_uvaudiomgr_rom_004056C8 & 0xF) {
        D_uvaudiomgr_rom_004056C8 = (D_uvaudiomgr_rom_004056C8 & ~0xF) + 0x10;
    }

    D_uvaudiomgr_rom_004056C4 = D_uvaudiomgr_rom_004056C8 - 0x10;
    D_uvaudiomgr_rom_004056CC = D_uvaudiomgr_rom_004056C8 + 0x110;
}

void uvAudioMgrThreadFunc(void *arg0) {
    s32 temp_s0;
    s32 var_s1;
    AudioMsg *msg;

    s32 done;
    done = FALSE;
    msg = NULL;
    var_s1 = 0;
    osRecvMesg(&D_uvaudiomgr_rom_004056F8, (OSMesg*)&msg, 1);

    _uvScAddClient(gScheduler, &sSchedulerClient, &D_uvaudiomgr_rom_004055F0);

    while (!done) {
        if (var_s1) { }
        osRecvMesg(&D_uvaudiomgr_rom_004055F0, (OSMesg*)&msg, 1);
        if (D_uvaudiomgr_rom_00405780 == 0) {
            func_80004958(0U, 0x29);
            switch (msg->gen.type) { /* irregular */
                case 4:
                    break;
                case 1:
                    temp_s0 = uvAudioMgrHandleFrameMesg(
                        D_uvaudiomgr_rom_00405430[(u32) sAudioFrameCount % 3U], var_s1);
                    if (D_uvaudiomgr_rom_00405784 != NULL) {
                        sUvCbackExports->func_uvcback_rom_004000F0(
                            (UvCback_Rom_004000F0 *) D_uvaudiomgr_rom_00405784, 0);
                    }
                    if (temp_s0 != 0) {
                        osRecvMesg(&D_uvaudiomgr_rom_00405628, (OSMesg*)&msg, 1);
                        uvAudioMgrHandleDoneMesg(msg->done.info);
                        var_s1 = msg->done.info;
                    }
                    if (D_uvaudiomgr_rom_00405788 != NULL) {
                        sUvCbackExports->func_uvcback_rom_004000F0(
                            (UvCback_Rom_004000F0 *) D_uvaudiomgr_rom_00405788, 0);
                    }
                    break;
                case 10:
                    done = TRUE;
                    break;
            }
        }
    }

    alClose(&D_uvaudiomgr_rom_00405660);
}

s32 uvAudioMgrHandleFrameMesg(AudioInfo *info, AudioInfo *previousInfo) {
    u32 audioPtr;
    Acmd *sp38;
    s32 cmdLen;
    s32 pad;

    func_8000110C(1);
    uvAudioMgrClearDma();
    audioPtr = osVirtualToPhysical(info->data);
    if (previousInfo != NULL) {
        osAiSetNextBuffer(previousInfo->data, previousInfo->frameSamples * 4);
    }
    info->frameSamples = ((D_uvaudiomgr_rom_004056C8 - (func_8000E460() >> 2)) + 0x110) & 0xFFF0;
    if (info->frameSamples < D_uvaudiomgr_rom_004056C4) {
        info->frameSamples = (s16) D_uvaudiomgr_rom_004056C4;
    }
    if (D_uvaudiomgr_rom_004056CC < info->frameSamples) {
        info->frameSamples = (s16) D_uvaudiomgr_rom_004056CC;
    }
    if (audioPtr == 0) {
        *(s32 *) 0 = 0; // fault
    }

    func_8000D7F0(&D_uvaudiomgr_rom_00405748);
    sp38 = alAudioFrame(sAudioMgrInfo.ACMDList[D_uvaudiomgr_rom_00401304], &cmdLen,
                        (s16 *) audioPtr, (s32) info->frameSamples);
    func_8000D7F0(&D_uvaudiomgr_rom_00405748);
    if (cmdLen == 0) {
        return 0;
    }
    info->task.next = NULL;
    info->task.msgQ = &D_uvaudiomgr_rom_00405628;
    info->task.msg = &info->msg;
    info->task.flags = 1;
    info->task.list.t.data_ptr = (u64 *) sAudioMgrInfo.ACMDList[D_uvaudiomgr_rom_00401304];
    info->task.list.t.data_size =
        (sp38 - sAudioMgrInfo.ACMDList[D_uvaudiomgr_rom_00401304]) * sizeof(Acmd);
    info->task.list.t.type = 2;
    info->task.list.t.ucode_boot = (u64 *) rspBootTextStart;
    info->task.list.t.ucode_boot_size =
        (u64 *) ((u32) gspF3DEX2_fifoTextStart - (u32) rspBootTextStart);
    info->task.list.t.flags = 0;
    info->task.list.t.ucode = (u64 *) aspMainTextStart;
    info->task.list.t.ucode_data = (u64 *) aspMainDataStart;
    info->task.list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    info->task.list.t.yield_data_ptr = NULL;
    info->task.list.t.yield_data_size = 0;
    info->task.list.t.dram_stack_size = 0;
    info->task.list.t.dram_stack = NULL;
    func_80004958(0U, 0x2C);
    func_8000110C(0);
    osSendMesg(_uvScGetCmdQ(gScheduler), &info->task, 1);
    D_uvaudiomgr_rom_00401304 ^= 1;
    return 1;
}

// stubbed
void uvAudioMgrHandleDoneMesg(s32 arg0) {
}

s32 uvAudioMgrDma(s32 addr, s32 len, UNUSED void *state) {
    void *foundBuffer;
    AMDMABuffer *dmaPtr;
    AMDMABuffer *lastDmaPtr;
    s32 delta;
    s32 addrEnd;
    s32 buffEnd;

    dmaPtr = sAudioDmaState.firstUsed;
    lastDmaPtr = NULL;

    addrEnd = addr + len;

    while (dmaPtr) {
        buffEnd = dmaPtr->startAddr + sAudioDmaBufSize;
        if ((u32) addr < dmaPtr->startAddr) {
            break;
        }

        if (buffEnd >= addrEnd) {
            dmaPtr->lastFrame = sAudioFrameCount;
            foundBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return osVirtualToPhysical(foundBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (AMDMABuffer *) dmaPtr->node.next;
    }

    dmaPtr = sAudioDmaState.firstFree;

    sAudioDmaState.firstFree = (AMDMABuffer *) dmaPtr->node.next;
    alUnlink(&dmaPtr->node);
    if (lastDmaPtr != NULL) {
        alLink(&dmaPtr->node, &lastDmaPtr->node);
    } else if (sAudioDmaState.firstUsed != NULL) {
        lastDmaPtr = sAudioDmaState.firstUsed;
        sAudioDmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = &lastDmaPtr->node;
        dmaPtr->node.prev = NULL;
        lastDmaPtr->node.prev = &dmaPtr->node;
    } else {
        sAudioDmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = NULL;
        dmaPtr->node.prev = NULL;
    }

    foundBuffer = dmaPtr->ptr;
    delta = addr & 1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = sAudioFrameCount;
    osPiStartDma((OSIoMesg *) &D_uvaudiomgr_rom_004056D0[D_uvaudiomgr_rom_00401300++], OS_MESG_PRI_HIGH,
                 OS_READ, addr, foundBuffer, sAudioDmaBufSize, &D_uvaudiomgr_rom_004056D8);
    return osVirtualToPhysical(foundBuffer) + delta;
}

s32 (*uvAudioMgrDmaNew(AMDMAState **state))(s32, s32, void *) {
    if (sAudioDmaState.initialized == 0) {
        sAudioDmaState.firstUsed = NULL;
        sAudioDmaState.firstFree = D_uvaudiomgr_rom_004056BC;
        sAudioDmaState.initialized = 1;
    }
    *state = &sAudioDmaState;
    return uvAudioMgrDma;
}

void uvAudioMgrClearDma(void) {
    AMDMABuffer *dmaPtr;
    void *ioMesg;
    AMDMABuffer *nextPtr;
    s32 i;

    ioMesg = NULL;
    for (i = 0; i < D_uvaudiomgr_rom_00401300; i++) {
        osRecvMesg(&D_uvaudiomgr_rom_004056D8, &ioMesg, 0);
    }
    dmaPtr = sAudioDmaState.firstUsed;
    while (dmaPtr != NULL) {
        nextPtr = dmaPtr->node.next;
        if (dmaPtr->lastFrame + 2 < sAudioFrameCount) {
            if (dmaPtr == sAudioDmaState.firstUsed) {
                sAudioDmaState.firstUsed = (AMDMABuffer *) dmaPtr->node.next;
            }
            alUnlink(&dmaPtr->node);
            if (sAudioDmaState.firstFree != NULL) {
                alLink(&dmaPtr->node, &sAudioDmaState.firstFree->node);
            } else {
                sAudioDmaState.firstFree = dmaPtr;
                dmaPtr->node.next = NULL;
                dmaPtr->node.prev = NULL;
            }
        }
        dmaPtr = (AMDMABuffer *) nextPtr;
    }
    D_uvaudiomgr_rom_00401300 = 0;
    sAudioFrameCount += 1;
}

void func_uvaudiomgr_rom_00401160(void) {
}

void func_uvaudiomgr_rom_00401168(void) {
}

void *func_uvaudiomgr_rom_00401170(s32 arg0) {
    switch (arg0) { /* irregular */
        case 1:
            return D_uvaudiomgr_rom_00405784;
        case 2:
            return D_uvaudiomgr_rom_00405788;
        default:
            return NULL;
    }
}

void func_uvaudiomgr_rom_004011A8(f32 arg0) {
    D_uvaudiomgr_rom_00405734 = arg0;
}

void func_uvaudiomgr_rom_004011B4(f32 *arg0) {
    *arg0 = D_uvaudiomgr_rom_00405734;
}

void func_uvaudiomgr_rom_004011C4(f32 arg0) {
    D_uvaudiomgr_rom_00405738 = arg0;
}

void func_uvaudiomgr_rom_004011D0(f32 *arg0) {
    *arg0 = D_uvaudiomgr_rom_00405738;
}

void func_uvaudiomgr_rom_004011E0(s32 arg0) {
    D_uvaudiomgr_rom_0040573C = arg0;
}

void func_uvaudiomgr_rom_004011EC(s32 *arg0) {
    *arg0 = D_uvaudiomgr_rom_0040573C;
}

void func_uvaudiomgr_rom_004011FC(f32 arg0) {
    D_uvaudiomgr_rom_00405740 = arg0;
}

void func_uvaudiomgr_rom_00401208(f32 *arg0) {
    *arg0 = D_uvaudiomgr_rom_00405740;
}

s32 func_uvaudiomgr_rom_00401218(void) {
    s32 *v1 = (s32 *) D_uvaudiomgr_rom_0040130C;
    return *v1;
}

void func_uvaudiomgr_rom_00401228(s32 arg0, s32 arg1, s32 arg2) {
    if ((arg0 >= 0) && (arg0 < 8) && (arg1 >= 0) && (arg1 < 8)) {
        alSynSetFXParam(&D_uvaudiomgr_rom_00405660.drvr, D_uvaudiomgr_rom_0040130C,
                        (s16) ((arg0 * 8) + arg1 + 2), &arg2);
    }
}

s32 func_uvaudiomgr_rom_0040129C(s32 arg0, s32 arg1) {
    UnkStruct_uvaudiomgr_rom_0040130C *v1 = D_uvaudiomgr_rom_0040130C;
    if ((arg0 >= 0) && (arg0 < 8) && (arg1 >= 0) && (arg1 < 8)) {
        return ((UnkStruct_uvaudiomgr_rom_0040130C *) ((u8 *) v1 + (arg0 << 4 << 1)))->unk8[arg1];
    }
    return -1;
}

void func_uvaudiomgr_rom_004012E4(void) {
    D_uvaudiomgr_rom_00405780 = 1;
}
