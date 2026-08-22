#include <stdlib.h>

#include "mem_arena.h"
#include "unity.h"

global MemArenaStats_t g_initialStats;

void Platform_FatalError( const char* msg )
{
   UNUSED_PARAM( msg );
}

internal MemArena_t* CreateArena( void )
{
   return MemArena_Create( 1024 );
}

internal MemArena_t* CreateSmallArena( void )
{
   return MemArena_Create( 288 );
}

internal void AssertEmptyStats( MemArenaStats_t stats )
{
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 0, stats.largestAllocatedBlock );
   TEST_ASSERT_EQUAL_size_t( g_initialStats.largestAvailableBlock, stats.largestAvailableBlock );
   TEST_ASSERT_EQUAL_size_t( g_initialStats.totalUnallocatedSpace, stats.totalUnallocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragmentedSpace );
   TEST_ASSERT_EQUAL_size_t( g_initialStats.totalUnusableSpace, stats.totalUnusableSpace );
}

void setUp( void )
{
   MemArena_t* arena = CreateArena();
   g_initialStats = MemArena_GetStats( arena );
   MemArena_Free( arena );
}

void tearDown( void ) {}

void test_MemArena_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, MemArena_GetStructSize() );
}

void test_MemArena_Create_ReportsRequestedSizeAndIsEmpty( void )
{
   MemArena_t* arena = CreateArena();

   TEST_ASSERT_NOT_NULL( arena );
   TEST_ASSERT_EQUAL_size_t( 1024, MemArena_GetSize( arena ) );
   TEST_ASSERT_TRUE( MemArena_IsEmpty( arena ) );
   AssertEmptyStats( MemArena_GetStats( arena ) );

   MemArena_Free( arena );
}

void test_MemArena_AllocMem_AppendsBlocksAndReportsStatistics( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();
   void* firstMemory = MemArena_AllocMem( arena, 16 );
   void* secondMemory = MemArena_AllocMem( arena, 32 );

   TEST_ASSERT_NOT_NULL( firstMemory );
   TEST_ASSERT_NOT_NULL( secondMemory );
   TEST_ASSERT_TRUE( firstMemory != secondMemory );
   TEST_ASSERT_FALSE( MemArena_IsEmpty( arena ) );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL_size_t( 2, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 48, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 32, stats.largestAllocatedBlock );
   TEST_ASSERT_TRUE( stats.largestAvailableBlock < g_initialStats.largestAvailableBlock );
   TEST_ASSERT_TRUE( stats.totalUnallocatedSpace < g_initialStats.totalUnallocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragments );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragmentedSpace );
   TEST_ASSERT_EQUAL_size_t( g_initialStats.totalUnusableSpace, stats.totalUnusableSpace );

   MemArena_Free( arena );
}

void test_MemArena_AllocMem_ReusesFreedBlockSpace( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateSmallArena();
   void* firstMemory = MemArena_AllocMem( arena, 64 );
   void* freedMemory = MemArena_AllocMem( arena, 32 );
   void* lastMemory = MemArena_AllocMem( arena, 64 );
   void* replacementMemory;

   MemArena_FreeMem( arena, freedMemory );
   replacementMemory = MemArena_AllocMem( arena, 24 );

   TEST_ASSERT_NOT_NULL( replacementMemory );
   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL_size_t( 3, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 152, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 64, stats.largestAllocatedBlock );

   MemArena_FreeMem( arena, firstMemory );
   MemArena_FreeMem( arena, replacementMemory );
   MemArena_FreeMem( arena, lastMemory );
   MemArena_Free( arena );
}

void test_MemArena_AllocMem_UsesFreedSpaceBeforeAppending( void )
{
   MemArena_t* arena = CreateSmallArena();
   void* firstMemory = MemArena_AllocMem( arena, 32 );
   MemArena_AllocMem( arena, 32 );
   void* lastMemory = MemArena_AllocMem( arena, 64 );
   void* replacementMemory;

   MemArena_FreeMem( arena, firstMemory );
   replacementMemory = MemArena_AllocMem( arena, 16 );

   TEST_ASSERT_NOT_NULL( replacementMemory );
   TEST_ASSERT_EQUAL_size_t( 3, MemArena_GetStats( arena ).totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 112, MemArena_GetStats( arena ).totalAllocatedSpace );

   MemArena_FreeMem( arena, replacementMemory );
   MemArena_FreeMem( arena, lastMemory );
   MemArena_Free( arena );
}

