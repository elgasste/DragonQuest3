#include <stdlib.h>
#include <string.h>

#include "direction.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "sprite_texture_set.h"
#include "unity.h"

global u8 g_fileData[256];
global size_t g_filePosition;
global File_t g_file;
global GameDataFileOffsets_t g_fileOffsets;
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

File_t* GameData_GetFile( GameData_t* gameData )
{
	UNUSED_PARAM( gameData );
	return &g_file;
}

GameDataFileOffsets_t GameData_GetFileOffsets( GameData_t* gameData )
{
	UNUSED_PARAM( gameData );
	return g_fileOffsets;
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

internal ActiveSpriteTextureSet_t* LoadTextureSet( void )
{
	return ActiveSpriteTextureSet_CreateFromGameData( (MemArena_t*)1, (GameData_t*)1 );
}

internal void WriteTextureSet( u32 count, u32 frameSize, u32 frameCount, const u32* textures )
{
	ActiveSpriteTextureSetInfo_t fileTextureSet;
	size_t textureCount;

	fileTextureSet.count = count;
	fileTextureSet.frameSize = frameSize;
	fileTextureSet.frameCount = frameCount;
	memcpy( g_fileData + g_fileOffsets.activeSpriteTextureSet, &fileTextureSet, sizeof( fileTextureSet ) );

	textureCount = count * frameSize * frameSize * frameCount * Direction_Count;
	memcpy( g_fileData + g_fileOffsets.activeSpriteTextureSet + sizeof( fileTextureSet ), textures, textureCount * sizeof( u32 ) );
}

void setUp( void )
{
	memset( g_fileData, 0, sizeof( g_fileData ) );
	g_filePosition = 0;
	g_file.size = 0;
	g_fileOffsets.activeSpriteTextureSet = 0;
	g_allocCount = 0;
	g_freeCount = 0;
	g_fatalErrorCount = 0;
}

void tearDown( void ) {}

void test_ActiveSpriteTextureSet_GetStructSize_ReturnsNonZeroSize( void )
{
	TEST_ASSERT_GREATER_THAN_size_t( 0, ActiveSpriteTextureSet_GetStructSize() );
}

void test_ActiveSpriteTextureSet_Create_LoadsHeaderAndTextures( void )
{
	u32 expectedTextures[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
	ActiveSpriteTextureSet_t* textureSet;

	g_file.size = sizeof( ActiveSpriteTextureSetInfo_t ) + sizeof( expectedTextures );
	WriteTextureSet( 1, 1, 2, expectedTextures );
	textureSet = LoadTextureSet();

	TEST_ASSERT_NOT_NULL( textureSet );
	TEST_ASSERT_EQUAL_UINT( 1, ActiveSpriteTextureSet_GetCount( textureSet ) );
	TEST_ASSERT_EQUAL_UINT( 1, ActiveSpriteTextureSet_GetFrameSize( textureSet ) );
	TEST_ASSERT_EQUAL_UINT( 2, ActiveSpriteTextureSet_GetFrameCount( textureSet ) );
	TEST_ASSERT_EQUAL_UINT( 10, ActiveSpriteTextureSet_GetTexture( textureSet, 0 )[0] );
	TEST_ASSERT_EQUAL_UINT( 80, ActiveSpriteTextureSet_GetTexture( textureSet, 7 )[0] );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );

	ActiveSpriteTextureSet_Free( textureSet, (MemArena_t*)1 );
}

void test_ActiveSpriteTextureSet_Create_ZeroCountDoesNotAllocateTextures( void )
{
	ActiveSpriteTextureSet_t* textureSet;

	g_file.size = sizeof( ActiveSpriteTextureSetInfo_t );
	WriteTextureSet( 0, 16, 2, 0 );
	textureSet = LoadTextureSet();

	TEST_ASSERT_NOT_NULL( textureSet );
	TEST_ASSERT_EQUAL_UINT( 0, ActiveSpriteTextureSet_GetCount( textureSet ) );
	TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 0, g_fatalErrorCount );

	ActiveSpriteTextureSet_Free( textureSet, (MemArena_t*)1 );
	TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

void test_ActiveSpriteTextureSet_Create_RejectsTruncatedTextures( void )
{
	u32 texture = 42;
	ActiveSpriteTextureSet_t* textureSet;

	g_file.size = sizeof( ActiveSpriteTextureSetInfo_t ) + sizeof( texture );
	WriteTextureSet( 1, 1, 2, &texture );
	textureSet = LoadTextureSet();

	TEST_ASSERT_NULL( textureSet );
	TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 1, g_freeCount );
}

void test_ActiveSpriteTextureSet_Free_ReleasesTextureSetAndTextures( void )
{
	u32 textures[8] = { 0 };
	ActiveSpriteTextureSet_t* textureSet;

	g_file.size = sizeof( ActiveSpriteTextureSetInfo_t ) + sizeof( textures );
	WriteTextureSet( 1, 1, 2, textures );
	textureSet = LoadTextureSet();
	ActiveSpriteTextureSet_Free( textureSet, (MemArena_t*)1 );

	TEST_ASSERT_EQUAL_UINT( 2, g_allocCount );
	TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

int main( void )
{
	UNITY_BEGIN();

	RUN_TEST( test_ActiveSpriteTextureSet_GetStructSize_ReturnsNonZeroSize );

	RUN_TEST( test_ActiveSpriteTextureSet_Create_LoadsHeaderAndTextures );
	RUN_TEST( test_ActiveSpriteTextureSet_Create_ZeroCountDoesNotAllocateTextures );
	RUN_TEST( test_ActiveSpriteTextureSet_Create_RejectsTruncatedTextures );
   
	RUN_TEST( test_ActiveSpriteTextureSet_Free_ReleasesTextureSetAndTextures );

	return UNITY_END();
}
