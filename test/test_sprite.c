#include <stdlib.h>

#include "direction.h"
#include "mem_arena.h"
#include "sprite.h"
#include "sprite_texture_set.h"
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
	TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );

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

	RUN_TEST( test_ActiveSprite_SetDirection_UpdatesDirection );

	RUN_TEST( test_ActiveSprite_SetFrameIndex_UpdatesFrameIndex );

	RUN_TEST( test_ActiveSprite_Setters_ReplacePreviousValues );
   
	RUN_TEST( test_ActiveSprite_Free_ReleasesAllocatedSpriteOnly );

	return UNITY_END();
}
