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

typedef struct PlatformRenderDisplayBufferCall_t
{
   Display_t* display;
   int callCount;
}
PlatformRenderDisplayBufferCall_t;

local_persist DisplayFillCall_t g_displayFillCall;
local_persist PlatformRenderDisplayBufferCall_t g_platformRenderDisplayBufferCall;

void setUp( void )
{
   g_displayFillCall.display = 0;
   g_displayFillCall.color = 0;
   g_displayFillCall.callCount = 0;

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

void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, Vector4i32_t viewport, i32 displayX, i32 displayY )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( viewport );
   UNUSED_PARAM( displayX );
   UNUSED_PARAM( displayY );
}

void Display_DrawRect( Display_t* display, i32 x, i32 y, i32 w, i32 h, u32 color )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( x );
   UNUSED_PARAM( y );
   UNUSED_PARAM( w );
   UNUSED_PARAM( h );
   UNUSED_PARAM( color );
}

void Display_DrawVector4i( Display_t* display, Vector4i32_t rect, u32 color )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( rect );
   UNUSED_PARAM( color );
}

void test_Game_Render_FillsDisplayBufferWithBlack( void )
{
   Game_t game;
   Display_t* display;

   display = Display_Create( 0, 0, 0 );
   game.display = display;

   Game_Render( &game );
   TEST_ASSERT_EQUAL_PTR( display, g_displayFillCall.display );
   TEST_ASSERT_EQUAL_HEX32( 0x00000000, g_displayFillCall.color );
   TEST_ASSERT_EQUAL_INT( 1, g_displayFillCall.callCount );
}

void test_Game_Render_RendersDisplayBuffer( void )
{
   Game_t game;
   Display_t* display;

   display = Display_Create( 0, 0, 0 );
   game.display = display;

   Game_Render( &game );
   TEST_ASSERT_EQUAL_PTR( display, g_platformRenderDisplayBufferCall.display );
   TEST_ASSERT_EQUAL_INT( 1, g_platformRenderDisplayBufferCall.callCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Render_FillsDisplayBufferWithBlack );

   RUN_TEST( test_Game_Render_RendersDisplayBuffer );

   return UNITY_END();
}