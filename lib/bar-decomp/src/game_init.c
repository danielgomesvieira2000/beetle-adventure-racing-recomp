// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"
#include "module.h"
#include "global_exports.h"

void uvUnloadModule(s32);
void _uvScInitClientList(void);
void uvSysInit(void);
void uvSetGameState(s32);                     /* extern */
void uvShowNoController(void);

extern s32 D_8001F7D4;
extern s32 D_8001F7D8;
extern s32 D_80020F90;
extern s32 D_80020FA0;
extern f32 D_80020FF8;
extern f32 D_80020FFC;
extern UnkStruct_8002D1A4* gGameExports;

// .data
s32 D_8001F630 = 0x3000;

void func_80000450(void) {
    s16 i;

    gGameSettings->unk6EAA = 0;
    gGameSettings->unk160 = 0;
    gGameSettings->unk170 = 0;
    gGameSettings->unk6FA0 = 0.009999999776f;
    gGameSettings->unk7C = 0.0f;
    gGameSettings->currentGameState = -1;
    gGameSettings->pauseFlag = 0;
    gGameSettings->debugState = 0;
    gGameSettings->unk18C = 1;
    gGameSettings->numMaxTxts = 0x1F4;
    gGameSettings->dbgTileSort = 1;
    gGameSettings->unk80 = 0.1f;
    gGameSettings->optionsSfxVol = 8;
    gGameSettings->optionsMusicVol = 8;
    gGameSettings->optionsSpeechVol = 8;
    gGameSettings->numPlayers = 1;
    gGameSettings->currentTrack = 0;
    gGameSettings->introReplayState = 0;
    gGameSettings->dbgOptsRecordIntro = 0;
    gGameSettings->dbgOptsGlare = 1;
    gGameSettings->dbgOptsFrameRate = 0;
    gGameSettings->unk94 = 0;
    gGameSettings->dbgAbortMode = 0;
    gGameSettings->unk6E9C = 0;
    gGameSettings->unk4C = 0;
    gGameSettings->unk48 = 3;
    gGameSettings->dbgDispViGamma = 0;
    gGameSettings->dbgDispViGamDith = 0;
    gGameSettings->dbgDispViDither = 1;
    gGameSettings->dbgDispViDivot = 1;
    gGameSettings->dbgDispAntialias = 1;
    gGameSettings->dbgOptsCarShadow = 1;
    gGameSettings->numAiCars = 7;
    gGameSettings->dbgSuspensionFlag = 1;
    gGameSettings->dbgTrackWeather = 0;
    gGameSettings->dbgFrateDisp = 1;
    gGameSettings->unk50 = 0;
    gGameSettings->dbgMxSpeed = 300.0f;

    for (i = 0; i < 4; i++) {
        gGameSettings->unk138[i].currentColor = i;
        gGameSettings->unk138[i].currentCar = (i % 3) + 1;
        gGameSettings->unk138[i].transmissionType = 1;

    }

    for (i = 0; i < 16; i++) {
        gGameSettings->unk150[i] = ' ';
        gGameSettings->unk6EB0[i] = ' ';
    }

    gGameSettings->unk150[15] = '\0';
    gGameSettings->unk6EB0[15] = '\0';
    gGameSettings->unk174 = 0;
    gGameSettings->unk180 = 0;
    gGameSettings->unk184 = 3;
    gGameSettings->initFlag = 1;
    gGameSettings->pad178[0] = 0;
    gGameSettings->pad178[1] = 1;
    gGameSettings->pad178[2] = 1;
    gGameSettings->pad178[3] = 1;
    gGameSettings->pad178[4] = 1;
    gGameSettings->pad178[6] = 0;
    gGameSettings->pad178[7] = 0;
    gGameSettings->unk6F74 = 1;
    gGameSettings->unk6EAC = 0;
    gGameSettings->unk6EAE = 0;
    gGameSettings->unkC = 0;
    gGameSettings->finishedIntroCount = 0;
    gGameSettings->unk10 = 0;
    gGameSettings->unk704C[0] = 0;
    gGameSettings->unk704C[1] = 0;
    gGameSettings->unk7058 = 0;
    gGameSettings->unk705C = 0;
    gGameSettings->unk7060 = 1;
    gGameSettings->unk6FB8 = 0x3639;
    gGameSettings->unk6FBC = 'NNSE';
    gGameSettings->unk6FB0 = "BEETLE RACING";
    gGameSettings->unk6FB4 = "\0";

    for (i = 0; i < 4; i++) {
        gGameSettings->unk6F78[i] = (i * 0x19) + 0x19;
    }

    for (i = 0; i < 8; i++) {
        gGameSettings->unk6EC0[i] = 0;
    }

    gGameSettings->pad178[5] = 1;
    gGameSettings->currentTrack = 5;
    uvSetGameState(0xE);
    if (gUvContExports->unk8(0) == 0) {
        uvShowNoController();
    }
    gUvGfxMgrExports->uvGfxEnableGamma(gGameSettings->dbgDispViGamma);
    for (i = 0; i < 8; i++) {
        gGameSettings->unk7084[i] = 0;
    }

    for (i = 0; i < 4; i++) {
        gGameSettings->unk7064[i] = 0;
        gGameSettings->unk7074[i] = 0;
    }

    gGameSettings->dbgFrateDisp = 0;
    gGameSettings->dbgHudState = 0;
    gGameSettings->unk18C = 0;
    while (gUvContExports->unk4() != 0) {
        gGameExports->unk4();
        if (D_8001F7D4 != 0) {
            D_8001F7D4 = 0;
            D_8001F7D8 = 0;
            gGameSettings->gameStateFlag = gGameSettings->currentGameState;
        }
        if (gGameSettings->gameStateFlag != -1) {
            uvSetGameState(gGameSettings->gameStateFlag);
        }
    }
}

