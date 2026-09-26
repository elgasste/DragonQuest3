#include "mocks/mock_clock.h"
#include "mocks/mock_animation.h"
#include "mocks/mock_display.h"
#include "mocks/mock_entity.h"
#include "mocks/mock_game_data.h"
#include "mocks/mock_input.h"
#include "mocks/mock_mem_arena.h"
#include "mocks/mock_npc.h"
#include "mocks/mock_player.h"
#include "mocks/mock_sprite.h"
#include "mocks/mock_sprite_texture_set.h"
#include "mocks/mock_tile_map.h"
#include "mocks/mock_tile_texture_set.h"

#include <stdlib.h>

#include "animation.h"
#include "clock.h"
#include "display.h"
#include "game.h"
#include "sprite.h"
#include "unity.h"

global u32 g_allocCount;
global u32 g_freeCount;
global u32 g_clockStartCount;
global u32 g_clockEndCount;
global r32 g_spriteTicDeltaSec;
global u32 g_spriteTicCount;
global ActiveSprite_t* g_spriteTicSprite;
global u32 g_inputResetCount;
global u32 g_platformHandleMessagesCount;
global u32 g_gameHandleInputCount;
global u32 g_gameRenderCount;
global u32 g_tileMapAnchorCount;
global u32 g_tileMapFreeCount;
global u32 g_tileTextureSetFreeCount;
global u32 g_activeSpriteTextureSetFreeCount;
global u32 g_gameDataFreeCount;
global u32 g_displayFreeCount;
global u32 g_entityFreeCount;
global u32 g_animationChainFreeCount;
global Clock_t* g_clock;
global Input_t* g_input;
global Display_t* g_display;
global GameData_t* g_gameData;
global TileTextureSet_t* g_tileTextureSet;
global ActiveSpriteTextureSet_t* g_activeSpriteTextureSet;
global ActiveSprite_t* g_playerSprite;
global ActiveSprite_t* g_firstPlayerSprite;
global ActiveSprite_t* g_npcSprite;
global Entity_t g_npcEntity;
global Npc_t g_npc;
global u32 g_npcCount;
global ActiveSpriteTextureSet_t* g_playerSpriteTextureSet;
global u32 g_playerSpriteTextureIndex;
global Direction_t g_playerSpriteDirection;
global Player_t* g_player;
global TileMap_t* g_tileMap;
global Entity_t* g_playerEntity;
global Vector4i32_t g_tileMapViewportInUnits;
global Vector4i32_t g_anchorViewport;
global i32 g_anchorX;
global i32 g_anchorY;
global u32 g_anchorTileSize;
global u32 g_tileMapGetPortalCount;
global TileMapPortal_t* g_testPortal;
global u32 g_tileMapId;
global u32 g_tileMapCenterEntityCount;
global AnimationChain_t* g_animationChain;

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

Entity_t* Entity_Create( MemArena_t* memArena, ActiveSprite_t* sprite )
{
   g_playerEntity = (Entity_t*)MemArena_AllocMem( memArena, sizeof( Entity_t ) );
   g_playerEntity->sprite = sprite;
   return g_playerEntity;
}

void Entity_Free( Entity_t* entity, MemArena_t* memArena )
{
   ActiveSprite_Free( entity->sprite, memArena );
   MemArena_FreeMem( memArena, entity );
   g_entityFreeCount++;
}

void Player_Init( Player_t* player,
                  MemArena_t* arena,
                  ActiveSpriteTextureSet_t* textureSet,
                  Vector2i32_t size,
                  Vector2i32_t spriteOffset )
{
   player->entity = Entity_Create( arena, ActiveSprite_Create( arena, textureSet ) );
   ActiveSprite_SetTextureIndex( Entity_GetSprite( player->entity ), 1 );
   Entity_SetSize( player->entity, size.x, size.y );
   Entity_SetSpriteOffset( player->entity, spriteOffset.x, spriteOffset.y );
}

size_t Player_GetStructSize( void )
{
   return sizeof( Player_t );
}

void Player_Free( MemArena_t* arena, Player_t* player )
{
   Entity_Free( player->entity, arena );
}

void Player_ResetChaining( Player_t* player )
{
   UNUSED_PARAM( player );
}

