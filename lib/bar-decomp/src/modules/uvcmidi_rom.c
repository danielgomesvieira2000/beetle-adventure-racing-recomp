// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"

typedef struct MidiSettings_s {
    /* 0x00 */ s32 unk0;   /* inferred */
    /* 0x04 */ void *unk4; /* inferred */
    /* 0x08 */ u16 unk8;   /* inferred */
    /* 0x0A */ u16 unkA;   /* inferred */
    /* 0x0C */ u16 unkC;   /* inferred */
    /* 0x0E */ char padE[2];
} MidiSettings; /* size = 0x10 */

void __entrypoint_func_uvcmidi_rom_400000(UvCMidi_Exports *exports);
void func_uvcmidi_rom_004003B4(void);
void func_uvcmidi_rom_00400454(s32 arg0);
void func_uvcmidi_rom_0040062C(s32 seqId);
void func_uvcmidi_rom_004006F4(void);
void func_uvcmidi_rom_0040077C(u32 arg0);
void func_uvcmidi_rom_004007B8(f32 arg0);
f32 func_uvcmidi_rom_00400810(void);
void func_uvcmidi_rom_00400854(void);
void func_uvcmidi_rom_00400878(void);
void func_uvcmidi_rom_004008C0(void);
void func_uvcmidi_rom_004008E4(f32 arg0);
void func_uvcmidi_rom_00400940(void);
void func_uvcmidi_rom_00400940(void);
f32 func_uvcmidi_rom_004009C0(u8 chan);
void func_uvcmidi_rom_00400A04(u8 chan, u8 priority);
void func_uvcmidi_rom_00400A38(u8 byte, u8 byte2);
f32 func_uvcmidi_rom_00400A84(u8 channel);
void func_uvcmidi_rom_00400AC8(u8 channel, u8 mix);
void func_uvcmidi_rom_00400AFC(u8 byte1, u8 byte2);
f32 func_uvcmidi_rom_00400B48(u8 channel);
void func_uvcmidi_rom_00400B8C(u8 channel, s32 pan);
void func_uvcmidi_rom_00400BC0(u8 status, s32 byte2);
f32 func_uvcmidi_rom_00400C0C(u8 arg0);
void func_uvcmidi_rom_00400C3C(u8 channel, s32 program);
void func_uvcmidi_rom_00400C70(u8 arg0, s32 arg1);
f32 func_uvcmidi_rom_00400CB8(u8 channel);
void func_uvcmidi_rom_00400CFC(u8 channel, u8 volume);
void func_uvcmidi_rom_00400D30(u8 arg0, u8 arg1);
void func_uvcmidi_rom_00400D7C(u8 status, u8 byte1, u8 byte2);
void func_uvcmidi_rom_00400DC0(u8 arg0, u8 arg1, u8 arg2);
void func_uvcmidi_rom_00400E10(u8 arg0, u8 arg1, u8 arg2);
void func_uvcmidi_rom_00400E60(u8 *arg0, s32 arg1, s32 *arg2);
ALVoiceState *func_uvcmidi_rom_00400EC4(void);
void func_uvcmidi_rom_00400ED4(ALVoiceState **arg0, ALVoiceState **arg1);
s32 func_uvcmidi_rom_00400EF8(void);
void func_uvcmidi_rom_00400F04(u8 *arg0);
ALSeqPlayer *func_uvcmidi_rom_00400FC8(void);
s16 func_uvcmidi_rom_00400FEC(void);

