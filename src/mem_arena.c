#include <stdlib.h>
#include <stdio.h>

#include "mem_arena.h"
#include "platform.h"

typedef struct MemArenaBlock_t MemArenaBlock_t;
struct MemArenaBlock_t
{
   // the size of the memory to be allocated, does not include the MemArenaBlock_t struct size
   size_t size;

   void* mem;
   MemArenaBlock_t* prev;
   MemArenaBlock_t* next;
};

struct MemArena_t
{
   // the entire size of the arena, including the MemArena_t struct
   size_t size;

   MemArenaBlock_t* firstBlock;
   MemArenaBlock_t* lastBlock;
};

internal void* MemArena_AllocTryAppend( MemArena_t* arena, size_t size );
internal void* MemArena_AllocTryInsert( MemArena_t* arena, size_t size );

MemArena_t* MemArena_Create( size_t size )
{
   MemArena_t *arena;

   // there should be enough space for at least one 1-byte block
   if ( size < ( sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1 ) )
   {
      Platform_FatalError( "requested arena size is too small" );
      return 0;
   }

   arena = (MemArena_t*)malloc( size );
   if ( !arena )
   {
      Platform_FatalError( "system memory allocation failed" );
      return 0;
   }

   arena->size = size;
   MemArena_Reset( arena );

   return arena;
}

void MemArena_Free( MemArena_t* arena )
{
   free( arena );
}

size_t MemArena_GetSize( MemArena_t* arena )
{
   return arena->size;
}

void MemArena_Reset( MemArena_t* arena )
{
   arena->firstBlock = 0;
   arena->lastBlock = 0;
}

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   void* mem;
   char msg[STRING_SIZE_DEFAULT];

   mem = MemArena_AllocTryAppend( arena, size );

   if ( !mem )
   {
      mem = MemArena_AllocTryInsert( arena, size );
   }

   if ( !mem )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "memory arena is out of memory" );
      Platform_FatalError( msg );
   }

   return mem;
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   MemArenaBlock_t* block;

   block = (MemArenaBlock_t*)( (u8*)mem - sizeof( MemArenaBlock_t ) );

   if ( block->prev )
      block->prev->next = block->next;
   if ( block->next )
      block->next->prev = block->prev;
   if ( arena->firstBlock == block )
      arena->firstBlock = block->next;
   if ( arena->lastBlock == block )
      arena->lastBlock = block->prev;
}

MemArenaStats_t MemArena_GetStats( MemArena_t* arena )
{
   MemArenaBlock_t* nextBlock;
   u8 *insertionPoint;
   size_t freeSize, smallestBlockSize, availableSize;
   MemArenaStats_t stats;

   stats.totalAllocatedBlocks = 0;
   stats.totalAllocatedSpace = 0;
   stats.largestAllocatedBlock = 0;
   stats.largestAvailableBlock = 0;
   stats.totalUnallocatedSpace = 0;
   stats.totalFragments = 0;
   stats.totalFragmentedSpace = 0;
   stats.totalUnusableSpace = 0;

   smallestBlockSize = sizeof( MemArenaBlock_t ) + 1;
   insertionPoint = (u8*)arena + sizeof( MemArena_t );
   nextBlock = arena->firstBlock;

   while ( 1 )
   {
      freeSize = nextBlock
         ? (u8*)nextBlock - insertionPoint
         : ( (u8*)arena + arena->size ) - insertionPoint;
      availableSize = ( freeSize < smallestBlockSize )
         ? 0
         : freeSize - sizeof( MemArenaBlock_t );

      stats.totalUnallocatedSpace += availableSize;

      if ( freeSize < smallestBlockSize )
      {
         stats.totalUnusableSpace += freeSize;
      }
      else if ( nextBlock )
      {
         stats.totalFragmentedSpace += availableSize;
         stats.totalFragments++;
      }

      if ( availableSize > stats.largestAvailableBlock )
      {
         stats.largestAvailableBlock = availableSize;
      }

      if ( nextBlock )
      {
         stats.totalAllocatedBlocks++;
         stats.totalAllocatedSpace += nextBlock->size;
         if ( nextBlock->size > stats.largestAllocatedBlock )
         {
            stats.largestAllocatedBlock = nextBlock->size;
         }
      }
      else
      {
         break;
      }

      insertionPoint = (u8*)nextBlock + sizeof( MemArenaBlock_t ) + nextBlock->size;
      nextBlock = nextBlock->next;
   }

   return stats;
}

b32 MemArena_IsEmpty( MemArena_t* arena )
{
   return ( arena->firstBlock == 0 && arena->lastBlock == 0 );
}

internal void* MemArena_AllocTryAppend( MemArena_t* arena, size_t size )
{
   size_t freeSize;
   MemArenaBlock_t* newBlock;

   freeSize = arena->lastBlock
      ? ( (u8*)arena + arena->size ) - ( u8* )( arena->lastBlock ) - sizeof( MemArenaBlock_t ) - arena->lastBlock->size
      : arena->size - sizeof( MemArena_t );

   if ( freeSize < ( size + sizeof( MemArenaBlock_t ) ) )
   {
      return 0;
   }

   if ( arena->lastBlock )
   {
      newBlock = (MemArenaBlock_t*)( (u8*)( arena->lastBlock ) + sizeof( MemArenaBlock_t ) + arena->lastBlock->size );
      arena->lastBlock->next = newBlock;
      newBlock->prev = arena->lastBlock;
      arena->lastBlock = newBlock;
   }
   else
   {
      newBlock = (MemArenaBlock_t*)( (u8*)arena + sizeof( MemArena_t ) );
      arena->firstBlock = newBlock;
      arena->lastBlock = newBlock;
      newBlock->prev = 0;
   }

   newBlock->next = 0;
   newBlock->size = size;
   newBlock->mem = (u8*)newBlock + sizeof( MemArenaBlock_t );
   return newBlock->mem;
}

internal void* MemArena_AllocTryInsert( MemArena_t* arena, size_t size )
{
   MemArenaBlock_t *nextBlock, *prevBlock, *newBlock;
   u8 *insertionPoint;
   size_t freeSize;

   insertionPoint = (u8*)arena + sizeof( MemArena_t );
   nextBlock = arena->firstBlock;
   prevBlock = 0;

   while ( 1 )
   {
      freeSize = nextBlock
         ? (u8*)nextBlock - insertionPoint
         : ( (u8*)arena + arena->size ) - insertionPoint;

      if ( freeSize >= ( size + sizeof( MemArenaBlock_t ) ) )
      {
         newBlock = (MemArenaBlock_t*)insertionPoint;
         newBlock->size = size;
         newBlock->mem = (u8*)newBlock + sizeof( MemArenaBlock_t );
         newBlock->prev = prevBlock;
         newBlock->next = nextBlock;

         if ( prevBlock )
         {
            prevBlock->next = newBlock;
            if ( arena->lastBlock == prevBlock )
               arena->lastBlock = newBlock;
         }
         else
            arena->firstBlock = newBlock;

         if ( nextBlock )
         {
            nextBlock->prev = newBlock;
            if ( arena->firstBlock == nextBlock )
               arena->firstBlock = newBlock;
         }
         else
            arena->lastBlock = newBlock;

         return newBlock->mem;
      }
      
      if ( !nextBlock )
      {
         break;
      }
      
      insertionPoint = (u8*)nextBlock + sizeof( MemArenaBlock_t ) + nextBlock->size;
      prevBlock = nextBlock;
      nextBlock = nextBlock->next;
   }

   return 0;
}
