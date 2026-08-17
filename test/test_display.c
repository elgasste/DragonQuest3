#include <stdlib.h>

#include "mocks/mock_pixel_buffer.h"

#include "display.h"
#include "mem_arena.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "unity.h"

typedef struct PixelBufferCreateCall_t
{
   MemArena_t* memArena;
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
   g_pixelBufferCreateCall.memArena = 0;
   g_pixelBufferCreateCall.w = 0;
   g_pixelBufferCreateCall.h = 0;
   g_pixelBufferCreateCall.callCount = 0;

   g_pixelBufferClearColorCall.buffer = 0;
   g_pixelBufferClearColorCall.color = 0;
   g_pixelBufferClearColorCall.callCount = 0;
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

PixelBuffer_t* PixelBuffer_Create( MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_t* buffer;

   UNUSED_PARAM( memArena );

   buffer = (PixelBuffer_t*)malloc( sizeof( PixelBuffer_t ) );
   buffer->w = w;
   buffer->h = h;
   buffer->mem = (u32*)calloc( w * h, sizeof( u32 ) );

   g_pixelBufferCreateCall.memArena = memArena;
   g_pixelBufferCreateCall.w = w;
   g_pixelBufferCreateCall.h = h;
   g_pixelBufferCreateCall.callCount++;

   return buffer;
}

void PixelBuffer_Free( PixelBuffer_t* buffer, MemArena_t* memArena )
{
   UNUSED_PARAM( memArena );
   free( buffer->mem );
   free( buffer );
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
   g_pixelBufferClearColorCall.buffer = buffer;
   g_pixelBufferClearColorCall.color = color;
   g_pixelBufferClearColorCall.callCount++;
}

void test_Display_Init_CreatesPixelBufferWithCorrectParameters( void )
{
   Display_t* display;
   MemArena_t memArena;

   display = Display_Create( &memArena, 20, 45 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferCreateCall.callCount );
   TEST_ASSERT_EQUAL( &memArena, g_pixelBufferCreateCall.memArena );
   TEST_ASSERT_EQUAL( 20, g_pixelBufferCreateCall.w );
   TEST_ASSERT_EQUAL( 45, g_pixelBufferCreateCall.h );

   Display_Free( display, &memArena );
}

void test_Display_Fill_FillsPixelBufferWithColor( void )
{
   Display_t* display;

   display = Display_Create( 0, 10, 10 );

   Display_Fill( display, 50 );
   TEST_ASSERT_EQUAL( 1, g_pixelBufferClearColorCall.callCount );
   TEST_ASSERT_EQUAL( 50, g_pixelBufferClearColorCall.color );

   Display_Free( display, 0 );
}

void test_Display_DrawRect_WritesPixelsInsideTheRect( void )
{
   Display_t* display;
   Vector4i32_t rect = { 1, 1, 2, 2 };
   const u32 color = 0x12345678u;

   display = Display_Create( 0, 4, 4 );

   Display_DrawRect( display, rect.x, rect.y, rect.w, rect.h, color );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawRect_ClampsToVisibleArea( void )
{
   Display_t* display;
   Vector4i32_t rect = { -2, -1, 6, 4 };
   const u32 color = 0x9abcdef0u;

   display = Display_Create( 0, 4, 4 );

   Display_DrawRect( display, rect.x, rect.y, rect.w, rect.h, color );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawVector4i_WritesPixelsInsideTheRect( void )
{
   Display_t* display;
   Vector4i32_t rect = { 1, 1, 2, 2 };
   const u32 color = 0x12345678u;

   display = Display_Create( 0, 4, 4 );

   Display_DrawVector4i( display, rect, color );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawVector4i_ClampsToVisibleArea( void )
{
   Display_t* display;
   Vector4i32_t rect = { -2, -1, 6, 4 };
   const u32 color = 0x9abcdef0u;

   display = Display_Create( 0, 4, 4 );

   Display_DrawVector4i( display, rect, color );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( color, Display_GetPixels( display )[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawPixelBuffer_CopiesPixelsWhenFullyVisible( void )
{
   Display_t* display;
   u32 source[4] = {
      0x11111111u, 0x22222222u,
      0x33333333u, 0x44444444u,
   };

   display = Display_Create( 0, 4, 4 );

   Display_DrawBuffer( display, source, 2, 2, 1, 1 );

   TEST_ASSERT_EQUAL( 0x11111111u, Display_GetPixels( display )[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x22222222u, Display_GetPixels( display )[ ( 1 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0x33333333u, Display_GetPixels( display )[ ( 2 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x44444444u, Display_GetPixels( display )[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawPixelBuffer_ClipsTopLeftAndCopiesVisiblePixels( void )
{
   Display_t* display;
   u32 source[9] = {
      1u, 2u, 3u,
      4u, 5u, 6u,
      7u, 8u, 9u,
   };

   display = Display_Create( 0, 4, 4 );

   Display_DrawBuffer( display, source, 3, 3, -1, -1 );

   TEST_ASSERT_EQUAL( 5u, Display_GetPixels( display )[ ( 0 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 6u, Display_GetPixels( display )[ ( 0 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 8u, Display_GetPixels( display )[ ( 1 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 9u, Display_GetPixels( display )[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 0 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 2 * 4 ) + 0 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawPixelBuffer_ClipsBottomRightAndCopiesVisiblePixels( void )
{
   Display_t* display;
   u32 source[9] = {
      1u, 2u, 3u,
      4u, 5u, 6u,
      7u, 8u, 9u,
   };

   display = Display_Create( 0, 4, 4 );

   Display_DrawBuffer( display, source, 3, 3, 3, 2 );

   TEST_ASSERT_EQUAL( 1u, Display_GetPixels( display )[ ( 2 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 4u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 2 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawPixelBuffer_DoesNothingWhenBufferIsFullyOffScreen( void )
{
   Display_t* display;
   u32 source[4] = {
      0xaaaaaaaau, 0xbbbbbbbbu,
      0xccccccccu, 0xddddddddu,
   };

   display = Display_Create( 0, 4, 4 );

   Display_DrawBuffer( display, source, 2, 2, 5, 1 );

   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawTileMapViewport_DrawsVisibleTiles( void )
{
   Display_t* display;
   u32 textures[16] = {
      0x01010101u, 0x01010101u, 0x01010101u, 0x01010101u,
      0x02020202u, 0x02020202u, 0x02020202u, 0x02020202u,
      0x03030303u, 0x03030303u, 0x03030303u, 0x03030303u,
      0x04040404u, 0x04040404u, 0x04040404u, 0x04040404u,
   };
   TileTextureSet_t textureSet;
   Tile_t tiles[4];
   TileMap_t tileMap;
   Vector4i32_t viewport = { 0, 0, 4, 4 };

   textureSet.count = 4;
   textureSet.tileSize = 2;
   textureSet.textures = textures;

   tiles[0].textureIndex = 0;
   tiles[1].textureIndex = 1;
   tiles[2].textureIndex = 2;
   tiles[3].textureIndex = 3;

   tileMap.id = 0;
   tileMap.tilesX = 2;
   tileMap.tilesY = 2;
   tileMap.tiles = tiles;
   tileMap.tileTextureSet = &textureSet;

   display = Display_Create( 0, 4, 4 );

   Display_DrawTileMapViewport( display, &tileMap, viewport, 0, 0 );

   TEST_ASSERT_EQUAL( 0x01010101u, Display_GetPixels( display )[ ( 0 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 0x01010101u, Display_GetPixels( display )[ ( 1 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x02020202u, Display_GetPixels( display )[ ( 0 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0x02020202u, Display_GetPixels( display )[ ( 1 * 4 ) + 3 ] );
   TEST_ASSERT_EQUAL( 0x03030303u, Display_GetPixels( display )[ ( 2 * 4 ) + 0 ] );
   TEST_ASSERT_EQUAL( 0x03030303u, Display_GetPixels( display )[ ( 3 * 4 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x04040404u, Display_GetPixels( display )[ ( 2 * 4 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0x04040404u, Display_GetPixels( display )[ ( 3 * 4 ) + 3 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawTileMapViewport_UsesViewportOffset( void )
{
   Display_t* display;
   u32 textures[16] = {
      0x11111111u, 0x11111111u, 0x11111111u, 0x11111111u,
      0x22222222u, 0x22222222u, 0x22222222u, 0x22222222u,
      0x33333333u, 0x33333333u, 0x33333333u, 0x33333333u,
      0x44444444u, 0x44444444u, 0x44444444u, 0x44444444u,
   };
   TileTextureSet_t textureSet;
   Tile_t tiles[4];
   TileMap_t tileMap;
   Vector4i32_t viewport = { 1, 1, 2, 2 };

   textureSet.count = 4;
   textureSet.tileSize = 2;
   textureSet.textures = textures;

   tiles[0].textureIndex = 0;
   tiles[1].textureIndex = 1;
   tiles[2].textureIndex = 2;
   tiles[3].textureIndex = 3;

   tileMap.id = 0;
   tileMap.tilesX = 2;
   tileMap.tilesY = 2;
   tileMap.tiles = tiles;
   tileMap.tileTextureSet = &textureSet;

   display = Display_Create( 0, 2, 2 );

   Display_DrawTileMapViewport( display, &tileMap, viewport, 0, 0 );

   TEST_ASSERT_EQUAL( 0x11111111u, Display_GetPixels( display )[ ( 0 * 2 ) + 0 ] );
   TEST_ASSERT_EQUAL( 0x22222222u, Display_GetPixels( display )[ ( 0 * 2 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x33333333u, Display_GetPixels( display )[ ( 1 * 2 ) + 0 ] );
   TEST_ASSERT_EQUAL( 0x44444444u, Display_GetPixels( display )[ ( 1 * 2 ) + 1 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawTileMapViewport_ClipsWhenDisplayPositionIsOffscreen( void )
{
   Display_t* display;
   u32 textures[16] = {
      0xabcdef01u, 0xabcdef01u, 0xabcdef01u, 0xabcdef01u,
      0x12345678u, 0x12345678u, 0x12345678u, 0x12345678u,
      0x87654321u, 0x87654321u, 0x87654321u, 0x87654321u,
      0x0f0f0f0fu, 0x0f0f0f0fu, 0x0f0f0f0fu, 0x0f0f0f0fu,
   };
   TileTextureSet_t textureSet;
   Tile_t tiles[4];
   TileMap_t tileMap;
   Vector4i32_t viewport = { 0, 0, 4, 4 };

   textureSet.count = 4;
   textureSet.tileSize = 2;
   textureSet.textures = textures;

   tiles[0].textureIndex = 0;
   tiles[1].textureIndex = 1;
   tiles[2].textureIndex = 2;
   tiles[3].textureIndex = 3;

   tileMap.id = 0;
   tileMap.tilesX = 2;
   tileMap.tilesY = 2;
   tileMap.tiles = tiles;
   tileMap.tileTextureSet = &textureSet;

   display = Display_Create( 0, 3, 3 );

   Display_DrawTileMapViewport( display, &tileMap, viewport, 2, 2 );

   TEST_ASSERT_EQUAL( 0xabcdef01u, Display_GetPixels( display )[ ( 2 * 3 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 0 * 3 ) + 0 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 1 * 3 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0u, Display_GetPixels( display )[ ( 2 * 3 ) + 1 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawTileMapViewport_CentersSmallMapInViewport( void )
{
   Display_t* display;
   u32 textures[9] = {
      0x01010101u, 0x02020202u, 0x03030303u,
      0x04040404u, 0x05050505u, 0x06060606u,
      0x07070707u, 0x08080808u, 0x09090909u,
   };
   TileTextureSet_t textureSet;
   Tile_t tiles[9];
   TileMap_t tileMap;
   Vector4i32_t viewport = { 1, 1, 5, 5 };
   u32 tileIndex;

   textureSet.count = 9;
   textureSet.tileSize = 1;
   textureSet.textures = textures;

   for ( tileIndex = 0; tileIndex < 9; tileIndex++ )
   {
      tiles[tileIndex].textureIndex = (u32)tileIndex;
   }

   tileMap.id = 0;
   tileMap.tilesX = 3;
   tileMap.tilesY = 3;
   tileMap.tiles = tiles;
   tileMap.tileTextureSet = &textureSet;
   tileMap.wraps = False;

   display = Display_Create( 0, 5, 5 );

   Display_DrawTileMapViewport( display, &tileMap, viewport, 0, 0 );

   TEST_ASSERT_EQUAL( 0x01010101u, Display_GetPixels( display )[ ( 1 * 5 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x02020202u, Display_GetPixels( display )[ ( 1 * 5 ) + 2 ] );
   TEST_ASSERT_EQUAL( 0x04040404u, Display_GetPixels( display )[ ( 2 * 5 ) + 1 ] );
   TEST_ASSERT_EQUAL( 0x05050505u, Display_GetPixels( display )[ ( 2 * 5 ) + 2 ] );

   Display_Free( display, 0 );
}

void test_Display_DrawTileMapViewport_RepeatsWrappedMap( void )
{
   Display_t* display;
   u32 textures[2] = {
      0x01010101u, 0x02020202u,
   };
   TileTextureSet_t textureSet;
   Tile_t tiles[2];
   TileMap_t tileMap;
   Vector4i32_t viewport = { 1, 0, 4, 1 };

   textureSet.count = 2;
   textureSet.tileSize = 1;
   textureSet.textures = textures;

   tiles[0].textureIndex = 0;
   tiles[1].textureIndex = 1;

   tileMap.id = 0;
   tileMap.tilesX = 2;
   tileMap.tilesY = 1;
   tileMap.tiles = tiles;
   tileMap.tileTextureSet = &textureSet;
   tileMap.wraps = True;

   display = Display_Create( 0, 4, 1 );

   Display_DrawTileMapViewport( display, &tileMap, viewport, 0, 0 );

   TEST_ASSERT_EQUAL( 0x02020202u, Display_GetPixels( display )[0] );
   TEST_ASSERT_EQUAL( 0x01010101u, Display_GetPixels( display )[1] );
   TEST_ASSERT_EQUAL( 0x02020202u, Display_GetPixels( display )[2] );
   TEST_ASSERT_EQUAL( 0x01010101u, Display_GetPixels( display )[3] );

   Display_Free( display, 0 );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Display_Init_CreatesPixelBufferWithCorrectParameters );

   RUN_TEST( test_Display_Fill_FillsPixelBufferWithColor );
   
   RUN_TEST( test_Display_DrawRect_WritesPixelsInsideTheRect );
   RUN_TEST( test_Display_DrawRect_ClampsToVisibleArea );

   RUN_TEST( test_Display_DrawVector4i_WritesPixelsInsideTheRect );
   RUN_TEST( test_Display_DrawVector4i_ClampsToVisibleArea );

   RUN_TEST( test_Display_DrawPixelBuffer_CopiesPixelsWhenFullyVisible );
   RUN_TEST( test_Display_DrawPixelBuffer_ClipsTopLeftAndCopiesVisiblePixels );
   RUN_TEST( test_Display_DrawPixelBuffer_ClipsBottomRightAndCopiesVisiblePixels );
   RUN_TEST( test_Display_DrawPixelBuffer_DoesNothingWhenBufferIsFullyOffScreen );
   
   RUN_TEST( test_Display_DrawTileMapViewport_DrawsVisibleTiles );
   RUN_TEST( test_Display_DrawTileMapViewport_UsesViewportOffset );
   RUN_TEST( test_Display_DrawTileMapViewport_ClipsWhenDisplayPositionIsOffscreen );
   RUN_TEST( test_Display_DrawTileMapViewport_CentersSmallMapInViewport );
   RUN_TEST( test_Display_DrawTileMapViewport_RepeatsWrappedMap );

   return UNITY_END();
}
