#include <string.h>
#include <stdlib.h>

#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "tile_map.h"
#include "unity.h"
#include "version.h"

#define MAX_PATH 260
#define MOCK_TILE_TEXTURE_COUNT 1
#define MOCK_TILE_TEXTURE_SIZE 8
#define MOCK_TILE_TEXTURE_BYTES ( MOCK_TILE_TEXTURE_COUNT * MOCK_TILE_TEXTURE_SIZE * MOCK_TILE_TEXTURE_SIZE * sizeof( u32 ) )

typedef struct PlatformLoadFileToMemoryCall_t
{
   char filePath[MAX_PATH];
   MemArena_t* memArena;
   u32* bytesRead;
   int callCount;
}
PlatformLoadFileToMemoryCall_t;

typedef struct MockGameDataFile_t
{
   GameDataHeader_t header;
   TileTextureSet_t tileTextureSet;
   u8 textureData[MOCK_TILE_TEXTURE_BYTES];
   u32 tileMapCount;
   TileMap_t tileMap;
   Tile_t tileData[1];
}
MockGameDataFile_t;

local_persist PlatformLoadFileToMemoryCall_t g_platformLoadFileToMemoryCall;
local_persist u8* g_mockFileContents;
local_persist u32 g_mockFileSize;
local_persist u32 g_fatalErrorCallCount;
local_persist u32 g_memArenaFreeCallCount;
local_persist void* g_lastMemArenaAlloc;
local_persist MockGameDataFile_t g_mockGameDataFile;

void setUp( void )
{
   if ( g_lastMemArenaAlloc )
   {
      free( g_lastMemArenaAlloc );
      g_lastMemArenaAlloc = 0;
   }

   g_platformLoadFileToMemoryCall.filePath[0] = '\0';
   g_platformLoadFileToMemoryCall.memArena = 0;
   g_platformLoadFileToMemoryCall.bytesRead = 0;
   g_platformLoadFileToMemoryCall.callCount = 0;
   g_mockFileContents = 0;
   g_mockFileSize = 0;
   g_fatalErrorCallCount = 0;
   g_memArenaFreeCallCount = 0;
}

void tearDown( void )
{
   if ( g_lastMemArenaAlloc )
   {
      free( g_lastMemArenaAlloc );
      g_lastMemArenaAlloc = 0;
   }
}

MemArenaResult_t MemArena_Alloc( MemArena_t* arena, void** user, size_t size )
{
   UNUSED_PARAM( arena );

   if ( !user )
   {
      return MemArenaResult_OutOfMemory;
   }

   g_lastMemArenaAlloc = malloc( size );
   if ( !g_lastMemArenaAlloc )
   {
      return MemArenaResult_OutOfMemory;
   }

   *user = g_lastMemArenaAlloc;

   return MemArenaResult_Success;
}

const char* MemArena_GetErrorMessage( MemArenaResult_t result )
{
   UNUSED_PARAM( result );
   return "mock mem arena error";
}

void MemArena_Free( MemArena_t* memArena, void* ptr )
{
   UNUSED_PARAM( memArena );
   UNUSED_PARAM( ptr );
   g_memArenaFreeCallCount++;
}

u8* Platform_LoadFileToMemory( const char* filePath, MemArena_t* memArena, u32* bytesRead )
{
   strncpy_s( g_platformLoadFileToMemoryCall.filePath, MAX_PATH, filePath, _TRUNCATE );
   g_platformLoadFileToMemoryCall.memArena = memArena;
   g_platformLoadFileToMemoryCall.bytesRead = bytesRead;
   g_platformLoadFileToMemoryCall.callCount++;

   if ( bytesRead )
   {
      *bytesRead = g_mockFileSize;
   }

   return g_mockFileContents;
}

void Platform_FatalError( const char* msg )
{
   UNUSED_PARAM( msg );
   g_fatalErrorCallCount++;
}

internal void SetupValidMockGameDataFile( void )
{
   u32 i;

   g_mockGameDataFile.header.magic[0] = 'D';
   g_mockGameDataFile.header.magic[1] = 'W';
   g_mockGameDataFile.header.magic[2] = '3';
   g_mockGameDataFile.header.magic[3] = 'D';
   g_mockGameDataFile.header.version.major = GAME_VERSION_MAJOR;
   g_mockGameDataFile.header.version.minor = GAME_VERSION_MINOR;
   g_mockGameDataFile.header.version.maint = GAME_VERSION_MAINT;
   g_mockGameDataFile.header.tileTextureSetOffset = sizeof( GameDataHeader_t );

   g_mockGameDataFile.tileTextureSet.count = MOCK_TILE_TEXTURE_COUNT;
   g_mockGameDataFile.tileTextureSet.tileSize = MOCK_TILE_TEXTURE_SIZE;
   g_mockGameDataFile.tileTextureSet.textures = 0;

   g_mockGameDataFile.header.tileMapsOffset = sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t ) + MOCK_TILE_TEXTURE_BYTES;
   g_mockGameDataFile.tileMapCount = 1;
   g_mockGameDataFile.tileMap.id = 0;
   g_mockGameDataFile.tileMap.w = 1;
   g_mockGameDataFile.tileMap.h = 1;
   g_mockGameDataFile.tileMap.tiles = 0;
   g_mockGameDataFile.tileMap.tileTextureSet = 0;
   g_mockGameDataFile.tileData[0].textureIndex = 0;

   for ( i = 0; i < MOCK_TILE_TEXTURE_BYTES; i++ )
   {
      g_mockGameDataFile.textureData[i] = (u8)i;
   }

   g_mockFileContents = (u8*)&g_mockGameDataFile;
   g_mockFileSize = sizeof( g_mockGameDataFile );
}