void test_MemArena_AllocMem_AppendsAfterMultipleExistingBlocks( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();

   MemArena_AllocMem( arena, 8 );
   MemArena_AllocMem( arena, 16 );
   MemArena_AllocMem( arena, 32 );
   MemArena_AllocMem( arena, 64 );
   stats = MemArena_GetStats( arena );

   TEST_ASSERT_EQUAL_size_t( 4, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 120, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 64, stats.largestAllocatedBlock );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragments );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragmentedSpace );

   MemArena_Reset( arena );
   MemArena_Free( arena );
}

void test_MemArena_FreeMem_RemovesOnlyTheSelectedBlock( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();
   void* firstMemory = MemArena_AllocMem( arena, 16 );
   void* middleMemory = MemArena_AllocMem( arena, 32 );
   void* lastMemory = MemArena_AllocMem( arena, 64 );

   MemArena_FreeMem( arena, middleMemory );
   stats = MemArena_GetStats( arena );

   TEST_ASSERT_FALSE( MemArena_IsEmpty( arena ) );
   TEST_ASSERT_EQUAL_size_t( 2, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 80, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 64, stats.largestAllocatedBlock );
   TEST_ASSERT_EQUAL_size_t( 1, stats.totalFragments );
   TEST_ASSERT_EQUAL_size_t( 32, stats.totalFragmentedSpace );

   MemArena_FreeMem( arena, firstMemory );
   MemArena_FreeMem( arena, lastMemory );

   TEST_ASSERT_TRUE( MemArena_IsEmpty( arena ) );
   AssertEmptyStats( MemArena_GetStats( arena ) );
   MemArena_Free( arena );
}

void test_MemArena_FreeMem_MultipleBlocksUpdatesFirstAndLastLinks( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();
   void* firstMemory = MemArena_AllocMem( arena, 8 );
   void* middleMemory = MemArena_AllocMem( arena, 16 );
   void* lastMemory = MemArena_AllocMem( arena, 32 );

   MemArena_FreeMem( arena, firstMemory );
   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL_size_t( 2, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 48, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 32, stats.largestAllocatedBlock );

   MemArena_FreeMem( arena, lastMemory );
   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL_size_t( 1, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 16, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 16, stats.largestAllocatedBlock );

   MemArena_FreeMem( arena, middleMemory );
   TEST_ASSERT_TRUE( MemArena_IsEmpty( arena ) );
   AssertEmptyStats( MemArena_GetStats( arena ) );
   MemArena_Free( arena );
}

void test_MemArena_FreeMem_UpdatesEmptyStateWhenLastBlockIsFreed( void )
{
   MemArena_t* arena = CreateArena();
   void* memory = MemArena_AllocMem( arena, 1 );

   MemArena_FreeMem( arena, memory );

   TEST_ASSERT_TRUE( MemArena_IsEmpty( arena ) );
   AssertEmptyStats( MemArena_GetStats( arena ) );
   MemArena_Free( arena );
}

void test_MemArena_Reset_MakesAllocatedSpaceAvailableAgain( void )
{
   MemArena_t* arena = CreateArena();

   MemArena_AllocMem( arena, 16 );
   MemArena_AllocMem( arena, 32 );
   MemArena_Reset( arena );

   TEST_ASSERT_TRUE( MemArena_IsEmpty( arena ) );
   AssertEmptyStats( MemArena_GetStats( arena ) );
   MemArena_Free( arena );
}

void test_MemArena_GetStats_TracksLargestAllocatedBlock( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();

   MemArena_AllocMem( arena, 24 );
   MemArena_AllocMem( arena, 80 );
   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL_size_t( 80, stats.largestAllocatedBlock );

   MemArena_FreeMem( arena, MemArena_AllocMem( arena, 16 ) );
   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL_size_t( 80, stats.largestAllocatedBlock );

   MemArena_Reset( arena );
   TEST_ASSERT_EQUAL_size_t( 0, MemArena_GetStats( arena ).largestAllocatedBlock );
   MemArena_Free( arena );
}

