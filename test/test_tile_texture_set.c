#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "game_data.h"
#include "mocks/mock_game_data.h"
#include "platform.h"
#include "tile_texture_set.h"
#include "unity.h"

typedef struct TileTextureSet_t
{
   TileTextureSetInfo_t info;
   u32* textures;
}
TileTextureSet_t;

global u8 g_fileData[128];
global File_t g_file;
global size_t g_filePosition;
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

void Platform_FatalError( const char* msg )
{
   UNUSED_PARAM( msg );
   g_fatalErrorCount++;
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

File_t* GameData_GetFile( GameData_t* gameData )
{
   return gameData->file;
}

GameDataFileOffsets_t GameData_GetFileOffsets( GameData_t* gameData )
{
   return gameData->fileOffsets;
}

void setUp( void )
{
   memset( g_fileData, 0, sizeof( g_fileData ) );
   g_file.stream = g_fileData;
   g_file.size = 0;
   g_filePosition = 0;
   g_fatalErrorCount = 0;
   g_allocCount = 0;
   g_freeCount = 0;
}

void tearDown( void ) {}

internal GameData_t CreateGameData( i32 tileTextureSetOffset )
{
   GameData_t gameData;

   memset( &gameData, 0, sizeof( gameData ) );
   gameData.file = &g_file;
   gameData.fileOffsets.tileTextureSet = tileTextureSetOffset;
   return gameData;
}

internal void WriteTileTextureSetInfo( i32 offset, u32 count, u32 tileSize )
{
   TileTextureSetInfo_t info = { count, tileSize };
   memcpy( g_fileData + offset, &info, sizeof( info ) );
}

void test_TileTextureSet_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, TileTextureSet_GetStructSize() );
}

void test_TileTextureSet_Create_LoadsInfoAndTextures( void )
{
   GameData_t gameData = CreateGameData( 16 );
   u32 expectedTextures[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
   TileTextureSet_t* textureSet;

   WriteTileTextureSetInfo( 16, 2, 2 );
   memcpy( g_fileData + 24, expectedTextures, sizeof( expectedTextures ) );
   g_file.size = 24 + sizeof( expectedTextures );

   textureSet = TileTextureSet_CreateFromGameData( (MemArena_t*)1, &gameData );

   TEST_ASSERT_NOT_NULL( textureSet );
   TEST_ASSERT_EQUAL_UINT( 2, TileTextureSet_GetCount( textureSet ) );
   TEST_ASSERT_EQUAL_UINT( 2, TileTextureSet_GetTileSize( textureSet ) );
   TEST_ASSERT_EQUAL_UINT( 2, g_allocCount );
   TEST_ASSERT_EQUAL_UINT( expectedTextures[0], TileTextureSet_GetTexture( textureSet, 0 )[0] );
   TEST_ASSERT_EQUAL_UINT( expectedTextures[4], TileTextureSet_GetTexture( textureSet, 1 )[0] );

   TileTextureSet_Free( textureSet, (MemArena_t*)1 );
   TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

void test_TileTextureSet_Create_WithNoTextures_DoesNotAllocateTextureData( void )
{
   GameData_t gameData = CreateGameData( 16 );
   TileTextureSet_t* textureSet;

   WriteTileTextureSetInfo( 16, 0, 16 );
   g_file.size = 16 + sizeof( TileTextureSetInfo_t );

   textureSet = TileTextureSet_CreateFromGameData( (MemArena_t*)1, &gameData );

   TEST_ASSERT_NOT_NULL( textureSet );
   TEST_ASSERT_EQUAL_UINT( 0, TileTextureSet_GetCount( textureSet ) );
   TEST_ASSERT_EQUAL_UINT( 16, TileTextureSet_GetTileSize( textureSet ) );
   TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );

   TileTextureSet_Free( textureSet, (MemArena_t*)1 );
}

void test_TileTextureSet_Create_WhenInfoIsTruncated_ReturnsNullAndFreesStruct( void )
{
   GameData_t gameData = CreateGameData( 16 );
   TileTextureSet_t* textureSet;

   g_file.size = 16 + sizeof( TileTextureSetInfo_t ) - 1;

   textureSet = TileTextureSet_CreateFromGameData( (MemArena_t*)1, &gameData );

   TEST_ASSERT_NULL( textureSet );
   TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );
   TEST_ASSERT_EQUAL_UINT( 0, g_allocCount );
}

void test_TileTextureSet_Create_WhenTexturesAreTruncated_ReturnsNullAndFreesStruct( void )
{
   GameData_t gameData = CreateGameData( 16 );
   TileTextureSet_t* textureSet;

   WriteTileTextureSetInfo( 16, 2, 2 );
   g_file.size = 24 + sizeof( u32 ) - 1;

   textureSet = TileTextureSet_CreateFromGameData( (MemArena_t*)1, &gameData );

   TEST_ASSERT_NULL( textureSet );
   TEST_ASSERT_EQUAL_UINT( 1, g_fatalErrorCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_allocCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_freeCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_TileTextureSet_GetStructSize_ReturnsNonZeroSize );
   RUN_TEST( test_TileTextureSet_Create_LoadsInfoAndTextures );
   RUN_TEST( test_TileTextureSet_Create_WithNoTextures_DoesNotAllocateTextureData );
   RUN_TEST( test_TileTextureSet_Create_WhenInfoIsTruncated_ReturnsNullAndFreesStruct );
   RUN_TEST( test_TileTextureSet_Create_WhenTexturesAreTruncated_ReturnsNullAndFreesStruct );

   return UNITY_END();
}