extern ALSeqPlayer *D_uvcmidi_rom_00401020;
extern ALBank *D_uvcmidi_rom_004010DC;
extern ALBankFile *D_uvcmidi_rom_004010E0;
extern ALSeqFile *D_uvcmidi_rom_004010E4;
extern void *D_uvcmidi_rom_004010E8;
extern u16 D_uvcmidi_rom_004011F8;
extern void *D_uvcmidi_rom_004011F0;
extern ALCSeq D_uvcmidi_rom_004010F0;
extern UvAudioMgr_Exports *D_uvcmidi_rom_004011FC;
extern f32 D_uvcmidi_rom_00401000;
extern f64 D_uvcmidi_rom_00401008;
extern f32 D_uvcmidi_rom_00401010;
extern f32 D_uvcmidi_rom_00401014;
extern ALSeqpConfig D_uvcmidi_rom_004010C0;
extern ALVoiceState *D_uvcmidi_rom_004011EC;
extern ALHeap D_uvcmidi_rom_004010B0;
extern MidiSettings *D_uvcmidi_rom_004011E8;
extern s32 D_uvcmidi_rom_004011F4;

void __entrypoint_func_uvcmidi_rom_400000(UvCMidi_Exports *exports) {
    uvUpdateFileAllocPtr((s32) exports);
    exports->func_uvcmidi_rom_00400454 = func_uvcmidi_rom_00400454;
    exports->func_uvcmidi_rom_0040062C = func_uvcmidi_rom_0040062C;
    exports->func_uvcmidi_rom_004006F4 = func_uvcmidi_rom_004006F4;
    exports->func_uvcmidi_rom_0040077C = func_uvcmidi_rom_0040077C;
    exports->func_uvcmidi_rom_004007B8 = func_uvcmidi_rom_004007B8;
    exports->func_uvcmidi_rom_00400810 = func_uvcmidi_rom_00400810;
    exports->func_uvcmidi_rom_00400854 = func_uvcmidi_rom_00400854;
    exports->func_uvcmidi_rom_00400878 = func_uvcmidi_rom_00400878;
    exports->func_uvcmidi_rom_004008C0 = func_uvcmidi_rom_004008C0;
    exports->func_uvcmidi_rom_004003B4 = func_uvcmidi_rom_004003B4;
    exports->func_uvcmidi_rom_004008E4 = func_uvcmidi_rom_004008E4;
    exports->func_uvcmidi_rom_00400BC0 = func_uvcmidi_rom_00400BC0;
    exports->func_uvcmidi_rom_00400940 = func_uvcmidi_rom_00400940;
    exports->func_uvcmidi_rom_00400E60 = func_uvcmidi_rom_00400E60;
    exports->func_uvcmidi_rom_00400C0C = func_uvcmidi_rom_00400C0C;
    exports->func_uvcmidi_rom_004009C0 = func_uvcmidi_rom_004009C0;
    exports->func_uvcmidi_rom_00400EC4 = func_uvcmidi_rom_00400EC4;
    exports->func_uvcmidi_rom_00400C3C = func_uvcmidi_rom_00400C3C;
    exports->func_uvcmidi_rom_00400A04 = func_uvcmidi_rom_00400A04;
    exports->func_uvcmidi_rom_00400ED4 = func_uvcmidi_rom_00400ED4;
    exports->func_uvcmidi_rom_00400C70 = func_uvcmidi_rom_00400C70;
    exports->func_uvcmidi_rom_00400A38 = func_uvcmidi_rom_00400A38;
    exports->func_uvcmidi_rom_00400EF8 = func_uvcmidi_rom_00400EF8;
    exports->func_uvcmidi_rom_00400CB8 = func_uvcmidi_rom_00400CB8;
    exports->func_uvcmidi_rom_00400A84 = func_uvcmidi_rom_00400A84;
    exports->func_uvcmidi_rom_00400F04 = func_uvcmidi_rom_00400F04;
    exports->func_uvcmidi_rom_00400CFC = func_uvcmidi_rom_00400CFC;
    exports->func_uvcmidi_rom_00400AC8 = func_uvcmidi_rom_00400AC8;
    exports->func_uvcmidi_rom_00400FC8 = func_uvcmidi_rom_00400FC8;
    exports->func_uvcmidi_rom_00400D30 = func_uvcmidi_rom_00400D30;
    exports->func_uvcmidi_rom_00400AFC = func_uvcmidi_rom_00400AFC;
    exports->func_uvcmidi_rom_00400FEC = func_uvcmidi_rom_00400FEC;
    exports->func_uvcmidi_rom_00400D7C = func_uvcmidi_rom_00400D7C;
    exports->func_uvcmidi_rom_00400B48 = func_uvcmidi_rom_00400B48;
    exports->func_uvcmidi_rom_00400DC0 = func_uvcmidi_rom_00400DC0;
    exports->func_uvcmidi_rom_00400B8C = func_uvcmidi_rom_00400B8C;
    exports->func_uvcmidi_rom_00400E10 = func_uvcmidi_rom_00400E10;
    D_uvcmidi_rom_004011E8 = uvGetSystemProp(3);
    if (D_uvcmidi_rom_004011E8 == NULL) {
        D_uvcmidi_rom_004010C0.maxVoices = 0x10;
        D_uvcmidi_rom_004010C0.maxEvents = 0x100;
        D_uvcmidi_rom_004010C0.maxChannels = 0x10;
        D_uvcmidi_rom_004011F4 = 0;
        D_uvcmidi_rom_004011F0 = NULL;
    } else {
        if (D_uvcmidi_rom_004011E8->unk8 != 0) {
            D_uvcmidi_rom_004010C0.maxVoices = D_uvcmidi_rom_004011E8->unk8;
        } else {
            D_uvcmidi_rom_004010C0.maxVoices = 0x10;
        }
        if (D_uvcmidi_rom_004011E8->unkA != 0) {
            D_uvcmidi_rom_004010C0.maxEvents = D_uvcmidi_rom_004011E8->unkA;
        } else {
            D_uvcmidi_rom_004010C0.maxEvents = 0x100;
        }
        if (D_uvcmidi_rom_004011E8->unkC != 0) {
            D_uvcmidi_rom_004010C0.maxChannels = D_uvcmidi_rom_004011E8->unkC;
        } else {
            D_uvcmidi_rom_004010C0.maxChannels = 0x10;
        }
        D_uvcmidi_rom_004011F4 = D_uvcmidi_rom_004011E8->unk0;
        D_uvcmidi_rom_004011F0 = D_uvcmidi_rom_004011E8->unk4;
    }
    if (D_uvcmidi_rom_004011F4 == 0) {
        D_uvcmidi_rom_004011F4 = (D_uvcmidi_rom_004010C0.maxVoices * 0x38)
                                 + (D_uvcmidi_rom_004010C0.maxEvents * 0x1C)
                                 + (D_uvcmidi_rom_004010C0.maxChannels * 0x10) + 0x12C;
    }

    if (D_uvcmidi_rom_004011F0 == NULL) {
        D_uvcmidi_rom_004011F8 |= 1;
        D_uvcmidi_rom_004011F0 = _uvMemAllocAlign8((u32) D_uvcmidi_rom_004011F4);
    }
    uvMemSet(D_uvcmidi_rom_004011F0, 0U, (u32) D_uvcmidi_rom_004011F4);
    alHeapInit(&D_uvcmidi_rom_004010B0, (u8 *) D_uvcmidi_rom_004011F0, D_uvcmidi_rom_004011F4);
    D_uvcmidi_rom_004010C0.heap = &D_uvcmidi_rom_004010B0;
    alCSPNew((ALCSPlayer *) D_uvcmidi_rom_00401020, &D_uvcmidi_rom_004010C0);
    D_uvcmidi_rom_004011EC = D_uvcmidi_rom_00401020->vFreeList;
    D_uvcmidi_rom_004011FC = uvLoadModule('AMGR');
}

