#include "mem_arena.h"
#include "unity.h"

static int g_fatalErrorCount;
static const char* g_fatalErrorMessage;

void Platform_FatalError( const char* msg )
{
   g_fatalErrorCount++;
   g_fatalErrorMessage = msg;
}

#define MEMARENA_TEST_HELPER_CREATE_ARENA( s ) \
   result = MemArena_Create( &arena, ( s ) ); \
   TEST_ASSERT_EQUAL( MemArenaResult_Success, result )

#define MEMARENA_TEST_HELPER_ALLOC( m, s ) \
   m = 0; \
   MemArena_Alloc( arena, &( m ), ( s ) ); \
   TEST_ASSERT_NOT_NULL( m )

internal MemArena_t* MemArenaTestHelper_CreateArenaWithBlockAtOffset( size_t arenaSize,
                                                                      size_t blockOffset,
                                                                      size_t blockSize )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   MemArenaBlock_t* block;

   MEMARENA_TEST_HELPER_CREATE_ARENA( arenaSize );

   block = (MemArenaBlock_t*)( (u8*)arena + blockOffset );
   arena->firstBlock = block;
   arena->lastBlock = block;
   block->prev = 0;
   block->next = 0;
   block->size = blockSize;
   block->mem = (u8*)block + sizeof( MemArenaBlock_t );

   return arena;
}

internal MemArena_t* MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( size_t arenaSize,
                                                                           size_t blockOffset1,
                                                                           size_t blockSize1,
                                                                           size_t blockOffset2,
                                                                           size_t blockSize2 )
{
   MemArena_t* arena;
   MemArenaBlock_t *block1, *block2;

   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, blockOffset1, blockSize1 );

   block1 = arena->firstBlock;
   block2 = (MemArenaBlock_t*)( (u8*)arena + blockOffset2 );
   block1->next = block2;
   block2->prev = block1;
   arena->lastBlock = block2;
   block2->next = 0;
   block2->size = blockSize2;
   block2->mem = (u8*)block2 + sizeof( MemArenaBlock_t );

   return arena;
}

void setUp( void ) {}
void tearDown( void ) {}

internal void test_MemArena_Create_ArenaTooSmall_ReturnsArenaTooSmall( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;

   result = MemArena_Create( &arena, sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) );
   TEST_ASSERT_EQUAL( MemArenaResult_ArenaTooSmall, result );
}

internal void test_MemArena_Create_ArenaMinSize_ReturnsSuccess( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;

   result = MemArena_Create( &arena, sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1 );
   TEST_ASSERT_EQUAL( MemArenaResult_Success, result );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Create_SetsCorrectParameters( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );
   TEST_ASSERT_EQUAL( arena->size, 1000 );
   TEST_ASSERT_EQUAL( arena->firstBlock, 0 );
   TEST_ASSERT_EQUAL( arena->lastBlock, 0 );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Destroy_CleansUpArena( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;

   arena = 0;
   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );
   TEST_ASSERT_NOT_NULL( arena );

   MemArena_Destroy( &arena );
   TEST_ASSERT_NULL( arena );
}

