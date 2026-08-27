#include "display.h"
#include "game.h"
#include "mocks/mock_entity.h"
#include "sprite.h"
#include "sprite_texture_set.h"
#include "tile_map.h"
#include "unity.h"

struct ActiveSprite_t
{
   ActiveSpriteTextureSet_t* textureSet;
   Direction_t dir;
   u32 frameIndex;
   u32 textureIndex;
};

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

typedef struct DisplayDrawBufferCall_t
{
   Display_t* display;
   u32* buffer;
   u32 bufferW;
   u32 bufferH;
   i32 displayX;
   i32 displayY;
   int callCount;
}
DisplayDrawBufferCall_t;

typedef struct PlatformRenderDisplayBufferCall_t
{
   Display_t* display;
   int callCount;
}
PlatformRenderDisplayBufferCall_t;

static Display_t* g_display;
#if defined( _WIN32 )
WinDebugFlags_t g_winDebugFlags;
#endif
static TileMap_t* g_tileMap;
static TileTextureSet_t* g_tileTextureSet;
static ActiveSpriteTextureSet_t* g_activeSpriteTextureSet;
static Entity_t g_playerEntity;
static ActiveSprite_t g_playerSprite;
static u32 g_playerTexture[1];
static Vector4i32_t g_viewportInPixels;
static Vector4i32_t g_playerRect;
static DisplayFillCall_t g_displayFillCall;
static DisplayDrawTileMapViewportCall_t g_displayDrawTileMapViewportCall;
static DisplayDrawRectCall_t g_displayDrawRectCall;
static DisplayDrawBufferCall_t g_displayDrawBufferCall;
static PlatformRenderDisplayBufferCall_t g_platformRenderDisplayBufferCall;

void setUp( void )
{
#if defined( _WIN32 )
   g_winDebugFlags.showDiagnostics = False;
   g_winDebugFlags.showHitBoxes = False;
#endif
   g_display = (Display_t*)1;
   g_tileMap = (TileMap_t*)2;
   g_tileTextureSet = (TileTextureSet_t*)3;
   g_activeSpriteTextureSet = (ActiveSpriteTextureSet_t*)5;
   g_playerSprite.textureSet = g_activeSpriteTextureSet;
   g_playerSprite.dir = Direction_Right;
   g_playerSprite.frameIndex = 1;
   g_playerSprite.textureIndex = 1;
   g_playerTexture[0] = 0x12345678;
   g_viewportInPixels.x = 10;
   g_viewportInPixels.y = 20;
   g_viewportInPixels.w = 320;
   g_viewportInPixels.h = 240;
   g_playerRect.x = 40 * WORLD_UNITS_PER_PIXEL;
   g_playerRect.y = 60 * WORLD_UNITS_PER_PIXEL;
   g_playerRect.w = 12 * WORLD_UNITS_PER_PIXEL;
   g_playerRect.h = 14 * WORLD_UNITS_PER_PIXEL;
   g_playerEntity.rect = g_playerRect;
   g_playerEntity.sprite = &g_playerSprite;
   g_playerEntity.spriteOffset.x = -2;
   g_playerEntity.spriteOffset.y = 3;

   g_displayFillCall.display = 0;
   g_displayFillCall.color = 0;
   g_displayFillCall.callCount = 0;

   g_displayDrawTileMapViewportCall.display = 0;
   g_displayDrawTileMapViewportCall.tileMap = 0;
   g_displayDrawTileMapViewportCall.tileTextureSet = 0;
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

   g_displayDrawBufferCall.display = 0;
   g_displayDrawBufferCall.buffer = 0;
   g_displayDrawBufferCall.bufferW = 0;
   g_displayDrawBufferCall.bufferH = 0;
   g_displayDrawBufferCall.displayX = 0;
   g_displayDrawBufferCall.displayY = 0;
   g_displayDrawBufferCall.callCount = 0;

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

ActiveSpriteTextureSet_t* Game_GetActiveSpriteTextureSet( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_activeSpriteTextureSet;
}

TileMap_t* Game_GetTileMap( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_tileMap;
}

Vector4i32_t TileMap_GetViewportInPixels( TileMap_t* tileMap )
{
   UNUSED_PARAM( tileMap );
   return g_viewportInPixels;
}

Entity_t* Game_GetPlayerEntity( Game_t* game )
{
   UNUSED_PARAM( game );
   return &g_playerEntity;
}

Vector4i32_t Entity_GetRect( Entity_t* entity )
{
   return entity->rect;
}

ActiveSprite_t* Entity_GetSprite( Entity_t* entity )
{
   return entity->sprite;
}

Vector2i32_t Entity_GetSpriteOffset( Entity_t* entity )
{
   return entity->spriteOffset;
}

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite )
{
   return activeSprite->dir;
}