void func_uvcmidi_rom_004003B4(void) {
    if (alSeqpGetState(D_uvcmidi_rom_00401020) != 0) {
        func_uvcmidi_rom_00400940();
    }
    alSeqpDelete(D_uvcmidi_rom_00401020);
    if (D_uvcmidi_rom_004011F8 & 1) {
        _uvMemFree(D_uvcmidi_rom_004011F0);
    }
    if (D_uvcmidi_rom_004011F8 & 2) {
        _uvMemFree(D_uvcmidi_rom_004010E0);
        _uvMemFree(D_uvcmidi_rom_004010E4);
        _uvMemFree(D_uvcmidi_rom_004010E8);
    }
    uvUnloadModule('AMGR');
}

void func_uvcmidi_rom_00400454(s32 arg0) {
    void *sp64;
    void *sp60;
    u32 sp5C;
    s32 fileId;
    ALSeqFile sp4C;
    s32 sp48;
    s32 maxSeqLen;
    s32 i;
    void *sp3C;

    fileId = uvFileReadHeader(uvGetFileData('UVMB', arg0));
    uvFileSearchTag(fileId, &sp5C, &sp64, '.CTL', 0);
    uvFileSearchTag(fileId, NULL, &sp60, '.TBL', 0);
    uvFileFree(fileId);
    D_uvcmidi_rom_004011F8 |= 2;
    D_uvcmidi_rom_004010E0 = _uvMemAlloc(sp5C, 0x10U);
    _uvMediaCopy(D_uvcmidi_rom_004010E0, sp64, sp5C);
    alBnkfNew(D_uvcmidi_rom_004010E0, (u8 *) sp60);
    D_uvcmidi_rom_004010DC = D_uvcmidi_rom_004010E0->bankArray[0];
    alSeqpSetBank(D_uvcmidi_rom_00401020, D_uvcmidi_rom_004010DC);
    fileId = uvFileReadHeader(uvGetFileData('UVMS', 0));
    uvFileSearchTag(fileId, NULL, &sp3C, 'SEQS', 0);
    uvFileFree(fileId);
    _uvMediaCopy(&sp4C, sp3C, 4U);
    i = sp4C.seqCount;
    sp48 = (i * 8) + 4;
    D_uvcmidi_rom_004010E4 = _uvMemAlloc(sp48, 0x10U);
    _uvMediaCopy(D_uvcmidi_rom_004010E4, sp3C, sp48);
    alSeqFileNew(D_uvcmidi_rom_004010E4, (u8 *) sp3C);
    maxSeqLen = 0; // maxSeqLen
    for (i = 0; i < D_uvcmidi_rom_004010E4->seqCount; i++) {
        if (D_uvcmidi_rom_004010E4->seqArray[i].len >= maxSeqLen) {
            maxSeqLen = D_uvcmidi_rom_004010E4->seqArray[i].len;
        }
    }

    if (maxSeqLen & 1) {
        maxSeqLen += 1;
    }
    D_uvcmidi_rom_004010E8 = _uvMemAlloc((u32) maxSeqLen, 0x10U);
}

