#include "display.h"
#include "game.h"
#include "tile_map.h"
#include "unity.h"

typedef struct DisplayFillCall_t
{
   Display_t* display;
   u32 color;
   int callCount;
}
DisplayFillCall_t;

typedef struct DisplayDrawTileMapViewportCall_t
{
   Display_t* display;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;
   Vector4i32_t viewport;
   i32 displayX;
   i32 displayY;
   int callCount;
}
DisplayDrawTileMapViewportCall_t;

typedef struct DisplayDrawRectCall_t
{
   Display_t* display;
   i32 x;
   i32 y;
   i32 w;
   i32 h;
   u32 color;
   int callCount;
}
DisplayDrawRectCall_t;

typedef struct PlatformRenderDisplayBufferCall_t
{
   Display_t* display;
   int callCount;
}
PlatformRenderDisplayBufferCall_t;

static Display_t* g_display;
static TileMap_t* g_tileMap;
static TileTextureSet_t* g_tileTextureSet;
static Vector4i32_t g_viewport;
static Vector4i32_t g_playerRect;
static DisplayFillCall_t g_displayFillCall;
static DisplayDrawTileMapViewportCall_t g_displayDrawTileMapViewportCall;
static DisplayDrawRectCall_t g_displayDrawRectCall;
static PlatformRenderDisplayBufferCall_t g_platformRenderDisplayBufferCall;

void setUp( void )
{
   g_display = (Display_t*)1;
   g_tileMap = (TileMap_t*)2;
   g_tileTextureSet = (TileTextureSet_t*)3;
   g_viewport.x = 10;
   g_viewport.y = 20;
   g_viewport.w = 320;
   g_viewport.h = 240;
   g_playerRect.x = 40;
   g_playerRect.y = 60;
   g_playerRect.w = 12;
   g_playerRect.h = 14;

   g_displayFillCall.display = 0;
   g_displayFillCall.color = 0;
   g_displayFillCall.callCount = 0;

   g_displayDrawTileMapViewportCall.display = 0;
   g_displayDrawTileMapViewportCall.tileMap = 0;
   g_displayDrawTileMapViewportCall.tileTextureSet = 0;
   g_displayDrawTileMapViewportCall.viewport.x = 0;
   g_displayDrawTileMapViewportCall.viewport.y = 0;
   g_displayDrawTileMapViewportCall.viewport.w = 0;
   g_displayDrawTileMapViewportCall.viewport.h = 0;
   g_displayDrawTileMapViewportCall.displayX = 0;
   g_displayDrawTileMapViewportCall.displayY = 0;
   g_displayDrawTileMapViewportCall.callCount = 0;

   g_displayDrawRectCall.display = 0;
   g_displayDrawRectCall.x = 0;
   g_displayDrawRectCall.y = 0;
   g_displayDrawRectCall.w = 0;
   g_displayDrawRectCall.h = 0;
   g_displayDrawRectCall.color = 0;
   g_displayDrawRectCall.callCount = 0;

   g_platformRenderDisplayBufferCall.display = 0;
   g_platformRenderDisplayBufferCall.callCount = 0;
}

void tearDown( void ) {}

Display_t* Display_Create( MemArena_t* memArena, u32 w, u32 h )
{
   UNUSED_PARAM( memArena );
   UNUSED_PARAM( w );
   UNUSED_PARAM( h );
   return (Display_t*)1;
}

void Platform_RenderDisplayBuffer( Display_t* display )
{
   g_platformRenderDisplayBufferCall.display = display;
   g_platformRenderDisplayBufferCall.callCount++;
}

Display_t* Game_GetDisplay( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_display;
}

TileTextureSet_t* Game_GetTileTextureSet( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_tileTextureSet;
}

TileMap_t* Game_GetTileMap( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_tileMap;
}

Vector4i32_t Game_GetTileMapViewport( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_viewport;
}

Vector4i32_t Game_GetPlayerRect( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_playerRect;
}

void Display_Fill( Display_t* display, u32 color )
{
   g_displayFillCall.display = display;
   g_displayFillCall.color = color;
   g_displayFillCall.callCount++;
}

void Display_DrawBuffer( Display_t* display, u32* buffer, u32 bufferW, u32 bufferH, i32 displayX, i32 displayY )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( buffer );
   UNUSED_PARAM( bufferW );
   UNUSED_PARAM( bufferH );
   UNUSED_PARAM( displayX );
   UNUSED_PARAM( displayY );
}

