#include <string.h>

#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "screen.h"
#include "unity.h"
#include "version.h"

#define MAX_PATH 260

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
   GameDataTileTexturesHeader_t tileTexturesHeader;
}
MockGameDataFile_t;

local_persist PlatformLoadFileToMemoryCall_t g_platformLoadFileToMemoryCall;
local_persist u8* g_mockFileContents;
local_persist u32 g_mockFileSize;
local_persist u32 g_fatalErrorCallCount;
local_persist u32 g_memArenaFreeCallCount;
local_persist MockGameDataFile_t g_mockGameDataFile;

void setUp( void )
{
   g_platformLoadFileToMemoryCall.filePath[0] = '\0';
   g_platformLoadFileToMemoryCall.memArena = 0;
   g_platformLoadFileToMemoryCall.bytesRead = 0;
   g_platformLoadFileToMemoryCall.callCount = 0;
   g_mockFileContents = 0;
   g_mockFileSize = 0;
   g_fatalErrorCallCount = 0;
   g_memArenaFreeCallCount = 0;
}

void tearDown( void ) {}

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
   g_mockGameDataFile.header.magic[0] = 'D';
   g_mockGameDataFile.header.magic[1] = 'W';
   g_mockGameDataFile.header.magic[2] = '3';
   g_mockGameDataFile.header.magic[3] = 'D';
   g_mockGameDataFile.header.version.major = GAME_VERSION_MAJOR;
   g_mockGameDataFile.header.version.minor = GAME_VERSION_MINOR;
   g_mockGameDataFile.header.version.maint = GAME_VERSION_MAINT;
   g_mockGameDataFile.header.tileTexturesHeaderOffset = sizeof( GameDataHeader_t );

   g_mockGameDataFile.tileTexturesHeader.count = 1;
   g_mockGameDataFile.tileTexturesHeader.tileSize = 8;
   g_mockGameDataFile.tileTexturesHeader.texturesOffset = 0;

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
   invalidHeader.tileTexturesHeaderOffset = sizeof( GameDataHeader_t );

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;
   g_mockFileSize = sizeof( GameDataHeader_t ) + sizeof( GameDataTileTexturesHeader_t );

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
   invalidHeader.tileTexturesHeaderOffset = sizeof( GameDataHeader_t );

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;
   g_mockFileSize = sizeof( GameDataHeader_t ) + sizeof( GameDataTileTexturesHeader_t );

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
   invalidHeader.tileTexturesHeaderOffset = sizeof( GameDataHeader_t );

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

void test_Game_LoadFromFile_TileTexturesHeaderTooSmallResultsInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   u8 fileContents[sizeof( GameDataHeader_t ) + sizeof( GameDataTileTexturesHeader_t ) - 1];
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
   header->tileTexturesHeaderOffset = sizeof( GameDataHeader_t );

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
   g_mockGameDataFile.tileTexturesHeader.texturesOffset = g_mockFileSize;

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
   RUN_TEST( test_Game_LoadFromFile_TileTexturesHeaderTooSmallResultsInFatalError );
   RUN_TEST( test_Game_LoadFromFile_InvalidTileTexturesDataOffsetResultsInFatalError );

   return UNITY_END();
}
