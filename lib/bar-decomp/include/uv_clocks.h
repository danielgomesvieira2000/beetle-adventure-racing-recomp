// SPDX-License-Identifier: AGPL-3.0-or-later
#ifndef BAR_UV_CLOCKS_H
#define BAR_UV_CLOCKS_H
#include <ultra64.h>

typedef struct uvClockState {
    u32 prevWrapCount;
    u32 prevLastCount;
} uvClockState_t;

void uvClkInit(void);
void uvClkUpdate(void);
f64 uvClkGetSec(s32 clk_id);
void uvClkReset(s32 clk_id);

#endif /* BAR_UV_CLOCKS_H */

