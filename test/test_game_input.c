#include "mocks/mock_entity.h"
#include "mocks/mock_input.h"
#include "mocks/mock_sprite.h"
#include "mocks/mock_tile_map.h"

#include <string.h>

#include "game.h"
#include "unity.h"

global Entity_t g_playerEntity;
global ActiveSprite_t g_playerSprite;
global Input_t g_input;
global Tile_t g_tile;

Entity_t* Game_GetActivePlayerEntity( Game_t* game )
{
   UNUSED_PARAM( game );
   return &g_playerEntity;
}

Input_t* Game_GetInput( Game_t* game )
{
   UNUSED_PARAM( game );
   return &g_input;
}

TileMap_t* Game_GetTileMap( Game_t* game )
{
   UNUSED_PARAM( game );
   return (TileMap_t*)1;
}

u32 TileMap_GetTileIndexForEntity( TileMap_t* tileMap, Entity_t* entity )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( entity );
   return 0;
}

Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 tileIndex )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( tileIndex );
   return &g_tile;
}

TileSpeed_t Tile_GetSpeed( Tile_t* tile )
{
   return tile->speed;
}

u32 Tile_GetVelocityFromSpeed( TileSpeed_t speed )
{
   switch ( speed )
   {
      case TileSpeed_ExtraSlow: return 30 * WORLD_UNITS_PER_PIXEL;
      case TileSpeed_Slow: return 45 * WORLD_UNITS_PER_PIXEL;
      case TileSpeed_Normal: return 60 * WORLD_UNITS_PER_PIXEL;

      default: return 60 * WORLD_UNITS_PER_PIXEL;
   }
}

ActiveSprite_t* Entity_GetSprite( Entity_t* entity )
{
   return entity->sprite;
}

Vector2i32_t Entity_GetVelocity( Entity_t* entity )
{
   return entity->velocity;
}

void Entity_SetVelocity( Entity_t* entity, i32 vx, i32 vy )
{
   entity->velocity.x = vx;
   entity->velocity.y = vy;
}

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite )
{
   return activeSprite->dir;
}

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir )
{
   activeSprite->dir = dir;
}

internal void SetButtonDown( InputButton_t button, b32 isDown )
{
   g_input.buttonStates[button].down = isDown;
}

b32 Input_IsButtonDown( Input_t* input, InputButton_t button )
{
   return input->buttonStates[button].down;
}

void setUp( void )
{
   memset( &g_input, 0, sizeof( g_input ) );
   memset( &g_playerEntity, 0, sizeof( g_playerEntity ) );
   g_playerEntity.sprite = &g_playerSprite;
   g_playerSprite.dir = Direction_Down;
   g_tile.speed = TileSpeed_Normal;
}

void tearDown( void ) {}

