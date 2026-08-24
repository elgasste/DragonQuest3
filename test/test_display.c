#include <stdlib.h>
#include <string.h>

#include "mocks/mock_pixel_buffer.h"
#include "mocks/mock_tile.h"
#include "mocks/mock_tile_map.h"
#include "mocks/mock_tile_texture_set.h"

#include "display.h"
#include "unity.h"

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   free( mem );
}

PixelBuffer_t* PixelBuffer_Create( MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_t* buffer;

   buffer = (PixelBuffer_t*)MemArena_AllocMem( memArena, sizeof( PixelBuffer_t ) );
   buffer->w = w;
   buffer->h = h;
   buffer->mem = (u32*)MemArena_AllocMem( memArena, w * h * sizeof( u32 ) );
   memset( buffer->mem, 0, w * h * sizeof( u32 ) );
   return buffer;
}

void PixelBuffer_Free( PixelBuffer_t* buffer, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, buffer->mem );
   MemArena_FreeMem( memArena, buffer );
}

u32 PixelBuffer_GetWidth( PixelBuffer_t* buffer )
{
   return buffer->w;
}

u32 PixelBuffer_GetHeight( PixelBuffer_t* buffer )
{
   return buffer->h;
}

u32* PixelBuffer_GetPixels( PixelBuffer_t* buffer )
{
   return buffer->mem;
}

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color )
{
   u32 i;
   u32 pixelCount = buffer->w * buffer->h;

   for ( i = 0; i < pixelCount; i++ )
   {
      buffer->mem[i] = color;
   }
}

u32 Tile_GetTextureIndex( Tile_t* tile )
{
   return tile->textureIndex;
}

u32 TileMap_GetTilesX( TileMap_t* tileMap )
{
   return tileMap->width;
}

u32 TileMap_GetTilesY( TileMap_t* tileMap )
{
   return tileMap->height;
}

b32 TileMap_GetWraps( TileMap_t* tileMap )
{
   return tileMap->wraps;
}

Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 x, u32 y )
{
   return &tileMap->tiles[y * tileMap->width + x];
}

u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->tileSize;
}

u32* TileTextureSet_GetTexture( TileTextureSet_t* tileTextureSet, u32 index )
{
   return &tileTextureSet->textures[index * tileTextureSet->tileSize * tileTextureSet->tileSize];
}

internal Display_t* CreateDisplay( u32 width, u32 height )
{
   return Display_Create( (MemArena_t*)1, width, height );
}

internal void AssertPixel( Display_t* display, u32 x, u32 y, u32 expected )
{
   const u32* pixels = Display_GetPixels( display );
   u32 width = Display_GetWidth( display );

   TEST_ASSERT_EQUAL_UINT32( expected, pixels[y * width + x] );
}

internal void AssertAllPixels( Display_t* display, u32 expected )
{
   u32 x, y;
   u32 width = Display_GetWidth( display );
   u32 height = Display_GetHeight( display );

   for ( y = 0; y < height; y++ )
   {
      for ( x = 0; x < width; x++ )
      {
         AssertPixel( display, x, y, expected );
      }
   }
}

void setUp( void ) {}
void tearDown( void ) {}

void test_Display_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, Display_GetStructSize() );
}

