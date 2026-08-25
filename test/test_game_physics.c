#include "mocks/mock_entity.h"
#include "mocks/mock_tile_map.h"
#include "mocks/mock_tile_texture_set.h"

#include "entity.h"
#include "game.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "unity.h"

typedef struct Game_t
{
   Clock_t* clock;
   Entity_t* playerEntity;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;
}
Game_t;

global Game_t g_game;
global Entity_t g_entity;
global TileMap_t g_tileMap;
global TileTextureSet_t g_textureSet;

r32 Clock_GetFrameSec( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   return 1.0f / 60.0f;
}

Clock_t* Game_GetClock( Game_t* game )
{
   UNUSED_PARAM( game );
   return (Clock_t*)1;
}

Entity_t* Game_GetPlayerEntity( Game_t* game )
{
   return game->playerEntity;
}

TileMap_t* Game_GetTileMap( Game_t* game )
{
   return game->tileMap;
}

TileTextureSet_t* Game_GetTileTextureSet( Game_t* game )
{
   return game->tileTextureSet;
}

Vector4i32_t Entity_GetRect( Entity_t* entity )
{
   return entity->rect;
}

Vector2i32_t Entity_GetVelocity( Entity_t* entity )
{
   return entity->velocity;
}

void Entity_SetPosition( Entity_t* entity, i32 x, i32 y )
{
   entity->rect.x = x;
   entity->rect.y = y;
}

void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy )
{
   entity->velocity.x = vx;
   entity->velocity.y = vy;
}

void Entity_SetTileIndex( Entity_t* entity, u32 tileIndex )
{
   entity->tileIndex = tileIndex;
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

u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->tileSize;
}

u32 TileMap_GetTileIndexForEntity( TileMap_t* tileMap, Entity_t* entity, u32 tileSize )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( entity );
   UNUSED_PARAM( tileSize );
   return 7;
}

void setUp( void )
{
   g_entity.rect.x = 20 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 30 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.w = 10 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = 12 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 3 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 4 * WORLD_UNITS_PER_PIXEL * 60;
   g_tileMap.width = 10;
   g_tileMap.height = 8;
   g_tileMap.wraps = False;
   g_textureSet.tileSize = 16;
   g_game.playerEntity = &g_entity;
   g_game.tileMap = &g_tileMap;
   g_game.tileTextureSet = &g_textureSet;
   g_game.clock = (Clock_t*)1;
}

void tearDown( void ) {}

void test_Game_TicPhysics_MovesPlayerByVelocity( void )
{
   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 23 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 34 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.velocity.x );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.velocity.y );
}

void test_Game_TicPhysics_ClampsPlayerAtLowerBounds( void )
{
   g_entity.rect.x = 2 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 1 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = -5 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = -4 * WORLD_UNITS_PER_PIXEL * 60;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 0, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.rect.y );
}

void test_Game_TicPhysics_ClampsPlayerAtUpperBounds( void )
{
   g_entity.rect.x = 150 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 110 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 20 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 20 * WORLD_UNITS_PER_PIXEL * 60;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 150 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 116 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
}

void test_Game_TicPhysics_DoesNotClampWrappingMap( void )
{
   g_tileMap.wraps = True;
   g_entity.rect.x = 150 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 110 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 20 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 20 * WORLD_UNITS_PER_PIXEL * 60;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 170 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 130 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
}

void test_Game_TicPhysics_UpdatesPlayerTileIndex( void )
{
   g_entity.tileIndex = 0;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_UINT( 7, g_entity.tileIndex );
}

void test_Game_TicPhysics_ClampsOversizedPlayerToOrigin( void )
{
   g_entity.rect.x = 5 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 5 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.w = 200 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = 200 * WORLD_UNITS_PER_PIXEL;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 0, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.rect.y );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_TicPhysics_MovesPlayerByVelocity );
   RUN_TEST( test_Game_TicPhysics_ClampsPlayerAtLowerBounds );
   RUN_TEST( test_Game_TicPhysics_ClampsPlayerAtUpperBounds );
   RUN_TEST( test_Game_TicPhysics_DoesNotClampWrappingMap );
   RUN_TEST( test_Game_TicPhysics_UpdatesPlayerTileIndex );

   RUN_TEST( test_Game_TicPhysics_ClampsOversizedPlayerToOrigin );

   return UNITY_END();
}