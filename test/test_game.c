#include <string.h>

#include "unity.h"
#include "game.h"
#include "mem_arena.h"
#include "pixel_buffer.h"

typedef struct PixelBufferCreateCall_t
{
   MemArena_t* memArena;
   u32 width;
   u32 height;
   PixelBuffer_t* createdBuffer;
}
PixelBufferCreateCall_t;

local_persist PixelBufferCreateCall_t g_pixelBufferCreateCall;
local_persist int g_pixelBufferCreateCallCount;

void setUp( void )
{
   g_pixelBufferCreateCallCount = 0;
   g_pixelBufferCreateCall.memArena = 0;
   g_pixelBufferCreateCall.width = 0;
   g_pixelBufferCreateCall.height = 0;
   g_pixelBufferCreateCall.createdBuffer = 0;
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
   UNUSED_PARAM( buffer );
   UNUSED_PARAM( color );
}

void test_Game_Create_CreatesGameWithCorrectParameters( void )
{
   MemArena_t arena;
   Game_t game;
   void (*messageHandler)( void ) = 0;
   void (*render)( void ) = 0;

   memset( &arena, 0, sizeof( arena ) );
   memset( &game, 0, sizeof( game ) );

   Game_Create( &game, &arena, messageHandler, render );

   TEST_ASSERT_EQUAL( &arena, game.memArena );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCallCount );
   TEST_ASSERT_EQUAL( &arena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, g_pixelBufferCreateCall.width );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, g_pixelBufferCreateCall.height );
   TEST_ASSERT_NOT_NULL( game.pixelBuffer );
   TEST_ASSERT_EQUAL( SCREEN_WIDTH, game.pixelBuffer->w );
   TEST_ASSERT_EQUAL( SCREEN_HEIGHT, game.pixelBuffer->h );
   TEST_ASSERT_EQUAL( messageHandler, game.platformMessageHandler );
   TEST_ASSERT_EQUAL( render, game.platformRender );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Create_CreatesGameWithCorrectParameters );

   return UNITY_END();
}
