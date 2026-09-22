#include "mocks/mock_entity.h"
#include "mocks/mock_npc.h"
#include "mocks/mock_player.h"
#include "mocks/mock_sprite.h"
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
global Entity_t g_playerEntity2;
global Player_t g_player;
global Player_t g_players[GAME_MAX_PLAYERS];
global ActiveSprite_t g_playerSprites[GAME_MAX_PLAYERS];
global u32 g_playerCount;
global u32 g_playerOrder[GAME_MAX_PLAYERS];
global PlayerMovement_t g_playerMovements[GAME_MAX_PLAYERS][PLAYER_MOVE_HISTORY_SIZE];
global u32 g_playerMovementIndices[GAME_MAX_PLAYERS];
global b32 g_playerChainNext[GAME_MAX_PLAYERS];
global Entity_t g_npcEntity;
global Entity_t g_npcEntity2;
global Npc_t g_npc;
global Npc_t g_npc2;
global u32 g_npcCount;
global TileMap_t g_tileMap;
global TileTextureSet_t g_textureSet;
global Tile_t g_tiles[80];
global u32 g_clockFrameCount;
global u32 g_gameOnPlayerTileIndexChangedCount;
global u32 g_gameOnPlayerTileIndexChangedTileIndex;
#if defined( _WIN32 )
WinDebugFlags_t g_winDebugFlags;
#endif

r32 Clock_GetFrameSec( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   return 1.0f / 60.0f;
}

u32 Clock_GetFrameCount( Clock_t* clock )
{
   UNUSED_PARAM( clock );
   return g_clockFrameCount;
}

Clock_t* Game_GetClock( Game_t* game )
{
   UNUSED_PARAM( game );
   return (Clock_t*)1;
}

Player_t* Game_GetPlayer( Game_t* game, u32 playerIndex )
{
   UNUSED_PARAM( game );
   return &g_players[playerIndex];
}

Player_t* Game_GetActivePlayer( Game_t* game )
{
   UNUSED_PARAM( game );
   return &g_players[g_playerOrder[0]];
}

u32 Game_GetPlayerCount( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_playerCount;
}

u32* Game_GetPlayerOrder( Game_t* game )
{
   UNUSED_PARAM( game );
   return g_playerOrder;
}

Entity_t* Game_GetActivePlayerEntity( Game_t* game )
{
   return game->playerEntity;
}

Entity_t* Player_GetEntity( const Player_t* player )
{
   return player->entity;
}

internal u32 GetMockPlayerIndex( const Player_t* player )
{
   return (u32)( player - g_players );
}

b32 Player_GetChainNextPlayer( const Player_t* player )
{
   return g_playerChainNext[ GetMockPlayerIndex( player ) ];
}

PlayerMovement_t Player_GetMovement( Player_t* player, u32 index )
{
   return g_playerMovements[ GetMockPlayerIndex( player ) ][index];
}

u32 Player_GetMovementChainIndex( const Player_t* player )
{
   return g_playerMovementIndices[ GetMockPlayerIndex( player ) ];
}

void Player_AddMovement( Player_t* player, PlayerMovement_t movement )
{
   u32 playerIndex = GetMockPlayerIndex( player );
   u32 movementIndex = g_playerMovementIndices[playerIndex];

   g_playerMovements[playerIndex][movementIndex] = movement;
   g_playerMovementIndices[playerIndex]++;
   if ( g_playerMovementIndices[playerIndex] >= 1 )
   {
      g_playerMovementIndices[playerIndex] = 0;
      g_playerChainNext[playerIndex] = True;
   }
}

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite )
{
   return activeSprite->dir;
}

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir )
{
   activeSprite->dir = dir;
}

ActiveSprite_t* Entity_GetSprite( Entity_t* entity )
{
   return entity->sprite;
}

TileMap_t* Game_GetTileMap( Game_t* game )
{
   return game->tileMap;
}

u32 TileMap_GetNpcCount( TileMap_t* tileMap )
{
   UNUSED_PARAM( tileMap );
   return g_npcCount;
}

Npc_t* TileMap_GetNpc( TileMap_t* tileMap, u32 npcIndex )
{
   UNUSED_PARAM( tileMap );
   return npcIndex == 0 ? &g_npc : &g_npc2;
}

Entity_t* Npc_GetEntity( Npc_t* npc )
{
   return npc->entity;
}

TileTextureSet_t* Game_GetTileTextureSet( Game_t* game )
{
   return game->tileTextureSet;
}

