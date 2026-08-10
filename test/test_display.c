#include <stdlib.h>

#include "display.h"
#include "mem_arena.h"
#include "pixel_buffer.h"
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

void FreeDisplayBuffer( Display_t* display )
{
   if ( !display || !display->buffer )
   {
      return;
   }

   free( display->buffer->mem );
   free( display->buffer );
   display->buffer = 0;
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

void test_Display_Init_CreatesPixelBufferWithCorrectParameters( void )
{
   Display_t display;
   MemArena_t memArena;

   display.buffer = (PixelBuffer_t*)malloc( sizeof( PixelBuffer_t ) );

   Display_Init( &display, &memArena, 20, 45 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCall.callCount );
   TEST_ASSERT_EQUAL( &( display.buffer ), g_pixelBufferCreateCall.pBuffer );
   TEST_ASSERT_EQUAL( &memArena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( 20, g_pixelBufferCreateCall.w );
   TEST_ASSERT_EQUAL( 45, g_pixelBufferCreateCall.h );

   FreeDisplayBuffer( &display );
}

void test_Display_Fill_FillsPixelBufferWithColor( void )
{
   Display_t display;

   Display_Init( &display, 0, 10, 10 );

   Display_Fill( &display, 50 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( 50, g_pixelBufferClearColorCall.color );

   FreeDisplayBuffer( &display );
}

void test_Display_DrawRect_WritesPixelsInsideTheRect( void )
{
   Display_t display;
   Vector4i32_t rect = { 1, 1, 2, 2 };
   const u32 color = 0x12345678u;

   Display_Init( &display, 0, 4, 4 );

   Display_DrawRect( &display, rect, color );

   TEST_ASSERT_EQUAL( color, display.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, display.buffer->mem[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( color, display.buffer->mem[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, display.buffer->mem[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[0] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeDisplayBuffer( &display );
}

void test_Display_DrawRect_ClampsToVisibleArea( void )
{
   Display_t display;
   Vector4i32_t rect = { -2, -1, 6, 4 };
   const u32 color = 0x9abcdef0u;

   Display_Init( &display, 0, 4, 4 );

   Display_DrawRect( &display, rect, color );

   TEST_ASSERT_EQUAL( color, display.buffer->mem[0] );
   TEST_ASSERT_EQUAL( color, display.buffer->mem[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeDisplayBuffer( &display );
}

void test_Display_DrawPixelBuffer_CopiesPixelsWhenFullyVisible( void )
{
   Display_t display;
   u32 source[4] = {
      0x11111111u, 0x22222222u,
      0x33333333u, 0x44444444u,
   };

   Display_Init( &display, 0, 4, 4 );

   Display_DrawBuffer( &display, source, 2, 2, 1, 1 );

   TEST_ASSERT_EQUAL( 0x11111111u, display.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x22222222u, display.buffer->mem[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0x33333333u, display.buffer->mem[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x44444444u, display.buffer->mem[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[0] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeDisplayBuffer( &display );
}

void test_Display_DrawPixelBuffer_ClipsTopLeftAndCopiesVisiblePixels( void )
{
   Display_t display;
   u32 source[9] = {
      1u, 2u, 3u,
      4u, 5u, 6u,
      7u, 8u, 9u,
   };

   Display_Init( &display, 0, 4, 4 );

   Display_DrawBuffer( &display, source, 3, 3, -1, -1 );

   TEST_ASSERT_EQUAL( 5u, display.buffer->mem[ ( 0 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 6u, display.buffer->mem[ ( 0 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 8u, display.buffer->mem[ ( 1 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 9u, display.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 0 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 2 * 4 ) + 0 ] );

   FreeDisplayBuffer( &display );
}

void test_Display_DrawPixelBuffer_ClipsBottomRightAndCopiesVisiblePixels( void )
{
   Display_t display;
   u32 source[9] = {
      1u, 2u, 3u,
      4u, 5u, 6u,
      7u, 8u, 9u,
   };

   Display_Init( &display, 0, 4, 4 );

   Display_DrawBuffer( &display, source, 3, 3, 3, 2 );

   TEST_ASSERT_EQUAL( 1u, display.buffer->mem[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 4u, display.buffer->mem[ ( 3 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 3 * 4 ) + 2 ] );

   FreeDisplayBuffer( &display );
}

void test_Display_DrawPixelBuffer_DoesNothingWhenBufferIsFullyOffScreen( void )
{
   Display_t display;
   u32 source[4] = {
      0xaaaaaaaau, 0xbbbbbbbbu,
      0xccccccccu, 0xddddddddu,
   };

   Display_Init( &display, 0, 4, 4 );

   Display_DrawBuffer( &display, source, 2, 2, 5, 1 );

   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[0] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0u, display.buffer->mem[ ( 3 * 4 ) + 3 ] );

   FreeDisplayBuffer( &display );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Display_Init_CreatesPixelBufferWithCorrectParameters );

   RUN_TEST( test_Display_Fill_FillsPixelBufferWithColor );
   
   RUN_TEST( test_Display_DrawRect_WritesPixelsInsideTheRect );
   RUN_TEST( test_Display_DrawRect_ClampsToVisibleArea );

   RUN_TEST( test_Display_DrawPixelBuffer_CopiesPixelsWhenFullyVisible );
   RUN_TEST( test_Display_DrawPixelBuffer_ClipsTopLeftAndCopiesVisiblePixels );
   RUN_TEST( test_Display_DrawPixelBuffer_ClipsBottomRightAndCopiesVisiblePixels );
   RUN_TEST( test_Display_DrawPixelBuffer_DoesNothingWhenBufferIsFullyOffScreen );

   return UNITY_END();
}
