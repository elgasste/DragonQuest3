#include <stdlib.h>

#include "direction.h"
#include "mem_arena.h"
#include "sprite.h"
#include "sprite_texture_set.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;
global u32 g_frameCount;

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

u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet )
{
	UNUSED_PARAM( textureSet );
	return g_frameCount;
}

void setUp( void )
{
	g_allocCount = 0;
	g_freeCount = 0;
	g_frameCount = 1;
}

void tearDown( void ) {}

void test_ActiveSprite_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, ActiveSprite_GetStructSize() );
}

void test_ActiveSprite_Create_InitializesDefaultState( void )
{
	ActiveSpriteTextureSet_t* textureSet = (ActiveSpriteTextureSet_t*)1;
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, textureSet );

	TEST_ASSERT_NOT_NULL( sprite );
	TEST_ASSERT_EQUAL_INT( Direction_Left, ActiveSprite_GetDirection( sprite ) );
	TEST_ASSERT_EQUAL_UINT( 0, ActiveSprite_GetFrameIndex( sprite ) );
	TEST_ASSERT_EQUAL_FLOAT( ACTIVE_SPRITE_FRAME_DURATION_SEC_DEFAULT, ActiveSprite_GetFrameDurationSec( sprite ) );
	TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_GetTextureSet_ReturnsTextureSetProvidedAtCreation( void )
{
	ActiveSpriteTextureSet_t* expectedTextureSet = (ActiveSpriteTextureSet_t*)1;
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, expectedTextureSet );

	TEST_ASSERT_EQUAL_PTR( expectedTextureSet, ActiveSprite_GetTextureSet( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_SetFrameDurationSec_UpdatesFrameDuration( void )
{
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, 0 );

	ActiveSprite_SetFrameDurationSec( sprite, 0.25f );

	TEST_ASSERT_EQUAL_FLOAT( 0.25f, ActiveSprite_GetFrameDurationSec( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_Tic_AdvancesFrameAfterDuration( void )
{
	ActiveSprite_t* sprite;

	g_frameCount = 3;
	sprite = ActiveSprite_Create( (MemArena_t*)1, (ActiveSpriteTextureSet_t*)1 );
	ActiveSprite_SetFrameDurationSec( sprite, 0.5f );

	ActiveSprite_Tic( sprite, 0.49f );
	TEST_ASSERT_EQUAL_UINT( 0, ActiveSprite_GetFrameIndex( sprite ) );
	ActiveSprite_Tic( sprite, 0.01f );
	TEST_ASSERT_EQUAL_UINT( 1, ActiveSprite_GetFrameIndex( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_Tic_WrapsFrameIndexAtFrameCount( void )
{
	ActiveSprite_t* sprite;

	g_frameCount = 3;
	sprite = ActiveSprite_Create( (MemArena_t*)1, (ActiveSpriteTextureSet_t*)1 );
	ActiveSprite_SetFrameIndex( sprite, 2 );
	ActiveSprite_SetFrameDurationSec( sprite, 0.5f );

	ActiveSprite_Tic( sprite, 0.5f );

	TEST_ASSERT_EQUAL_UINT( 0, ActiveSprite_GetFrameIndex( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_Tic_AdvancesMultipleFramesAndRetainsRemainder( void )
{
	ActiveSprite_t* sprite;

	g_frameCount = 4;
	sprite = ActiveSprite_Create( (MemArena_t*)1, (ActiveSpriteTextureSet_t*)1 );
	ActiveSprite_SetFrameDurationSec( sprite, 0.5f );

	ActiveSprite_Tic( sprite, 1.25f );
	TEST_ASSERT_EQUAL_UINT( 2, ActiveSprite_GetFrameIndex( sprite ) );
	ActiveSprite_Tic( sprite, 0.25f );
	TEST_ASSERT_EQUAL_UINT( 3, ActiveSprite_GetFrameIndex( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_SetDirection_UpdatesDirection( void )
{
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, 0 );

	ActiveSprite_SetDirection( sprite, Direction_Down );

	TEST_ASSERT_EQUAL_INT( Direction_Down, ActiveSprite_GetDirection( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_SetFrameIndex_UpdatesFrameIndex( void )
{
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, 0 );

	ActiveSprite_SetFrameIndex( sprite, 7 );

	TEST_ASSERT_EQUAL_UINT( 7, ActiveSprite_GetFrameIndex( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_Setters_ReplacePreviousValues( void )
{
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, 0 );

	ActiveSprite_SetDirection( sprite, Direction_Up );
	ActiveSprite_SetFrameIndex( sprite, 3 );
	ActiveSprite_SetDirection( sprite, Direction_Right );
	ActiveSprite_SetFrameIndex( sprite, 11 );

	TEST_ASSERT_EQUAL_INT( Direction_Right, ActiveSprite_GetDirection( sprite ) );
	TEST_ASSERT_EQUAL_UINT( 11, ActiveSprite_GetFrameIndex( sprite ) );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );
}

void test_ActiveSprite_Free_ReleasesAllocatedSpriteOnly( void )
{
	ActiveSprite_t* sprite = ActiveSprite_Create( (MemArena_t*)1, (ActiveSpriteTextureSet_t*)1 );

	ActiveSprite_Free( sprite, (MemArena_t*)1 );

	TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_freeCount );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_ActiveSprite_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_ActiveSprite_Create_InitializesDefaultState );

	RUN_TEST( test_ActiveSprite_GetTextureSet_ReturnsTextureSetProvidedAtCreation );

	RUN_TEST( test_ActiveSprite_SetDirection_UpdatesDirection );

	RUN_TEST( test_ActiveSprite_SetFrameDurationSec_UpdatesFrameDuration );

	RUN_TEST( test_ActiveSprite_Tic_AdvancesFrameAfterDuration );
	RUN_TEST( test_ActiveSprite_Tic_WrapsFrameIndexAtFrameCount );
	RUN_TEST( test_ActiveSprite_Tic_AdvancesMultipleFramesAndRetainsRemainder );

	RUN_TEST( test_ActiveSprite_SetFrameIndex_UpdatesFrameIndex );

	RUN_TEST( test_ActiveSprite_Setters_ReplacePreviousValues );
   
	RUN_TEST( test_ActiveSprite_Free_ReleasesAllocatedSpriteOnly );

	return UNITY_END();
}