Entity_t* Player_GetEntity( const Player_t* player )
{
   return player->entity;
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

void Entity_SetTileIndex( Entity_t* entity, u32 tileIndex )
{
   u32 oldTileIndex = entity->tileIndex;
   entity->tileIndex = tileIndex;
   UNUSED_PARAM( oldTileIndex );
}

u32 Entity_GetTileIndex( Entity_t* entity )
{
   return entity->tileIndex;
}

ActiveSprite_t* Entity_GetSprite( Entity_t* entity )
{
   return entity->sprite;
}

u32 TileMap_GetNpcCount( TileMap_t* tileMap )
{
   UNUSED_PARAM( tileMap );
   return g_npcCount;
}

Npc_t* TileMap_GetNpc( TileMap_t* tileMap, u32 npcIndex )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( npcIndex );
   return &g_npc;
}

Entity_t* Npc_GetEntity( Npc_t* npc )
{
   return npc->entity;
}

void Npc_Tic( Npc_t* npc )
{
   ActiveSprite_Tic( Entity_GetSprite( Npc_GetEntity( npc ) ), CLOCK_FRAME_SEC );
}

Vector2i32_t Entity_GetSpriteOffset( Entity_t* entity )
{
   return entity->spriteOffset;
}

void Entity_SetSprite( Entity_t* entity, ActiveSprite_t* sprite )
{
   entity->sprite = sprite;
}

void Entity_SetSpriteOffset( Entity_t* entity, i32 offsetX, i32 offsetY )
{
   entity->spriteOffset.x = offsetX;
   entity->spriteOffset.y = offsetY;
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
   g_tileTextureSet->info.tileSize = 16;
   return g_tileTextureSet;
}

void TileTextureSet_Free( TileTextureSet_t* tileTextureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileTextureSet );
   g_tileTextureSetFreeCount++;
}

ActiveSpriteTextureSet_t* ActiveSpriteTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData )
{
   UNUSED_PARAM( gameData );
   g_activeSpriteTextureSet = (ActiveSpriteTextureSet_t*)MemArena_AllocMem( memArena, sizeof( ActiveSpriteTextureSet_t ) );
   return g_activeSpriteTextureSet;
}

void ActiveSpriteTextureSet_Free( ActiveSpriteTextureSet_t* textureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, textureSet );
   g_activeSpriteTextureSetFreeCount++;
}

ActiveSprite_t* ActiveSprite_Create( MemArena_t* memArena, ActiveSpriteTextureSet_t* textureSet )
{
   ActiveSprite_t* sprite = (ActiveSprite_t*)MemArena_AllocMem( memArena, sizeof( ActiveSprite_t ) );
   sprite->dir = Direction_Down;
   if ( !g_firstPlayerSprite )
   {
      g_firstPlayerSprite = sprite;
   }
   g_playerSprite = sprite;
   g_playerSpriteTextureSet = textureSet;
   return sprite;
}

void ActiveSprite_Free( ActiveSprite_t* activeSprite, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, activeSprite );
}

void ActiveSprite_Tic( ActiveSprite_t* activeSprite, r32 deltaSec )
{
   g_spriteTicSprite = activeSprite;
   g_spriteTicDeltaSec = deltaSec;
   g_spriteTicCount++;
}

void ActiveSprite_SetTextureIndex( ActiveSprite_t* activeSprite, u32 textureIndex )
{
   UNUSED_PARAM( activeSprite );
   g_playerSpriteTextureIndex = textureIndex;
}

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir )
{
   activeSprite->dir = dir;
   g_playerSpriteDirection = dir;
}

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite )
{
   return activeSprite->dir;
}

ActiveSpriteTextureSet_t* ActiveSprite_GetTextureSet( ActiveSprite_t* activeSprite )
{
   UNUSED_PARAM( activeSprite );
   return g_playerSpriteTextureSet;
}

u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet )
{
   return tileTextureSet->info.tileSize;
}

TileMap_t* TileMap_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData, ActiveSpriteTextureSet_t* activeSpriteTextureSet, u32 tileMapId, u32 tileSizePixels )
{
   UNUSED_PARAM( gameData );
   UNUSED_PARAM( activeSpriteTextureSet );
   UNUSED_PARAM( tileSizePixels );
   g_tileMap = (TileMap_t*)MemArena_AllocMem( memArena, sizeof( TileMap_t ) );
   g_tileMapId = tileMapId;
   g_tileMap->info.tilesX = tileMapId;
   g_tileMap->info.tilesY = tileMapId;
   g_tileMap->tiles = 0;
   return g_tileMap;
}

void TileMap_Free( TileMap_t* tileMap, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileMap );
   g_tileMapFreeCount++;
}

