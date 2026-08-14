#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "unity.h"
#include "version.h"

PACKED_STRUCT
typedef struct TestGameDataMetaData_t
{
   char magic[4];
   GameDataVersion_t version;
   i32 tileTextureSetOffset;
   i32 tileMapsOffset;
}
TestGameDataMetaData_t;
END_PACKED_STRUCT

typedef struct TestGameLoadState_t
{
   u8* fileData;
   i32 fileSize;
   i32 fileCursor;
   int freeCount;
   int fatalErrorCount;
   int openFileCount;
   int readCount;
   int seekCount;
   const char* fatalErrorMessage;
}
TestGameLoadState_t;

local_persist TestGameLoadState_t g_state;

void setUp( void )
{
   memset( &g_state, 0, sizeof( g_state ) );
}

void tearDown( void ) {}

internal void TestGameLoad_FreeGameData( Game_t* game )
{
   if ( !game->gameData )
   {
      return;
   }

   free( game->gameData->file );
   free( game->gameData->tileMapFileOffsets );
   free( game->gameData );
   game->gameData = 0;
}

internal void TestGameLoad_FreeGameResources( Game_t* game )
{
   if ( game->tileMap )
   {
      free( game->tileMap->tiles );
      free( game->tileMap );
      game->tileMap = 0;
   }
   if ( game->tileTextureSet )
   {
      free( game->tileTextureSet->textures );
      free( game->tileTextureSet );
      game->tileTextureSet = 0;
   }
   TestGameLoad_FreeGameData( game );
}

internal void TestGameLoad_Write( i32 offset, const void* data, size_t size )
{
   memcpy( g_state.fileData + offset, data, size );
}

internal void TestGameLoad_CreateValidFile( void )
{
   TestGameDataMetaData_t metaData;
   TileTextureSet_t textureSet;
   GameDataTileMapFileOffset_t mapOffsets[2];
   TileMap_t tileMap;
   Tile_t tiles[2];
   i32 textureOffset;
   i32 mapsOffset;
   i32 tileMapOffset;
   u32 mapCount;
   u32 textures[8];

   textureOffset = sizeof( TestGameDataMetaData_t );
   mapsOffset = textureOffset + sizeof( TileTextureSet_t ) + ( 2 * 2 * 2 * sizeof( u32 ) );
   tileMapOffset = mapsOffset + sizeof( u32 ) + ( 2 * sizeof( GameDataTileMapFileOffset_t ) );
   g_state.fileSize = tileMapOffset + sizeof( TileMap_t ) + sizeof( tiles );
   g_state.fileData = (u8*)calloc( 1, g_state.fileSize );

   memcpy( metaData.magic, GAME_DATA_MAGIC, sizeof( metaData.magic ) );
   metaData.version.major = GAME_VERSION_MAJOR;
   metaData.version.minor = GAME_VERSION_MINOR;
   metaData.version.maint = GAME_VERSION_MAINT;
   metaData.tileTextureSetOffset = textureOffset;
   metaData.tileMapsOffset = mapsOffset;
   TestGameLoad_Write( 0, &metaData, sizeof( metaData ) );

   textureSet.count = 2;
   textureSet.tileSize = 2;
   textureSet.textures = 0;
   TestGameLoad_Write( textureOffset, &textureSet, sizeof( textureSet ) );
   textures[0] = 1;
   textures[1] = 2;
   textures[2] = 3;
   textures[3] = 4;
   textures[4] = 5;
   textures[5] = 6;
   textures[6] = 7;
   textures[7] = 8;
   TestGameLoad_Write( textureOffset + sizeof( textureSet ), textures, sizeof( textures ) );

   mapCount = 2;
   TestGameLoad_Write( mapsOffset, &mapCount, sizeof( mapCount ) );
   mapOffsets[0].id = 7;
   mapOffsets[0].offset = tileMapOffset;
   mapOffsets[1].id = 9;
   mapOffsets[1].offset = tileMapOffset;
   TestGameLoad_Write( mapsOffset + sizeof( u32 ), mapOffsets, sizeof( mapOffsets ) );

   tileMap.id = 9;
   tileMap.w = 2;
   tileMap.h = 1;
   tileMap.wraps = True;
   tileMap.tiles = 0;
   tileMap.tileTextureSet = 0;
   tiles[0].textureIndex = 1;
   tiles[1].textureIndex = 0;
   TestGameLoad_Write( tileMapOffset, &tileMap, sizeof( tileMap ) );
   TestGameLoad_Write( tileMapOffset + sizeof( tileMap ), tiles, sizeof( tiles ) );
}

