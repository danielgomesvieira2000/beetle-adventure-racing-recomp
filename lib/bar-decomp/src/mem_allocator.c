// SPDX-License-Identifier: AGPL-3.0-or-later
#include "common.h"

void _uvMemAllocInit(void);
void func_80002AEC(s32);
void func_80002B2C(s32);
void* _uvMemAlloc(u32 size, u32 alignment);

MemBlock* gMemBlockHead;
extern MemBlock gMemBlock[0x79912];

// .data
s32 D_8001F7A0 = 0;
s32 D_8001F7A4 = 0;
s32 D_8001F7A8 = 0;
s32 D_8001F7AC = 0;
s32 D_8001F7B0 = 0;
s32 D_8001F7B4 = 0;
s32 D_8001F7B8 = 0;
s32 D_8001F7BC = 0;
s32 D_8001F7C0 = 0;
s32 D_8001F7C4 = 0;

extern u8 D_8001F7D0;
extern s32 D_8002F7D8;
extern s32 D_8002F7DC;
extern u8 D_80022BD8[];


void _uvMemAllocInitStartUp(void) {
    _uvMemAllocInit();
    if ((D_8002F7D8 != 0) || (D_8001F7D0 != 0)) {
        func_80002B2C(D_8001F7D0);
        func_80002AEC(D_8001F7D0);
    }
}

void _uvMemAllocInit(void) {
    gMemBlockHead = gMemBlock;
    gMemBlockHead->next = NULL;
    gMemBlockHead->size = ((u32) 0x80400000 - (u32)gMemBlock);
    D_8001F7A0 = 1;
    D_8001F7A4 = 1;
}

UNUSED void _uvMemUnusedDbgFunc(void) {
    MemBlock *block;

    block = gMemBlockHead;
    while (block != NULL) {
        block = block->next;
    }
}

void func_80002AEC(s32 val) {
    void* sp1C;

    sp1C = &((s32*)&sp1C)[-12];
    uvMemSet(D_80022BD8, val, ((u8*)&sp1C - (u8*)&D_80022BD8) - 0x30);
}

void func_80002B2C(s32 arg0) {
    MemBlock* var_s0;

    for (var_s0 = gMemBlockHead; var_s0 != NULL; var_s0 = var_s0->next) {
        uvMemSet(var_s0 + 1, arg0 & 0xFF & 0xFF, var_s0->size - 8);
    }
}

