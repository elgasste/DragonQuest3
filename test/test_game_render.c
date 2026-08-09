#include "game.h"
#include "screen.h"
#include "unity.h"

typedef struct ScreenFillCall_t
{
   Screen_t* screen;
   u32 color;
   int callCount;
}
ScreenFillCall_t;

typedef struct PlatformRenderScreenBufferCall_t
{
   Screen_t* screen;
   int callCount;
}
PlatformRenderScreenBufferCall_t;

local_persist ScreenFillCall_t g_screenFillCall;
local_persist PlatformRenderScreenBufferCall_t g_platformRenderScreenBufferCall;

void setUp( void )
{
   g_screenFillCall.screen = 0;
   g_screenFillCall.color = 0;
   g_screenFillCall.callCount = 0;

   g_platformRenderScreenBufferCall.screen = 0;
   g_platformRenderScreenBufferCall.callCount = 0;
}

void tearDown( void ) {}

void Platform_RenderScreenBuffer( Screen_t* screen )
{
   g_platformRenderScreenBufferCall.screen = screen;
   g_platformRenderScreenBufferCall.callCount++;
}

void Screen_Fill( Screen_t* screen, u32 color )
{
   g_screenFillCall.screen = screen;
   g_screenFillCall.color = color;
   g_screenFillCall.callCount++;
}

void Screen_DrawBuffer( Screen_t* screen, u32* buffer, u32 bufferW, u32 bufferH, i32 screenX, i32 screenY )
{
   UNUSED_PARAM( screen );
   UNUSED_PARAM( buffer );
   UNUSED_PARAM( bufferW );
   UNUSED_PARAM( bufferH );
   UNUSED_PARAM( screenX );
   UNUSED_PARAM( screenY );
}

void Screen_DrawTileMapViewport( Screen_t* screen, TileMap_t* tileMap, Vector4i32_t viewport, i32 screenX, i32 screenY )
{
   UNUSED_PARAM( screen );
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( viewport );
   UNUSED_PARAM( screenX );
   UNUSED_PARAM( screenY );
}

void test_Game_Render_FillsScreenBufferWithBlack( void )
{
   Game_t game;
   Screen_t screen;

   game.screen = &screen;

   Game_Render( &game );
   TEST_ASSERT_EQUAL_PTR( &screen, g_screenFillCall.screen );
   TEST_ASSERT_EQUAL_HEX32( 0x00000000, g_screenFillCall.color );
   TEST_ASSERT_EQUAL_INT( 1, g_screenFillCall.callCount );
}

void test_Game_Render_RendersScreenBuffer( void )
{
   Game_t game;
   Screen_t screen;

   game.screen = &screen;

   Game_Render( &game );
   TEST_ASSERT_EQUAL_PTR( &screen, g_platformRenderScreenBufferCall.screen );
   TEST_ASSERT_EQUAL_INT( 1, g_platformRenderScreenBufferCall.callCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_Render_FillsScreenBufferWithBlack );
   RUN_TEST( test_Game_Render_RendersScreenBuffer );

   return UNITY_END();
}