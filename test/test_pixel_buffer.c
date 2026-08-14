#include <stdio.h>
#include <string.h>

#include "mem_arena.h"
#include "pixel_buffer.h"
#include "platform.h"
#include "unity.h"

local_persist int g_fatalErrorCallCount = 0;
local_persist char g_fatalErrorMessage[256] = {0};

void setUp( void )
{
   g_fatalErrorCallCount = 0;
   g_fatalErrorMessage[0] = '\0';
}

void tearDown( void ) {}

internal void test_PixelBuffer_Create_CreatesBufferWithCorrectDimensions( void )
{
   MemArena_t* arena;
   PixelBuffer_t* buffer;

   MemArena_Create( &arena, 1024 );
   PixelBuffer_Create( &buffer, arena, 10, 20 );

   TEST_ASSERT_EQUAL( 10, buffer->w );
   TEST_ASSERT_EQUAL( 20, buffer->h );
   TEST_ASSERT_NOT_NULL( buffer->mem );

   MemArena_Destroy( &arena );
}

internal void test_PixelBuffer_ClearColor_ClearsColor( void )
{
   MemArena_t* arena;
   PixelBuffer_t* buffer;
   u32 color;
   size_t x, y;

   MemArena_Create( &arena, 1024 );
   PixelBuffer_Create( &buffer, arena, 10, 10 );

   color = 0xFF00FF00;
   PixelBuffer_ClearColor( buffer, color );

   for ( y = 0; y < buffer->h; y++ )
   {
      for ( x = 0; x < buffer->w; x++ )
      {
         TEST_ASSERT_EQUAL( color, buffer->mem[y * buffer->w + x] );
      }
   }

   MemArena_Destroy( &arena );
}

void Platform_FatalError( const char* message )
{
   g_fatalErrorCallCount++;

   if ( message )
   {
      snprintf( g_fatalErrorMessage, sizeof( g_fatalErrorMessage ), "%s", message );
   }
}

int main( void )
{
   UNITY_BEGIN();
   
   RUN_TEST( test_PixelBuffer_Create_CreatesBufferWithCorrectDimensions );

   RUN_TEST( test_PixelBuffer_ClearColor_ClearsColor );
   
   return UNITY_END();
}
