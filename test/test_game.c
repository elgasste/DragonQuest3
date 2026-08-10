#include "clock.h"
#include "display.h"
#include "game.h"
#include "mem_arena.h"
#include "platform.h"
#include "unity.h"

typedef struct InputInitCall_t
{
   Input_t* input;
   int callCount;
}
InputInitCall_t;

typedef struct InputResetPressStatesCall_t
{
   Input_t* input;
   int callCount;
}
InputResetPressStatesCall_t;

typedef struct DisplayInitCall_t
{
   Display_t* display;
   MemArena_t* memArena;
   u32 w;
   u32 h;
   int callCount;
}
DisplayInitCall_t;

typedef struct GameRenderCall_t
{
   Game_t* game;
   int callCount;
}
GameRenderCall_t;

local_persist int g_clockInitCallCount;
local_persist InputInitCall_t g_inputInitCall;
local_persist InputResetPressStatesCall_t g_inputResetPressStatesCall;
local_persist DisplayInitCall_t g_displayInitCall;
local_persist int g_clockStartFrameCallCount;
local_persist int g_platformHandleMessagesCallCount;
local_persist GameRenderCall_t g_gameRenderCall;
local_persist int g_clockEndFrameCallCount;
local_persist Game_t* g_currentGame;
local_persist MemArena_t* g_memArena;

void setUp( void )
{
   g_clockInitCallCount = 0;
   g_inputInitCall.input = 0;
   g_inputInitCall.callCount = 0;
   g_inputResetPressStatesCall.input = 0;
   g_inputResetPressStatesCall.callCount = 0;
   g_displayInitCall.display = 0;
   g_displayInitCall.memArena = 0;
   g_displayInitCall.w = 0;
   g_displayInitCall.h = 0;
   g_displayInitCall.callCount = 0;
   g_clockStartFrameCallCount = 0;
   g_platformHandleMessagesCallCount = 0;
   g_gameRenderCall.game = 0;
   g_gameRenderCall.callCount = 0;
   g_clockEndFrameCallCount = 0;

   g_currentGame = 0;

   g_memArena = 0;
   MemArena_Create( &g_memArena, 1024 );
}

void tearDown( void )
{
   MemArena_Destroy( &g_memArena );
}

void Clock_Init( Clock_t* clock, u32 fps )
{
   UNUSED_PARAM( clock );
   UNUSED_PARAM( fps );

   g_clockInitCallCount++;
}

void Input_Init( Input_t* input )
{
   g_inputInitCall.input = input;
   g_inputInitCall.callCount++;
}

void Input_ResetPressStates( Input_t* input )
{
   g_inputResetPressStatesCall.input = input;
   g_inputResetPressStatesCall.callCount++;
}

void Clock_StartFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );

   g_clockStartFrameCallCount++;
}

void Clock_EndFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );

   g_clockEndFrameCallCount++;
}

void Display_Init( Display_t* display, MemArena_t* memArena, u32 w, u32 h )
{
   g_displayInitCall.display = display;
   g_displayInitCall.memArena = memArena;
   g_displayInitCall.w = w;
   g_displayInitCall.h = h;
   g_displayInitCall.callCount++;
}

void Game_Render( Game_t* game )
{
   g_gameRenderCall.game = game;
   g_gameRenderCall.callCount++;
}

void Platform_FatalError( const char* msg )
{
   // TODO
   UNUSED_PARAM( msg );
}

void Platform_HandleMessages( Game_t* game )
{
   UNUSED_PARAM( game );

   g_platformHandleMessagesCallCount++;
   if ( g_currentGame != 0 && g_platformHandleMessagesCallCount >= 3 )
   {
      Game_Stop( g_currentGame );
   }
}

void test_Game_Init_CreatesGameWithCorrectParameters( void )
{
   Game_t game;

   Game_Init( &game, g_memArena );
   TEST_ASSERT_EQUAL_PTR( g_memArena, game.memArena );
   TEST_ASSERT_EQUAL( 1, g_clockInitCallCount );
   TEST_ASSERT_EQUAL_PTR( game.input, g_inputInitCall.input );
   TEST_ASSERT_EQUAL( 1, g_inputInitCall.callCount );
   TEST_ASSERT_EQUAL( 1, g_displayInitCall.callCount );
   TEST_ASSERT_EQUAL_PTR( game.display, g_displayInitCall.display );
   TEST_ASSERT_EQUAL_PTR( g_memArena, g_displayInitCall.memArena );
   TEST_ASSERT_EQUAL( DISPLAY_WIDTH, g_displayInitCall.w );
   TEST_ASSERT_EQUAL( DISPLAY_HEIGHT, g_displayInitCall.h );
}

void test_Game_Run_StopsAfterMultipleMessageHandlerTicks( void )
{
   Game_t game;

   g_currentGame = &game;
   Game_Init( &game, g_memArena );

   Game_Run( &game );
   TEST_ASSERT_EQUAL( 3, g_clockStartFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_clockEndFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_inputResetPressStatesCall.callCount );
   TEST_ASSERT_EQUAL_PTR( game.input, g_inputResetPressStatesCall.input );
   TEST_ASSERT_EQUAL( 3, g_platformHandleMessagesCallCount );
   TEST_ASSERT_EQUAL( 3, g_gameRenderCall.callCount );
   TEST_ASSERT_EQUAL_PTR( &game, g_gameRenderCall.game );
   TEST_ASSERT_TRUE( game.shutdown );
}

void test_Game_Run_ResetsInputPressStatesEveryFrame( void )
{
   Game_t game;

   g_currentGame = &game;
   Game_Init( &game, g_memArena );

   Game_Run( &game );
   TEST_ASSERT_EQUAL( 3, g_clockStartFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_inputResetPressStatesCall.callCount );
   TEST_ASSERT_EQUAL_PTR( game.input, g_inputResetPressStatesCall.input );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Init_CreatesGameWithCorrectParameters );

   RUN_TEST( test_Game_Run_StopsAfterMultipleMessageHandlerTicks );
   RUN_TEST( test_Game_Run_ResetsInputPressStatesEveryFrame );

   return UNITY_END();
}
