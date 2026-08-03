#include <string.h>

#include "unity.h"
#include "game.h"
#include "mem_arena.h"
#include "pixel_buffer.h"
#include "platform_ops.h"

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

local_persist PixelBufferCreateCall_t g_pixelBufferCreateCall;
local_persist int g_pixelBufferCreateCallCount;
local_persist PixelBufferClearColorCall_t g_pixelBufferClearColorCall;
local_persist int g_platformHandleMessagesCallCount;
local_persist int g_platformRenderScreenBufferCallCount;
local_persist Game_t* g_currentGame;

void setUp( void )
{
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
}

void tearDown( void ) {}

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
   MemArena_t arena;
   Game_t game;

   memset( &arena, 0, sizeof( arena ) );
   memset( &game, 0, sizeof( game ) );

   Game_Create( &game, &arena );

   TEST_ASSERT_EQUAL( &arena, game.memArena );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCallCount );
   TEST_ASSERT_EQUAL( &arena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, g_pixelBufferCreateCall.width );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, g_pixelBufferCreateCall.height );
   TEST_ASSERT_NOT_NULL( game.pixelBuffer );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, game.pixelBuffer->w );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, game.pixelBuffer->h );
}

void test_Game_Run_StopsAfterMultipleMessageHandlerTicks( void )
{
   MemArena_t arena;
   Game_t game;

   memset( &arena, 0, sizeof( arena ) );
   memset( &game, 0, sizeof( game ) );

   g_currentGame = &game;
   Game_Create( &game, &arena );

   Game_Run( &game );

   TEST_ASSERT_EQUAL( 3, g_platformHandleMessagesCallCount );
   TEST_ASSERT_EQUAL( 3, g_platformRenderScreenBufferCallCount );
   TEST_ASSERT_EQUAL( 3, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( game.pixelBuffer, g_pixelBufferClearColorCall.buffer );
   TEST_ASSERT_EQUAL( 0, g_pixelBufferClearColorCall.color );
   TEST_ASSERT_TRUE( game.shutdown );
}

void test_Game_Run_StopsAfterMultipleRenderHandlerTicks( void )
{
   MemArena_t arena;
   Game_t game;

   memset( &arena, 0, sizeof( arena ) );
   memset( &game, 0, sizeof( game ) );

   g_currentGame = &game;
   Game_Create( &game, &arena );

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