void test_Game_HandleInput_MovesLeft_SetsVelocityAndDirection( void )
{
   SetButtonDown( InputButton_Left, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( -(i32)( 60 * WORLD_UNITS_PER_PIXEL ), Entity_GetVelocity( &g_playerEntity ).x );
   TEST_ASSERT_EQUAL_INT( Direction_Left, ActiveSprite_GetDirection( &g_playerSprite ) );
}

void test_Game_HandleInput_MovesRight_SetsVelocityAndDirection( void )
{
   SetButtonDown( InputButton_Right, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( (i32)( 60 * WORLD_UNITS_PER_PIXEL ), Entity_GetVelocity( &g_playerEntity ).x );
   TEST_ASSERT_EQUAL_INT( Direction_Right, ActiveSprite_GetDirection( &g_playerSprite ) );
}

void test_Game_HandleInput_MovesUp_SetsVelocityAndDirection( void )
{
   SetButtonDown( InputButton_Up, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( -(i32)( 60 * WORLD_UNITS_PER_PIXEL ), Entity_GetVelocity( &g_playerEntity ).y );
   TEST_ASSERT_EQUAL_INT( Direction_Up, ActiveSprite_GetDirection( &g_playerSprite ) );
}

void test_Game_HandleInput_MovesDown_SetsVelocityAndDirection( void )
{
   SetButtonDown( InputButton_Down, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( (i32)( 60 * WORLD_UNITS_PER_PIXEL ), Entity_GetVelocity( &g_playerEntity ).y );
   TEST_ASSERT_EQUAL_INT( Direction_Down, ActiveSprite_GetDirection( &g_playerSprite ) );
}

void test_Game_HandleInput_UsesTileSpeedForVelocityMagnitude( void )
{
   g_tile.speed = TileSpeed_ExtraSlow;
   SetButtonDown( InputButton_Right, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( (i32)( 30 * WORLD_UNITS_PER_PIXEL ), Entity_GetVelocity( &g_playerEntity ).x );
}

void test_Game_HandleInput_AppliesDiagonalVelocityMultiplier( void )
{
   i32 expectedVelocity;

   SetButtonDown( InputButton_Right, True );
   SetButtonDown( InputButton_Down, True );

   Game_HandleInput( (Game_t*)1 );

   expectedVelocity = (i32)( 60 * WORLD_UNITS_PER_PIXEL * 0.707f );
   TEST_ASSERT_EQUAL_INT( expectedVelocity, Entity_GetVelocity( &g_playerEntity ).x );
   TEST_ASSERT_EQUAL_INT( expectedVelocity, Entity_GetVelocity( &g_playerEntity ).y );
}

void test_Game_HandleInput_PreservesFacingDirectionWhileStrafingPerpendicularly( void )
{
   g_playerSprite.dir = Direction_Up;
   SetButtonDown( InputButton_Left, True );
   SetButtonDown( InputButton_Up, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( Direction_Up, ActiveSprite_GetDirection( &g_playerSprite ) );
}

void test_Game_HandleInput_OpposingHorizontalButtons_DoesNotChangeVelocityX( void )
{
   SetButtonDown( InputButton_Left, True );
   SetButtonDown( InputButton_Right, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( 0, Entity_GetVelocity( &g_playerEntity ).x );
}

void test_Game_HandleInput_OpposingVerticalButtons_DoesNotChangeVelocityY( void )
{
   SetButtonDown( InputButton_Up, True );
   SetButtonDown( InputButton_Down, True );

   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( 0, Entity_GetVelocity( &g_playerEntity ).y );
}

void test_Game_HandleInput_NoButtonsPressed_LeavesVelocityUnchanged( void )
{
   Game_HandleInput( (Game_t*)1 );

   TEST_ASSERT_EQUAL_INT( 0, Entity_GetVelocity( &g_playerEntity ).x );
   TEST_ASSERT_EQUAL_INT( 0, Entity_GetVelocity( &g_playerEntity ).y );
   TEST_ASSERT_EQUAL_INT( Direction_Down, ActiveSprite_GetDirection( &g_playerSprite ) );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_HandleInput_MovesLeft_SetsVelocityAndDirection );
   RUN_TEST( test_Game_HandleInput_MovesRight_SetsVelocityAndDirection );
   RUN_TEST( test_Game_HandleInput_MovesUp_SetsVelocityAndDirection );
   RUN_TEST( test_Game_HandleInput_MovesDown_SetsVelocityAndDirection );
   RUN_TEST( test_Game_HandleInput_UsesTileSpeedForVelocityMagnitude );
   RUN_TEST( test_Game_HandleInput_AppliesDiagonalVelocityMultiplier );
   RUN_TEST( test_Game_HandleInput_PreservesFacingDirectionWhileStrafingPerpendicularly );
   RUN_TEST( test_Game_HandleInput_OpposingHorizontalButtons_DoesNotChangeVelocityX );
   RUN_TEST( test_Game_HandleInput_OpposingVerticalButtons_DoesNotChangeVelocityY );
   RUN_TEST( test_Game_HandleInput_NoButtonsPressed_LeavesVelocityUnchanged );

   return UNITY_END();
}
