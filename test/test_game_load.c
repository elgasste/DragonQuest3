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

local_persist PlatformLoadFileToMemoryCall_t g_platformLoadFileToMemoryCall;
local_persist u8* g_mockFileContents;
local_persist u32 g_fatalErrorCallCount;
local_persist u32 g_memArenaFreeCallCount;

void setUp( void )
{
   g_platformLoadFileToMemoryCall.filePath[0] = '\0';
   g_platformLoadFileToMemoryCall.memArena = 0;
   g_platformLoadFileToMemoryCall.bytesRead = 0;
   g_platformLoadFileToMemoryCall.callCount = 0;
   g_mockFileContents = 0;
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

   return g_mockFileContents;
}

void Platform_FatalError( const char* msg )
{
   UNUSED_PARAM( msg );
   g_fatalErrorCallCount++;
}

void test_Game_LoadFromFile_CallsPlatformLoadFileToMemory( void )
{
   Game_t game;
   MemArena_t memArena;
   const char* testFilePath = "mock.dw3d";

   game.memArena = &memArena;

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

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;

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

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&invalidHeader;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 1, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_ValidHeaderAndVersionDoesNotResultInFatalError( void )
{
   Game_t game;
   MemArena_t memArena;
   GameDataHeader_t validHeader;
   const char* testFilePath = "mock.dw3d";

   validHeader.magic[0] = 'D';
   validHeader.magic[1] = 'W';
   validHeader.magic[2] = '3';
   validHeader.magic[3] = 'D';
   validHeader.version.major = GAME_VERSION_MAJOR;
   validHeader.version.minor = GAME_VERSION_MINOR;
   validHeader.version.maint = GAME_VERSION_MAINT;

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&validHeader;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 0, g_fatalErrorCallCount );
}

void test_Game_LoadFromFile_LoadedSuccessfullyFreesFileBuffer( void )
{
   Game_t game;
   MemArena_t memArena;
   GameDataHeader_t validHeader;
   const char* testFilePath = "mock.dw3d";

   validHeader.magic[0] = 'D';
   validHeader.magic[1] = 'W';
   validHeader.magic[2] = '3';
   validHeader.magic[3] = 'D';
   validHeader.version.major = GAME_VERSION_MAJOR;
   validHeader.version.minor = GAME_VERSION_MINOR;
   validHeader.version.maint = GAME_VERSION_MAINT;

   game.memArena = &memArena;
   g_mockFileContents = (u8*)&validHeader;

   Game_LoadFromFile( &game, testFilePath );
   TEST_ASSERT_EQUAL_INT( 1, g_platformLoadFileToMemoryCall.callCount );
   TEST_ASSERT_EQUAL_INT( 0, g_fatalErrorCallCount );
   TEST_ASSERT_EQUAL_INT( 1, g_memArenaFreeCallCount );
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
   RUN_TEST( test_Game_LoadFromFile_LoadedSuccessfullyFreesFileBuffer );

   return UNITY_END();
}