internal void TestGameLoad_CreateGame( Game_t* game )
{
   memset( game, 0, sizeof( *game ) );
   game->memArena = (MemArena_t*)1;
}

internal void TestGameLoad_DisposeFixture( void )
{
   free( g_state.fileData );
   g_state.fileData = 0;
}

void MemArena_Alloc( MemArena_t* arena, void** user, size_t size )
{
   UNUSED_PARAM( arena );

   *user = calloc( 1, size );
}

void MemArena_Free( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   if ( mem )
   {
      g_state.freeCount++;
      free( mem );
   }
}

void Platform_FatalError( const char* message )
{
   g_state.fatalErrorCount++;
   g_state.fatalErrorMessage = message;
}

void Platform_OpenFile( File_t* file, const char* filePath )
{
   UNUSED_PARAM( filePath );
   file->stream = g_state.fileData;
   file->size = g_state.fileSize;
   g_state.openFileCount++;
}

void Platform_ReadFileBytes( File_t* file, u8* buffer, size_t size )
{
   UNUSED_PARAM( file );
   memcpy( buffer, g_state.fileData + g_state.fileCursor, size );
   g_state.fileCursor += (i32)size;
   g_state.readCount++;
}

void Platform_FileSeek( File_t* file, i32 offset, i32 origin )
{
   UNUSED_PARAM( file );
   UNUSED_PARAM( origin );
   g_state.fileCursor = offset;
   g_state.seekCount++;
}

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   UNUSED_PARAM( memArena );
   if ( tileMap )
   {
      free( tileMap->tiles );
      tileMap->tiles = 0;
   }
}

