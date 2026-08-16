#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "display.h"
#include "file.h"
#include "game.h"
#include "game_data.h"
#include "input.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "unity.h"

typedef struct GameTestCalls_t
{
   int clockInit;
   int clockStartFrame;
   int clockEndFrame;
   int displayInit;
   int displayCleanup;
   int gameLoadGameData;
   int gameDataCleanup;
   int tileMapCleanup;
   int tileTextureSetCleanup;
   int gameHandleInput;
   int gameRender;
   int inputInit;
   int inputResetPressStates;
   int memArenaAlloc;
   int memArenaFree;
   int platformFatalError;
   int platformHandleMessages;
   int platformCloseFile;
   u32 clockInitFps;
   const char* gameLoadGameDataPath;
   const char* fatalErrorMessage;
}
GameTestCalls_t;

local_persist GameTestCalls_t g_calls;

void setUp( void )
{
   memset( &g_calls, 0, sizeof( g_calls ) );
}

void tearDown( void ) {}

void Clock_Init( Clock_t* clock, u32 fps )
{
   UNUSED_PARAM( clock );
   g_calls.clockInit++;
   g_calls.clockInitFps = fps;
}

void Clock_StartFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   g_calls.clockStartFrame++;
}

void Clock_EndFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   g_calls.clockEndFrame++;
}

void Display_Init( Display_t* display, MemArena_t* memArena, u32 w, u32 h )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( memArena );
   UNUSED_PARAM( w );
   UNUSED_PARAM( h );
   g_calls.displayInit++;
}

void Display_Cleanup( Display_t* display, MemArena_t* memArena )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( memArena );
   g_calls.displayCleanup++;
}

void Game_LoadGameData( Game_t* game, const char* gameDataFilePath )
{
   UNUSED_PARAM( game );
   g_calls.gameLoadGameData++;
   g_calls.gameLoadGameDataPath = gameDataFilePath;
}

void GameData_Cleanup( GameData_t* gameData, MemArena_t* memArena )
{
   UNUSED_PARAM( gameData );
   UNUSED_PARAM( memArena );
   g_calls.gameDataCleanup++;
}

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( memArena );
   g_calls.tileMapCleanup++;
}

void TileTextureSet_Cleanup( TileTextureSet_t* tileTextureSet, MemArena_t* memArena )
{
   UNUSED_PARAM( tileTextureSet );
   UNUSED_PARAM( memArena );
   g_calls.tileTextureSetCleanup++;
}

void Game_LoadTileMapFromId( Game_t* game, u32 id )
{
   UNUSED_PARAM( game );
   UNUSED_PARAM( id );
}

void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, Vector4i32_t* viewport, u32 x, u32 y )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( x );
   UNUSED_PARAM( y );

   if ( viewport )
   {
      viewport->x = 0;
      viewport->y = 0;
   }
}

void Game_HandleInput( Game_t* game )
{
   UNUSED_PARAM( game );
   g_calls.gameHandleInput++;
}

void Game_Render( Game_t* game )
{
   UNUSED_PARAM( game );
   g_calls.gameRender++;
}

void Input_Init( Input_t* input )
{
   UNUSED_PARAM( input );
   g_calls.inputInit++;
}

void Input_ResetPressStates( Input_t* input )
{
   UNUSED_PARAM( input );
   g_calls.inputResetPressStates++;
}

const char* MemArena_GetErrorMessage( MemArenaResult_t result )
{
   UNUSED_PARAM( result );
   return "stubbed memory arena error";
}

void* MemArena_Alloc( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   g_calls.memArenaAlloc++;

   return malloc( size );
}

void MemArena_Free( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   g_calls.memArenaFree++;
   free( mem );
}

void Platform_FatalError( const char* message )
{
   UNUSED_PARAM( message );
   g_calls.platformFatalError++;
   g_calls.fatalErrorMessage = message;
}

void Platform_HandleMessages( Game_t* game )
{
   g_calls.platformHandleMessages++;
   Game_Stop( game );
}

void Platform_CloseFile( File_t* file )
{
   UNUSED_PARAM( file );
   g_calls.platformCloseFile++;
}

