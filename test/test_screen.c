#include <stdlib.h>

#include "mem_arena.h"
#include "pixel_buffer.h"
#include "screen.h"
#include "unity.h"

typedef struct PixelBufferCreateCall_t
{
   MemArena_t* memArena;
   PixelBuffer_t** pBuffer;
   u32 w;
   u32 h;
   int callCount;
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
local_persist PixelBufferClearColorCall_t g_pixelBufferClearColorCall;

void FreeScreenBuffer( Screen_t* screen )
{
   if ( !screen || !screen->buffer )
   {
      return;
   }

   free( screen->buffer->mem );
   free( screen->buffer );
   screen->buffer = 0;
}

void setUp( void )
{
   g_pixelBufferCreateCall.pBuffer = 0;
   g_pixelBufferCreateCall.memArena = 0;
   g_pixelBufferCreateCall.w = 0;
   g_pixelBufferCreateCall.h = 0;
   g_pixelBufferCreateCall.callCount = 0;

   g_pixelBufferClearColorCall.buffer = 0;
   g_pixelBufferClearColorCall.color = 0;
   g_pixelBufferClearColorCall.callCount = 0;
}

void tearDown( void ) {}

void PixelBuffer_Create( PixelBuffer_t** buffer, MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_t* newBuffer;

   newBuffer = (PixelBuffer_t*)malloc( sizeof( PixelBuffer_t ) );
   newBuffer->w = w;
   newBuffer->h = h;
   newBuffer->mem = (u32*)calloc( w * h, sizeof( u32 ) );

   *buffer = newBuffer;

   g_pixelBufferCreateCall.pBuffer = buffer;
   g_pixelBufferCreateCall.memArena = memArena;
   g_pixelBufferCreateCall.w = w;
   g_pixelBufferCreateCall.h = h;
   g_pixelBufferCreateCall.callCount++;
}

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color )
{
   g_pixelBufferClearColorCall.buffer = buffer;
   g_pixelBufferClearColorCall.color = color;
   g_pixelBufferClearColorCall.callCount++;
}