void func_80002B80(MemBlock* arg0) {
    MemBlock* var_a2;
    MemBlock* var_v0;
    s32 temp_v1;

    var_v0 = NULL;
    temp_v1 = arg0->size;
    if (gMemBlockHead == NULL) {
        gMemBlockHead = arg0;
        D_8001F7A0 += 1;
        return;
    }

    var_a2 = gMemBlockHead;
    while (var_a2 != NULL && var_a2 < arg0) {
        var_v0 = var_a2;
        var_a2 = var_a2->next;
    }

    if (var_v0 != NULL) {
        if ((u32)arg0 == ( (u32) var_v0 + (u32)var_v0->size)) {
            if ((u32)var_a2 == ((u32)arg0 + (u32)temp_v1)) {
                var_v0->size += temp_v1;
                var_v0->size = var_v0->size + var_a2->size;
                var_v0->next = var_a2->next;
                D_8001F7A0 -= 1;
                return;
            }
        }
    }
    if (var_v0 != NULL) {
        if ((u32)arg0 == ((u32)var_v0 + (u32)var_v0->size)) {
            var_v0->size += temp_v1;
            return;
        }
    }
    if ((u32)var_a2 == ((u32)arg0 + temp_v1)) {
        arg0->next = var_a2->next;

        arg0->size = (0, temp_v1) + var_a2->size; // FAKE
        if (var_v0 != NULL) {
            var_v0->next = arg0;
            return;
        }
        gMemBlockHead = arg0;
        return;
    }
    arg0->next = var_a2;
    if (var_v0 != NULL) {
        var_v0->next = arg0;
    } else {
        gMemBlockHead = arg0;
    }
    D_8001F7A0++;
    if ((u32) D_8001F7A4 < (u32) D_8001F7A0) {
        D_8001F7A4 = D_8001F7A0;
    }
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/mem_allocator/func_80002CD0.s")

// called at start of every GameState transition during gameplay
void func_80002EAC(s32 arg0) {
    _uvMemAllocInit();
    D_8001F7A0 = 1;
    D_8001F7A4 = 1;
    D_8001F7A8 = 0;
    D_8001F7AC = 0;
    D_8001F7B0 = 0;
    D_8001F7B4 = 0;
    D_8001F7B8 = 0;
    D_8001F7BC = 0;
    // non-zero prevents freeze frame of graphics during GameState transitions
    // Changing D_8001F7D0 to FFs causes white screen
    if ((arg0 != 0) || (D_8002F7D8 != 0) || (D_8001F7D0 != 0)) {
        func_80002B2C((s32) D_8001F7D0);
        func_80002AEC((s32) D_8001F7D0);
    }
}


void *_uvMemAllocAlign8(u32 size) {
    return _uvMemAlloc(size, 8U);
}

void* _uvMemAlloc(u32 size, u32 alignment) {
    u32 sp44;
    s32 var_t1;
    s32 var_a0;
    MemBlock* temp_a0;
    MemBlock* var_t0;
    MemBlock* var_t3;
    MemBlock* var_v0;
    s32 temp_v1;
    MemBlock* sp24;
    s32 var_t2;

    sp44 = size;

    var_t1 = -1;
    var_t2 = FALSE;
    var_t0 = NULL;
    var_t3 = NULL;
    if (((s32)size <= 0) || ((s32)size >= 0x800000)) {
        return NULL;
    }
    size = ALIGN8(size);
    D_8001F7B8 = 0;

    for (var_v0 = gMemBlockHead; var_v0 != NULL; var_v0 = var_v0->next) {

        var_a0 = alignment - ((u32) &var_v0->size & (alignment - 1));
        if (alignment == var_a0) {
            temp_v1 = 0;
        } else {
            temp_v1 = var_a0;
        }
        if ((temp_v1 + size) < var_v0->size) {
            var_t1 = var_v0->size;
            var_t3 = var_t0;
            var_t2 = TRUE;
            sp24 = var_v0;
            break;
        }
        var_t0 = var_v0;
        D_8001F7B8 += 1;
    }
    D_8001F7BC += D_8001F7B8;
    if ((!var_t2) || (var_t1 < size) || (var_t1 == -1)) {
        D_8002F7DC = 1;
        return NULL;
    }

    temp_v1 = alignment - ((s32) &sp24->size & (alignment - 1));
    if (alignment == temp_v1) {
        var_a0 = 0;
    } else {
        var_a0 = temp_v1;
    }
    size += var_a0;
    if (var_t3 == NULL) {
        gMemBlockHead = sp24->next;
    } else {
        var_t3->next = sp24->next;
    }

    temp_a0 = (u8*)sp24 + size;
    D_8001F7A0--;
    if ((size + 8) < sp24->size) {
        temp_a0->next = NULL;
        temp_a0->size = sp24->size - size;
        var_t1 = size;
        func_80002B80(temp_a0);
    }
    sp24 = (u8*)sp24 + var_a0;
    sp24->next = (var_t1 / 4) | ((var_a0 >> 2) << 0x14);
    D_8001F7B0 += D_8001F7A0;
    D_8001F7A8++;
    if (D_8002F7D8 != 0) {
        uvMemSet(&sp24->size, 0, sp44);
    }
    return &sp24->size;
}

#pragma GLOBAL_ASM("asm/us/nonmatchings/mem_allocator/_uvMemFree.s")

void _uvMemUnusedDbgFunc1(void) {
    MemBlock* block;
    s32 size;
    u32 var_a0;
    u32 var_v1;
    s32 blockSize;


    for (blockSize = 0, var_v1 = 0, var_a0 = -1, block = gMemBlockHead; block != NULL; block = block->next, blockSize += size) {
        size = block->size;
        if (size < var_a0) {
            var_a0 = size;
        }

        if (var_v1 < size) {
            var_v1 = size;
        }
    }
}

s32 _uvMemGetBlocksSize(void) {
    MemBlock* block;
    s32 blocksSize;

    block = gMemBlockHead;
    blocksSize = 0;

    while (block != NULL) {
        blocksSize += block->size;
        block = block->next;
    }

    return blocksSize;
}

void* _uvMemAllocAlign16(u32 size) {
   return _uvMemAlloc(size, 0x10U);
}

void uvMemFree(void *ptr) {
    _uvMemFree(ptr);
}