internal void test_MemArena_Reset_ResetsBlockPointers( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );
   MEMARENA_TEST_HELPER_ALLOC( mem1, 10 );

   MEMARENA_TEST_HELPER_ALLOC( mem2, 10 );
   TEST_ASSERT_EQUAL( mem1, arena->firstBlock->mem );
   TEST_ASSERT_EQUAL( mem2, arena->lastBlock->mem );

   MemArena_Reset( arena );
   TEST_ASSERT_NULL( arena->firstBlock );
   TEST_ASSERT_NULL( arena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Free_DisposingFirstBlockAdjustsPrevAndNextPointers( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2, *mem3;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );

   MEMARENA_TEST_HELPER_ALLOC( mem1, 10 );
   MEMARENA_TEST_HELPER_ALLOC( mem2, 20 );
   MEMARENA_TEST_HELPER_ALLOC( mem3, 30 );
   TEST_ASSERT_EQUAL( mem1, arena->firstBlock->mem );
   TEST_ASSERT_EQUAL( mem2, arena->firstBlock->next->mem );
   TEST_ASSERT_EQUAL( mem3, arena->lastBlock->mem );

   MemArena_Free( arena, mem1 );
   TEST_ASSERT_EQUAL( arena->lastBlock, arena->firstBlock->next );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock->prev );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( mem2 - sizeof( MemArenaBlock_t ), arena->firstBlock );
   TEST_ASSERT_EQUAL( mem3 - sizeof( MemArenaBlock_t ), arena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Free_DisposingCenterBlockAdjustsPrevAndNextPointers( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2, *mem3;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );

   MEMARENA_TEST_HELPER_ALLOC( mem1, 10 );
   MEMARENA_TEST_HELPER_ALLOC( mem2, 20 );
   MEMARENA_TEST_HELPER_ALLOC( mem3, 30 );
   TEST_ASSERT_EQUAL( mem1, arena->firstBlock->mem );
   TEST_ASSERT_EQUAL( mem2, arena->firstBlock->next->mem );
   TEST_ASSERT_EQUAL( mem3, arena->lastBlock->mem );

   MemArena_Free( arena, mem2 );
   TEST_ASSERT_EQUAL( arena->lastBlock, arena->firstBlock->next );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock->prev );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( mem1 - sizeof( MemArenaBlock_t ), arena->firstBlock );
   TEST_ASSERT_EQUAL( mem3 - sizeof( MemArenaBlock_t ), arena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Free_DisposingLastBlockAdjustsPrevAndNextPointers( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2, *mem3;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );

   MEMARENA_TEST_HELPER_ALLOC( mem1, 10 );
   MEMARENA_TEST_HELPER_ALLOC( mem2, 20 );
   MEMARENA_TEST_HELPER_ALLOC( mem3, 30 );
   TEST_ASSERT_EQUAL( mem1, arena->firstBlock->mem );
   TEST_ASSERT_EQUAL( mem2, arena->firstBlock->next->mem );
   TEST_ASSERT_EQUAL( mem3, arena->lastBlock->mem );

   MemArena_Free( arena, mem3 );
   TEST_ASSERT_EQUAL( arena->lastBlock, arena->firstBlock->next );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock->prev );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( mem1 - sizeof( MemArenaBlock_t ), arena->firstBlock );
   TEST_ASSERT_EQUAL( mem2 - sizeof( MemArenaBlock_t ), arena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_NoBlocksAllocatedWithSpaceAvailable_AllocatesBlock( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );
   TEST_ASSERT_NULL( arena->firstBlock );

   MEMARENA_TEST_HELPER_ALLOC( mem, 20 );
   TEST_ASSERT_NOT_NULL( mem );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_NoBlocksAllocatedWithNoSpaceAvailable_DoesNotAllocateBlock( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );
   TEST_ASSERT_NULL( arena->firstBlock );

   g_fatalErrorCount = 0;
   g_fatalErrorMessage = 0;
   mem = 0;
   MemArena_Alloc( arena, &mem, 1000 );
   TEST_ASSERT_NULL( mem );
   TEST_ASSERT_EQUAL( 1, g_fatalErrorCount );
   TEST_ASSERT_NOT_NULL( g_fatalErrorMessage );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_OneImmediateBlockPresentWithSpaceAvailable_AllocatesBlock( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;

   MEMARENA_TEST_HELPER_CREATE_ARENA( 1000 );

   MEMARENA_TEST_HELPER_ALLOC( mem, 10 );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ), (u8*)( arena->firstBlock ) );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, mem );
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem );

   MEMARENA_TEST_HELPER_ALLOC( mem, 10 );
   TEST_ASSERT_NOT_EQUAL( arena->firstBlock->mem, mem );
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock );
   TEST_ASSERT_NULL( arena->lastBlock->next );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_OneImmediateBlockPresentWithNoSpaceAvailable_DoesNotAllocateBlock( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;

   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( sizeof( MemArenaBlock_t ) * 2 ) + 19 );

   MEMARENA_TEST_HELPER_ALLOC( mem, 10 );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ), (u8*)( arena->firstBlock ) );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, mem );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( arena->lastBlock->next );

   g_fatalErrorCount = 0;
   g_fatalErrorMessage = 0;
   mem = 0;
   MemArena_Alloc( arena, &mem, 10 );
   TEST_ASSERT_NULL( mem );
   TEST_ASSERT_EQUAL( 1, g_fatalErrorCount );
   TEST_ASSERT_NOT_NULL( g_fatalErrorMessage );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_OneOffsetBlockPresentWithPrecedingSpaceAvailable_InsertsBlock( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset;
   u8* mem;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 ) - 1;
   blockOffset = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + blockSize;
   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, blockOffset, blockSize );
   
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ), mem );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, mem );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, mem );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_OneOffsetBlockPresentWithSpaceAvailableAfter_AppendsBlock( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset;
   u8* mem;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 ) - 1;
   blockOffset = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + blockSize - 1;
   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, blockOffset, blockSize );

   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + blockOffset + ( sizeof( MemArenaBlock_t ) + blockSize ) + sizeof( MemArenaBlock_t ), mem);
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_OneOffsetBlockPresentWithNoSpaceAvailableOnEitherSide_DoesNotAllocateBlock( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset;
   u8* mem;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 ) - 2;
   blockOffset = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + blockSize - 1;
   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, blockOffset, blockSize );

   g_fatalErrorCount = 0;
   g_fatalErrorMessage = 0;
   mem = 0;
   MemArena_Alloc( arena, &mem, blockSize );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock );
   TEST_ASSERT_NULL( arena->firstBlock->next );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( mem );
   TEST_ASSERT_EQUAL( 1, g_fatalErrorCount );
   TEST_ASSERT_NOT_NULL( g_fatalErrorMessage );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_TwoBlocksPresentWithSpaceBetween_InsertsBlock( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset1, blockOffset2;
   u8* mem;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 );
   blockOffset1 = sizeof( MemArena_t );
   blockOffset2 = blockOffset1 + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 );
   arena = MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( arenaSize, blockOffset1, blockSize, blockOffset2, blockSize );

   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + ( sizeof( MemArenaBlock_t ) * 2 ) + blockSize, mem );
   TEST_ASSERT_EQUAL( arena->firstBlock->next->mem, mem );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock->next );
   TEST_ASSERT_EQUAL( arena->firstBlock->next->prev, arena->firstBlock );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev->next, arena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_TwoBlocksPresentWithNoSpaceBetweenOrAfter_DoesNotAllocateBlock( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset1, blockOffset2;
   u8* mem;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 );
   blockOffset1 = sizeof( MemArena_t ) + 1;
   blockOffset2 = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 );
   arena = MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( arenaSize, blockOffset1, blockSize, blockOffset2, blockSize );

   g_fatalErrorCount = 0;
   g_fatalErrorMessage = 0;
   mem = 0;
   MemArena_Alloc( arena, &mem, blockSize );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( mem );
   TEST_ASSERT_EQUAL( 1, g_fatalErrorCount );
   TEST_ASSERT_NOT_NULL( g_fatalErrorMessage );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_Alloc_TwoBlocksPresentWithNoSpaceBetweenButSpaceAfter_AppendsBlock( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset1, blockOffset2;
   u8* mem;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 );
   blockOffset1 = sizeof( MemArena_t );
   blockOffset2 = blockOffset1 + sizeof( MemArenaBlock_t ) + blockSize;
   arena = MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( arenaSize, blockOffset1, blockSize, blockOffset2, blockSize );

   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) + sizeof( MemArenaBlock_t ), mem );
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem );
   TEST_ASSERT_NULL( arena->firstBlock->prev );
   TEST_ASSERT_NULL( arena->lastBlock->next );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock->next );
   TEST_ASSERT_EQUAL( arena->firstBlock->next->prev, arena->firstBlock );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev->next, arena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_AllocSubArena_AllocatesAndInitializesMemArena( void )
{
   MemArena_t *arena, *subArena;
   MemArenaResult_t result;
   size_t arenaSize, subArenaSize;

   arenaSize = 1000;
   MEMARENA_TEST_HELPER_CREATE_ARENA( arenaSize );

   subArenaSize = 500;
   subArena = 0;
   MemArena_AllocSubArena( arena, &subArena, subArenaSize );
   TEST_ASSERT_NOT_NULL( subArena );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ), subArena );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, subArena );
   TEST_ASSERT_EQUAL( subArenaSize, subArena->size );
   TEST_ASSERT_NULL( subArena->firstBlock );
   TEST_ASSERT_NULL( subArena->lastBlock );

   MemArena_Destroy( &arena );
}