void func_uvcmidi_rom_0040062C(s32 seqId) {
    s32 seqLen;
    s32 seqAlign;

    seqLen = D_uvcmidi_rom_004010E4->seqArray[seqId].len;
    seqAlign = seqLen;
    if (seqLen & 1) {
        seqAlign = seqLen + 1;
    }
    if (alSeqpGetState(D_uvcmidi_rom_00401020) != 0) {
        func_uvcmidi_rom_00400940();
    }
    _uvMediaCopy(D_uvcmidi_rom_004010E8, D_uvcmidi_rom_004010E4->seqArray[seqId].offset,
                 (u32) seqAlign);
    alCSeqNew(&D_uvcmidi_rom_004010F0, (u8 *) D_uvcmidi_rom_004010E8);
    alSeqpSetBank((ALSeqPlayer *) D_uvcmidi_rom_00401020, D_uvcmidi_rom_004010DC);
    alSeqpSetSeq((ALSeqPlayer *) D_uvcmidi_rom_00401020, (ALSeq *) &D_uvcmidi_rom_004010F0);
}

void func_uvcmidi_rom_004006F4(void) {
    f32 sp1C;

    D_uvcmidi_rom_004011FC->func_uvaudiomgr_rom_004011D0(&sp1C);
    if (alSeqpGetState(D_uvcmidi_rom_00401020) != 0) {
        func_uvcmidi_rom_00400940();
    }
    alSeqpPlay(D_uvcmidi_rom_00401020);
    alSeqpSetVol(D_uvcmidi_rom_00401020, (s16) (s32) (sp1C * D_uvcmidi_rom_00401000));
}

