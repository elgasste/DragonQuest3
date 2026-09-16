#include <stdlib.h>

#include "mem_arena.h"
#include "platform.h"
#include "player.h"
#include "entity.h"
#include "sprite.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;
global u32 g_fatalErrorCount;

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

void Platform_FatalError( const char* msg )
{
	UNUSED_PARAM( msg );
	g_fatalErrorCount++;
}

u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet )
{
	UNUSED_PARAM( textureSet );
	return 1;
}

void setUp( void )
{
	g_allocCount = 0;
	g_freeCount = 0;
	g_fatalErrorCount = 0;
}

void tearDown( void ) {}

internal Player_t* CreateTestPlayer( void )
{
	return Player_Create( (MemArena_t*)1,
	                      (ActiveSpriteTextureSet_t*)2,
	                      (Vector2i32_t){ 12, 14 },
	                      (Vector2i32_t){ -2, 3 } );
}

void test_Player_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, Player_GetStructSize() );
}

void test_Player_Create_InitializesDefaultName( void )
{
	Player_t* player = CreateTestPlayer();

	TEST_ASSERT_EQUAL_STRING( "JDoe", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 3, g_allocCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_Create_InitializesEntityState( void )
{
	Player_t* player = CreateTestPlayer();
	Entity_t* entity = Player_GetEntity( player );
	Vector4i32_t rect = Entity_GetRect( entity );
	Vector2i32_t velocity = Entity_GetVelocity( entity );
	Vector2i32_t spriteOffset = Entity_GetSpriteOffset( entity );

	TEST_ASSERT_NOT_NULL( entity );
	TEST_ASSERT_EQUAL_INT( 0, Entity_GetTileIndex( entity ) );
	TEST_ASSERT_EQUAL_INT( 0, rect.x );
	TEST_ASSERT_EQUAL_INT( 0, rect.y );
	TEST_ASSERT_EQUAL_INT( 12, rect.w );
	TEST_ASSERT_EQUAL_INT( 14, rect.h );
	TEST_ASSERT_EQUAL_INT( 0, velocity.x );
	TEST_ASSERT_EQUAL_INT( 0, velocity.y );
	TEST_ASSERT_EQUAL_INT( -2, spriteOffset.x );
	TEST_ASSERT_EQUAL_INT( 3, spriteOffset.y );
	TEST_ASSERT_EQUAL_PTR( (ActiveSpriteTextureSet_t*)2,
	                       ActiveSprite_GetTextureSet( Entity_GetSprite( entity ) ) );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_SetName_UpdatesName( void )
{
	Player_t* player = CreateTestPlayer();

	Player_SetName( player, "Hero" );

	TEST_ASSERT_EQUAL_STRING( "Hero", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_SetName_AcceptsMaximumLengthName( void )
{
	Player_t* player = CreateTestPlayer();

	Player_SetName( player, "12345678" );

	TEST_ASSERT_EQUAL_STRING( "12345678", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_SetName_RejectsNameLongerThanMaximum( void )
{
	Player_t* player = CreateTestPlayer();

	Player_SetName( player, "123456789" );

	TEST_ASSERT_EQUAL_STRING( "JDoe", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_Free_ReleasesPlayer( void )
{
	Player_t* player = CreateTestPlayer();

	Player_Free( (MemArena_t*)1, player );

	TEST_ASSERT_EQUAL_UINT( 3, g_freeCount );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_Player_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_Player_Create_InitializesDefaultName );
	RUN_TEST( test_Player_Create_InitializesEntityState );

   RUN_TEST( test_Player_Free_ReleasesPlayer );

	RUN_TEST( test_Player_SetName_UpdatesName );
	RUN_TEST( test_Player_SetName_AcceptsMaximumLengthName );
	RUN_TEST( test_Player_SetName_RejectsNameLongerThanMaximum );

	return UNITY_END();
}