u32 ActiveSprite_GetFrameIndex( ActiveSprite_t* activeSprite )
{
   return activeSprite->frameIndex;
}

u32 ActiveSprite_GetTextureIndex( ActiveSprite_t* activeSprite )
{
   return activeSprite->textureIndex;
}

u32 ActiveSpriteTextureSet_GetFrameCount( ActiveSpriteTextureSet_t* textureSet )
{
   UNUSED_PARAM( textureSet );
   return 3;
}

u32 ActiveSpriteTextureSet_GetFrameSize( ActiveSpriteTextureSet_t* textureSet )
{
   UNUSED_PARAM( textureSet );
   return 16;
}

u32* ActiveSpriteTextureSet_GetTexture( ActiveSpriteTextureSet_t* textureSet, u32 index )
{
   UNUSED_PARAM( textureSet );
   TEST_ASSERT_EQUAL_UINT( 19, index );
   return g_playerTexture;
}

void Display_Fill( Display_t* display, u32 color )
{
   g_displayFillCall.display = display;
   g_displayFillCall.color = color;
   g_displayFillCall.callCount++;
}

void Display_DrawBuffer( Display_t* display, u32* buffer, u32 bufferW, u32 bufferH, i32 displayX, i32 displayY )
{
   g_displayDrawBufferCall.display = display;
   g_displayDrawBufferCall.buffer = buffer;
   g_displayDrawBufferCall.bufferW = bufferW;
   g_displayDrawBufferCall.bufferH = bufferH;
   g_displayDrawBufferCall.displayX = displayX;
   g_displayDrawBufferCall.displayY = displayY;
   g_displayDrawBufferCall.callCount++;
}

void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, TileTextureSet_t* tileTextureSet, i32 displayX, i32 displayY )
{
   g_displayDrawTileMapViewportCall.display = display;
   g_displayDrawTileMapViewportCall.tileMap = tileMap;
   g_displayDrawTileMapViewportCall.tileTextureSet = tileTextureSet;
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
   TEST_ASSERT_EQUAL_INT( 0, g_displayDrawTileMapViewportCall.displayX );
   TEST_ASSERT_EQUAL_INT( 0, g_displayDrawTileMapViewportCall.displayY );
   TEST_ASSERT_EQUAL_INT( 1, g_displayDrawTileMapViewportCall.callCount );
}

void test_Game_Render_DrawsPlayerRelativeToViewport( void )
{
   Game_Render( (Game_t*)4 );

   TEST_ASSERT_EQUAL_PTR( g_display, g_displayDrawBufferCall.display );
   TEST_ASSERT_EQUAL_PTR( g_playerTexture, g_displayDrawBufferCall.buffer );
   TEST_ASSERT_EQUAL_UINT( 16, g_displayDrawBufferCall.bufferW );
   TEST_ASSERT_EQUAL_UINT( 16, g_displayDrawBufferCall.bufferH );
   TEST_ASSERT_EQUAL_INT( 28, g_displayDrawBufferCall.displayX );
   TEST_ASSERT_EQUAL_INT( 43, g_displayDrawBufferCall.displayY );
   TEST_ASSERT_EQUAL_INT( 1, g_displayDrawBufferCall.callCount );
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