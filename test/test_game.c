#include "mocks/mock_clock.h"
#include "mocks/mock_display.h"
#include "mocks/mock_entity.h"
#include "mocks/mock_game_data.h"
#include "mocks/mock_input.h"
#include "mocks/mock_mem_arena.h"
#include "mocks/mock_tile_map.h"
#include "mocks/mock_tile_texture_set.h"

#include <stdlib.h>

#include "game.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;
global u32 g_clockStartCount;
global u32 g_clockEndCount;
global u32 g_inputResetCount;
global u32 g_platformHandleMessagesCount;
global u32 g_gameHandleInputCount;
global u32 g_gameRenderCount;
global u32 g_tileMapAnchorCount;
global u32 g_tileMapFreeCount;
global u32 g_tileTextureSetFreeCount;
global u32 g_gameDataFreeCount;
global u32 g_displayFreeCount;
global u32 g_entityFreeCount;
global Clock_t* g_clock;
global Input_t* g_input;
global Display_t* g_display;
global GameData_t* g_gameData;
global TileTextureSet_t* g_tileTextureSet;
global TileMap_t* g_tileMap;
global Entity_t* g_playerEntity;
global Vector4i32_t g_tileMapViewportUnits;
global Vector4i32_t g_anchorViewport;
global i32 g_anchorX;
global i32 g_anchorY;
global u32 g_anchorTileSize;

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   g_allocCount++;
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   g_freeCount++;
   free( mem );
}

Clock_t* Clock_Create( MemArena_t* memArena )
{
   g_clock = (Clock_t*)MemArena_AllocMem( memArena, sizeof( Clock_t ) );
   g_clock->fps = GAME_DEFAULT_FPS;
   return g_clock;
}

void Clock_Free( Clock_t* clock, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, clock );
}

void Clock_StartFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   g_clockStartCount++;
}

void Clock_EndFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   g_clockEndCount++;
}

Input_t* Input_Create( MemArena_t* memArena )
{
   g_input = (Input_t*)MemArena_AllocMem( memArena, sizeof( Input_t ) );
   return g_input;
}

void Input_Free( Input_t* input, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, input );
}

void Input_ResetPressStates( Input_t* input )
{
   UNUSED_PARAM( input );
   g_inputResetCount++;
}

Display_t* Display_Create( MemArena_t* memArena, u32 w, u32 h )
{
   g_display = (Display_t*)MemArena_AllocMem( memArena, sizeof( Display_t ) );
   UNUSED_PARAM( w );
   UNUSED_PARAM( h );
   return g_display;
}

void Display_Free( Display_t* display, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, display );
   g_displayFreeCount++;
}

Entity_t* Entity_Create( MemArena_t* memArena )
{
   g_playerEntity = (Entity_t*)MemArena_AllocMem( memArena, sizeof( Entity_t ) );
   return g_playerEntity;
}

void Entity_Free( Entity_t* entity, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, entity );
   g_entityFreeCount++;
}

Vector4i32_t Entity_GetRect( Entity_t* entity )
{
   return entity->rect;
}

void Entity_SetPosition( Entity_t* entity, i32 x, i32 y )
{
   entity->rect.x = x;
   entity->rect.y = y;
}

void Entity_SetSize( Entity_t* entity, i32 w, i32 h )
{
   entity->rect.w = w;
   entity->rect.h = h;
}

void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy )
{
   entity->velocity.x = vx;
   entity->velocity.y = vy;
}

GameData_t* GameData_Create( MemArena_t* memArena, const char* filePath )
{
   UNUSED_PARAM( filePath );
   g_gameData = (GameData_t*)MemArena_AllocMem( memArena, sizeof( GameData_t ) );
   return g_gameData;
}

void GameData_Free( GameData_t* gameData, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, gameData );
   g_gameDataFreeCount++;
}

TileTextureSet_t* TileTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData )
{
   UNUSED_PARAM( gameData );
   g_tileTextureSet = (TileTextureSet_t*)MemArena_AllocMem( memArena, sizeof( TileTextureSet_t ) );
   g_tileTextureSet->tileSize = 16;
   return g_tileTextureSet;
}

void TileTextureSet_Free( TileTextureSet_t* tileTextureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileTextureSet );
   g_tileTextureSetFreeCount++;
}

u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->tileSize;
}

TileMap_t* TileMap_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData, u32 tileMapId )
{
   UNUSED_PARAM( gameData );
   g_tileMap = (TileMap_t*)MemArena_AllocMem( memArena, sizeof( TileMap_t ) );
   g_tileMap->width = tileMapId;
   g_tileMap->height = tileMapId;
   g_tileMap->tiles = 0;
   return g_tileMap;
}

void TileMap_Free( TileMap_t* tileMap, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileMap );
   g_tileMapFreeCount++;
}

Vector4i32_t TileMap_GetViewportUnits( TileMap_t* tileMap )
{
   UNUSED_PARAM( tileMap );
   return g_tileMapViewportUnits;
}

void TileMap_SetViewportUnits( TileMap_t* tileMap, Vector4i32_t viewportUnits )
{
   UNUSED_PARAM( tileMap );
   g_tileMapViewportUnits = viewportUnits;
}

void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, u32 x, u32 y, u32 tileSize )
{
   UNUSED_PARAM( tileMap );
   g_tileMapAnchorCount++;
   g_anchorViewport = g_tileMapViewportUnits;
   g_anchorX = (i32)x;
   g_anchorY = (i32)y;
   g_anchorTileSize = tileSize;
}

