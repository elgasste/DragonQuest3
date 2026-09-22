#include "animation.h"
#include "clock.h"
#include "display.h"
#include "entity.h"
#include "game.h"
#include "game_data.h"
#include "input.h"
#include "mem_arena.h"
#include "npc.h"
#include "player.h"
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

   Player_t* players;
   u32 playerCount;
   u32 playerOrder[GAME_MAX_PLAYERS];

   b32 shutdown;
};

internal void Game_Tic( Game_t* game );
internal void Game_TicEntities( Game_t* game, r32 deltaSec );
internal void Game_EnterPortal( Game_t* game, TileMapPortal_t* portal );

size_t Game_GetStructSize( void )
{
   return sizeof( Game_t );
}

Game_t* Game_Create( MemArena_t* memArena, const char* gameDataFilePath )
{
   u32 i;
   Game_t* game;
   Player_t* player;

   game = (Game_t*)MemArena_AllocMem( memArena, sizeof( Game_t ) );
   game->memArena = memArena;
   
   game->clock = Clock_Create( memArena, GAME_DEFAULT_FPS );
   game->input = Input_Create( game->memArena );
   game->display = Display_Create( game->memArena, DISPLAY_WIDTH, DISPLAY_HEIGHT );
   game->gameData = GameData_Create( game->memArena, gameDataFilePath );
   game->tileTextureSet = TileTextureSet_CreateFromGameData( game->memArena, game->gameData );
   game->activeSpriteTextureSet = ActiveSpriteTextureSet_CreateFromGameData( game->memArena, game->gameData );

   // TODO: temporary, everything from here down will come from the game data file.
   game->tileMap = TileMap_CreateFromGameData( memArena, game->gameData, game->activeSpriteTextureSet, 1, TileTextureSet_GetTileSize( game->tileTextureSet ) );
   game->animationChain = AnimationChain_Create( memArena, 32 );

   game->playerCount = GAME_MAX_PLAYERS;
   game->players = (Player_t*)MemArena_AllocMem( game->memArena, Player_GetStructSize() * game->playerCount );
   for ( i = 0; i < game->playerCount; i++ )
   {
      player = (Player_t*)( (u8*)game->players + i * Player_GetStructSize() );
      Player_Init( player,
                   game->memArena,
                   game->activeSpriteTextureSet,
                   (Vector2i32_t){ 12 * WORLD_UNITS_PER_PIXEL, 12 * WORLD_UNITS_PER_PIXEL },
                   (Vector2i32_t){ -2, -2 } );
      ActiveSprite_SetTextureIndex( Entity_GetSprite( Player_GetEntity( player ) ), i );
      TileMap_CenterEntityInTile( game->tileMap, Player_GetEntity( player ), ( TileMap_GetTilesX( game->tileMap ) * 20 ) + 20 );
      game->playerOrder[i] = i;
   }

   // TODO: should this come from the game data file? or is it too integral to the game engine?
   TileMap_SetViewportInUnits( game->tileMap,
                               (Vector4i32_t){ 0, 0, DISPLAY_WIDTH * WORLD_UNITS_PER_PIXEL, DISPLAY_HEIGHT * WORLD_UNITS_PER_PIXEL } );

   return game;
}

void Game_Free( Game_t* game, MemArena_t* memArena )
{
   u32 i;

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

   for ( i = 0; i < game->playerCount; i++ )
   {
      Player_Free( game->memArena, (Player_t*)( (u8*)game->players + i * Player_GetStructSize() ) );
   }
   MemArena_FreeMem( game->memArena, game->players );

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

u32 Game_GetPlayerCount( Game_t* game )
{
   return game->playerCount;
}

Entity_t* Game_GetPlayerEntity( Game_t* game, u32 playerIndex )
{
   return Player_GetEntity( Game_GetPlayer( game, playerIndex ) );
}

Entity_t* Game_GetActivePlayerEntity( Game_t* game )
{
   return Player_GetEntity( Game_GetActivePlayer( game ) );
}

Player_t* Game_GetPlayer( Game_t* game, u32 playerIndex )
{
   return (Player_t*)( (u8*)game->players + playerIndex * Player_GetStructSize() );
}

Player_t* Game_GetActivePlayer( Game_t* game )
{
   return (Player_t*)( (u8*)game->players + game->playerOrder[0] * Player_GetStructSize() );
}

u32* Game_GetPlayerOrder( Game_t* game )
{
   return game->playerOrder;
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

void Game_SetPlayerRect( Game_t* game, u32 playerIndex, Vector4i32_t playerRect )
{
   Player_t* player;

   player = (Player_t*)( (u8*)game->players + playerIndex * Player_GetStructSize() );
   Entity_SetPosition( Player_GetEntity( player ), playerRect.x, playerRect.y );
   Entity_SetSize( Player_GetEntity( player ), playerRect.w, playerRect.h );
}

void Game_OnPlayerTileIndexChanged( Game_t* game, u32 newTileIndex )
{
   TileMapPortal_t* portal;

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

internal void Game_Tic( Game_t* game )
{
   r32 deltaSec;
   Player_t* activePlayer;

   deltaSec = Clock_GetFrameSec( game->clock );
   
   if ( AnimationChain_GetIsRunning( game->animationChain ) )
   {
      AnimationChain_Tic( game->animationChain, deltaSec );
   }
   else
   {
      Game_HandleInput( game );
   }

   Game_TicEntities( game, deltaSec );
   Game_TicPhysics( game );

   activePlayer = Game_GetActivePlayer( game );
   TileMap_AnchorViewportToEntity( game->tileMap, Player_GetEntity( activePlayer ) );
}

internal void Game_TicEntities( Game_t* game, r32 deltaSec )
{
   u32 i;
   Player_t* player;
   Entity_t* playerEntity;
   ActiveSprite_t* playerSprite;

   for ( i = 0; i < game->playerCount; i++ )
   {
      player = (Player_t*)( (u8*)game->players + i * Player_GetStructSize() );
      playerEntity = Player_GetEntity( player );
      playerSprite = Entity_GetSprite( playerEntity );
      ActiveSprite_Tic( playerSprite, deltaSec );
   }

   for ( i = 0; i < TileMap_GetNpcCount( game->tileMap ); i++ )
   {
      Npc_Tic( TileMap_GetNpc( game->tileMap, i ), game->clock );
   }
}

internal void Game_EnterPortal( Game_t* game, TileMapPortal_t* portal )
{
   u32 i, destinationTileMapId, destinationTileIndex;
   Player_t* player;
   Entity_t* playerEntity;

   destinationTileMapId = TileMapPortal_GetDestinationTileMapId( portal );
   destinationTileIndex = TileMapPortal_GetDestinationTileIndex( portal );

   if ( destinationTileMapId != TileMap_GetId( game->tileMap ) )
   {
      TileMap_Free( game->tileMap, game->memArena );
      game->tileMap = TileMap_CreateFromGameData( game->memArena, game->gameData, game->activeSpriteTextureSet, destinationTileMapId, TileTextureSet_GetTileSize( game->tileTextureSet ) );
   }

   for ( i = 0; i < game->playerCount; i++ )
   {
      player = (Player_t*)( (u8*)game->players + i * Player_GetStructSize() );
      playerEntity = Player_GetEntity( player );
      TileMap_CenterEntityInTile( game->tileMap, playerEntity, destinationTileIndex );
      ActiveSprite_SetDirection( Entity_GetSprite( playerEntity), TileMapPortal_GetDestinationDir( portal ) );
   }
}