internal void test_memArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalAllocatedBlocks( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalAllocatedBlocks );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalAllocatedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalAllocatedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForLargestAllocatedBlock( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.largestAllocatedBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_BlocksExistReturnsTotalAllocatedBlocks( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 2, stats.totalAllocatedBlocks );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_BlocksExistReturnsTotalSizeOfAllBlocksForTotalAllocatedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( blockSize * 2, stats.totalAllocatedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_BlocksExistReturnsLargestAllocatedBlock( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;
   size_t blockSize1, blockSize2;
   MemArenaStats_t stats;

   blockSize1 = 100;
   blockSize2 = 200;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize1 ) + ( sizeof( MemArenaBlock_t ) + blockSize2 ) ) );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize1 );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize2 );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( blockSize2, stats.largestAllocatedBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_EmptyArenaReturnsCorrectLargestAvailableBlockSize( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( ( blockSize * 2 ) + sizeof( MemArenaBlock_t ), stats.largestAvailableBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_SpaceAvailableBeforeFirstBlockIsCountedInLargestAvailableBlockSize( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );
   MEMARENA_TEST_HELPER_ALLOC( mem1, blockSize );
   MEMARENA_TEST_HELPER_ALLOC( mem2, blockSize );

   MemArena_Free( arena, mem1 );
   TEST_ASSERT_EQUAL( mem2, arena->firstBlock->mem );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + blockSize, arena->firstBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( blockSize, stats.largestAvailableBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_SpaceBetweenBlocksIsCountedInLargestAvailableBlockSize( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2, *mem3;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 ) );
   MEMARENA_TEST_HELPER_ALLOC( mem1, blockSize );
   MEMARENA_TEST_HELPER_ALLOC( mem2, blockSize );
   MEMARENA_TEST_HELPER_ALLOC( mem3, blockSize );

   MemArena_Free( arena, mem2 );
   TEST_ASSERT_EQUAL( mem3, arena->firstBlock->next->mem );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ), arena->lastBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( blockSize, stats.largestAvailableBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_SpaceAfterLastBlockIsCountedInLargestAvailableBlockSize( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8* mem;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) );
   MEMARENA_TEST_HELPER_ALLOC( mem, blockSize );
   TEST_ASSERT_EQUAL( arena->firstBlock, (u8*)arena + sizeof( MemArena_t ) );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( blockSize, stats.largestAvailableBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_VaryingSizesFoundReturnsLargestAvailableBlockSize( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2, *mem3, *mem4, *mem5;
   size_t size1, size2, size3, size4, size5;
   MemArenaStats_t stats;

   size1 = 40;
   size2 = 90;
   size3 = 150;
   size4 = 35;
   size5 = 26;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( sizeof( MemArenaBlock_t ) * 5 ) + size1 + size2 + size3 + size4 + size5 );
   MEMARENA_TEST_HELPER_ALLOC( mem1, size1 );
   MEMARENA_TEST_HELPER_ALLOC( mem2, size2 );
   MEMARENA_TEST_HELPER_ALLOC( mem3, size3 );
   MEMARENA_TEST_HELPER_ALLOC( mem4, size4 );
   MEMARENA_TEST_HELPER_ALLOC( mem5, size5 );

   MemArena_Free( arena, mem1 );
   MemArena_Free( arena, mem3 );
   MemArena_Free( arena, mem5 );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, mem2 );
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem4 );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock );
   TEST_ASSERT_EQUAL( size2, arena->firstBlock->size );
   TEST_ASSERT_EQUAL( size4, arena->lastBlock->size );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( size3, stats.largestAvailableBlock );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_NoBlocksAllocatedReturnsEntireArenaInTotalUnallocatedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   MemArenaStats_t stats;

   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1000 );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 1000, stats.totalUnallocatedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_OneOffsetBlockIncludesSpaceBeforeBlockInTotalUnallocatedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2;
   size_t blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 3 ) );
   MEMARENA_TEST_HELPER_ALLOC( mem1, blockSize );
   MEMARENA_TEST_HELPER_ALLOC( mem2, blockSize );

   MemArena_Free( arena, mem1 );
   TEST_ASSERT_EQUAL( mem2, arena->firstBlock->mem );
   TEST_ASSERT_EQUAL( arena->firstBlock, arena->lastBlock );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + blockSize, arena->firstBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( blockSize * 2, stats.totalUnallocatedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_MultipleBlocksIncludesAllEmptySpaceInTotalUnallocatedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   u8 *mem1, *mem2, *mem3, *mem4, *mem5;
   size_t size1, size2, size3, size4, size5;
   MemArenaStats_t stats;

   size1 = 40;
   size2 = 90;
   size3 = 150;
   size4 = 35;
   size5 = 26;
   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + ( sizeof( MemArenaBlock_t ) * 5 ) + size1 + size2 + size3 + size4 + size5 );
   MEMARENA_TEST_HELPER_ALLOC( mem1, size1 );
   MEMARENA_TEST_HELPER_ALLOC( mem2, size2 );
   MEMARENA_TEST_HELPER_ALLOC( mem3, size3 );
   MEMARENA_TEST_HELPER_ALLOC( mem4, size4 );
   MEMARENA_TEST_HELPER_ALLOC( mem5, size5 );

   MemArena_Free( arena, mem1 );
   MemArena_Free( arena, mem3 );
   MemArena_Free( arena, mem5 );
   TEST_ASSERT_EQUAL( arena->firstBlock->mem, mem2 );
   TEST_ASSERT_EQUAL( arena->lastBlock->mem, mem4 );
   TEST_ASSERT_EQUAL( arena->firstBlock->next, arena->lastBlock );
   TEST_ASSERT_EQUAL( arena->lastBlock->prev, arena->firstBlock );
   TEST_ASSERT_EQUAL( size2, arena->firstBlock->size );
   TEST_ASSERT_EQUAL( size4, arena->lastBlock->size );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( size1 + size3 + size5, stats.totalUnallocatedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalFragmentedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   MemArenaStats_t stats;

   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1 );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalFragmentedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalFragments( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   MemArenaStats_t stats;

   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1 );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalFragments );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_OneGapBeforeFirstBlockReturnsOneForTotalFragments( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( sizeof( MemArenaBlock_t ) + blockSize ) + ( sizeof( MemArenaBlock_t ) + 10 ) + 25;
   blockOffset = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 10;
   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, blockOffset, blockSize );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 1, stats.totalFragments );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_GapsBeforeAndBetweenBlocksReturnTwoForTotalFragments( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset1, blockOffset2;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) + ( ( sizeof( MemArenaBlock_t ) + 10 ) * 2 ) + 5;
   blockOffset1 = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 10;
   blockOffset2 = blockOffset1 + sizeof( MemArenaBlock_t ) + blockSize + sizeof( MemArenaBlock_t ) + 10;
   arena = MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( arenaSize, blockOffset1, blockSize, blockOffset2, blockSize );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 2, stats.totalFragments );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_UnusableGapBeforeFirstBlockIsNotCountedAsFragment( void )
{
   MemArena_t* arena;
   size_t arenaSize, blockSize, blockOffset;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( sizeof( MemArenaBlock_t ) + blockSize ) + sizeof( MemArenaBlock_t );
   blockOffset = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t );
   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, blockOffset, blockSize );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalFragments );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_OneOffsetBlockWithSpaceBeforeBlockIncludesSpaceInTotalFragmentedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t arenaSize, blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 );
   MEMARENA_TEST_HELPER_CREATE_ARENA( arenaSize );

   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1, arena->firstBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 1, stats.totalFragmentedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_MultipleFragmentsReturnsTotalCountForTotalFragmentedSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t arenaSize, blockSize, blockOffset1, blockOffset2;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) + ( ( sizeof( MemArenaBlock_t ) + 10 ) * 2 );
   blockOffset1 = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 10;
   blockOffset2 = blockOffset1 + ( sizeof( MemArenaBlock_t ) * 2 ) + blockSize + 10;
   MEMARENA_TEST_HELPER_CREATE_ARENA( arenaSize );

   arena = MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( arenaSize, blockOffset1, blockSize, blockOffset2, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + blockOffset1, arena->firstBlock );
   TEST_ASSERT_EQUAL( (u8*)arena + blockOffset2, arena->lastBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 20, stats.totalFragmentedSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalUnusableSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   MemArenaStats_t stats;

   MEMARENA_TEST_HELPER_CREATE_ARENA( sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ) + 1 );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalUnusableSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_OneOffsetBlockWithSpaceBeforeBlockIncludesSpaceInTotalUnusableSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t arenaSize, blockSize;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 );
   MEMARENA_TEST_HELPER_CREATE_ARENA( arenaSize );

   arena = MemArenaTestHelper_CreateArenaWithBlockAtOffset( arenaSize, sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ), blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + sizeof( MemArena_t ) + sizeof( MemArenaBlock_t ), arena->firstBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( sizeof( MemArenaBlock_t ), stats.totalUnusableSpace );

   MemArena_Destroy( &arena );
}

