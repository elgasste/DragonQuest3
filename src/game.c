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

internal void Game_Init( Game_t* game, const char* gameDataFilePath );
internal b32 Game_AllocInfrastructure( Game_t* game );

void Game_Create( Game_t** game, MemArena_t* memArena, const char* gameDataFilePath )
{
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   memArenaResult = MemArena_Alloc( memArena, (void**)game, sizeof( Game_t ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to allocate memory for game object: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return;
   }

   ( *game )->memArena = memArena;
   Game_Init( *game, gameDataFilePath );
}

void Game_Destroy( Game_t** game )
{
   MemArena_Free( ( *game )->memArena, ( *game )->clock );
   MemArena_Free( ( *game )->memArena, ( *game )->input );

   Display_Cleanup( ( *game )->display, ( *game )->memArena );
   MemArena_Free( ( *game )->memArena, ( *game )->display );

   GameData_Cleanup( ( *game )->gameData, ( *game )->memArena );
   MemArena_Free( ( *game )->memArena, ( *game )->gameData );

   TileMap_Cleanup( ( *game )->tileMap, ( *game )->memArena );
   MemArena_Free( ( *game )->memArena, ( *game )->tileMap );

   TileTextureSet_Cleanup( ( *game )->tileTextureSet, ( *game )->memArena );
   MemArena_Free( ( *game )->memArena, ( *game )->tileTextureSet );

   MemArena_Free( ( *game )->memArena, *game );
   *game = 0;
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

internal void Game_Init( Game_t* game, const char* gameDataFilePath )
{
   if ( !Game_AllocInfrastructure( game ) )
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

internal b32 Game_AllocInfrastructure( Game_t* game )
{
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   memArenaResult = MemArena_Alloc( game->memArena, &game->clock, sizeof( Clock_t ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to create memory arena for clock: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return False;
   }

   memArenaResult = MemArena_Alloc( game->memArena, &game->input, sizeof( Input_t ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to create memory arena for input: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return False;
   }

   memArenaResult = MemArena_Alloc( game->memArena, &game->display, sizeof( Display_t ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to create memory arena for display: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return False;
   }

   return True;
}
