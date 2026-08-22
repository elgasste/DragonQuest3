#if !defined( MOCK_MEM_ARENA_H )
#define MOCK_MEM_ARENA_H

#include "../../include/mem_arena.h"

typedef struct MemArena_t
{
   u8* memory;
   u32 size;
   u32 used;
}
MemArena_t;

#endif // MOCK_MEM_ARENA_H
