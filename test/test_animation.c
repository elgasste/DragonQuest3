#include <stdlib.h>

#include "mocks/mock_animation.h"
#include "animation.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   g_allocCount++;
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   g_freeCount++;
   free( mem );
}

void setUp( void )
{
   g_allocCount = 0;
   g_freeCount = 0;
}

void tearDown( void ) {}

void test_Animation_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, Animation_GetStructSize() );
}

void test_AnimationChain_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, AnimationChain_GetStructSize() );
}

void test_AnimationChain_Create_AllocatesChainAndAnimationArray( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 16 );

   TEST_ASSERT_NOT_NULL( chain );
   TEST_ASSERT_EQUAL_UINT( 2, g_allocCount );
   TEST_ASSERT_EQUAL_UINT( 16, chain->maxAnimations );
   TEST_ASSERT_NOT_NULL( chain->animations );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Create_StoresMaxAnimationsCorrectly( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 32 );

   TEST_ASSERT_EQUAL_UINT( 32, chain->maxAnimations );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Create_WithDifferentMaxValues( void )
{
   AnimationChain_t* chain1 = AnimationChain_Create( (MemArena_t*)1, 1 );
   TEST_ASSERT_EQUAL_UINT( 1, chain1->maxAnimations );
   AnimationChain_Free( chain1, (MemArena_t*)1 );

   g_allocCount = 0;
   g_freeCount = 0;

   AnimationChain_t* chain2 = AnimationChain_Create( (MemArena_t*)1, 255 );
   TEST_ASSERT_EQUAL_UINT( 255, chain2->maxAnimations );
   AnimationChain_Free( chain2, (MemArena_t*)1 );
}

void test_AnimationChain_Free_ReleasesMemory( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 10 );
   g_freeCount = 0;

   AnimationChain_Free( chain, (MemArena_t*)1 );

   TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

void test_AnimationChain_Create_InitializesAnimationPointer( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );

   TEST_ASSERT_NOT_NULL( chain->animations );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Animation_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_AnimationChain_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_AnimationChain_Create_AllocatesChainAndAnimationArray );
   RUN_TEST( test_AnimationChain_Create_StoresMaxAnimationsCorrectly );
   RUN_TEST( test_AnimationChain_Create_WithDifferentMaxValues );

   RUN_TEST( test_AnimationChain_Free_ReleasesMemory );
   
   RUN_TEST( test_AnimationChain_Create_InitializesAnimationPointer );

   return UNITY_END();
}
