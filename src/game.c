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

struct Game_t
{
   MemArena_t* memArena;

   Clock_t* clock;
   Input_t* input;
   Display_t* display;
   GameData_t* gameData;

   TileTextureSet_t* tileTextureSet;

   TileMap_t *tileMap;
   Vector4i32_t tileMapViewport;

   // TODO: this is the player, temporarily
   Vector4i32_t playerRect;

   b32 shutdown;
};

internal void Game_Tic( Game_t* game );

size_t Game_GetSize( void )
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

   game->tileMap = 0;
   
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
   game->tileMap = TileMap_CreateFromGameData( memArena, game->gameData, 1 );

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
      MemArena_FreeMem( memArena, game->tileMap );
   }

   if ( game->tileTextureSet )
   {
      TileTextureSet_Free( game->tileTextureSet, memArena );
      MemArena_FreeMem( memArena, game->tileTextureSet );
   }

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

TileMap_t* Game_GetTileMap( Game_t* game )
{
   return game->tileMap;
}

Vector4i32_t Game_GetTileMapViewport( Game_t* game )
{
   return game->tileMapViewport;
}

Vector4i32_t Game_GetPlayerRect( Game_t* game )
{
   return game->playerRect;
}

void Game_SetPlayerRect( Game_t* game, Vector4i32_t playerRect )
{
   game->playerRect = playerRect;
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
   TileMap_AnchorViewportToPoint( game->tileMap, &game->tileMapViewport, centerX, centerY, TileTextureSet_GetTileSize( game->tileTextureSet ) );
}
