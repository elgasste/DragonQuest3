#include <stdlib.h>
#include <string.h>

#include "mocks/mock_game_data.h"

#include "entity.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "npc.h"
#include "sprite.h"
#include "unity.h"

global u8 g_fileData[ sizeof( NpcInfo_t ) ];
global File_t g_file;
global size_t g_filePosition;
global u32 g_fatalErrorCount;
global u32 g_allocCount;
global u32 g_freeCount;
#if defined( _WIN32 )
WinDebugFlags_t g_winDebugFlags;
#endif

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

File_t* GameData_GetFile( GameData_t* gameData )
{
	return gameData->file;
}

void Platform_FileSeek( File_t* file, i32 offset, i32 origin )
{
	UNUSED_PARAM( file );
	UNUSED_PARAM( origin );
	g_filePosition = (size_t)offset;
}

void Platform_ReadFileBytes( File_t* file, u8* buffer, size_t size )
{
	UNUSED_PARAM( file );
	memcpy( buffer, g_fileData + g_filePosition, size );
	g_filePosition += size;
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

internal GameData_t* CreateGameData( void )
{
	static GameData_t gameData;

	gameData.file = &g_file;
	return &gameData;
}

internal void WriteNpcInfo( void )
{
	NpcInfo_t info = { 23, 18, 20, -3, 4, Direction_Up, 7, True };

	memcpy( g_fileData, &info, sizeof( info ) );
}

void setUp( void )
{
	memset( g_fileData, 0, sizeof( g_fileData ) );
	g_file.stream = g_fileData;
	g_file.size = sizeof( g_fileData );
	g_filePosition = 0;
	g_fatalErrorCount = 0;
	g_allocCount = 0;
	g_freeCount = 0;
}

void tearDown( void ) {}

void test_Npc_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, Npc_GetStructSize() );
}

void test_Npc_CreateFromGameData_LoadsNpcAndEntityState( void )
{
	ActiveSpriteTextureSet_t* textureSet = (ActiveSpriteTextureSet_t*)1;
	Npc_t* npc;
	Entity_t* entity;
	Vector4i32_t rect;
	Vector2i32_t spriteOffset;
	ActiveSprite_t* sprite;

	WriteNpcInfo();
	npc = Npc_CreateFromGameData( (MemArena_t*)1, CreateGameData(), 0, textureSet );

	TEST_ASSERT_NOT_NULL( npc );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 3, g_allocCount );
	TEST_ASSERT_TRUE( Npc_GetWanders( npc ) );

	entity = Npc_GetEntity( npc );
	TEST_ASSERT_NOT_NULL( entity );
	TEST_ASSERT_EQUAL_UINT( 23, Entity_GetTileIndex( entity ) );
	rect = Entity_GetRect( entity );
	TEST_ASSERT_EQUAL_INT( 18, rect.w );
	TEST_ASSERT_EQUAL_INT( 20, rect.h );
	spriteOffset = Entity_GetSpriteOffset( entity );
	TEST_ASSERT_EQUAL_INT( -3, spriteOffset.x );
	TEST_ASSERT_EQUAL_INT( 4, spriteOffset.y );

	sprite = Entity_GetSprite( entity );
	TEST_ASSERT_EQUAL_PTR( textureSet, ActiveSprite_GetTextureSet( sprite ) );
	TEST_ASSERT_EQUAL_UINT( 7, ActiveSprite_GetTextureIndex( sprite ) );
	TEST_ASSERT_EQUAL_INT( Direction_Up, ActiveSprite_GetDirection( sprite ) );

	Npc_Free( npc, (MemArena_t*)1 );
	TEST_ASSERT_EQUAL_UINT( 3, g_freeCount );
}

void test_Npc_CreateFromGameData_RejectsTruncatedInfo( void )
{
	Npc_t* npc;

	g_file.size = sizeof( NpcInfo_t ) - 1;
	npc = Npc_CreateFromGameData( (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );

	TEST_ASSERT_NULL( npc );
	TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 0, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 0, g_freeCount );
}

void test_Npc_SetWanders_UpdatesWandersState( void )
{
	Npc_t* npc;

	WriteNpcInfo();
	npc = Npc_CreateFromGameData( (MemArena_t*)1, CreateGameData(), 0, (ActiveSpriteTextureSet_t*)1 );
	Npc_SetWanders( npc, False );

	TEST_ASSERT_FALSE( Npc_GetWanders( npc ) );

	Npc_Free( npc, (MemArena_t*)1 );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_Npc_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_Npc_CreateFromGameData_LoadsNpcAndEntityState );
	RUN_TEST( test_Npc_CreateFromGameData_RejectsTruncatedInfo );
   
	RUN_TEST( test_Npc_SetWanders_UpdatesWandersState );

	return UNITY_END();
}