void test_Game_LoadGameData_LoadsMetadataTexturesAndMapOffsets( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   TestGameLoad_CreateGame( &game );

   Game_LoadGameData( &game, "fixture.dw3d" );

   TEST_ASSERT_NOT_NULL( game.gameData );
   TEST_ASSERT_EQUAL( GAME_VERSION_MAJOR, game.gameData->version.major );
   TEST_ASSERT_EQUAL( GAME_VERSION_MINOR, game.gameData->version.minor );
   TEST_ASSERT_EQUAL( GAME_VERSION_MAINT, game.gameData->version.maint );
   TEST_ASSERT_EQUAL( 2, game.gameData->tileMapCount );
   TEST_ASSERT_NOT_NULL( game.gameData->tileMapFileOffsets );
   TEST_ASSERT_NOT_NULL( game.tileTextureSet );
   TEST_ASSERT_EQUAL( 2, game.tileTextureSet->count );
   TEST_ASSERT_EQUAL( 2, game.tileTextureSet->tileSize );
   TEST_ASSERT_EQUAL( 8, game.tileTextureSet->textures[7] );
   TEST_ASSERT_EQUAL( 1, g_state.openFileCount );
   TEST_ASSERT_EQUAL( 0, g_state.fatalErrorCount );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

void test_Game_LoadGameData_RejectsInvalidMagic( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   g_state.fileData[0] = 'X';
   TestGameLoad_CreateGame( &game );

   Game_LoadGameData( &game, "fixture.dw3d" );

   TEST_ASSERT_EQUAL( 2, g_state.fatalErrorCount );
   TEST_ASSERT_EQUAL_STRING( "failed to load game data.", g_state.fatalErrorMessage );
   TEST_ASSERT_NULL( game.tileTextureSet );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

void test_Game_LoadGameData_RejectsIncompatibleVersion( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   g_state.fileData[4] = GAME_VERSION_MAJOR + 1;
   TestGameLoad_CreateGame( &game );

   Game_LoadGameData( &game, "fixture.dw3d" );

   TEST_ASSERT_EQUAL( 2, g_state.fatalErrorCount );
   TEST_ASSERT_EQUAL_STRING( "failed to load game data.", g_state.fatalErrorMessage );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

void test_Game_LoadGameData_RejectsTruncatedTextureData( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   g_state.fileSize = sizeof( TestGameDataMetaData_t ) + sizeof( TileTextureSet_t ) + ( 2 * 2 * 2 * sizeof( u32 ) ) - 1;
   TestGameLoad_CreateGame( &game );

   Game_LoadGameData( &game, "fixture.dw3d" );

   TEST_ASSERT_EQUAL( 2, g_state.fatalErrorCount );
   TEST_ASSERT_EQUAL_STRING( "failed to load game data.", g_state.fatalErrorMessage );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

void test_Game_LoadTileMapFromId_LoadsMapAndConnectsTextureSet( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   TestGameLoad_CreateGame( &game );
   Game_LoadGameData( &game, "fixture.dw3d" );
   Game_LoadTileMapFromId( &game, 9 );

   TEST_ASSERT_NOT_NULL( game.tileMap );
   TEST_ASSERT_EQUAL( 9, game.tileMap->id );
   TEST_ASSERT_EQUAL( 2, game.tileMap->w );
   TEST_ASSERT_EQUAL( 1, game.tileMap->h );
   TEST_ASSERT_EQUAL( True, game.tileMap->wraps );
   TEST_ASSERT_EQUAL( 1, game.tileMap->tiles[0].textureIndex );
   TEST_ASSERT_EQUAL( 0, game.tileMap->tiles[1].textureIndex );
   TEST_ASSERT_EQUAL_PTR( game.tileTextureSet, game.tileMap->tileTextureSet );
   TEST_ASSERT_EQUAL( 0, g_state.fatalErrorCount );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

void test_Game_LoadTileMapFromId_ReportsMissingMap( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   TestGameLoad_CreateGame( &game );
   Game_LoadGameData( &game, "fixture.dw3d" );
   Game_LoadTileMapFromId( &game, 99 );

   TEST_ASSERT_EQUAL( 1, g_state.fatalErrorCount );
   TEST_ASSERT_EQUAL_STRING( "failed to load tile map with ID 99: not found in game data file.", g_state.fatalErrorMessage );
   TEST_ASSERT_NULL( game.tileMap );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

void test_Game_LoadTileMapFromId_RejectsTruncatedTiles( void )
{
   Game_t game;

   TestGameLoad_CreateValidFile();
   g_state.fileSize -= sizeof( Tile_t );
   TestGameLoad_CreateGame( &game );
   Game_LoadGameData( &game, "fixture.dw3d" );
   Game_LoadTileMapFromId( &game, 9 );

   TEST_ASSERT_EQUAL( 1, g_state.fatalErrorCount );
   TEST_ASSERT_EQUAL_STRING( "game data file is too small to contain all the requested tile map tiles.", g_state.fatalErrorMessage );

   TestGameLoad_FreeGameResources( &game );
   TestGameLoad_DisposeFixture();
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_LoadGameData_LoadsMetadataTexturesAndMapOffsets );
   RUN_TEST( test_Game_LoadGameData_RejectsInvalidMagic );
   RUN_TEST( test_Game_LoadGameData_RejectsIncompatibleVersion );
   RUN_TEST( test_Game_LoadGameData_RejectsTruncatedTextureData );
   
   RUN_TEST( test_Game_LoadTileMapFromId_LoadsMapAndConnectsTextureSet );
   RUN_TEST( test_Game_LoadTileMapFromId_ReportsMissingMap );
   RUN_TEST( test_Game_LoadTileMapFromId_RejectsTruncatedTiles );

   return UNITY_END();
}