void uvSetGameState(s32 gameStateId) {
    s32 timesLoaded;
    s32 i;
    s32 pad;
    f32 sec;

    if (gUvGfxMgrExports != NULL) {
        gUvGfxMgrExports->func_uvgfxmgr_rom_00402090();
        if (gCurrentGameState != -1) {
            gGameExports->unkC(gCurrentGameState);
        }
        timesLoaded = uvGetFileInstanceCount('UVMO', uvGetModuleFileId('MIDI'));
        for (i = 0; i < timesLoaded; i++) {
            uvUnloadModule('MIDI');
        }

        timesLoaded = uvGetFileInstanceCount('UVMO', uvGetModuleFileId('EMIT'));
        for (i = 0; i < timesLoaded; i++) {
            uvUnloadModule('EMIT');
        }

        timesLoaded = uvGetFileInstanceCount('UVMO', uvGetModuleFileId('AMGR'));
        for (i = 0; i < timesLoaded; i++) {
            uvUnloadModule('AMGR');
        }
        gUvContExports->unk6C();
        _uvScInitClientList();

        sec = uvClkGetSec(0x6A);

        while (sec + 0.2f < uvClkGetSec(0x6A)) {
        }

    }
    gCurrentGameState = gameStateId;
    gGameStateFlag = -1;
    uvSysInit();
    gSkidExports = 0;
    gSprayExports = 0;
    gGlareExports = 0;
    gAiExports = 0;
    gFlagExports = 0;
    gTdataExports = 0;
    gVolTestExports = 0;
    gBattleExports = 0;
    gPowerupExports = 0;
    gLetterExports = 0; // Ladybug pickups
    gWeaponExports = 0;
    gRainExports = 0;   // Snow effect in Mount Mayhem
    gRumbleExports = 0;
    gGameExports = uvLoadModule('game');
    gGameExports->unk8(gameStateId);
}

char* gNoControllerStrings[] = {"NO CONTROLLER IN", "CONTROLLER SOCKET 1", "PLEASE POWER OFF AND", "ATTACH A CONTROLLER"};

// Displays error message and loops forever when no controllers are connected
void uvShowNoController(void) {
    s32 i;
    s32 line;
    s32 s4;
    s32 s3;
    uvLoadFile('UVFT', 1);
    while (1) {
        gUvFontExports->uvSetFont(5); // font ID
        gUvFontExports->uvFontColor(0, 100, 200, 255); // RGBA color
        gUvGfxMgrExports->func_uvgfxmgr_rom_004007F8();
        gUvGfxMgrExports->func_uvgfxmgr_rom_00401BD4(0, 0x13F, 0, 0xEF);  // bg size?
        gUvGfxMgrExports->func_uvgfxmgr_rom_00401788(0, 0, 0);            // black BG color

        for (line = 0; line < 4; line++) {
            // clang-format off
            gUvFontExports->uvFontPrintStr(160 - (gUvFontExports->uvFontWidth(gNoControllerStrings[line]) / 2), 150 - line * 20,
                              gNoControllerStrings[line]);
            // clang-format on
        }

        gUvFontExports->uvFontGenDList();
        gUvGfxMgrExports->func_uvgfxmgr_rom_00400B24();
    }
}