void test_Display_Create_StoresDimensionsAndInitializesPixels( void )
{
   Display_t* display = CreateDisplay( 3, 2 );

   TEST_ASSERT_NOT_NULL( display );
   TEST_ASSERT_EQUAL_UINT( 3, Display_GetWidth( display ) );
   TEST_ASSERT_EQUAL_UINT( 2, Display_GetHeight( display ) );
   TEST_ASSERT_NOT_NULL( Display_GetPixels( display ) );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_Fill_FillsTheWholeDisplay( void )
{
   Display_t* display = CreateDisplay( 3, 2 );

   Display_Fill( display, 0x00112233u );
   AssertAllPixels( display, 0x00112233u );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawRect_DrawsRectangleAtRequestedPosition( void )
{
   Display_t* display = CreateDisplay( 4, 3 );

   Display_DrawRect( display, 1, 1, 2, 2, 0x00ABCDEFu );

   AssertPixel( display, 0, 0, 0 );
   AssertPixel( display, 1, 1, 0x00ABCDEFu );
   AssertPixel( display, 2, 1, 0x00ABCDEFu );
   AssertPixel( display, 1, 2, 0x00ABCDEFu );
   AssertPixel( display, 2, 2, 0x00ABCDEFu );
   AssertPixel( display, 3, 2, 0 );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawRect_ClipsRectangleToDisplayBounds( void )
{
   Display_t* display = CreateDisplay( 4, 3 );

   Display_DrawRect( display, -1, -1, 3, 3, 0x00010203u );

   AssertPixel( display, 0, 0, 0x00010203u );
   AssertPixel( display, 1, 0, 0x00010203u );
   AssertPixel( display, 0, 1, 0x00010203u );
   AssertPixel( display, 1, 1, 0x00010203u );
   AssertPixel( display, 2, 2, 0 );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawVector4i_UsesVectorAsRectangle( void )
{
   Vector4i32_t rect = { 1, 0, 2, 1 };
   Display_t* display = CreateDisplay( 4, 2 );

   Display_DrawVector4i( display, rect, 0x00FEDCBAu );

   AssertPixel( display, 0, 0, 0 );
   AssertPixel( display, 1, 0, 0x00FEDCBAu );
   AssertPixel( display, 2, 0, 0x00FEDCBAu );
   AssertPixel( display, 3, 0, 0 );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawBuffer_ClipsSourceAndDestination( void )
{
   u32 source[] = { 1, 2, 3, 4, 5, 6 };
   Display_t* display = CreateDisplay( 3, 2 );

   Display_DrawBuffer( display, source, 3, 2, -1, 0 );

   AssertPixel( display, 0, 0, 2 );
   AssertPixel( display, 1, 0, 3 );
   AssertPixel( display, 2, 0, 0 );
   AssertPixel( display, 0, 1, 5 );
   AssertPixel( display, 1, 1, 6 );
   AssertPixel( display, 2, 1, 0 );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawTileMapViewport_DrawsVisibleNonWrappingTiles( void )
{
   Tile_t tiles[] = { { 0 }, { 1 }, { 2 }, { 3 } };
   u32 textures[] = { 0x00000011u, 0x00000022u, 0x00000033u, 0x00000044u };
   TileMap_t tileMap = { 2, 2, False, tiles };
   TileTextureSet_t textureSet = { 4, 1, textures };
   Display_t* display = CreateDisplay( 2, 2 );

   Display_DrawTileMapViewport( display, &tileMap, &textureSet, (Vector4i32_t){ 0, 0, 2 * WORLD_UNITS_PER_PIXEL, 2 * WORLD_UNITS_PER_PIXEL }, 0, 0 );

   AssertPixel( display, 0, 0, 0x00000011u );
   AssertPixel( display, 1, 0, 0x00000022u );
   AssertPixel( display, 0, 1, 0x00000033u );
   AssertPixel( display, 1, 1, 0x00000044u );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawTileMapViewport_DrawsWrappingTilesAcrossViewport( void )
{
   Tile_t tiles[] = { { 0 } };
   u32 textures[] = { 0x00000077u };
   TileMap_t tileMap = { 1, 1, True, tiles };
   TileTextureSet_t textureSet = { 1, 1, textures };
   Display_t* display = CreateDisplay( 3, 2 );

   Display_DrawTileMapViewport( display, &tileMap, &textureSet, (Vector4i32_t){ 0, 0, 3 * WORLD_UNITS_PER_PIXEL, 2 * WORLD_UNITS_PER_PIXEL }, 0, 0 );

   AssertAllPixels( display, 0x00000077u );
   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawTileMapViewport_RepeatsMultiTileMapInBothAxes( void )
{
   Tile_t tiles[] = { { 0 }, { 1 }, { 2 }, { 3 } };
   u32 textures[] = { 0x00000011u, 0x00000022u, 0x00000033u, 0x00000044u };
   TileMap_t tileMap = { 2, 2, True, tiles };
   TileTextureSet_t textureSet = { 4, 1, textures };
   Display_t* display = CreateDisplay( 4, 4 );

   Display_DrawTileMapViewport( display, &tileMap, &textureSet, (Vector4i32_t){ 0, 0, 4 * WORLD_UNITS_PER_PIXEL, 4 * WORLD_UNITS_PER_PIXEL }, 0, 0 );

   AssertPixel( display, 0, 0, 0x00000011u );
   AssertPixel( display, 1, 0, 0x00000022u );
   AssertPixel( display, 2, 0, 0x00000011u );
   AssertPixel( display, 3, 0, 0x00000022u );
   AssertPixel( display, 0, 1, 0x00000033u );
   AssertPixel( display, 1, 1, 0x00000044u );
   AssertPixel( display, 2, 1, 0x00000033u );
   AssertPixel( display, 3, 1, 0x00000044u );
   AssertPixel( display, 0, 2, 0x00000011u );
   AssertPixel( display, 1, 2, 0x00000022u );
   AssertPixel( display, 0, 3, 0x00000033u );
   AssertPixel( display, 1, 3, 0x00000044u );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawTileMapViewport_HandlesNegativeWrappingViewport( void )
{
   Tile_t tiles[] = { { 0 }, { 1 }, { 2 }, { 3 } };
   u32 textures[] = { 0x00000011u, 0x00000022u, 0x00000033u, 0x00000044u };
   TileMap_t tileMap = { 2, 2, True, tiles };
   TileTextureSet_t textureSet = { 4, 1, textures };
   Display_t* display = CreateDisplay( 3, 3 );

   Display_DrawTileMapViewport( display, &tileMap, &textureSet, (Vector4i32_t){ -1 * WORLD_UNITS_PER_PIXEL, -1 * WORLD_UNITS_PER_PIXEL, 3 * WORLD_UNITS_PER_PIXEL, 3 * WORLD_UNITS_PER_PIXEL }, 0, 0 );

   AssertPixel( display, 0, 0, 0x00000044u );
   AssertPixel( display, 1, 0, 0x00000033u );
   AssertPixel( display, 2, 0, 0x00000044u );
   AssertPixel( display, 0, 1, 0x00000022u );
   AssertPixel( display, 1, 1, 0x00000011u );
   AssertPixel( display, 2, 1, 0x00000022u );
   AssertPixel( display, 0, 2, 0x00000044u );
   AssertPixel( display, 1, 2, 0x00000033u );
   AssertPixel( display, 2, 2, 0x00000044u );

   Display_Free( display, (MemArena_t*)1 );
}

void test_Display_DrawTileMapViewport_AppliesDisplayOffsetForWrappingMap( void )
{
   Tile_t tiles[] = { { 0 } };
   u32 textures[] = { 0x00000099u };
   TileMap_t tileMap = { 1, 1, True, tiles };
   TileTextureSet_t textureSet = { 1, 1, textures };
   Display_t* display = CreateDisplay( 4, 4 );

   Display_DrawTileMapViewport( display, &tileMap, &textureSet, (Vector4i32_t){ 0, 0, 2 * WORLD_UNITS_PER_PIXEL, 2 * WORLD_UNITS_PER_PIXEL }, 1, 1 );

   AssertPixel( display, 0, 0, 0 );
   AssertPixel( display, 1, 1, 0x00000099u );
   AssertPixel( display, 2, 1, 0x00000099u );
   AssertPixel( display, 1, 2, 0x00000099u );
   AssertPixel( display, 2, 2, 0x00000099u );
   AssertPixel( display, 3, 3, 0 );

   Display_Free( display, (MemArena_t*)1 );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Display_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_Display_Create_StoresDimensionsAndInitializesPixels );

   RUN_TEST( test_Display_Fill_FillsTheWholeDisplay );

   RUN_TEST( test_Display_DrawRect_DrawsRectangleAtRequestedPosition );
   RUN_TEST( test_Display_DrawRect_ClipsRectangleToDisplayBounds );

   RUN_TEST( test_Display_DrawVector4i_UsesVectorAsRectangle );

   RUN_TEST( test_Display_DrawBuffer_ClipsSourceAndDestination );

   RUN_TEST( test_Display_DrawTileMapViewport_DrawsVisibleNonWrappingTiles );
   RUN_TEST( test_Display_DrawTileMapViewport_DrawsWrappingTilesAcrossViewport );
   RUN_TEST( test_Display_DrawTileMapViewport_RepeatsMultiTileMapInBothAxes );
   RUN_TEST( test_Display_DrawTileMapViewport_HandlesNegativeWrappingViewport );
   RUN_TEST( test_Display_DrawTileMapViewport_AppliesDisplayOffsetForWrappingMap );

   return UNITY_END();
}
