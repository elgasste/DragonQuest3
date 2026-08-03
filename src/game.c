#include <stdio.h>

#include "game.h"
#include "mem_arena.h"
#include "pixel_buffer.h"
#include "platform_ops.h"

void Game_Create( Game_t* game, MemArena_t* memArena )
{
   game->memArena = memArena;

   PixelBuffer_Create( &( game->pixelBuffer ), memArena, SCREEN_WIDTH, SCREEN_HEIGHT );
}

void Game_Run( Game_t* game )
{
   game->shutdown = False;

   while ( !game->shutdown )
   {
      // TODO: use the new clock we just added
      //Clock_StartFrame( &( game->clock ) );
      //Input_ResetState( &( game->input ) );

      PlatformOps_HandleMessages();

      PixelBuffer_ClearColor( game->pixelBuffer, 0 );

      // TODO
      //Game_Tic( game );

      PlatformOps_RenderScreenBuffer();

      // TODO
      //Clock_EndFrame( &game->clock );
   }
}

void Game_Stop( Game_t* game )
{
   game->shutdown = True;
}