void test_Screen_Init_CreatesPixelBufferWithCorrectParameters( void )
{
   Screen_t screen;
   MemArena_t memArena;

   screen.buffer = (PixelBuffer_t*)malloc( sizeof( PixelBuffer_t ) );

   Screen_Init( &screen, &memArena, 20, 45 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCall.callCount );
   TEST_ASSERT_EQUAL( &( screen.buffer ), g_pixelBufferCreateCall.pBuffer );
   TEST_ASSERT_EQUAL( &memArena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( 20, g_pixelBufferCreateCall.w );
   TEST_ASSERT_EQUAL( 45, g_pixelBufferCreateCall.h );

   FreeScreenBuffer( &screen );
}

void test_Screen_Fill_FillsPixelBufferWithColor( void )
{
   Screen_t screen;

   Screen_Init( &screen, 0, 10, 10 );

   Screen_Fill( &screen, 50 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( 50, g_pixelBufferClearColorCall.color );

   FreeScreenBuffer( &screen );
}

void test_Screen_DrawRect_WritesPixelsInsideTheRect( void )
{
   Screen_t screen;
   Vector4i32_t rect = { 1, 1, 2, 2 };
   const u32 color = 0x12345678u;

   Screen_Init( &screen, 0, 4, 4 );

   Screen_DrawRect( &screen, rect, color );

   TEST_ASSERT_EQUAL( color, screen.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, screen.buffer->mem[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( color, screen.buffer->mem[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, screen.buffer->mem[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[0] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeScreenBuffer( &screen );
}

void test_Screen_DrawRect_ClampsToVisibleArea( void )
{
   Screen_t screen;
   Vector4i32_t rect = { -2, -1, 6, 4 };
   const u32 color = 0x9abcdef0u;

   Screen_Init( &screen, 0, 4, 4 );

   Screen_DrawRect( &screen, rect, color );

   TEST_ASSERT_EQUAL( color, screen.buffer->mem[0] );
   TEST_ASSERT_EQUAL( color, screen.buffer->mem[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeScreenBuffer( &screen );
}

void test_Screen_DrawPixelBuffer_CopiesPixelsWhenFullyVisible( void )
{
   Screen_t screen;
   u32 source[4] = {
      0x11111111u, 0x22222222u,
      0x33333333u, 0x44444444u,
   };

   Screen_Init( &screen, 0, 4, 4 );

   Screen_DrawPixelBuffer( &screen, &( PixelBuffer_t ){ .mem = source, .w = 2, .h = 2 }, 1, 1 );

   TEST_ASSERT_EQUAL( 0x11111111u, screen.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x22222222u, screen.buffer->mem[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0x33333333u, screen.buffer->mem[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x44444444u, screen.buffer->mem[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[0] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeScreenBuffer( &screen );
}

void test_Screen_DrawPixelBuffer_ClipsTopLeftAndCopiesVisiblePixels( void )
{
   Screen_t screen;
   u32 source[9] = {
      1u, 2u, 3u,
      4u, 5u, 6u,
      7u, 8u, 9u,
   };

   Screen_Init( &screen, 0, 4, 4 );

   Screen_DrawPixelBuffer( &screen, &( PixelBuffer_t ){ .mem = source, .w = 3, .h = 3 }, -1, -1 );

   TEST_ASSERT_EQUAL( 5u, screen.buffer->mem[ ( 0 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 6u, screen.buffer->mem[ ( 0 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 8u, screen.buffer->mem[ ( 1 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 9u, screen.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 0 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 2 * 4 ) + 0 ] );

   FreeScreenBuffer( &screen );
}

void test_Screen_DrawPixelBuffer_ClipsBottomRightAndCopiesVisiblePixels( void )
{
   Screen_t screen;
   u32 source[9] = {
      1u, 2u, 3u,
      4u, 5u, 6u,
      7u, 8u, 9u,
   };

   Screen_Init( &screen, 0, 4, 4 );

   Screen_DrawPixelBuffer( &screen, &( PixelBuffer_t ){ .mem = source, .w = 3, .h = 3 }, 3, 2 );

   TEST_ASSERT_EQUAL( 1u, screen.buffer->mem[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 4u, screen.buffer->mem[ ( 3 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 3 * 4 ) + 2 ] );

   FreeScreenBuffer( &screen );
}

void test_Screen_DrawPixelBuffer_DoesNothingWhenBufferIsFullyOffScreen( void )
{
   Screen_t screen;
   u32 source[4] = {
      0xaaaaaaaau, 0xbbbbbbbbu,
      0xccccccccu, 0xddddddddu,
   };

   Screen_Init( &screen, 0, 4, 4 );

   Screen_DrawPixelBuffer( &screen, &( PixelBuffer_t ){ .mem = source, .w = 2, .h = 2 }, 5, 1 );

   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[0] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0u, screen.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeScreenBuffer( &screen );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Screen_Init_CreatesPixelBufferWithCorrectParameters );

   RUN_TEST( test_Screen_Fill_FillsPixelBufferWithColor );
   
   RUN_TEST( test_Screen_DrawRect_WritesPixelsInsideTheRect );
   RUN_TEST( test_Screen_DrawRect_ClampsToVisibleArea );

   RUN_TEST( test_Screen_DrawPixelBuffer_CopiesPixelsWhenFullyVisible );
   RUN_TEST( test_Screen_DrawPixelBuffer_ClipsTopLeftAndCopiesVisiblePixels );
   RUN_TEST( test_Screen_DrawPixelBuffer_ClipsBottomRightAndCopiesVisiblePixels );
   RUN_TEST( test_Screen_DrawPixelBuffer_DoesNothingWhenBufferIsFullyOffScreen );

   return UNITY_END();
}