Vector4i32_t TileMap_GetViewportInUnits( TileMap_t* tileMap )
{
   UNUSED_PARAM( tileMap );
   return g_tileMapViewportInUnits;
}

void TileMap_SetViewportInUnits( TileMap_t* tileMap, Vector4i32_t viewportInUnits )
{
   UNUSED_PARAM( tileMap );
   g_tileMapViewportInUnits = viewportInUnits;
}

void TileMap_AnchorViewportToPointUnits( TileMap_t* tileMap, u32 x, u32 y )
{
   UNUSED_PARAM( tileMap );
   g_tileMapAnchorCount++;
   g_anchorViewport = g_tileMapViewportInUnits;
   g_anchorX = (i32)x;
   g_anchorY = (i32)y;
}

void TileMap_AnchorViewportToEntity( TileMap_t* tileMap, Entity_t* entity )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( entity );
   g_tileMapAnchorCount++;
}

u32 TileMap_GetTileIndexForEntity( TileMap_t* tileMap, Entity_t* entity )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( entity );
   return 7;
}

u32 TileMap_GetTilesX( TileMap_t* tileMap )
{
   return tileMap->info.tilesX;
}

void TileMap_CenterEntityInTile( TileMap_t* tileMap, Entity_t* entity, u32 tileIndex )
{
   UNUSED_PARAM( tileMap );
   g_tileMapCenterEntityCount++;
   Entity_SetPosition( entity, 100 * WORLD_UNITS_PER_PIXEL, 100 * WORLD_UNITS_PER_PIXEL );
   Entity_SetTileIndex( entity, tileIndex );
}

TileMapPortal_t* TileMap_GetPortal( TileMap_t* tileMap, u32 tileIndex )
{
   UNUSED_PARAM( tileMap );
   g_tileMapGetPortalCount++;

   if ( g_testPortal && g_testPortal->sourceTileIndex == tileIndex )
   {
      return g_testPortal;
   }

   return 0;
}

u32 TileMap_GetId( TileMap_t* tileMap )
{
   UNUSED_PARAM( tileMap );
   return g_tileMapId;
}

u32 TileMapPortal_GetDestinationTileMapId( TileMapPortal_t* portal )
{
   return portal->destinationTileMapId;
}

u32 TileMapPortal_GetDestinationTileIndex( TileMapPortal_t* portal )
{
   return portal->destinationTileIndex;
}

Direction_t TileMapPortal_GetDestinationDir( TileMapPortal_t* portal )
{
   return portal->destinationDir;
}

AnimationChain_t* AnimationChain_Create( MemArena_t* memArena, u32 maxAnimations )
{
   g_animationChain = (AnimationChain_t*)MemArena_AllocMem( memArena, sizeof( AnimationChain_t ) );
   g_animationChain->animations = (Animation_t*)MemArena_AllocMem( memArena, sizeof( Animation_t ) * maxAnimations );
   g_animationChain->maxAnimations = maxAnimations;
   g_animationChain->count = 0;
   g_animationChain->curAnimation = 0;
   g_animationChain->isRunning = False;
   return g_animationChain;
}

void AnimationChain_Free( AnimationChain_t* chain, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, chain->animations );
   MemArena_FreeMem( memArena, chain );
   g_animationChainFreeCount++;
}

u32 AnimationChain_GetMaxAnimations( AnimationChain_t* chain )
{
   return chain->maxAnimations;
}

u32 AnimationChain_GetCount( AnimationChain_t* chain )
{
   return chain->count;
}

r32 AnimationChain_GetIsRunning( AnimationChain_t* chain )
{
   return chain->isRunning;
}

void AnimationChain_Reset( AnimationChain_t* chain )
{
   chain->count = 0;
   chain->curAnimation = 0;
   chain->isRunning = False;
}

void AnimationChain_Push( AnimationChain_t* chain, AnimationType_t type, r32 duration, void (*finishedCallback)( void* callbackData1, void* callbackData2 ), void* callbackData1, void* callbackData2 )
{
   Animation_t* animation = &chain->animations[ chain->count ];

   animation->type = type;
   animation->duration = duration;
   animation->finishedCallback = finishedCallback;
   animation->callbackData1 = callbackData1;
   animation->callbackData2 = callbackData2;
   chain->count++;
}

void AnimationChain_Start( AnimationChain_t* chain, void (*finishedCallback)( void* callbackData1, void* callbackData2 ), void* callbackData1, void* callbackData2 )
{
   chain->finishedCallback = finishedCallback;
   chain->callbackData1 = callbackData1;
   chain->callbackData2 = callbackData2;
   chain->isRunning = True;
}

