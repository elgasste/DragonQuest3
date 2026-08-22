#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "unity.h"
#include "version.h"

global u8 g_fileData[256];
global File_t g_file;
global size_t g_filePosition;
global u32 g_openCount;
global u32 g_closeCount;
global u32 g_fatalErrorCount;
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

void Platform_OpenFile( File_t* file, const char* filePath )
{
	UNUSED_PARAM( filePath );
	file->stream = g_fileData;
	file->size = g_file.size;
	g_openCount++;
}

void Platform_CloseFile( File_t* file )
{
	UNUSED_PARAM( file );
	g_closeCount++;
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

internal void WriteValidGameData( b32 invalidTileMapOffset )
{
	GameDataVersion_t version = { GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_MAINT };
	GameDataFileOffsets_t offsets = { 32, 64 };
	GameDataObjectOffset_t mapOffsets[2] = { { 7, 16 }, { 9, 32 } };
	u32 tileMapCount = 2;

	memset( g_fileData, 0, sizeof( g_fileData ) );
	memcpy( g_fileData, GAME_DATA_MAGIC, 4 );
	memcpy( g_fileData + 4, &version, sizeof( version ) );
	memcpy( g_fileData + 7, &offsets, sizeof( offsets ) );
	memcpy( g_fileData + offsets.tileMaps, &tileMapCount, sizeof( tileMapCount ) );
	if ( invalidTileMapOffset )
	{
		mapOffsets[1].offset = 300;
	}
	memcpy( g_fileData + offsets.tileMaps + sizeof( tileMapCount ), mapOffsets, sizeof( mapOffsets ) );
	g_file.size = 128;
}

internal GameData_t* CreateGameData( void )
{
	return GameData_Create( (MemArena_t*)1, "test.dw3d" );
}

void setUp( void )
{
	memset( g_fileData, 0, sizeof( g_fileData ) );
	g_file.stream = 0;
	g_file.size = 0;
	g_filePosition = 0;
	g_openCount = 0;
	g_closeCount = 0;
	g_fatalErrorCount = 0;
	g_allocCount = 0;
	g_freeCount = 0;
}

void tearDown( void ) {}

void test_GameData_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, GameData_GetStructSize() );
}

void test_GameData_Create_LoadsMetadataAndTileMapOffsets( void )
{
	GameDataFileOffsets_t expectedFileOffsets = { 32, 64 };
	GameData_t* gameData;
	GameDataObjectOffset_t offset;

	WriteValidGameData( False );
	gameData = CreateGameData();

	TEST_ASSERT_NOT_NULL( gameData );
	TEST_ASSERT_EQUAL_UINT( 1, g_openCount );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 2, GameData_GetTileMapCount( gameData ) );
	TEST_ASSERT_EQUAL_UINT8( GAME_VERSION_MAJOR, GameData_GetVersion( gameData ).major );
	TEST_ASSERT_EQUAL_UINT8( GAME_VERSION_MINOR, GameData_GetVersion( gameData ).minor );
	TEST_ASSERT_EQUAL_UINT8( GAME_VERSION_MAINT, GameData_GetVersion( gameData ).maint );
	TEST_ASSERT_EQUAL_INT( expectedFileOffsets.tileTextureSet, GameData_GetFileOffsets( gameData ).tileTextureSet );
	TEST_ASSERT_EQUAL_INT( expectedFileOffsets.tileMaps, GameData_GetFileOffsets( gameData ).tileMaps );

	offset = GameData_GetTileMapOffset( gameData, 9 );
	TEST_ASSERT_EQUAL_UINT( 9, offset.id );
	TEST_ASSERT_EQUAL_INT( 32, offset.offset );
	offset = GameData_GetTileMapOffset( gameData, 99 );
	TEST_ASSERT_EQUAL_UINT( 0, offset.id );
	TEST_ASSERT_EQUAL_INT( -1, offset.offset );

	GameData_Free( gameData, (MemArena_t*)1 );
	TEST_ASSERT_EQUAL_UINT( 1, g_closeCount );
}

void test_GameData_SetTileMapOffsets_ReplacesOffsetsAndCount( void )
{
	GameDataObjectOffset_t* offsets;
	GameData_t* gameData;

	WriteValidGameData( False );
	gameData = CreateGameData();
	offsets = (GameDataObjectOffset_t*)malloc( sizeof( GameDataObjectOffset_t ) );
	offsets[0].id = 42;
	offsets[0].offset = 84;
	GameData_SetTileMapOffsets( gameData, offsets, 1 );

	TEST_ASSERT_EQUAL_UINT( 1, GameData_GetTileMapCount( gameData ) );
	TEST_ASSERT_EQUAL_UINT( 42, GameData_GetTileMapOffset( gameData, 42 ).id );
	TEST_ASSERT_EQUAL_INT( 84, GameData_GetTileMapOffset( gameData, 42 ).offset );

	GameData_Free( gameData, (MemArena_t*)1 );
}

void test_GameData_Free_ReleasesGameDataAndFileAllocations( void )
{
	GameData_t* gameData;

	WriteValidGameData( False );
	gameData = CreateGameData();
	GameData_Free( gameData, (MemArena_t*)1 );

	TEST_ASSERT_EQUAL_UINT( 3, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 3, g_freeCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_closeCount );
}

void test_GameData_Create_InvalidMetadataCleansUpAllocatedObjects( void )
{
	GameData_t* gameData;

	WriteValidGameData( False );
	g_fileData[0] = 'X';
	gameData = CreateGameData();

	TEST_ASSERT_NULL( gameData );
	TEST_ASSERT_EQUAL_UINT( 2, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_openCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_closeCount );
	TEST_ASSERT_EQUAL_UINT( 2, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

void test_GameData_Create_InvalidTileMapOffsetFreesOffsetTable( void )
{
	GameData_t* gameData;

	WriteValidGameData( True );
	gameData = CreateGameData();

	TEST_ASSERT_NULL( gameData );
	TEST_ASSERT_EQUAL_UINT( 2, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 3, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 3, g_freeCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_closeCount );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_GameData_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_GameData_Create_LoadsMetadataAndTileMapOffsets );

	RUN_TEST( test_GameData_SetTileMapOffsets_ReplacesOffsetsAndCount );

	RUN_TEST( test_GameData_Free_ReleasesGameDataAndFileAllocations );
   
	RUN_TEST( test_GameData_Create_InvalidMetadataCleansUpAllocatedObjects );
	RUN_TEST( test_GameData_Create_InvalidTileMapOffsetFreesOffsetTable );

	return UNITY_END();
}
