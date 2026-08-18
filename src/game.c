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

   *pGame = (Game_t*)MemArena_AllocMem( memArena, sizeof( Game_t ) );

   game = *pGame;
   game->memArena = memArena;
   
   game->clock = Clock_Create( memArena );
   game->input = Input_Create( game->memArena );
   game->display = Display_Create( game->memArena, DISPLAY_WIDTH, DISPLAY_HEIGHT );

   Clock_Init( game->clock, GAME_DEFAULT_FPS );
   Input_ResetAllStates( game->input );

   game->gameData = 0;
   game->tileMap = 0;
   game->tileTextureSet = 0;

   Game_LoadGameData( game, gameDataFilePath );
   
   // TODO: should this come from the game data file? or is it too integral to the game engine?
   game->tileMapViewport.x = 0;
   game->tileMapViewport.y = 0;
   game->tileMapViewport.w = DISPLAY_WIDTH;
   game->tileMapViewport.h = DISPLAY_HEIGHT;
   game->playerRect.x = 10;
   game->playerRect.y = 10;
   game->playerRect.w = 12;
   game->playerRect.h = 14;

   // TODO: temporary, this will eventually be part of the game data file
   game->tileMap = 0;
   Game_LoadTileMapFromId( game, 1 );
}

void Game_Destroy( Game_t** pGame, MemArena_t* memArena )
{
   if ( !pGame || !*pGame )
   {
      return;
   }

   Clock_Free( ( *pGame )->clock, memArena );
   Input_Free( ( *pGame )->input, memArena );

   Display_Free( ( *pGame )->display, memArena );

   GameData_Cleanup( ( *pGame )->gameData, memArena );
   MemArena_FreeMem( memArena, ( *pGame )->gameData );

   TileMap_Cleanup( ( *pGame )->tileMap, memArena );
   MemArena_FreeMem( memArena, ( *pGame )->tileMap );

   TileTextureSet_Cleanup( ( *pGame )->tileTextureSet, memArena );
   MemArena_FreeMem( memArena, ( *pGame )->tileTextureSet );

   MemArena_FreeMem( memArena, *pGame );
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
      Game_HandleInput( game );
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
   i32 centerX, centerY;

   centerX = game->playerRect.x + ( game->playerRect.w / 2 );
   centerY = game->playerRect.y + ( game->playerRect.h / 2 );
   TileMap_AnchorViewportToPoint( game->tileMap, &game->tileMapViewport, centerX, centerY );
}
