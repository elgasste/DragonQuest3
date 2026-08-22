#include <stdlib.h>

#include "mem_arena.h"
#include "pixel_buffer.h"
#include "unity.h"

global u32 g_allocationCount;
global u32 g_freeCount;

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   g_allocationCount++;
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   g_freeCount++;
   free( mem );
}

internal PixelBuffer_t* CreatePixelBuffer( u32 width, u32 height )
{
   return PixelBuffer_Create( 0, width, height );
}

internal void AssertPixelsEqual( const u32* expected, const u32* actual, size_t count )
{
   size_t i;

   for ( i = 0; i < count; i++ )
   {
      TEST_ASSERT_EQUAL_UINT32( expected[i], actual[i] );
   }
}

void setUp( void )
{
   g_allocationCount = 0;
   g_freeCount = 0;
}

void tearDown( void ) {}

void test_PixelBuffer_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, PixelBuffer_GetStructSize() );
}

void test_PixelBuffer_Create_StoresDimensionsAndAllocatesPixels( void )
{
   PixelBuffer_t* buffer = CreatePixelBuffer( 3, 2 );

   TEST_ASSERT_NOT_NULL( buffer );
   TEST_ASSERT_EQUAL_UINT( 3, PixelBuffer_GetWidth( buffer ) );
   TEST_ASSERT_EQUAL_UINT( 2, PixelBuffer_GetHeight( buffer ) );
   TEST_ASSERT_NOT_NULL( PixelBuffer_GetPixels( buffer ) );
   TEST_ASSERT_EQUAL_UINT( 2, g_allocationCount );

   PixelBuffer_Free( buffer, 0 );
}

void test_PixelBuffer_GetPixels_ReturnsWritablePixelMemory( void )
{
   u32 expected[] = { 0x00000011u, 0x00000022u, 0x00000033u, 0x00000044u };
   PixelBuffer_t* buffer = CreatePixelBuffer( 2, 2 );
   u32* pixels = PixelBuffer_GetPixels( buffer );

   pixels[0] = expected[0];
   pixels[1] = expected[1];
   pixels[2] = expected[2];
   pixels[3] = expected[3];

   AssertPixelsEqual( expected, PixelBuffer_GetPixels( buffer ), 4 );
   PixelBuffer_Free( buffer, 0 );
}

void test_PixelBuffer_ClearColor_FillsEveryPixel( void )
{
   u32 expected[6] = { 0x00A1B2C3u, 0x00A1B2C3u, 0x00A1B2C3u, 0x00A1B2C3u, 0x00A1B2C3u, 0x00A1B2C3u };
   PixelBuffer_t* buffer = CreatePixelBuffer( 3, 2 );

   PixelBuffer_ClearColor( buffer, 0x00A1B2C3u );

   AssertPixelsEqual( expected, PixelBuffer_GetPixels( buffer ), 6 );
   PixelBuffer_Free( buffer, 0 );
}

void test_PixelBuffer_ClearColor_OverwritesPreviousPixelValues( void )
{
   u32* pixels;
   PixelBuffer_t* buffer = CreatePixelBuffer( 2, 2 );

   pixels = PixelBuffer_GetPixels( buffer );
   pixels[0] = 1;
   pixels[1] = 2;
   pixels[2] = 3;
   pixels[3] = 4;
   PixelBuffer_ClearColor( buffer, 0x00FFFFFFu );

   TEST_ASSERT_EQUAL_UINT32( 0x00FFFFFFu, pixels[0] );
   TEST_ASSERT_EQUAL_UINT32( 0x00FFFFFFu, pixels[1] );
   TEST_ASSERT_EQUAL_UINT32( 0x00FFFFFFu, pixels[2] );
   TEST_ASSERT_EQUAL_UINT32( 0x00FFFFFFu, pixels[3] );
   PixelBuffer_Free( buffer, 0 );
}

void test_PixelBuffer_Free_ReleasesBufferAndPixels( void )
{
   PixelBuffer_t* buffer = CreatePixelBuffer( 4, 4 );

   PixelBuffer_Free( buffer, 0 );

   TEST_ASSERT_EQUAL_UINT( 2, g_allocationCount );
   TEST_ASSERT_EQUAL_UINT( 2, g_freeCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_PixelBuffer_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_PixelBuffer_Create_StoresDimensionsAndAllocatesPixels );

   RUN_TEST( test_PixelBuffer_GetPixels_ReturnsWritablePixelMemory );

   RUN_TEST( test_PixelBuffer_ClearColor_FillsEveryPixel );
   RUN_TEST( test_PixelBuffer_ClearColor_OverwritesPreviousPixelValues );
   
   RUN_TEST( test_PixelBuffer_Free_ReleasesBufferAndPixels );

   return UNITY_END();
}
