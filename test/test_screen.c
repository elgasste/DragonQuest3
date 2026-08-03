#include <stdlib.h>

#include "mem_arena.h"
#include "pixel_buffer.h"
#include "screen.h"
#include "unity.h"

typedef struct PixelBufferCreateCall_t
{
   MemArena_t* memArena;
   PixelBuffer_t** buffer;
   u32 w;
   u32 h;
   int callCount;
}
PixelBufferCreateCall_t;

local_persist PixelBufferCreateCall_t g_pixelBufferCreateCall;

void setUp( void )
{
   g_pixelBufferCreateCall.buffer = 0;
   g_pixelBufferCreateCall.memArena = 0;
   g_pixelBufferCreateCall.w = 0;
   g_pixelBufferCreateCall.h = 0;
   g_pixelBufferCreateCall.callCount = 0;
}

void tearDown( void ) {}

void PixelBuffer_Create( PixelBuffer_t** buffer, MemArena_t* memArena, u32 w, u32 h )
{
   g_pixelBufferCreateCall.buffer = buffer;
   g_pixelBufferCreateCall.memArena = memArena;
   g_pixelBufferCreateCall.w = w;
   g_pixelBufferCreateCall.h = h;
   g_pixelBufferCreateCall.callCount++;
}

void test_Screen_Init_CreatesPixelBufferWithCorrectParameters( void )
{
   Screen_t screen;
   MemArena_t memArena;

   screen.buffer = (PixelBuffer_t*)malloc( sizeof( PixelBuffer_t ) );

   Screen_Init( &screen, &memArena, 20, 45 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCall.callCount );
   TEST_ASSERT_EQUAL( &( screen.buffer ), g_pixelBufferCreateCall.buffer );
   TEST_ASSERT_EQUAL( &memArena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( 20, g_pixelBufferCreateCall.w );
   TEST_ASSERT_EQUAL( 45, g_pixelBufferCreateCall.h );

   free( screen.buffer );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Screen_Init_CreatesPixelBufferWithCorrectParameters );

   return UNITY_END();
}