void Game_OnPlayerTileIndexChanged( Game_t* game, u32 newTileIndex )
{
   UNUSED_PARAM( game );
   g_gameOnPlayerTileIndexChangedCount++;
   g_gameOnPlayerTileIndexChangedTileIndex = newTileIndex;
}

Vector4i32_t Entity_GetRect( Entity_t* entity )
{
   return entity->rect;
}

u32 Entity_GetTileIndex( Entity_t* entity )
{
   return entity->tileIndex;
}

i32 Entity_GetVelocityX( Entity_t* entity )
{
   return entity->velocity.x;
}

i32 Entity_GetVelocityY( Entity_t* entity )
{
   return entity->velocity.y;
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

void Entity_SetVelocityX( Entity_t* entity, i32 vx )
{
   entity->velocity.x = vx;
}

void Entity_SetVelocityY( Entity_t* entity, i32 vy )
{
   entity->velocity.y = vy;
}

void Entity_SetTileIndex( Entity_t* entity, u32 tileIndex )
{
   entity->tileIndex = tileIndex;
}

u32 TileMap_GetTilesX( TileMap_t* tileMap )
{
   return tileMap->info.tilesX;
}

u32 TileMap_GetTilesY( TileMap_t* tileMap )
{
   return tileMap->info.tilesY;
}

Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 tileIndex )
{
   UNUSED_PARAM( tileMap );
   return &g_tiles[tileIndex];
}

b32 Tile_GetIsPassable( Tile_t* tile )
{
   return tile->isPassable;
}

b32 TileMap_GetWraps( TileMap_t* tileMap )
{
   return tileMap->info.wraps;
}

void TileMap_WrapEntityPosition( TileMap_t* tileMap, Entity_t* entity )
{
   Vector4i32_t rect = Entity_GetRect( entity );
   i32 mapWidth = (i32)tileMap->info.tilesX * 16 * WORLD_UNITS_PER_PIXEL;
   i32 mapHeight = (i32)tileMap->info.tilesY * 16 * WORLD_UNITS_PER_PIXEL;

   rect.x %= mapWidth;
   rect.y %= mapHeight;
   if ( rect.x < 0 )
   {
      rect.x += mapWidth;
   }
   if ( rect.y < 0 )
   {
      rect.y += mapHeight;
   }
   Entity_SetPosition( entity, rect.x, rect.y );
}

u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->info.tileSize;
}

u32 TileMap_GetTileIndexForEntity( TileMap_t* tileMap, Entity_t* entity )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( entity );
   return 7;
}

void setUp( void )
{
#if defined( _WIN32 )
   g_winDebugFlags.showDiagnostics = False;
   g_winDebugFlags.noClip = False;
   g_winDebugFlags.showHitBoxes = False;
   g_winDebugFlags.moveFast = False;
#endif
   g_clockFrameCount = 0;
   g_playerCount = 1;
   for ( u32 i = 0; i < GAME_MAX_PLAYERS; i++ )
   {
      g_playerOrder[i] = i;
      g_playerMovements[i][0] = (PlayerMovement_t){ { 0, 0 }, Direction_Down };
      g_playerMovementIndices[i] = 0;
      g_playerChainNext[i] = False;
      g_playerSprites[i].dir = Direction_Down;
      g_players[i].entity = 0;
   }
   g_npcCount = 0;
   g_gameOnPlayerTileIndexChangedCount = 0;
   g_gameOnPlayerTileIndexChangedTileIndex = 0;
   g_entity.rect.x = 20 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 30 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.w = 10 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = 12 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 3 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 4 * WORLD_UNITS_PER_PIXEL * 60;
   g_npc.entity = &g_npcEntity;
   g_npcEntity.rect.x = 16 * WORLD_UNITS_PER_PIXEL;
   g_npcEntity.rect.y = 0;
   g_npcEntity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_npcEntity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_npc2.entity = &g_npcEntity2;
   g_npcEntity2.rect.x = 17 * WORLD_UNITS_PER_PIXEL;
   g_npcEntity2.rect.y = 0;
   g_npcEntity2.rect.w = WORLD_UNITS_PER_PIXEL;
   g_npcEntity2.rect.h = WORLD_UNITS_PER_PIXEL;
   g_tileMap.info.tilesX = 10;
   g_tileMap.info.tilesY = 8;
   g_tileMap.info.wraps = False;
   g_tileMap.tiles = g_tiles;
   for ( u32 i = 0; i < 80; i++ )
   {
      g_tiles[i].isPassable = True;
   }
   g_textureSet.info.tileSize = 16;
   g_player.entity = &g_entity;
   g_players[0].entity = &g_entity;
   g_players[1].entity = &g_playerEntity2;
   g_entity.sprite = &g_playerSprites[0];
   g_playerEntity2.sprite = &g_playerSprites[1];
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

void test_Game_TicPhysics_StopsBeforeNonPassableTile( void )
{
   g_entity.rect.x = 0;
   g_entity.rect.y = 0;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 35 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 0;
   g_tiles[2].isPassable = False;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 31 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
}

void test_Game_TicPhysics_StopsBeforeNpc( void )
{
   g_npcCount = 1;
   g_entity.rect.x = 0;
   g_entity.rect.y = 0;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 30 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 0;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 15 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
}

void test_Game_TicPhysics_MovesNpcWithoutNotifyingPlayerTileChange( void )
{
   g_npcCount = 1;
   g_entity.tileIndex = 7;
   g_gameOnPlayerTileIndexChangedCount = 0;
   g_npcEntity.rect.x = 0;
   g_npcEntity.rect.y = 0;
   g_npcEntity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_npcEntity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_npcEntity.velocity.x = 3 * WORLD_UNITS_PER_PIXEL * 60;
   g_npcEntity.velocity.y = 0;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 3 * WORLD_UNITS_PER_PIXEL, g_npcEntity.rect.x );
   TEST_ASSERT_EQUAL_INT( 3 * 60 * WORLD_UNITS_PER_PIXEL, g_npcEntity.velocity.x );
   TEST_ASSERT_EQUAL_UINT( 0, g_gameOnPlayerTileIndexChangedCount );
}

