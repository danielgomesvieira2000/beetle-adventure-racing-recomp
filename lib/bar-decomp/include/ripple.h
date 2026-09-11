#ifndef BAR_RIPPLE_H
#define BAR_RIPPLE_H
typedef struct Ripple_Exports_s {
    /* 0x00 */ void (*func_ripple_004006B8)(void);                      /* inferred */
    /* 0x04 */ void (*func_ripple_004002D8)(void);                      /* inferred */
    /* 0x08 */ void (*addRipple)(Mtx4F*, f32);              /* inferred */
    /* 0x0C */ void (*func_ripple_004005A0)(s32);                   /* inferred */
    /* 0x10 */ s32 (*getActiveRippleCount)(void);                      /* inferred */
    /* 0x14 */ void (*resetRipple)(void);                     /* inferred */
} Ripple_Exports;                                   /* size = 0x18 */
#endif /* BAR_RIPPLE_H */
