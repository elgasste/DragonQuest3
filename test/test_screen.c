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

void setUp( void )
{
   g_pixelBufferCreateCall.pBuffer = 0;
   g_pixelBufferCreateCall.memArena = 0;
   g_pixelBufferCreateCall.w = 0;
   g_pixelBufferCreateCall.h = 0;
   g_pixelBufferCreateCall.callCount = 0;

   g_pixelBufferClearColorCall.color = 0;
   g_pixelBufferClearColorCall.callCount = 0;
}

void tearDown( void ) {}

void PixelBuffer_Create( PixelBuffer_t** buffer, MemArena_t* memArena, u32 w, u32 h )
{
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

   free( screen.buffer );
}

void test_Screen_Fill_FillsPixelBufferWithColor( void )
{
   Screen_t screen;

   Screen_Init( &screen, 0, 10, 10 );

   Screen_Fill( &screen, 50 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( 50, g_pixelBufferClearColorCall.color );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Screen_Init_CreatesPixelBufferWithCorrectParameters );

   RUN_TEST( test_Screen_Fill_FillsPixelBufferWithColor );

   return UNITY_END();
}
