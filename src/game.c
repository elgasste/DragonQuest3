#include <stdio.h>

#include "clock.h"
#include "display.h"
#include "game.h"
#include "game_data.h"
#include "input.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "tile_texture_set.h"

internal void Game_Tic( Game_t* game );

void Game_Create( Game_t** pGame, MemArena_t* memArena, const char* gameDataFilePath )
{
   Game_t* game;

   *pGame = (Game_t*)MemArena_Alloc( memArena, sizeof( Game_t ) );

   game = *pGame;
   game->memArena = memArena;
   
   game->clock = (Clock_t*)MemArena_Alloc( game->memArena, sizeof( Clock_t ) );
   game->input = (Input_t*)MemArena_Alloc( game->memArena, sizeof( Input_t ) );
   game->display = (Display_t*)MemArena_Alloc( game->memArena, sizeof( Display_t ) );

   Clock_Init( game->clock, GAME_DEFAULT_FPS );
   Input_Init( game->input );
   Display_Init( game->display, game->memArena, DISPLAY_WIDTH, DISPLAY_HEIGHT );

   game->gameData = 0;
   game->tileMap = 0;
   game->tileTextureSet = 0;

   Game_LoadGameData( game, gameDataFilePath );
   
   // TODO: should this come from the game data file? or is it too integral to the game engine?
   game->tileMapViewport.x = 0;
   game->tileMapViewport.y = 0;
   game->tileMapViewport.w = DISPLAY_WIDTH;
   game->tileMapViewport.h = DISPLAY_HEIGHT;
   game->playerPos.x = 0;
   game->playerPos.y = 0;

   // TODO: temporary, this will eventually be part of the game data file
   game->tileMap = 0;
   Game_LoadTileMapFromId( game, 0 );
}

void Game_Destroy( Game_t** pGame )
{
   if ( !pGame || !*pGame )
   {
      return;
   }

   MemArena_Free( ( *pGame )->memArena, ( *pGame )->clock );
   MemArena_Free( ( *pGame )->memArena, ( *pGame )->input );

   Display_Cleanup( ( *pGame )->display, ( *pGame )->memArena );
   MemArena_Free( ( *pGame )->memArena, ( *pGame )->display );

   GameData_Cleanup( ( *pGame )->gameData, ( *pGame )->memArena );
   MemArena_Free( ( *pGame )->memArena, ( *pGame )->gameData );

   TileMap_Cleanup( ( *pGame )->tileMap, ( *pGame )->memArena );
   MemArena_Free( ( *pGame )->memArena, ( *pGame )->tileMap );

   TileTextureSet_Cleanup( ( *pGame )->tileTextureSet, ( *pGame )->memArena );
   MemArena_Free( ( *pGame )->memArena, ( *pGame )->tileTextureSet );

   MemArena_Free( ( *pGame )->memArena, *pGame );
   *pGame = 0;
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
   TileMap_AnchorViewportToPoint( game->tileMap, &game->tileMapViewport, game->playerPos.x, game->playerPos.y );
}
