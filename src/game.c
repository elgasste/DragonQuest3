#include "animation.h"
#include "clock.h"
#include "display.h"
#include "entity.h"
#include "game.h"
#include "game_data.h"
#include "input.h"
#include "mem_arena.h"
#include "sprite.h"
#include "sprite_texture_set.h"
#include "tile_map.h"
#include "tile_texture_set.h"

struct Game_t
{
   MemArena_t* memArena;

   Clock_t* clock;
   Input_t* input;
   Display_t* display;
   GameData_t* gameData;

   TileTextureSet_t* tileTextureSet;
   ActiveSpriteTextureSet_t* activeSpriteTextureSet;

   TileMap_t *tileMap;
   AnimationChain_t* animationChain;

   // TODO: this is the player, temporarily
   ActiveSprite_t* playerSprite;
   Entity_t* playerEntity;

   b32 shutdown;
};

internal void Game_Tic( Game_t* game );
internal void Game_OnPlayerTileIndexChanged( void* receiver, u32 oldTileIndex, u32 newTileIndex );
internal void Game_EnterPortal( Game_t* game, TileMapPortal_t* portal );

size_t Game_GetStructSize( void )
{
   return sizeof( Game_t );
}

Game_t* Game_Create( MemArena_t* memArena, const char* gameDataFilePath )
{
   Game_t* game;

   game = (Game_t*)MemArena_AllocMem( memArena, sizeof( Game_t ) );
   game->memArena = memArena;
   
   game->clock = Clock_Create( memArena, GAME_DEFAULT_FPS );
   game->input = Input_Create( game->memArena );
   game->display = Display_Create( game->memArena, DISPLAY_WIDTH, DISPLAY_HEIGHT );
   game->gameData = GameData_Create( game->memArena, gameDataFilePath );
   game->tileTextureSet = TileTextureSet_CreateFromGameData( game->memArena, game->gameData );
   game->activeSpriteTextureSet = ActiveSpriteTextureSet_CreateFromGameData( game->memArena, game->gameData );

   // TODO: temporary, everything from here down will come from the game data file.
   game->tileMap = TileMap_CreateFromGameData( memArena, game->gameData, 1, TileTextureSet_GetTileSize( game->tileTextureSet ) );
   game->animationChain = AnimationChain_Create( memArena, 32 );

   game->playerSprite = ActiveSprite_Create( game->memArena, game->activeSpriteTextureSet );
   ActiveSprite_SetTextureIndex( game->playerSprite, 1 );
   game->playerEntity = Entity_Create( game->memArena );
   Entity_SetSize( game->playerEntity, 12 * WORLD_UNITS_PER_PIXEL, 12 * WORLD_UNITS_PER_PIXEL );
   Entity_SetVelocity( game->playerEntity, 0, 0 );
   Entity_SetSprite( game->playerEntity, game->playerSprite );
   Entity_SetSpriteOffset( game->playerEntity, -2, -2 );
   Entity_SetOnTileIndexChanged( game->playerEntity, game, Game_OnPlayerTileIndexChanged );

   TileMap_CenterEntityInTile( game->tileMap, game->playerEntity, ( TileMap_GetTilesX( game->tileMap ) * 20 ) + 20 );

   // TODO: should this come from the game data file? or is it too integral to the game engine?
   TileMap_SetViewportInUnits( game->tileMap, (Vector4i32_t){ 0, 0, DISPLAY_WIDTH * WORLD_UNITS_PER_PIXEL, DISPLAY_HEIGHT * WORLD_UNITS_PER_PIXEL } );

   return game;
}

void Game_Free( Game_t* game, MemArena_t* memArena )
{
   Clock_Free( game->clock, memArena );
   Input_Free( game->input, memArena );
   Display_Free( game->display, memArena );
   GameData_Free( game->gameData, memArena );

   if ( game->tileMap )
   {
      TileMap_Free( game->tileMap, memArena );
   }
   AnimationChain_Free( game->animationChain, memArena );

   TileTextureSet_Free( game->tileTextureSet, memArena );
   ActiveSpriteTextureSet_Free( game->activeSpriteTextureSet, memArena );

   ActiveSprite_Free( game->playerSprite, memArena );
   Entity_Free( game->playerEntity, memArena );

   MemArena_FreeMem( memArena, game );
}

