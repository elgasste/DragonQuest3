#include <stdio.h>

#include "clock.h"
#include "game.h"
#include "mem_arena.h"
#include "platform_ops.h"
#include "screen.h"

internal b32 Game_AllocObjects( Game_t* game );

void Game_Init( Game_t* game, MemArena_t* memArena )
{
   game->memArena = memArena;

   if ( !Game_AllocObjects( game ) )
   {
      // the game should shut down if we've reached this point, but we still
      // need to do this so the unit tests won't fail.
      return;
   }

   Clock_Init( game->clock, GAME_DEFAULT_FPS );
   Screen_Init( game->screen, game->memArena, SCREEN_WIDTH, SCREEN_HEIGHT );
}

void Game_Run( Game_t* game )
{
   game->shutdown = False;

   while ( !game->shutdown )
   {
      Clock_StartFrame( game->clock );

      // MUFFINS: we should do input next

      // TODO
      //Input_ResetState( &( game->input ) );

      PlatformOps_HandleMessages( game );

      Screen_Fill( game->screen, 0 );

      // TODO
      //Game_Tic( game );

      PlatformOps_RenderScreenBuffer( game->screen );

      Clock_EndFrame( game->clock );
   }
}

void Game_Stop( Game_t* game )
{
   game->shutdown = True;
}

internal b32 Game_AllocObjects( Game_t* game )
{
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   memArenaResult = MemArena_Alloc( game->memArena, &game->clock, sizeof( Clock_t ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "Failed to create memory arena for clock: %s", MemArena_GetErrorMessage( memArenaResult ) );
      PlatformOps_FatalError( msg );
      return False;
   }

   memArenaResult = MemArena_Alloc( game->memArena, &game->screen, sizeof( Screen_t ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "Failed to create memory arena for screen: %s", MemArena_GetErrorMessage( memArenaResult ) );
      PlatformOps_FatalError( msg );
      return False;
   }

   return True;
}