void test_Game_Create_InitializesGameInfrastructure( void )
{
   Game_t* game = 0;
   MemArena_t arena;
   const char* gameDataPath = "test-game-data.dw3d";

   Game_Create( &game, &arena, gameDataPath );

   TEST_ASSERT_NOT_NULL( game );
   TEST_ASSERT_EQUAL_PTR( &arena, game->memArena );
   TEST_ASSERT_NOT_NULL( game->clock );
   TEST_ASSERT_NOT_NULL( game->input );
   TEST_ASSERT_NOT_NULL( game->display );
   TEST_ASSERT_EQUAL( 4, g_calls.memArenaAlloc );
   TEST_ASSERT_EQUAL( 1, g_calls.clockInit );
   TEST_ASSERT_EQUAL( GAME_DEFAULT_FPS, g_calls.clockInitFps );
   TEST_ASSERT_EQUAL( 1, g_calls.inputInit );
   TEST_ASSERT_EQUAL( 1, g_calls.displayInit );
   TEST_ASSERT_EQUAL( 1, g_calls.gameLoadGameData );
   TEST_ASSERT_EQUAL_PTR( gameDataPath, g_calls.gameLoadGameDataPath );

   Game_Destroy( &game );
   TEST_ASSERT_NULL( game );
}

void test_Game_Stop_SetsShutdownFlag( void )
{
   Game_t game;

   game.shutdown = False;
   Game_Stop( &game );

   TEST_ASSERT_EQUAL( True, game.shutdown );
}

void test_Game_Run_ProcessesOneFrameBeforePlatformStopsGame( void )
{
   Game_t game;
   Clock_t clock;
   Input_t input;
   Display_t display;

   memset( &game, 0, sizeof( game ) );
   game.clock = &clock;
   game.input = &input;
   game.display = &display;

   Game_Run( &game );

   TEST_ASSERT_EQUAL( True, game.shutdown );
   TEST_ASSERT_EQUAL( 1, g_calls.clockStartFrame );
   TEST_ASSERT_EQUAL( 1, g_calls.inputResetPressStates );
   TEST_ASSERT_EQUAL( 1, g_calls.platformHandleMessages );
   TEST_ASSERT_EQUAL( 1, g_calls.gameRender );
   TEST_ASSERT_EQUAL( 1, g_calls.gameHandleInput );
   TEST_ASSERT_EQUAL( 1, g_calls.clockEndFrame );
}

void test_Game_Run_ProcessesMultipleFramesUntilShutdown( void )
{
   Game_t game;
   Clock_t clock;
   Input_t input;
   Display_t display;

   memset( &game, 0, sizeof( game ) );
   game.clock = &clock;
   game.input = &input;
   game.display = &display;

   g_calls.platformHandleMessages = 0;
   g_calls.gameRender = 0;

   Game_Run( &game );

   TEST_ASSERT_EQUAL( True, game.shutdown );
   TEST_ASSERT_EQUAL( 1, g_calls.platformHandleMessages );
   TEST_ASSERT_EQUAL( 1, g_calls.gameRender );
   TEST_ASSERT_EQUAL( 1, g_calls.gameHandleInput );
}

void test_Game_Destroy_CleansUpAllOwnedResources( void )
{
   Game_t* game;
   MemArena_t arena;

   game = (Game_t*)malloc( sizeof( Game_t ) );
   game->memArena = &arena;
   game->clock = (Clock_t*)malloc( sizeof( Clock_t ) );
   game->input = (Input_t*)malloc( sizeof( Input_t ) );
   game->display = (Display_t*)malloc( sizeof( Display_t ) );
   game->gameData = (GameData_t*)malloc( sizeof( GameData_t ) );
   game->tileMap = (TileMap_t*)malloc( sizeof( TileMap_t ) );
   game->tileTextureSet = (TileTextureSet_t*)malloc( sizeof( TileTextureSet_t ) );

   Game_Destroy( &game );

   TEST_ASSERT_NULL( game );
   TEST_ASSERT_EQUAL( 1, g_calls.displayCleanup );
   TEST_ASSERT_EQUAL( 1, g_calls.gameDataCleanup );
   TEST_ASSERT_EQUAL( 1, g_calls.tileMapCleanup );
   TEST_ASSERT_EQUAL( 1, g_calls.tileTextureSetCleanup );
   TEST_ASSERT_EQUAL( 7, g_calls.memArenaFree );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Create_InitializesGameInfrastructure );

   RUN_TEST( test_Game_Stop_SetsShutdownFlag );

   RUN_TEST( test_Game_Run_ProcessesMultipleFramesUntilShutdown );
   RUN_TEST( test_Game_Run_ProcessesOneFrameBeforePlatformStopsGame );

   RUN_TEST( test_Game_Destroy_CleansUpAllOwnedResources );

   return UNITY_END();
}