internal void test_MemArena_GetStats_MultipleUnusableFragmentsReturnsTotalCountForTotalUnusableSpace( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   size_t arenaSize, blockSize, blockOffset1, blockOffset2;
   MemArenaStats_t stats;

   blockSize = 100;
   arenaSize = sizeof( MemArena_t ) + ( ( sizeof( MemArenaBlock_t ) + blockSize ) * 2 ) + ( sizeof( MemArenaBlock_t ) * 3 );
   blockOffset1 = sizeof( MemArena_t ) + sizeof( MemArenaBlock_t );
   blockOffset2 = blockOffset1 + ( sizeof( MemArenaBlock_t ) * 2 ) + blockSize;
   MEMARENA_TEST_HELPER_CREATE_ARENA( arenaSize );

   arena = MemArenaTestHelper_CreateArenaWithTwoBlocksAtOffsets( arenaSize, blockOffset1, blockSize, blockOffset2, blockSize );
   TEST_ASSERT_EQUAL( (u8*)arena + blockOffset1, arena->firstBlock );
   TEST_ASSERT_EQUAL( (u8*)arena + blockOffset2, arena->lastBlock );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( sizeof( MemArenaBlock_t ) * 3, stats.totalUnusableSpace );

   MemArena_Destroy( &arena );
}

