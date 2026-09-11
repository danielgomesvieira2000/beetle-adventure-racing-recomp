// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef BAR_FUNCTIONS_H
#define BAR_FUNCTIONS_H
u32 uvFileGetEntryTag(s32 arg0, u32* arg1, void** arg2);
void mio0Decode(void* src, u8* dst);
u64 uvMemRead(void* vAddr, u32 nbytes);
void _uvDMA(void* vAddr, u32 devAddr, u32 nbytes);
void _uvMediaCopy(void* vAddr, void* devAddr, u32 nbytes);
void uvMemSet(void* vAddr, u8 value, u32 nbytes);
void* uvGetLoadedModule(s32 tag);
void* uvLoadModule(s32 tag);
void* uvGetSystemProp(s32 index);
void uvUnloadModule(s32 tag);
void _uvScAddClient(OSSched *sc, OSScClient *client, OSMesgQueue *mq);
void func_8000226C(s32*, s32*, s32*, u32);                   /* extern */
void func_80002088(s32*, s32*, s32*, s32);                   /* extern */
s32 _uvMemGetBlocksSize(void);
void func_8000110C(s32);                                 /* extern */
void func_8000D7F0(s32*);                                /* extern */
u32 func_8000E460(void);                                /* extern */
#endif /* BAR_FUNCTIONS_H */