void AnimationChain_Tic( AnimationChain_t* chain, r32 deltaTime )
{
   UNUSED_PARAM( chain );
   UNUSED_PARAM( deltaTime );
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
   g_spriteTicDeltaSec = 0.0f;
   g_spriteTicCount = 0;
   g_spriteTicSprite = 0;
   g_inputResetCount = 0;
   g_platformHandleMessagesCount = 0;
   g_gameHandleInputCount = 0;
   g_gameRenderCount = 0;
   g_tileMapAnchorCount = 0;
   g_tileMapFreeCount = 0;
   g_tileTextureSetFreeCount = 0;
   g_activeSpriteTextureSetFreeCount = 0;
   g_playerSprite = 0;
   g_firstPlayerSprite = 0;
   g_npcCount = 0;
   g_npcSprite = (ActiveSprite_t*)2;
   g_npc.entity = &g_npcEntity;
   g_npcEntity.sprite = g_npcSprite;
   g_playerSpriteTextureSet = 0;
   g_playerSpriteTextureIndex = 0;
   g_playerSpriteDirection = Direction_Down;
   g_gameDataFreeCount = 0;
   g_displayFreeCount = 0;
   g_entityFreeCount = 0;
   g_animationChainFreeCount = 0;
   g_tileMapGetPortalCount = 0;
   g_testPortal = 0;
   g_tileMapId = 1;
   g_tileMapCenterEntityCount = 0;
}

void tearDown( void ) {}

void test_Game_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_size_t( 0, Game_GetStructSize() );
}