int main( void )
{
   UNITY_BEGIN();
   
   RUN_TEST( test_MemArena_Create_ArenaTooSmall_ReturnsArenaTooSmall );
   RUN_TEST( test_MemArena_Create_ArenaMinSize_ReturnsSuccess );
   RUN_TEST( test_MemArena_Create_SetsCorrectParameters );

   RUN_TEST( test_MemArena_Destroy_CleansUpArena );

   RUN_TEST( test_MemArena_Reset_ResetsBlockPointers );

   RUN_TEST( test_MemArena_Free_DisposingFirstBlockAdjustsPrevAndNextPointers );
   RUN_TEST( test_MemArena_Free_DisposingCenterBlockAdjustsPrevAndNextPointers );
   RUN_TEST( test_MemArena_Free_DisposingLastBlockAdjustsPrevAndNextPointers );

   RUN_TEST( test_MemArena_Alloc_NoBlocksAllocatedWithSpaceAvailable_AllocatesBlock );
   RUN_TEST( test_MemArena_Alloc_NoBlocksAllocatedWithNoSpaceAvailable_DoesNotAllocateBlock );
   RUN_TEST( test_MemArena_Alloc_OneImmediateBlockPresentWithSpaceAvailable_AllocatesBlock );
   RUN_TEST( test_MemArena_Alloc_OneImmediateBlockPresentWithNoSpaceAvailable_DoesNotAllocateBlock );
   RUN_TEST( test_MemArena_Alloc_OneOffsetBlockPresentWithPrecedingSpaceAvailable_InsertsBlock );
   RUN_TEST( test_MemArena_Alloc_OneOffsetBlockPresentWithSpaceAvailableAfter_AppendsBlock );
   RUN_TEST( test_MemArena_Alloc_OneOffsetBlockPresentWithNoSpaceAvailableOnEitherSide_DoesNotAllocateBlock );
   RUN_TEST( test_MemArena_Alloc_TwoBlocksPresentWithSpaceBetween_InsertsBlock );
   RUN_TEST( test_MemArena_Alloc_TwoBlocksPresentWithNoSpaceBetweenOrAfter_DoesNotAllocateBlock );
   RUN_TEST( test_MemArena_Alloc_TwoBlocksPresentWithNoSpaceBetweenButSpaceAfter_AppendsBlock );

   RUN_TEST( test_MemArena_AllocSubArena_AllocatesAndInitializesMemArena );

   RUN_TEST( test_memArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalAllocatedBlocks );
   RUN_TEST( test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalAllocatedSpace );
   RUN_TEST( test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForLargestAllocatedBlock );
   RUN_TEST( test_MemArena_GetStats_BlocksExistReturnsTotalAllocatedBlocks );
   RUN_TEST( test_MemArena_GetStats_BlocksExistReturnsTotalSizeOfAllBlocksForTotalAllocatedSpace );
   RUN_TEST( test_MemArena_GetStats_BlocksExistReturnsLargestAllocatedBlock );
   RUN_TEST( test_MemArena_GetStats_EmptyArenaReturnsCorrectLargestAvailableBlockSize );
   RUN_TEST( test_MemArena_GetStats_SpaceAvailableBeforeFirstBlockIsCountedInLargestAvailableBlockSize );
   RUN_TEST( test_MemArena_GetStats_SpaceBetweenBlocksIsCountedInLargestAvailableBlockSize );
   RUN_TEST( test_MemArena_GetStats_SpaceAfterLastBlockIsCountedInLargestAvailableBlockSize );
   RUN_TEST( test_MemArena_GetStats_VaryingSizesFoundReturnsLargestAvailableBlockSize );
   RUN_TEST( test_MemArena_GetStats_NoBlocksAllocatedReturnsEntireArenaInTotalUnallocatedSpace );
   RUN_TEST( test_MemArena_GetStats_OneOffsetBlockIncludesSpaceBeforeBlockInTotalUnallocatedSpace );
   RUN_TEST( test_MemArena_GetStats_MultipleBlocksIncludesAllEmptySpaceInTotalUnallocatedSpace );
   RUN_TEST( test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalFragments );
   RUN_TEST( test_MemArena_GetStats_OneGapBeforeFirstBlockReturnsOneForTotalFragments );
   RUN_TEST( test_MemArena_GetStats_GapsBeforeAndBetweenBlocksReturnTwoForTotalFragments );
   RUN_TEST( test_MemArena_GetStats_UnusableGapBeforeFirstBlockIsNotCountedAsFragment );
   RUN_TEST( test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalFragmentedSpace );
   RUN_TEST( test_MemArena_GetStats_OneOffsetBlockWithSpaceBeforeBlockIncludesSpaceInTotalFragmentedSpace );
   RUN_TEST( test_MemArena_GetStats_MultipleFragmentsReturnsTotalCountForTotalFragmentedSpace );
   RUN_TEST( test_MemArena_GetStats_NoBlocksAllocatedReturnsZeroForTotalUnusableSpace );
   RUN_TEST( test_MemArena_GetStats_OneOffsetBlockWithSpaceBeforeBlockIncludesSpaceInTotalUnusableSpace );
   RUN_TEST( test_MemArena_GetStats_MultipleUnusableFragmentsReturnsTotalCountForTotalUnusableSpace );
   
   return UNITY_END();
}
