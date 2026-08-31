#include <stdlib.h>

#include "mocks/mock_animation.h"
#include "animation.h"
#include "platform.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;
global u32 g_animationCallbackCount;
global u32 g_chainCallbackCount;
global void* g_callbackData;

void Platform_FatalError( const char* msg )
{
   UNUSED_PARAM( msg );
}

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
   g_animationCallbackCount = 0;
   g_chainCallbackCount = 0;
   g_callbackData = 0;
}

void tearDown( void ) {}

void AnimationFinishedCallback( void* callbackData )
{
   g_animationCallbackCount++;
   g_callbackData = callbackData;
}

void ChainFinishedCallback( void* callbackData )
{
   g_chainCallbackCount++;
   g_callbackData = callbackData;
}

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

void test_AnimationChain_Create_InitializesState( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );

   TEST_ASSERT_EQUAL_UINT( 8, AnimationChain_GetMaxAnimations( chain ) );
   TEST_ASSERT_EQUAL_UINT( 0, AnimationChain_GetCount( chain ) );
   TEST_ASSERT_EQUAL_FLOAT( False, AnimationChain_GetIsRunning( chain ) );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Push_StoresAnimation( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );
   void* callbackData = (void*)42;

   AnimationChain_Push( chain, AnimationType_FadeOut, 2.5f, AnimationFinishedCallback, callbackData );

   TEST_ASSERT_EQUAL_UINT( 1, AnimationChain_GetCount( chain ) );
   TEST_ASSERT_EQUAL_INT( AnimationType_FadeOut, chain->animations[0].type );
   TEST_ASSERT_EQUAL_FLOAT( 2.5f, chain->animations[0].duration );
   TEST_ASSERT_EQUAL_FLOAT( 0.0f, chain->animations[0].elapsed );
   TEST_ASSERT_EQUAL_PTR( AnimationFinishedCallback, chain->animations[0].finishedCallback );
   TEST_ASSERT_EQUAL_PTR( callbackData, chain->animations[0].callbackData );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Start_SetsRunningAndCallback( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );
   void* callbackData = (void*)42;

   AnimationChain_Start( chain, ChainFinishedCallback, callbackData );

   TEST_ASSERT_EQUAL_FLOAT( True, AnimationChain_GetIsRunning( chain ) );
   TEST_ASSERT_EQUAL_PTR( ChainFinishedCallback, chain->finishedCallback );
   TEST_ASSERT_EQUAL_PTR( callbackData, chain->callbackData );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Tic_AdvancesAndFinishesAnimations( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );
   void* callbackData = (void*)42;

   AnimationChain_Push( chain, AnimationType_Pause, 1.0f, AnimationFinishedCallback, callbackData );
   AnimationChain_Push( chain, AnimationType_FadeIn, 2.0f, AnimationFinishedCallback, callbackData );
   AnimationChain_Start( chain, ChainFinishedCallback, callbackData );

   AnimationChain_Tic( chain, 0.5f );
   TEST_ASSERT_EQUAL_FLOAT( 0.5f, chain->animations[0].elapsed );
   TEST_ASSERT_EQUAL_UINT( 0, g_animationCallbackCount );
   TEST_ASSERT_EQUAL_FLOAT( True, AnimationChain_GetIsRunning( chain ) );

   AnimationChain_Tic( chain, 0.5f );
   TEST_ASSERT_EQUAL_UINT( 1, g_animationCallbackCount );
   TEST_ASSERT_EQUAL_UINT( 1, chain->curAnimation );
   TEST_ASSERT_EQUAL_FLOAT( True, AnimationChain_GetIsRunning( chain ) );

   AnimationChain_Tic( chain, 2.0f );
   TEST_ASSERT_EQUAL_UINT( 2, g_animationCallbackCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_chainCallbackCount );
   TEST_ASSERT_EQUAL_FLOAT( False, AnimationChain_GetIsRunning( chain ) );
   TEST_ASSERT_EQUAL_PTR( callbackData, g_callbackData );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Tic_DoesNothingWhenNotRunning( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );

   AnimationChain_Push( chain, AnimationType_Pause, 1.0f, AnimationFinishedCallback, 0 );
   AnimationChain_Tic( chain, 1.0f );

   TEST_ASSERT_EQUAL_FLOAT( 0.0f, chain->animations[0].elapsed );
   TEST_ASSERT_EQUAL_UINT( 0, g_animationCallbackCount );

   AnimationChain_Free( chain, (MemArena_t*)1 );
}

void test_AnimationChain_Reset_ClearsState( void )
{
   AnimationChain_t* chain = AnimationChain_Create( (MemArena_t*)1, 8 );

   AnimationChain_Push( chain, AnimationType_Pause, 1.0f, AnimationFinishedCallback, 0 );
   AnimationChain_Start( chain, ChainFinishedCallback, 0 );
   AnimationChain_Tic( chain, 1.0f );
   AnimationChain_Reset( chain );

   TEST_ASSERT_EQUAL_UINT( 0, AnimationChain_GetCount( chain ) );
   TEST_ASSERT_EQUAL_UINT( 0, chain->curAnimation );
   TEST_ASSERT_EQUAL_FLOAT( False, AnimationChain_GetIsRunning( chain ) );
   TEST_ASSERT_NULL( chain->finishedCallback );
   TEST_ASSERT_NULL( chain->callbackData );

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
   RUN_TEST( test_AnimationChain_Create_InitializesState );
   
   RUN_TEST( test_AnimationChain_Push_StoresAnimation );
   
   RUN_TEST( test_AnimationChain_Start_SetsRunningAndCallback );
   
   RUN_TEST( test_AnimationChain_Tic_AdvancesAndFinishesAnimations );
   RUN_TEST( test_AnimationChain_Tic_DoesNothingWhenNotRunning );
   
   RUN_TEST( test_AnimationChain_Reset_ClearsState );

   return UNITY_END();
}
