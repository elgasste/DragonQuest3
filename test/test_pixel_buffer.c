#include <stdio.h>
#include <stdlib.h>
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

void* MemArena_Alloc( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   return malloc( size );
}

void MemArena_Free( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   free( mem );
}

internal void test_PixelBuffer_Init_SetsCorrectDimensions( void )
{
   PixelBuffer_t* buffer;

   buffer = PixelBuffer_Create( 0, 10, 20 );

   TEST_ASSERT_EQUAL( 10, PixelBuffer_GetWidth( buffer ) );
   TEST_ASSERT_EQUAL( 20, PixelBuffer_GetHeight( buffer ) );
   TEST_ASSERT_NOT_NULL( PixelBuffer_GetPixels( buffer ) );

   PixelBuffer_Free( buffer, 0 );
}

internal void test_PixelBuffer_ClearColor_ClearsColor( void )
{
   PixelBuffer_t* buffer;
   u32 color;
   size_t x, y;

   buffer = PixelBuffer_Create( 0, 10, 10 );

   color = 0xFF00FF00;
   PixelBuffer_ClearColor( buffer, color );

   for ( y = 0; y < PixelBuffer_GetHeight( buffer ); y++ )
   {
      for ( x = 0; x < PixelBuffer_GetWidth( buffer ); x++ )
      {
         TEST_ASSERT_EQUAL( color, ((u32*)PixelBuffer_GetPixels( buffer ))[y * PixelBuffer_GetWidth( buffer ) + x] );
      }
   }

   PixelBuffer_Free( buffer, 0 );
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
   
   RUN_TEST( test_PixelBuffer_Init_SetsCorrectDimensions );

   RUN_TEST( test_PixelBuffer_ClearColor_ClearsColor );
   
   return UNITY_END();
}