void func_uvcmidi_rom_0040077C(u32 arg0) {
    ALCSeqMarker marker;

    alCSeqNewMarker(&D_uvcmidi_rom_004010F0, &marker, arg0);
    alCSeqSetLoc(&D_uvcmidi_rom_004010F0, &marker);
}

void func_uvcmidi_rom_004007B8(f32 arg0) {
    if (alSeqpGetState(D_uvcmidi_rom_00401020) != 0) {
        alSeqpSetTempo(D_uvcmidi_rom_00401020, (s32) (D_uvcmidi_rom_00401008 / (f64) arg0));
    }
}

f32 func_uvcmidi_rom_00400810(void) {
    s32 temp_v0;

    temp_v0 = alCSPGetTempo((ALCSPlayer *) D_uvcmidi_rom_00401020);
    if (temp_v0 == 0) {
        return 0.0f;
    }
    return D_uvcmidi_rom_00401010 / (f32) temp_v0;
}

void func_uvcmidi_rom_00400854(void) {
    alCSeqGetTicks(&D_uvcmidi_rom_004010F0);
}

void func_uvcmidi_rom_00400878(void) {
    s32 sp1C;

    sp1C = alCSeqGetTicks(&D_uvcmidi_rom_004010F0);
    alCSeqTicksToSec(&D_uvcmidi_rom_004010F0, sp1C,
                     alCSPGetTempo((ALCSPlayer *) D_uvcmidi_rom_00401020));
}

void func_uvcmidi_rom_004008C0(void) {
    alSeqpGetState(D_uvcmidi_rom_00401020);
}

void func_uvcmidi_rom_004008E4(f32 arg0) {
    if (alSeqpGetState(D_uvcmidi_rom_00401020) != 0) {
        alSeqpSetVol(D_uvcmidi_rom_00401020, (s16) (s32) (arg0 * D_uvcmidi_rom_00401014));
    }
}

void func_uvcmidi_rom_00400940(void) {
    alSeqpStop(D_uvcmidi_rom_00401020);
    uvClkReset(0x6A);
    while (alSeqpGetState(D_uvcmidi_rom_00401020) != 0) {
        if ((uvClkGetSec(0x6A) > 2.0)) {
            return;
        }
    }
}

f32 func_uvcmidi_rom_004009C0(u8 chan) {
    return alSeqpGetChlPriority(D_uvcmidi_rom_00401020, chan);
}

void func_uvcmidi_rom_00400A04(u8 chan, u8 priority) {
    alSeqpSetChlPriority(D_uvcmidi_rom_00401020, chan, priority);
}

void func_uvcmidi_rom_00400A38(u8 byte, u8 byte2) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (byte + 0xB0), 0x10U, (u8) (s32) byte2);
}

f32 func_uvcmidi_rom_00400A84(u8 channel) {
    return alSeqpGetChlFXMix(D_uvcmidi_rom_00401020, channel);
}

void func_uvcmidi_rom_00400AC8(u8 channel, u8 mix) {
    alSeqpSetChlFXMix(D_uvcmidi_rom_00401020, channel, mix);
}

void func_uvcmidi_rom_00400AFC(u8 byte1, u8 byte2) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (byte1 + 0xB0), 0x5BU, byte2);
}

f32 func_uvcmidi_rom_00400B48(u8 channel) {
    return alSeqpGetChlPan(D_uvcmidi_rom_00401020, channel);
}

// uvaSetChannelPan
void func_uvcmidi_rom_00400B8C(u8 channel, s32 pan) {
    alSeqpSetChlPan(D_uvcmidi_rom_00401020, channel, pan);
}