void test_Game_GetPlayerEntity_ReturnsRequestedEntity( void )
{
   Game_t* game = CreateGame();

   TEST_ASSERT_EQUAL_PTR( Player_GetEntity( Game_GetPlayer( game, 0 ) ), Game_GetPlayerEntity( game, 0 ) );
   TEST_ASSERT_EQUAL_PTR( Player_GetEntity( Game_GetPlayer( game, 2 ) ), Game_GetPlayerEntity( game, 2 ) );
   TEST_ASSERT_EQUAL_PTR( Player_GetEntity( Game_GetPlayer( game, 3 ) ), Game_GetPlayerEntity( game, 3 ) );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_GetActivePlayerEntity_UsesPlayerOrder( void )
{
   Game_t* game = CreateGame();

   Game_GetPlayerOrder( game )[0] = 2;
   TEST_ASSERT_EQUAL_PTR( Player_GetEntity( Game_GetPlayer( game, 2 ) ), Game_GetActivePlayerEntity( game ) );

   Game_GetPlayerOrder( game )[0] = 1;
   TEST_ASSERT_EQUAL_PTR( Player_GetEntity( Game_GetPlayer( game, 1 ) ), Game_GetActivePlayerEntity( game ) );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_Create_InitializesPlayerOrder( void )
{
   Game_t* game = CreateGame();

   TEST_ASSERT_EQUAL_UINT( GAME_MAX_PLAYERS, Game_GetPlayerCount( game ) );
   for ( u32 i = 0; i < GAME_MAX_PLAYERS; i++ )
   {
      TEST_ASSERT_EQUAL_UINT( i, Game_GetPlayerOrder( game )[i] );
   }

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_Create_InitializesDependenciesAndDefaultState( void )
{
   Vector4i32_t viewportInUnits;
   Vector4i32_t playerRect;
   Game_t* game = CreateGame();

   TEST_ASSERT_NOT_NULL( game );
   TEST_ASSERT_EQUAL_PTR( g_clock, Game_GetClock( game ) );
   TEST_ASSERT_EQUAL_PTR( g_input, Game_GetInput( game ) );
   TEST_ASSERT_EQUAL_PTR( g_display, Game_GetDisplay( game ) );
   TEST_ASSERT_EQUAL_PTR( g_gameData, Game_GetGameData( game ) );
   TEST_ASSERT_EQUAL_PTR( g_tileTextureSet, Game_GetTileTextureSet( game ) );
   TEST_ASSERT_EQUAL_PTR( g_activeSpriteTextureSet, Game_GetActiveSpriteTextureSet( game ) );
   TEST_ASSERT_EQUAL_PTR( g_tileMap, Game_GetTileMap( game ) );
   TEST_ASSERT_NOT_NULL( g_animationChain );
   TEST_ASSERT_EQUAL_UINT( 32, g_animationChain->maxAnimations );

   viewportInUnits = TileMap_GetViewportInUnits( Game_GetTileMap( game ) );
   TEST_ASSERT_EQUAL_INT( 0, viewportInUnits.x );
   TEST_ASSERT_EQUAL_INT( 0, viewportInUnits.y );
   TEST_ASSERT_EQUAL_INT( DISPLAY_WIDTH * WORLD_UNITS_PER_PIXEL, viewportInUnits.w );
   TEST_ASSERT_EQUAL_INT( DISPLAY_HEIGHT * WORLD_UNITS_PER_PIXEL, viewportInUnits.h );

   TEST_ASSERT_EQUAL_UINT( 4, Game_GetPlayerCount( game ) );
   playerRect = Entity_GetRect( Game_GetPlayerEntity( game, 0 ) );
   TEST_ASSERT_EQUAL_INT( 100 * WORLD_UNITS_PER_PIXEL, playerRect.x );
   TEST_ASSERT_EQUAL_INT( 100 * WORLD_UNITS_PER_PIXEL, playerRect.y );
   TEST_ASSERT_EQUAL_INT( 12 * WORLD_UNITS_PER_PIXEL, playerRect.w );
   TEST_ASSERT_EQUAL_INT( 12 * WORLD_UNITS_PER_PIXEL, playerRect.h );
   TEST_ASSERT_EQUAL_UINT( 40, Game_GetPlayerEntity( game, 0 )->tileIndex );
   TEST_ASSERT_EQUAL_PTR( g_firstPlayerSprite, Entity_GetSprite( Game_GetPlayerEntity( game, 0 ) ) );
   TEST_ASSERT_EQUAL_PTR( g_activeSpriteTextureSet, ActiveSprite_GetTextureSet( g_playerSprite ) );
   TEST_ASSERT_EQUAL_UINT( 3, g_playerSpriteTextureIndex );
   TEST_ASSERT_EQUAL_INT( -2, Entity_GetSpriteOffset( Game_GetPlayerEntity( game, 0 ) ).x );
   TEST_ASSERT_EQUAL_INT( -2, Entity_GetSpriteOffset( Game_GetPlayerEntity( game, 0 ) ).y );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_SetPlayerRect_UpdatesPlayerRectangle( void )
{
   Vector4i32_t playerRect = { 25, 30, 18, 20 };
   Game_t* game = CreateGame();

   Game_SetPlayerRect( game, 0, playerRect );

   playerRect = Entity_GetRect( Game_GetPlayerEntity( game, 0 ) );
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
   TEST_ASSERT_EQUAL_UINT( 1, g_gameRenderCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_clockEndCount );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_Run_TicsPlayerSpriteWithClockFrameDuration( void )
{
   Game_t* game = CreateGame();

   Game_Run( game );

   TEST_ASSERT_EQUAL_UINT( 4, g_spriteTicCount );
   TEST_ASSERT_EQUAL_PTR( g_playerSprite, g_spriteTicSprite );
   TEST_ASSERT_EQUAL_FLOAT( CLOCK_FRAME_SEC, g_spriteTicDeltaSec );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_Run_TicsNpcSpritesWithClockFrameDuration( void )
{
   Game_t* game = CreateGame();

   g_npcCount = 1;
   Game_Run( game );

   TEST_ASSERT_EQUAL_UINT( 5, g_spriteTicCount );
   TEST_ASSERT_EQUAL_PTR( g_npcSprite, g_spriteTicSprite );
   TEST_ASSERT_EQUAL_FLOAT( CLOCK_FRAME_SEC, g_spriteTicDeltaSec );

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
   TEST_ASSERT_EQUAL_UINT( 1, g_activeSpriteTextureSetFreeCount );
   TEST_ASSERT_EQUAL_UINT( 4, g_entityFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, g_animationChainFreeCount );
   TEST_ASSERT_EQUAL_UINT( 19, g_freeCount );
}

void test_Game_OnPlayerTileIndexChanged_DoesNothingWhenNoPortal( void )
{
   Game_t* game = CreateGame();
   g_testPortal = 0;
   g_tileMapGetPortalCount = 0;

   Entity_SetTileIndex( Game_GetPlayerEntity( game, 0 ), 10 );
   Game_OnPlayerTileIndexChanged( game, 10 );

   TEST_ASSERT_EQUAL_UINT( 1, g_tileMapGetPortalCount );
   TEST_ASSERT_EQUAL_UINT( 10, Entity_GetTileIndex( Game_GetPlayerEntity( game, 0 ) ) );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_OnPlayerTileIndexChanged_EntersPortalWhenPresentAndMapIsUnchanged( void )
{
   Game_t* game = CreateGame();
   TileMapPortal_t portal = { 20, 1, 13, Direction_Left };

   g_tileMapGetPortalCount = 0;
   g_tileMapCenterEntityCount = 0;
   g_tileMapFreeCount = 0;
   g_testPortal = &portal;
   g_tileMapId = 1;

   Entity_SetTileIndex( Game_GetPlayerEntity( game, 0 ), 20 );
   Game_OnPlayerTileIndexChanged( game, 20 );

   TEST_ASSERT_EQUAL_UINT( 1, g_tileMapGetPortalCount );
   TEST_ASSERT_EQUAL_UINT( 0, g_tileMapCenterEntityCount );
   TEST_ASSERT_EQUAL_UINT( 0, g_tileMapFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, TileMap_GetId( Game_GetTileMap( game ) ) );
   TEST_ASSERT_EQUAL_UINT( 3, AnimationChain_GetCount( g_animationChain ) );
   TEST_ASSERT_EQUAL_FLOAT( True, AnimationChain_GetIsRunning( g_animationChain ) );
   TEST_ASSERT_EQUAL_UINT( 20, Entity_GetTileIndex( Game_GetPlayerEntity( game, 0 ) ) );
   TEST_ASSERT_EQUAL_INT( Direction_Down, g_playerSpriteDirection );

   Game_Free( game, (MemArena_t*)1 );
}

void test_Game_OnPlayerTileIndexChanged_EntersPortalWhenPresentAndMapChanges( void )
{
   Game_t* game = CreateGame();
   TileMapPortal_t portal = { 20, 2, 24, Direction_Up };

   g_tileMapGetPortalCount = 0;
   g_tileMapCenterEntityCount = 0;
   g_tileMapFreeCount = 0;
   g_testPortal = &portal;
   g_tileMapId = 1;

   Entity_SetTileIndex( Game_GetPlayerEntity( game, 0 ), 20 );
   Game_OnPlayerTileIndexChanged( game, 20 );

   TEST_ASSERT_EQUAL_UINT( 1, g_tileMapGetPortalCount );
   TEST_ASSERT_EQUAL_UINT( 0, g_tileMapCenterEntityCount );
   TEST_ASSERT_EQUAL_UINT( 0, g_tileMapFreeCount );
   TEST_ASSERT_EQUAL_UINT( 1, TileMap_GetId( Game_GetTileMap( game ) ) );
   TEST_ASSERT_EQUAL_UINT( 3, AnimationChain_GetCount( g_animationChain ) );
   TEST_ASSERT_EQUAL_FLOAT( True, AnimationChain_GetIsRunning( g_animationChain ) );
   TEST_ASSERT_EQUAL_UINT( 20, Entity_GetTileIndex( Game_GetPlayerEntity( game, 0 ) ) );
   TEST_ASSERT_EQUAL_INT( Direction_Down, g_playerSpriteDirection );

   Game_Free( game, (MemArena_t*)1 );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_GetStructSize_ReturnsNonZeroSize );
   
   RUN_TEST( test_Game_GetPlayerEntity_ReturnsRequestedEntity );
   
   RUN_TEST( test_Game_GetActivePlayerEntity_UsesPlayerOrder );
   
   RUN_TEST( test_Game_Create_InitializesPlayerOrder );
   RUN_TEST( test_Game_Create_InitializesDependenciesAndDefaultState );

   RUN_TEST( test_Game_SetPlayerRect_UpdatesPlayerRectangle );

   RUN_TEST( test_Game_Run_ExecutesOneFrameAndUpdatesViewport );
   RUN_TEST( test_Game_Run_TicsPlayerSpriteWithClockFrameDuration );
   RUN_TEST( test_Game_Run_TicsNpcSpritesWithClockFrameDuration );
   
   RUN_TEST( test_Game_Free_ReleasesAllDependencies );

   RUN_TEST( test_Game_OnPlayerTileIndexChanged_DoesNothingWhenNoPortal );
   RUN_TEST( test_Game_OnPlayerTileIndexChanged_EntersPortalWhenPresentAndMapIsUnchanged );
   RUN_TEST( test_Game_OnPlayerTileIndexChanged_EntersPortalWhenPresentAndMapChanges );

   return UNITY_END();
}
