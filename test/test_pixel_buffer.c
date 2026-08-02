#include "unity.h"
#include "pixel_buffer.h"
#include "mem_arena.h"

internal void test_PixelBuffer_ClearColor_ClearsColor( void );

void setUp( void ) {}
void tearDown( void ) {}

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

int main( void )
{
   UNITY_BEGIN();
   
   RUN_TEST( test_PixelBuffer_ClearColor_ClearsColor );
   
   return UNITY_END();
}

void FatalError( const char* message )
{
   TEST_FAIL_MESSAGE( message );
}