#include <string.h>

#include "clock.h"
#include "game.h"
#include "mem_arena.h"
#include "pixel_buffer.h"
#include "platform_ops.h"
#include "unity.h"

typedef struct PixelBufferCreateCall_t
{
   MemArena_t* memArena;
   u32 width;
   u32 height;
   PixelBuffer_t* createdBuffer;
}
PixelBufferCreateCall_t;

typedef struct PixelBufferClearColorCall_t
{
   PixelBuffer_t* buffer;
   u32 color;
   int callCount;
}
PixelBufferClearColorCall_t;

local_persist int g_clockInitCallCount;
local_persist int g_clockStartFrameCallCount;
local_persist int g_clockEndFrameCallCount;
local_persist PixelBufferCreateCall_t g_pixelBufferCreateCall;
local_persist int g_pixelBufferCreateCallCount;
local_persist PixelBufferClearColorCall_t g_pixelBufferClearColorCall;
local_persist int g_platformHandleMessagesCallCount;
local_persist int g_platformRenderScreenBufferCallCount;
local_persist Game_t* g_currentGame;
local_persist MemArena_t* g_memArena;

void setUp( void )
{
   g_clockInitCallCount = 0;
   g_clockStartFrameCallCount = 0;
   g_clockEndFrameCallCount = 0;

   g_pixelBufferCreateCallCount = 0;
   g_pixelBufferCreateCall.memArena = 0;
   g_pixelBufferCreateCall.width = 0;
   g_pixelBufferCreateCall.height = 0;
   g_pixelBufferCreateCall.createdBuffer = 0;

   g_pixelBufferClearColorCall.buffer = 0;
   g_pixelBufferClearColorCall.color = 0;
   g_pixelBufferClearColorCall.callCount = 0;

   g_platformHandleMessagesCallCount = 0;
   g_platformRenderScreenBufferCallCount = 0;
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

void PixelBuffer_Create( PixelBuffer_t** pBuffer, MemArena_t* memArena, u32 w, u32 h )
{
   static PixelBuffer_t pixelBufferMock;

   g_pixelBufferCreateCallCount++;
   g_pixelBufferCreateCall.memArena = memArena;
   g_pixelBufferCreateCall.width = w;
   g_pixelBufferCreateCall.height = h;
   g_pixelBufferCreateCall.createdBuffer = &pixelBufferMock;

   pixelBufferMock.w = w;
   pixelBufferMock.h = h;
   pixelBufferMock.mem = 0;

   *pBuffer = &pixelBufferMock;
}

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color )
{
   g_pixelBufferClearColorCall.buffer = buffer;
   g_pixelBufferClearColorCall.color = color;
   g_pixelBufferClearColorCall.callCount++;
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

void test_Game_Create_CreatesGameWithCorrectParameters( void )
{
   Game_t game;

   Game_Create( &game, g_memArena );
   TEST_ASSERT_EQUAL( g_memArena, game.memArena );
   TEST_ASSERT_EQUAL( 1, g_clockInitCallCount );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCallCount );
   TEST_ASSERT_EQUAL( g_memArena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, g_pixelBufferCreateCall.width );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, g_pixelBufferCreateCall.height );
   TEST_ASSERT_NOT_NULL( game.pixelBuffer );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, game.pixelBuffer->w );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, game.pixelBuffer->h );
}

void test_Game_Run_StopsAfterMultipleMessageHandlerTicks( void )
{
   Game_t game;

   g_currentGame = &game;
   Game_Create( &game, g_memArena );

   Game_Run( &game );
   TEST_ASSERT_EQUAL( 3, g_clockStartFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_clockEndFrameCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformHandleMessagesCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformRenderScreenBufferCallCount );
   TEST_ASSERT_EQUAL( 3, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( game.pixelBuffer, g_pixelBufferClearColorCall.buffer );
   TEST_ASSERT_EQUAL( 0, g_pixelBufferClearColorCall.color );
   TEST_ASSERT_TRUE( game.shutdown );
}

void test_Game_Run_StopsAfterMultipleRenderHandlerTicks( void )
{
   Game_t game;

   g_currentGame = &game;
   Game_Create( &game, g_memArena );

   Game_Run( &game );
   TEST_ASSERT_EQUAL( 3, g_platformHandleMessagesCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformRenderScreenBufferCallCount );
   TEST_ASSERT_EQUAL( 3, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( game.pixelBuffer, g_pixelBufferClearColorCall.buffer );
   TEST_ASSERT_EQUAL( 0, g_pixelBufferClearColorCall.color );
   TEST_ASSERT_TRUE( game.shutdown );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Create_CreatesGameWithCorrectParameters );

   RUN_TEST( test_Game_Run_StopsAfterMultipleMessageHandlerTicks );
   RUN_TEST( test_Game_Run_StopsAfterMultipleRenderHandlerTicks );

   return UNITY_END();
}