void test_MemArena_GetStats_ReportsSingleBlockAndRemainingCapacity( void )
{
   MemArenaStats_t initialStats;
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();

   initialStats = MemArena_GetStats( arena );
   MemArena_AllocMem( arena, 40 );
   stats = MemArena_GetStats( arena );

   TEST_ASSERT_EQUAL_size_t( 1, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 40, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 40, stats.largestAllocatedBlock );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragments );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragmentedSpace );
   TEST_ASSERT_EQUAL_size_t( initialStats.totalUnusableSpace, stats.totalUnusableSpace );
   TEST_ASSERT_TRUE( stats.largestAvailableBlock < initialStats.largestAvailableBlock );
   TEST_ASSERT_TRUE( stats.totalUnallocatedSpace < initialStats.totalUnallocatedSpace );

   MemArena_Free( arena );
}

void test_MemArena_GetStats_ReportsLargestAvailableFragment( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateArena();
   void* firstMemory = MemArena_AllocMem( arena, 16 );
   void* middleMemory = MemArena_AllocMem( arena, 64 );
   void* lastMemory = MemArena_AllocMem( arena, 32 );

   MemArena_FreeMem( arena, firstMemory );
   MemArena_FreeMem( arena, middleMemory );
   stats = MemArena_GetStats( arena );

   TEST_ASSERT_EQUAL_size_t( 1, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 32, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 32, stats.largestAllocatedBlock );
   TEST_ASSERT_EQUAL_size_t( 1, stats.totalFragments );
   TEST_ASSERT_TRUE( stats.largestAvailableBlock >= 16 );
   TEST_ASSERT_TRUE( stats.totalFragmentedSpace > 0 );
   TEST_ASSERT_TRUE( stats.totalUnallocatedSpace > 0 );

   MemArena_FreeMem( arena, lastMemory );
   MemArena_Free( arena );
}

void test_MemArena_GetStats_ReportsUnusableSpace( void )
{
   MemArenaStats_t stats;
   MemArena_t* arena = CreateSmallArena();

   MemArena_AllocMem( arena, 64 );
   MemArena_AllocMem( arena, 32 );
   MemArena_AllocMem( arena, 64 );
   MemArena_AllocMem( arena, 48 );
   stats = MemArena_GetStats( arena );

   TEST_ASSERT_EQUAL_size_t( 4, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL_size_t( 208, stats.totalAllocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 64, stats.largestAllocatedBlock );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalFragments );
   TEST_ASSERT_EQUAL_size_t( 0, stats.totalUnallocatedSpace );
   TEST_ASSERT_EQUAL_size_t( 0, stats.largestAvailableBlock );
   TEST_ASSERT_TRUE( stats.totalUnusableSpace > 0 );

   MemArena_Free( arena );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_MemArena_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_MemArena_Create_ReportsRequestedSizeAndIsEmpty );

   RUN_TEST( test_MemArena_AllocMem_AppendsBlocksAndReportsStatistics );
   RUN_TEST( test_MemArena_AllocMem_ReusesFreedBlockSpace );
   RUN_TEST( test_MemArena_AllocMem_UsesFreedSpaceBeforeAppending );
   RUN_TEST( test_MemArena_AllocMem_AppendsAfterMultipleExistingBlocks );

   RUN_TEST( test_MemArena_FreeMem_RemovesOnlyTheSelectedBlock );
   RUN_TEST( test_MemArena_FreeMem_MultipleBlocksUpdatesFirstAndLastLinks );
   RUN_TEST( test_MemArena_FreeMem_UpdatesEmptyStateWhenLastBlockIsFreed );

   RUN_TEST( test_MemArena_Reset_MakesAllocatedSpaceAvailableAgain );
   
   RUN_TEST( test_MemArena_GetStats_TracksLargestAllocatedBlock );
   RUN_TEST( test_MemArena_GetStats_ReportsSingleBlockAndRemainingCapacity );
   RUN_TEST( test_MemArena_GetStats_ReportsLargestAvailableFragment );
   RUN_TEST( test_MemArena_GetStats_ReportsUnusableSpace );
   
   return UNITY_END();
}
