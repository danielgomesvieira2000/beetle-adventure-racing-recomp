// SPDX-License-Identifier: AGPL-3.0-or-later
#include "os.h"
#include "uv_clocks.h"
#include "macros.h"

// .bss
u32 sCpuLastCount;
s32 sWrapCount; // Counts the times the counter is wrapped
uvClockState_t D_8002D948[3];
uvClockState_t D_8002D960[8];

/* Need to make these arrays because the compiler doesn't want to put them before the stubbed string */
static const f64 D_80021030[1] = { 93.875101696 };
static const f64 sCpuClockSpeed[1] = { 45751932.2845432162 };

uvClockState_t *uvGetClk(s32 clk_id) {
    if (clk_id >= 100) {
        return &D_8002D960[clk_id - 100];
    }
    if (clk_id >= 3) {
        return NULL;
    }
    return &D_8002D948[clk_id];
}

void uvClkInit(void) {
    sCpuLastCount = osGetCount();
    sWrapCount = 0;
}

void uvClkUpdate(void) {
    u32 currentCount = osGetCount();
    if (((u32) sCpuLastCount >= 0xF0000001U) && (currentCount < 0x10000000U)) {
        sWrapCount++;
    }
    sCpuLastCount = currentCount;
}

f64 uvClkGetSec(s32 clk_id) {
    uvClockState_t *clock;
    f64 var_fv1;
    f64 var_fa0;

    clock = uvGetClk(clk_id);
    if (clock == NULL) {
#ifdef _ISPRINT
        _uvDebugPrintf("uvClkGetSec: unknown clock %d\n", clk_id);
#endif
        return 0.0;
    }
    uvClkUpdate();
    var_fv1 = sWrapCount - clock->prevWrapCount;
    var_fv1 *= D_80021030[0];
    var_fa0 = ((f64) sCpuLastCount - clock->prevLastCount) / sCpuClockSpeed[0];
    return var_fv1 + var_fa0;
}

void uvClkReset(s32 clk_id) {
    uvClockState_t *clock;

    uvClkUpdate();
    clock = uvGetClk(clk_id);
    if (clock != NULL) {
        clock->prevLastCount = sCpuLastCount;
        clock->prevWrapCount = sWrapCount;
    } else {
        PRINTF("Null free head\n");
    }
}
