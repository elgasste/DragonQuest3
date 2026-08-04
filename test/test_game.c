#include "clock.h"
#include "game.h"
#include "mem_arena.h"
#include "platform_ops.h"
#include "screen.h"
#include "unity.h"

typedef struct ScreenInitCall_t
{
   Screen_t* screen;
   MemArena_t* memArena;
   u32 w;
   u32 h;
   int callCount;
}
ScreenInitCall_t;

typedef struct ScreenFillCall_t
{
   Screen_t* screen;
   u32 color;
   int callCount;
}
ScreenFillCall_t;

local_persist int g_clockInitCallCount;
local_persist ScreenInitCall_t g_screenInitCall;
local_persist int g_clockStartFrameCallCount;
local_persist int g_platformHandleMessagesCallCount;
local_persist ScreenFillCall_t g_screenFillCall;
local_persist int g_platformRenderScreenBufferCallCount;
local_persist int g_clockEndFrameCallCount;
local_persist Game_t* g_currentGame;
local_persist MemArena_t* g_memArena;

void setUp( void )
{
   g_clockInitCallCount = 0;
   g_screenInitCall.screen = 0;
   g_screenInitCall.memArena = 0;
   g_screenInitCall.w = 0;
   g_screenInitCall.h = 0;
   g_screenInitCall.callCount = 0;
   g_clockStartFrameCallCount = 0;
   g_platformHandleMessagesCallCount = 0;
   g_screenFillCall.screen = 0;
   g_screenFillCall.color = 0;
   g_screenFillCall.callCount = 0;
   g_platformRenderScreenBufferCallCount = 0;
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

void Screen_Init( Screen_t* screen, MemArena_t* memArena, u32 w, u32 h )
{
   g_screenInitCall.screen = screen;
   g_screenInitCall.memArena = memArena;
   g_screenInitCall.w = w;
   g_screenInitCall.h = h;
   g_screenInitCall.callCount++;
}

void Screen_Fill( Screen_t* screen, u32 color )
{
   g_screenFillCall.screen = screen;
   g_screenFillCall.color = color;
   g_screenFillCall.callCount++;
}

void PlatformOps_FatalError( const char* msg )
{
   // TODO
   UNUSED_PARAM( msg );
}

void PlatformOps_HandleMessages( void )
{
   g_platformHandleMessagesCallCount++;
   if ( g_currentGame != 0 && g_platformHandleMessagesCallCount >= 3 )
   {
      Game_Stop( g_currentGame );
   }
}

void PlatformOps_RenderScreenBuffer( void )
{
   g_platformRenderScreenBufferCallCount++;
   if ( g_currentGame != 0 && g_platformRenderScreenBufferCallCount >= 3 )
   {
      Game_Stop( g_currentGame );
   }
}

void test_Game_Init_CreatesGameWithCorrectParameters( void )
{
   Game_t game;

   Game_Init( &game, g_memArena );
   TEST_ASSERT_EQUAL( g_memArena, game.memArena );
   TEST_ASSERT_EQUAL( 1, g_clockInitCallCount );
   TEST_ASSERT_EQUAL( 1, g_screenInitCall.callCount );
   TEST_ASSERT_EQUAL( game.screen, g_screenInitCall.screen );
   TEST_ASSERT_EQUAL( g_memArena, g_screenInitCall.memArena );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, g_screenInitCall.w );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, g_screenInitCall.h );
}

void test_Game_Run_StopsAfterMultipleMessageHandlerTicks( void )
{
   Game_t game;

   g_currentGame = &game;
   Game_Init( &game, g_memArena );

   Game_Run( &game );
   TEST_ASSERT_EQUAL( 3, g_clockStartFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_clockEndFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformHandleMessagesCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformRenderScreenBufferCallCount );
   TEST_ASSERT_EQUAL( 3, g_screenFillCall.callCount );
   TEST_ASSERT_EQUAL( 0, g_screenFillCall.color );
   TEST_ASSERT_TRUE( game.shutdown );
}

void test_Game_Run_StopsAfterMultipleRenderHandlerTicks( void )
{
   Game_t game;

   g_currentGame = &game;
   Game_Init( &game, g_memArena );

   Game_Run( &game );
   TEST_ASSERT_EQUAL( 3, g_platformHandleMessagesCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformRenderScreenBufferCallCount );
   TEST_ASSERT_EQUAL( 3, g_screenFillCall.callCount );
   TEST_ASSERT_EQUAL( 0, g_screenFillCall.color );
   TEST_ASSERT_TRUE( game.shutdown );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Init_CreatesGameWithCorrectParameters );

   RUN_TEST( test_Game_Run_StopsAfterMultipleMessageHandlerTicks );
   RUN_TEST( test_Game_Run_StopsAfterMultipleRenderHandlerTicks );

   return UNITY_END();
}