void test_Game_LoadFromFile_CallsPlatformLoadFileToMemory( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;
   SetupValidMockGameDataFile();

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_STRING( testFilePath, g_platformLoadFileToMemoryCall.filePath );
   TEST_ASSERT_EQUAL_PTR( &memArena, g_platformLoadFileToMemoryCall.memArena );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
}

void test_Game_LoadFromFile_LoadFileToMemoryFailsResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;
   g_mockFileSize = 0;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_FileTooSmallResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   u8 smallFileContents[2] = { 0x00, 0x01 };
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;
   g_mockFileContents = smallFileContents;
   g_mockFileSize = sizeof( smallFileContents );

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_InvalidHeaderMagicNumberResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   GameDataHeader_t invalidHeader;
   const char* testFilePath = "mock.dw3d";

   invalidHeader.magic[0] = 'B';
   invalidHeader.magic[1] = 'A';
   invalidHeader.magic[2] = 'D';
   invalidHeader.magic[3] = '!';
   invalidHeader.version.major = GAME_VERSION_MAJOR;
   invalidHeader.version.minor = GAME_VERSION_MINOR;
   invalidHeader.version.maint = GAME_VERSION_MAINT;
   invalidHeader.tileTextureSetOffset = sizeof( GameDataHeader_t );

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;
   g_mockFileSize = sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t );

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_InvalidHeaderVersionResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   GameDataHeader_t invalidHeader;
   const char* testFilePath = "mock.dw3d";

   invalidHeader.magic[0] = 'D';
   invalidHeader.magic[1] = 'W';
   invalidHeader.magic[2] = '3';
   invalidHeader.magic[3] = 'D';
   invalidHeader.version.major = GAME_VERSION_MAJOR + 1;
   invalidHeader.version.minor = GAME_VERSION_MINOR;
   invalidHeader.version.maint = GAME_VERSION_MAINT;
   invalidHeader.tileTextureSetOffset = sizeof( GameDataHeader_t );

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;
   g_mockFileSize = sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t );

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_ValidHeaderAndVersionDoesNotResultInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;
   SetupValidMockGameDataFile();

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 0, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_InvalidTileTexturesOffsetResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   GameDataHeader_t invalidHeader;
   const char* testFilePath = "mock.dw3d";

   invalidHeader.magic[0] = 'D';
   invalidHeader.magic[1] = 'W';
   invalidHeader.magic[2] = '3';
   invalidHeader.magic[3] = 'D';
   invalidHeader.version.major = GAME_VERSION_MAJOR;
   invalidHeader.version.minor = GAME_VERSION_MINOR;
   invalidHeader.version.maint = GAME_VERSION_MAINT;
   invalidHeader.tileTextureSetOffset = sizeof( GameDataHeader_t );

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;
   g_mockFileSize = sizeof( GameDataHeader_t );

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_LoadedSuccessfullyFreesFileBuffer( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;
   SetupValidMockGameDataFile();

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 0, g_fatalErrorCallCount );
   TEST_ASSERT_EQUAL_INT( 1, g_memArenaFreeCallCount );
}

