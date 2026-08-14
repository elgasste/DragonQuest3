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

void Game_Create( Game_t** pGame, MemArena_t* memArena, const char* gameDataFilePath )
{
   Game_t* game;

   MemArena_Alloc( memArena, (void**)pGame, sizeof( Game_t ) );
   if ( !*pGame )
   {
      return;
   }

   game = *pGame;
   game->memArena = memArena;
   
   MemArena_Alloc( game->memArena, &game->clock, sizeof( Clock_t ) );
   if ( !game->clock )
   {
      return;
   }

   MemArena_Alloc( game->memArena, &game->input, sizeof( Input_t ) );
   if ( !game->input )
   {
      return;
   }

   MemArena_Alloc( game->memArena, &game->display, sizeof( Display_t ) );
   if ( !game->display )
   {
      return;
   }

   Clock_Init( game->clock, GAME_DEFAULT_FPS );
   Input_Init( game->input );
   Display_Init( game->display, game->memArena, DISPLAY_WIDTH, DISPLAY_HEIGHT );

   game->gameData = 0;
   game->tileMap = 0;
   game->tileTextureSet = 0;

   Game_LoadGameData( game, gameDataFilePath );
   
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

      // TODO
      //Game_Tic( game );

      Game_Render( game );
      Clock_EndFrame( game->clock );
   }
}

void Game_Stop( Game_t* game )
{
   game->shutdown = True;
}
