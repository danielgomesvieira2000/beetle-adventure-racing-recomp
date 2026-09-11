#ifndef UV_SCHEDULER_H
#define UV_SCHEDULER_H
void _uvScAddClient(OSSched *sc, OSScClient *client, OSMesgQueue *mq);
void _uvScCreateScheduler(OSSched *sc, void *stack, s32 priority, u8 mode, u8 numFields);
void _uvScCreateScheduler(OSSched *sc, void *stack, s32 priority, u8 mode, u8 numFields);
void _uvScDlistRecover(void);
void _uvScDoneAud(void);
void _uvScDoneGfx(void);
s32 _uvScGetCmdQ(OSSched *s);
void _uvScHandleNMI(void);
void _uvScHandleNMI(void);
void _uvScHandleRDP(void);
void _uvScHandleRDP(void);
void _uvScHandleRSP(void);
void _uvScHandleRSP(void);
void _uvScHandleRetrace(void);
void _uvScHandleRetrace(void);
void _uvScInitClientList(void);
void _uvScMain(void *arg0);
void _uvScMain(void *);
void _uvScRemoveClient(OSSched *sc, OSScClient *c);
void _uvScRunAud(void);
void _uvScRunGfx(void);
void _uvScRunGfx(void);
void func_80003C1C(void);
void func_800048C8(s32 arg0);
void func_800048D4(s32 arg0);
void func_800048DC(void);
void func_800048DC(void);
void func_800048E4(void);
void func_800048E4(void);
void func_80004958(u8 arg0, s32 arg1);
void func_80004958(u8, s32);
void func_80004A78(s32 arg0);
f64 func_80004A84(void);
void func_80004B1C(s32 arg0, s32 arg1, s32 arg2);
void func_80004B2C(s32 arg0);
void uvSetVideoMode(void);
#endif /* UV_SCHEDULER_H  */