void test_Game_TicPhysics_StopsNpcBeforeAnotherNpc( void )
{
   g_npcCount = 2;
   g_entity.tileIndex = 7;
   g_npcEntity.velocity.x = 0;
   g_npcEntity2.velocity.x = -60 * WORLD_UNITS_PER_PIXEL * 60;
   g_npcEntity2.velocity.y = 0;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 17 * WORLD_UNITS_PER_PIXEL, g_npcEntity2.rect.x );
}

void test_Game_TicPhysics_DoesNotEnterNonPassableTile( void )
{
   g_entity.rect.x = 0;
   g_entity.rect.y = 0;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 20 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 0;
   g_tiles[1].isPassable = False;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 15 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
}

void test_Game_TicPhysics_MovesDiagonally( void )
{
   i32 i;

   g_entity.rect.x = 0;
   g_entity.rect.y = 0;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 60 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.y = 60 * WORLD_UNITS_PER_PIXEL;

   for ( i = 0; i < 10; i++ )
   {
      g_clockFrameCount = (u32)i;
      g_entity.velocity.x = (i32)( 60 * WORLD_UNITS_PER_PIXEL * 0.707f );
      g_entity.velocity.y = (i32)( 60 * WORLD_UNITS_PER_PIXEL * 0.707f );
      Game_TicPhysics( &g_game );
   }

   TEST_ASSERT_EQUAL_INT( 7 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 7 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
}

void test_Game_TicPhysics_AllowsOtherAxisAfterCollision( void )
{
   g_entity.rect.x = 0;
   g_entity.rect.y = 0;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 20 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 20 * WORLD_UNITS_PER_PIXEL * 60;
   g_tiles[1].isPassable = False;
   g_tiles[11].isPassable = False;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 15 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 20 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
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
   g_tileMap.info.wraps = True;
   g_entity.rect.x = 150 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 110 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 20 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = 20 * WORLD_UNITS_PER_PIXEL * 60;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 10 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 2 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
}

void test_Game_TicPhysics_UpdatesPlayerTileIndex( void )
{
   g_entity.tileIndex = 0;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_UINT( 7, g_entity.tileIndex );
}

void test_Game_TicPhysics_NotifiesWhenPlayerTileIndexChanges( void )
{
   g_entity.tileIndex = 0;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_UINT( 1, g_gameOnPlayerTileIndexChangedCount );
   TEST_ASSERT_EQUAL_UINT( 7, g_gameOnPlayerTileIndexChangedTileIndex );
}

void test_Game_TicPhysics_DoesNotNotifyWhenPlayerTileIndexIsUnchanged( void )
{
   g_entity.tileIndex = 7;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_UINT( 0, g_gameOnPlayerTileIndexChangedCount );
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

void test_Game_TicPhysics_WrapsPlayerAtLowerBounds( void )
{
   g_tileMap.info.wraps = True;
   g_entity.rect.x = 5 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 3 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = -10 * WORLD_UNITS_PER_PIXEL * 60;
   g_entity.velocity.y = -5 * WORLD_UNITS_PER_PIXEL * 60;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 155 * WORLD_UNITS_PER_PIXEL, g_entity.rect.x );
   TEST_ASSERT_EQUAL_INT( 126 * WORLD_UNITS_PER_PIXEL, g_entity.rect.y );
}

void test_Game_TicPhysics_PropagatesMovementToFollowingPlayer( void )
{
   g_playerCount = 2;
   g_playerOrder[0] = 0;
   g_playerOrder[1] = 1;
   g_entity.rect.x = 32 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 48 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.x = 60 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.y = 0;
   g_playerEntity2.rect = g_entity.rect;
   g_playerEntity2.rect.x = 0;
   g_playerEntity2.rect.y = 0;
   g_playerSprites[0].dir = Direction_Right;
   g_playerSprites[1].dir = Direction_Left;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 33 * WORLD_UNITS_PER_PIXEL, g_playerEntity2.rect.x );
   TEST_ASSERT_EQUAL_INT( 48 * WORLD_UNITS_PER_PIXEL, g_playerEntity2.rect.y );
   TEST_ASSERT_EQUAL_INT( Direction_Right, g_playerSprites[1].dir );
}