Clock_t* Game_GetClock( Game_t* game )
{
   return game->clock;
}

Input_t* Game_GetInput( Game_t* game )
{
   return game->input;
}

Display_t* Game_GetDisplay( Game_t* game )
{
   return game->display;
}

GameData_t* Game_GetGameData( Game_t* game )
{
   return game->gameData;
}

TileTextureSet_t* Game_GetTileTextureSet( Game_t* game )
{
   return game->tileTextureSet;
}

ActiveSpriteTextureSet_t* Game_GetActiveSpriteTextureSet( Game_t* game )
{
   return game->activeSpriteTextureSet;
}

TileMap_t* Game_GetTileMap( Game_t* game )
{
   return game->tileMap;
}

AnimationChain_t* Game_GetAnimationChain( Game_t* game )
{
   return game->animationChain;
}

Entity_t* Game_GetPlayerEntity( Game_t* game )
{
   return game->playerEntity;
}

void Game_SetPlayerRect( Game_t* game, Vector4i32_t playerRect )
{
   Entity_SetPosition( game->playerEntity, playerRect.x, playerRect.y );
   Entity_SetSize( game->playerEntity, playerRect.w, playerRect.h );
}

void Game_Run( Game_t* game )
{
   game->shutdown = False;

   while ( !game->shutdown )
   {
      Clock_StartFrame( game->clock );
      Input_ResetPressStates( game->input );
      Platform_HandleMessages( game );
      Game_Tic( game );
      Game_Render( game );
      Clock_EndFrame( game->clock );
   }
}

void Game_Stop( Game_t* game )
{
   game->shutdown = True;
}

internal void Game_Tic( Game_t* game )
{
   r32 deltaSec;

   deltaSec = Clock_GetFrameSec( game->clock );
   
   if ( AnimationChain_GetIsRunning( game->animationChain ) )
   {
      AnimationChain_Tic( game->animationChain, deltaSec );
   }
   else
   {
      Game_HandleInput( game );
      Game_TicPhysics( game );
   }

   ActiveSprite_Tic( game->playerSprite, deltaSec );
   TileMap_AnchorViewportToEntity( game->tileMap, game->playerEntity );
}

internal void Game_OnPlayerTileIndexChanged( void* receiver, u32 oldTileIndex, u32 newTileIndex )
{
   TileMapPortal_t* portal;
   Game_t* game = (Game_t*)receiver;

   UNUSED_PARAM( oldTileIndex );

   portal = TileMap_GetPortal( game->tileMap, newTileIndex );
   if ( portal )
   {
      AnimationChain_Reset( game->animationChain );
      AnimationChain_Push( game->animationChain, AnimationType_FadeOut, 0.2f, Game_EnterPortal, game, portal );
      AnimationChain_Push( game->animationChain, AnimationType_Blackout, 0.2f, 0, 0, 0 );
      AnimationChain_Push( game->animationChain, AnimationType_FadeIn, 0.2f, 0, 0, 0 );
      AnimationChain_Start( game->animationChain, 0, 0, 0 );
   }
}

internal void Game_EnterPortal( Game_t* game, TileMapPortal_t* portal )
{
   u32 destinationTileMapId, destinationTileIndex;

   destinationTileMapId = TileMapPortal_GetDestinationTileMapId( portal );
   destinationTileIndex = TileMapPortal_GetDestinationTileIndex( portal );

   if ( destinationTileMapId != TileMap_GetId( game->tileMap ) )
   {
      TileMap_Free( game->tileMap, game->memArena );
      game->tileMap = TileMap_CreateFromGameData( game->memArena, game->gameData, destinationTileMapId, TileTextureSet_GetTileSize( game->tileTextureSet ) );
      // TODO: we shouldn't have to do this every time we swap tile maps, maybe it should be stored somewhere else?
      TileMap_SetViewportInUnits( game->tileMap, (Vector4i32_t){ 0, 0, DISPLAY_WIDTH * WORLD_UNITS_PER_PIXEL, DISPLAY_HEIGHT * WORLD_UNITS_PER_PIXEL } );
   }

   TileMap_CenterEntityInTile( game->tileMap, game->playerEntity, destinationTileIndex );
}
