#ifndef BAR_MEM_ALLOCATOR_H
#define BAR_MEM_ALLOCATOR_H
void *_uvMemAlloc(u32 size, u32 alignment);
void *_uvMemAllocAlign16(u32 size);
void *_uvMemAllocAlign8(u32 size);
void _uvMemAllocInit(void);
void _uvMemAllocInitStartUp(void);
s32 _uvMemGetBlocksSize(void);
void func_80002EAC(s32 arg0);
void uvMemFree(void *ptr);
void _uvMemFree(void *ptr);
#endif /* BAR_MEM_ALLOCATOR_H */