void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, TileTextureSet_t* tileTextureSet, Vector4i32_t viewport, i32 displayX, i32 displayY )
{
   g_displayDrawTileMapViewportCall.display = display;
   g_displayDrawTileMapViewportCall.tileMap = tileMap;
   g_displayDrawTileMapViewportCall.tileTextureSet = tileTextureSet;
   g_displayDrawTileMapViewportCall.viewport = viewport;
   g_displayDrawTileMapViewportCall.displayX = displayX;
   g_displayDrawTileMapViewportCall.displayY = displayY;
   g_displayDrawTileMapViewportCall.callCount++;
}

void Display_DrawRect( Display_t* display, i32 x, i32 y, i32 w, i32 h, u32 color )
{
   g_displayDrawRectCall.display = display;
   g_displayDrawRectCall.x = x;
   g_displayDrawRectCall.y = y;
   g_displayDrawRectCall.w = w;
   g_displayDrawRectCall.h = h;
   g_displayDrawRectCall.color = color;
   g_displayDrawRectCall.callCount++;
}

void Display_DrawVector4i( Display_t* display, Vector4i32_t rect, u32 color )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( rect );
   UNUSED_PARAM( color );
}

void test_Game_Render_FillsDisplayWithBlack( void )
{
   Game_Render( (Game_t*)4 );

   TEST_ASSERT_EQUAL_PTR( g_display, g_displayFillCall.display );
   TEST_ASSERT_EQUAL_HEX32( 0x00000000, g_displayFillCall.color );
   TEST_ASSERT_EQUAL_INT( 1, g_displayFillCall.callCount );
}

void test_Game_Render_RendersTileMapViewport( void )
{
   Game_Render( (Game_t*)4 );

   TEST_ASSERT_EQUAL_PTR( g_display, g_displayDrawTileMapViewportCall.display );
   TEST_ASSERT_EQUAL_PTR( g_tileMap, g_displayDrawTileMapViewportCall.tileMap );
   TEST_ASSERT_EQUAL_PTR( g_tileTextureSet, g_displayDrawTileMapViewportCall.tileTextureSet );
   TEST_ASSERT_EQUAL_INT( 10, g_displayDrawTileMapViewportCall.viewport.x );
   TEST_ASSERT_EQUAL_INT( 20, g_displayDrawTileMapViewportCall.viewport.y );
   TEST_ASSERT_EQUAL_INT( 320, g_displayDrawTileMapViewportCall.viewport.w );
   TEST_ASSERT_EQUAL_INT( 240, g_displayDrawTileMapViewportCall.viewport.h );
   TEST_ASSERT_EQUAL_INT( 0, g_displayDrawTileMapViewportCall.displayX );
   TEST_ASSERT_EQUAL_INT( 0, g_displayDrawTileMapViewportCall.displayY );
   TEST_ASSERT_EQUAL_INT( 1, g_displayDrawTileMapViewportCall.callCount );
}

void test_Game_Render_DrawsPlayerRelativeToViewport( void )
{
   Game_Render( (Game_t*)4 );

   TEST_ASSERT_EQUAL_PTR( g_display, g_displayDrawRectCall.display );
   TEST_ASSERT_EQUAL_INT( 30, g_displayDrawRectCall.x );
   TEST_ASSERT_EQUAL_INT( 40, g_displayDrawRectCall.y );
   TEST_ASSERT_EQUAL_INT( 12, g_displayDrawRectCall.w );
   TEST_ASSERT_EQUAL_INT( 14, g_displayDrawRectCall.h );
   TEST_ASSERT_EQUAL_HEX32( 0x00666666, g_displayDrawRectCall.color );
   TEST_ASSERT_EQUAL_INT( 1, g_displayDrawRectCall.callCount );
}

void test_Game_Render_PresentsDisplayBuffer( void )
{
   Game_Render( (Game_t*)4 );

   TEST_ASSERT_EQUAL_PTR( g_display, g_platformRenderDisplayBufferCall.display );
   TEST_ASSERT_EQUAL_INT( 1, g_platformRenderDisplayBufferCall.callCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Render_FillsDisplayWithBlack );
   RUN_TEST( test_Game_Render_RendersTileMapViewport );
   RUN_TEST( test_Game_Render_DrawsPlayerRelativeToViewport );
   RUN_TEST( test_Game_Render_PresentsDisplayBuffer );

   return UNITY_END();
}