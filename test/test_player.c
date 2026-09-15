#include <stdlib.h>

#include "mem_arena.h"
#include "platform.h"
#include "player.h"
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

void setUp( void )
{
	g_allocCount = 0;
	g_freeCount = 0;
	g_fatalErrorCount = 0;
}

void tearDown( void ) {}

void test_Player_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, Player_GetStructSize() );
}

void test_Player_Create_InitializesDefaultName( void )
{
	Player_t* player = Player_Create( (MemArena_t*)1 );

	TEST_ASSERT_EQUAL_STRING( "JDoe", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_SetName_UpdatesName( void )
{
	Player_t* player = Player_Create( (MemArena_t*)1 );

	Player_SetName( player, "Hero" );

	TEST_ASSERT_EQUAL_STRING( "Hero", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_SetName_AcceptsMaximumLengthName( void )
{
	Player_t* player = Player_Create( (MemArena_t*)1 );

	Player_SetName( player, "12345678" );

	TEST_ASSERT_EQUAL_STRING( "12345678", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_SetName_RejectsNameLongerThanMaximum( void )
{
	Player_t* player = Player_Create( (MemArena_t*)1 );

	Player_SetName( player, "123456789" );

	TEST_ASSERT_EQUAL_STRING( "JDoe", Player_GetName( player ) );
	TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );

	Player_Free( (MemArena_t*)1, player );
}

void test_Player_Free_ReleasesPlayer( void )
{
	Player_t* player = Player_Create( (MemArena_t*)1 );

	Player_Free( (MemArena_t*)1, player );

	TEST_ASSERT_EQUAL_UINT( 1, g_freeCount );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_Player_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_Player_Create_InitializesDefaultName );

   RUN_TEST( test_Player_Free_ReleasesPlayer );

	RUN_TEST( test_Player_SetName_UpdatesName );
	RUN_TEST( test_Player_SetName_AcceptsMaximumLengthName );
	RUN_TEST( test_Player_SetName_RejectsNameLongerThanMaximum );

	return UNITY_END();
}