void test_Game_TicPhysics_PropagatesMovementThroughPlayerChain( void )
{
   g_playerCount = 3;
   g_playerOrder[0] = 0;
   g_playerOrder[1] = 1;
   g_playerOrder[2] = 2;
   g_entity.rect.x = 32 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.y = 48 * WORLD_UNITS_PER_PIXEL;
   g_entity.rect.w = WORLD_UNITS_PER_PIXEL;
   g_entity.rect.h = WORLD_UNITS_PER_PIXEL;
   g_playerEntity2.rect = g_entity.rect;
   g_playerEntity2.rect.x = 0;
   g_playerEntity2.rect.y = 0;
   g_players[2].entity = &g_npcEntity2;
   g_npcEntity2.rect = g_entity.rect;
   g_npcEntity2.rect.x = 0;
   g_npcEntity2.rect.y = 0;
   g_npcEntity2.sprite = &g_playerSprites[2];
   g_entity.velocity.x = 60 * WORLD_UNITS_PER_PIXEL;
   g_entity.velocity.y = 0;
   g_playerSprites[0].dir = Direction_Right;
   g_playerSprites[1].dir = Direction_Left;
   g_playerSprites[2].dir = Direction_Up;

   Game_TicPhysics( &g_game );

   TEST_ASSERT_EQUAL_INT( 33 * WORLD_UNITS_PER_PIXEL, g_playerEntity2.rect.x );
   TEST_ASSERT_EQUAL_INT( 33 * WORLD_UNITS_PER_PIXEL, g_npcEntity2.rect.x );
   TEST_ASSERT_EQUAL_INT( Direction_Right, g_playerSprites[1].dir );
   TEST_ASSERT_EQUAL_INT( Direction_Right, g_playerSprites[2].dir );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_TicPhysics_MovesPlayerByVelocity );
   RUN_TEST( test_Game_TicPhysics_StopsBeforeNonPassableTile );
   RUN_TEST( test_Game_TicPhysics_StopsBeforeNpc );
   RUN_TEST( test_Game_TicPhysics_MovesNpcWithoutNotifyingPlayerTileChange );
   RUN_TEST( test_Game_TicPhysics_StopsNpcBeforeAnotherNpc );
   RUN_TEST( test_Game_TicPhysics_DoesNotEnterNonPassableTile );
   RUN_TEST( test_Game_TicPhysics_MovesDiagonally );
   RUN_TEST( test_Game_TicPhysics_AllowsOtherAxisAfterCollision );
   RUN_TEST( test_Game_TicPhysics_ClampsPlayerAtLowerBounds );
   RUN_TEST( test_Game_TicPhysics_ClampsPlayerAtUpperBounds );
   RUN_TEST( test_Game_TicPhysics_DoesNotClampWrappingMap );
   RUN_TEST( test_Game_TicPhysics_UpdatesPlayerTileIndex );
   RUN_TEST( test_Game_TicPhysics_NotifiesWhenPlayerTileIndexChanges );
   RUN_TEST( test_Game_TicPhysics_DoesNotNotifyWhenPlayerTileIndexIsUnchanged );
   RUN_TEST( test_Game_TicPhysics_ClampsOversizedPlayerToOrigin );
   RUN_TEST( test_Game_TicPhysics_WrapsPlayerAtLowerBounds );
   RUN_TEST( test_Game_TicPhysics_PropagatesMovementToFollowingPlayer );
   RUN_TEST( test_Game_TicPhysics_PropagatesMovementThroughPlayerChain );

   return UNITY_END();
}