void test_Game_LoadFromFile_TileTextureSetTooSmallResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   u8 fileContents[sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t ) - 1];
   GameDataHeader_t* header;
   const char* testFilePath = "mock.dw3d";

   memset( fileContents, 0, sizeof( fileContents ) );
   header = (GameDataHeader_t*)fileContents;
   header->magic[0] = 'D';
   header->magic[1] = 'W';
   header->magic[2] = '3';
   header->magic[3] = 'D';
   header->version.major = GAME_VERSION_MAJOR;
   header->version.minor = GAME_VERSION_MINOR;
   header->version.maint = GAME_VERSION_MAINT;
   header->tileTextureSetOffset = sizeof( GameDataHeader_t );

   game.memArena = &memArena;
   g_mockFileContents = fileContents;
   g_mockFileSize = sizeof( fileContents );

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_InvalidTileTexturesDataOffsetResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   SetupValidMockGameDataFile();
   g_mockFileSize = sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t );

   game.memArena = &memArena;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_TileTexturesPayloadTooSmallResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   u32 expectedTexturesSize;
   const char* testFilePath = "mock.dw3d";

   SetupValidMockGameDataFile();
   expectedTexturesSize = g_mockGameDataFile.tileTextureSet.count *
                          g_mockGameDataFile.tileTextureSet.tileSize *
                          g_mockGameDataFile.tileTextureSet.tileSize *
                          sizeof( u32 );
   g_mockFileSize = sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t ) + expectedTexturesSize - 1;

   game.memArena = &memArena;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_ZeroTileTextureCountDoesNotRequireTexturePayload( void )
{
   Game_t game;
   MemArena_t memArena;
   u8 fileContents[sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t ) + sizeof( u32 ) + sizeof( TileMap_t ) + sizeof( Tile_t )];
   GameDataHeader_t* header;
   TileTextureSet_t* textureSet;
   u32* tileMapCount;
   TileMap_t* tileMap;
   Tile_t* tile;
   const char* testFilePath = "mock.dw3d";

   memset( fileContents, 0, sizeof( fileContents ) );

   header = (GameDataHeader_t*)fileContents;
   header->magic[0] = 'D';
   header->magic[1] = 'W';
   header->magic[2] = '3';
   header->magic[3] = 'D';
   header->version.major = GAME_VERSION_MAJOR;
   header->version.minor = GAME_VERSION_MINOR;
   header->version.maint = GAME_VERSION_MAINT;
   header->tileTextureSetOffset = sizeof( GameDataHeader_t );
   header->tileMapsOffset = sizeof( GameDataHeader_t ) + sizeof( TileTextureSet_t );

   textureSet = (TileTextureSet_t*)( fileContents + header->tileTextureSetOffset );
   textureSet->count = 0;
   textureSet->tileSize = MOCK_TILE_TEXTURE_SIZE;
   textureSet->textures = 0;

   tileMapCount = (u32*)( fileContents + header->tileMapsOffset );
   *tileMapCount = 1;

   tileMap = (TileMap_t*)( fileContents + header->tileMapsOffset + sizeof( u32 ) );
   tileMap->id = 0;
   tileMap->w = 1;
   tileMap->h = 1;
   tileMap->tiles = 0;
   tileMap->tileTextureSet = 0;

   tile = (Tile_t*)( (u8*)tileMap + sizeof( TileMap_t ) );
   tile->textureIndex = 0;

   g_mockFileContents = fileContents;
   g_mockFileSize = sizeof( fileContents );

   game.memArena = &memArena;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 0, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_LoadsTileMapData( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;
   SetupValidMockGameDataFile();

   Game_LoadFromFile( &game, testFilePath );

   TEST_ASSERT_EQUAL_INT( 0, g_fatalErrorCallCount );
   TEST_ASSERT_EQUAL_UINT32( 1, game.tileMapCount );
   TEST_ASSERT_EQUAL_UINT32( 0, game.tileMaps[0].id );
   TEST_ASSERT_EQUAL_UINT32( 1, game.tileMaps[0].w );
   TEST_ASSERT_EQUAL_UINT32( 1, game.tileMaps[0].h );
   TEST_ASSERT_EQUAL_PTR( game.tileTextureSet, game.tileMaps[0].tileTextureSet );
   TEST_ASSERT_EQUAL_UINT32( 0, game.tileMaps[0].tiles[0].textureIndex );
}

void test_Game_LoadFromFile_InvalidTileMapsOffsetResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   SetupValidMockGameDataFile();
   g_mockGameDataFile.header.tileMapsOffset = g_mockFileSize;

   game.memArena = &memArena;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_TileMapHeaderTooSmallResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   SetupValidMockGameDataFile();
   g_mockFileSize = g_mockGameDataFile.header.tileMapsOffset + sizeof( u32 ) - 1;

   game.memArena = &memArena;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_TileMapTileDataTooSmallResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   SetupValidMockGameDataFile();
   g_mockFileSize = g_mockGameDataFile.header.tileMapsOffset + sizeof( u32 ) + sizeof( TileMap_t );

   game.memArena = &memArena;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_LoadFromFile_CallsPlatformLoadFileToMemory );
   RUN_TEST( test_Game_LoadFromFile_LoadFileToMemoryFailsResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_FileTooSmallResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_InvalidHeaderMagicNumberResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_InvalidHeaderVersionResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_ValidHeaderAndVersionDoesNotResultInFatalError );
   RUN_TEST( test_Game_LoadFromFile_InvalidTileTexturesOffsetResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_LoadedSuccessfullyFreesFileBuffer );
   RUN_TEST( test_Game_LoadFromFile_TileTextureSetTooSmallResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_InvalidTileTexturesDataOffsetResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_TileTexturesPayloadTooSmallResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_ZeroTileTextureCountDoesNotRequireTexturePayload );
   RUN_TEST( test_Game_LoadFromFile_LoadsTileMapData );
   RUN_TEST( test_Game_LoadFromFile_InvalidTileMapsOffsetResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_TileMapHeaderTooSmallResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_TileMapTileDataTooSmallResultsInFatalError );

   return UNITY_END();
}