void func_uvcmidi_rom_00400BC0(u8 status, s32 byte2) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (status + 0xB0), 0xAU, byte2);
}

f32 func_uvcmidi_rom_00400C0C(u8 arg0) {
    return alSeqpGetChlProgram(D_uvcmidi_rom_00401020, arg0);
}

void func_uvcmidi_rom_00400C3C(u8 channel, s32 program) {
    alSeqpSetChlProgram(D_uvcmidi_rom_00401020, channel, program);
}

void func_uvcmidi_rom_00400C70(u8 arg0, s32 arg1) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (arg0 + 0xC0), arg1, 0U);
}

f32 func_uvcmidi_rom_00400CB8(u8 channel) {
    return alSeqpGetChlVol(D_uvcmidi_rom_00401020, channel);
}

void func_uvcmidi_rom_00400CFC(u8 channel, u8 volume) {
    alSeqpSetChlVol(D_uvcmidi_rom_00401020, channel, volume);
}

void func_uvcmidi_rom_00400D30(u8 arg0, u8 arg1) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (arg0 + 0xB0), 7U, arg1);
}

void func_uvcmidi_rom_00400D7C(u8 status, u8 byte1, u8 byte2) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, status, byte1, byte2);
}

void func_uvcmidi_rom_00400DC0(u8 arg0, u8 arg1, u8 arg2) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (arg0 + 0x90), arg1, arg2);
}

void func_uvcmidi_rom_00400E10(u8 arg0, u8 arg1, u8 arg2) {
    alSeqpSendMidi(D_uvcmidi_rom_00401020, 0, (arg0 + 0x80), arg1, arg2);
}

void func_uvcmidi_rom_00400E60(u8 *arg0, s32 arg1, s32 *arg2) {
    ALVoiceState *node;
    s32 nodePos;

    node = D_uvcmidi_rom_00401020->vAllocHead;
    nodePos = 0;
    while ((node != NULL) && (node != D_uvcmidi_rom_00401020->vAllocTail)) {
        if (nodePos < arg1) {
            arg0[nodePos] = node->envPhase;
        }
        node = node->next;
        nodePos++;
    }

    *arg2 = nodePos;
}

ALVoiceState *func_uvcmidi_rom_00400EC4(void) {
    return D_uvcmidi_rom_00401020->vFreeList;
}

void func_uvcmidi_rom_00400ED4(ALVoiceState **arg0, ALVoiceState **arg1) {
    *arg0 = D_uvcmidi_rom_00401020->vAllocHead;
    *arg1 = D_uvcmidi_rom_00401020->vAllocTail;
}

s32 func_uvcmidi_rom_00400EF8(void) {
    return D_uvcmidi_rom_004011EC;
}

void func_uvcmidi_rom_00400F04(u8 *arg0) {
    ALVoiceState *tail;
    ALVoiceState *head;
    s32 i;

    ALVoiceState *v0 = D_uvcmidi_rom_004011EC;

    for (i = 0; i < D_uvcmidi_rom_004010C0.maxVoices; i++) {
        arg0[i] = 0xFF;
    }

    tail = D_uvcmidi_rom_00401020->vAllocTail;
    head = D_uvcmidi_rom_00401020->vAllocHead;
    while ((head != tail) && (head != NULL)) {
        i = ((u32) v0 - (u32) head) / 56U;
        i = ABS(i);
        if ((s32) i < D_uvcmidi_rom_004010C0.maxVoices) {
            arg0[i] = head->envPhase;
        }
        head = head->next;
    }
}

ALSeqPlayer *func_uvcmidi_rom_00400FC8(void) {
    if (D_uvcmidi_rom_00401020 != NULL) {
        return D_uvcmidi_rom_00401020;
    }
    return NULL;
}

// uvaGetSeqCount
s16 func_uvcmidi_rom_00400FEC(void) {
    return D_uvcmidi_rom_004010E4->seqCount;
}