void Platform_HandleMessages( Game_t* game )
{
   g_platformHandleMessagesCount++;
   Game_Stop( game );
}

void Game_HandleInput( Game_t* game )
{
   UNUSED_PARAM( game );
   g_gameHandleInputCount++;
}

void Game_Render( Game_t* game )
{
   UNUSED_PARAM( game );
   g_gameRenderCount++;
}

void Game_TicPhysics( Game_t* game )
{
   UNUSED_PARAM( game );
}

internal Game_t* CreateGame( void )
{
   return Game_Create( (MemArena_t*)1, "test.dw3d" );
}

void setUp( void )
{
   g_allocCount = 0;
   g_freeCount = 0;
   g_clockStartCount = 0;
   g_clockEndCount = 0;
   g_inputResetCount = 0;
   g_platformHandleMessagesCount = 0;
   g_gameHandleInputCount = 0;
   g_gameRenderCount = 0;
   g_tileMapAnchorCount = 0;
   g_tileMapFreeCount = 0;
   g_tileTextureSetFreeCount = 0;
   g_gameDataFreeCount = 0;
   g_displayFreeCount = 0;
   g_entityFreeCount = 0;
}

void tearDown( void ) {}

void test_Game_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, Game_GetStructSize() );
}

void test_Game_Create_InitializesDependenciesAndDefaultState( void )
{
   Vector4i32_t viewportUnits;
   Vector4i32_t playerRect;
   Game_t* game = CreateGame();

   TEST_ASSERT_NOT_NULL( game );
   TEST_ASSERT_EQUAL_PTR( g_clock, Game_GetClock( game ) );
   TEST_ASSERT_EQUAL_PTR( g_input, Game_GetInput( game ) );
   TEST_ASSERT_EQUAL_PTR( g_display, Game_GetDisplay( game ) );
   TEST_ASSERT_EQUAL_PTR( g_gameData, Game_GetGameData( game ) );
   TEST_ASSERT_EQUAL_PTR( g_tileTextureSet, Game_GetTileTextureSet( game ) );
   TEST_ASSERT_EQUAL_PTR( g_tileMap, Game_GetTileMap( game ) );

   viewportUnits = Game_GetTileMapViewportUnits( game );
   TEST_ASSERT_EQUAL_INT( 0, viewportUnits.x );
   TEST_ASSERT_EQUAL_INT( 0, viewportUnits.y );
   TEST_ASSERT_EQUAL_INT( DISPLAY_WIDTH * WORLD_UNITS_PER_PIXEL, viewportUnits.w );
   TEST_ASSERT_EQUAL_INT( DISPLAY_HEIGHT * WORLD_UNITS_PER_PIXEL, viewportUnits.h );

   playerRect = Entity_GetRect( Game_GetPlayerEntity( game ) );
   TEST_ASSERT_EQUAL_INT( 100 * WORLD_UNITS_PER_PIXEL, playerRect.x );
   TEST_ASSERT_EQUAL_INT( 100 * WORLD_UNITS_PER_PIXEL, playerRect.y );
   TEST_ASSERT_EQUAL_INT( 12 * WORLD_UNITS_PER_PIXEL, playerRect.w );
   TEST_ASSERT_EQUAL_INT( 12 * WORLD_UNITS_PER_PIXEL, playerRect.h );
   TEST_ASSERT_EQUAL_UINT( GAME_DEFAULT_FPS, g_clock->fps );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_SetPlayerRect_UpdatesPlayerRectangle( void )
{
   Vector4i32_t playerRect = { 25, 30, 18, 20 };
   Game_t* game = CreateGame();

   Game_SetPlayerRect( game, playerRect );

   playerRect = Entity_GetRect( Game_GetPlayerEntity( game ) );
   TEST_ASSERT_EQUAL_INT( 25, playerRect.x );
   TEST_ASSERT_EQUAL_INT( 30, playerRect.y );
   TEST_ASSERT_EQUAL_INT( 18, playerRect.w );
   TEST_ASSERT_EQUAL_INT( 20, playerRect.h );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_Run_ExecutesOneFrameAndUpdatesViewport( void )
{
   Game_t* game = CreateGame();

   Game_Run( game );

   TEST_ASSERT_EQUAL_UINT( 1, g_clockStartCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_inputResetCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_platformHandleMessagesCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_gameHandleInputCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_tileMapAnchorCount );
   TEST_ASSERT_EQUAL_INT( 16, g_anchorTileSize );
   TEST_ASSERT_EQUAL_UINT( 1, g_gameRenderCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_clockEndCount );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_Free_ReleasesAllDependencies( void )
{
   Game_t* game = CreateGame();

   Game_Free( game, (MemArena_t*)1 );

   TEST_ASSERT_EQUAL_UINT( 1, g_displayFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_gameDataFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_tileMapFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_tileTextureSetFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_entityFreeCount );
   TEST_ASSERT_EQUAL_UINT( 8, g_freeCount );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_Game_Create_InitializesDependenciesAndDefaultState );

   RUN_TEST( test_Game_SetPlayerRect_UpdatesPlayerRectangle );

   RUN_TEST( test_Game_Run_ExecutesOneFrameAndUpdatesViewport );
   
   RUN_TEST( test_Game_Free_ReleasesAllDependencies );

   return UNITY_END();
}
