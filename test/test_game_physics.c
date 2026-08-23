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
   Entity_t* playerEntity;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;
}
Game_t;

global Game_t g_game;
global Entity_t g_entity;
global TileMap_t g_tileMap;
global TileTextureSet_t g_textureSet;

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

void setUp( void )
{
   g_entity.rect.x = 20;
   g_entity.rect.y = 30;
   g_entity.rect.w = 10;
   g_entity.rect.h = 12;
   g_entity.velocity.x = 3;
   g_entity.velocity.y = 4;
   g_tileMap.width = 10;
   g_tileMap.height = 8;
   g_tileMap.wraps = False;
   g_textureSet.tileSize = 16;
   g_game.playerEntity = &g_entity;
   g_game.tileMap = &g_tileMap;
   g_game.tileTextureSet = &g_textureSet;
}

void tearDown( void ) {}

void test_Game_TicPhysics_MovesPlayerByVelocity( void )
{
   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 23, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 34, g_entity.rect.y );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.velocity.x );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.velocity.y );
}

void test_Game_TicPhysics_ClampsPlayerAtLowerBounds( void )
{
   g_entity.rect.x = 2;
   g_entity.rect.y = 1;
   g_entity.velocity.x = -5;
   g_entity.velocity.y = -4;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 0, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 0, g_entity.rect.y );
}

void test_Game_TicPhysics_ClampsPlayerAtUpperBounds( void )
{
   g_entity.rect.x = 150;
   g_entity.rect.y = 110;
   g_entity.velocity.x = 20;
   g_entity.velocity.y = 20;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 150, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 116, g_entity.rect.y );
}

void test_Game_TicPhysics_DoesNotClampWrappingMap( void )
{
   g_tileMap.wraps = True;
   g_entity.rect.x = 150;
   g_entity.rect.y = 110;
   g_entity.velocity.x = 20;
   g_entity.velocity.y = 20;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 170, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 130, g_entity.rect.y );
}

void test_Game_TicPhysics_ClampsOversizedPlayerToOrigin( void )
{
   g_entity.rect.x = 5;
   g_entity.rect.y = 5;
   g_entity.rect.w = 200;
   g_entity.rect.h = 200;

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
   
   RUN_TEST( test_Game_TicPhysics_ClampsOversizedPlayerToOrigin );

   return UNITY_END();
}