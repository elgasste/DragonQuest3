#if !defined( MEM_ARENA_H )
#define MEM_ARENA_H

#include "common.h"

typedef struct MemArena_t MemArena_t;

typedef struct MemArenaStats_t
{
   size_t totalAllocatedBlocks;
   size_t totalAllocatedSpace;
   size_t largestAllocatedBlock;
   size_t largestAvailableBlock;
   size_t totalUnallocatedSpace;
   size_t totalFragments;
   size_t totalFragmentedSpace;
   size_t totalUnusableSpace;
}
MemArenaStats_t;

MemArena_t* MemArena_Create( size_t size );
void MemArena_Free( MemArena_t* arena );

size_t MemArena_GetSize( MemArena_t* arena );

void MemArena_Reset( MemArena_t* arena );
void* MemArena_AllocMem( MemArena_t* arena, size_t size );
void MemArena_FreeMem( MemArena_t* arena, void* mem );

MemArenaStats_t MemArena_GetStats( MemArena_t* arena );
b32 MemArena_IsEmpty( MemArena_t* arena );

#endif // MEM_ARENA_H
