#include <stdio.h>

#include "game.h"
#include "mem_arena.h"
#include "pixel_buffer.h"

void Game_Create( Game_t* game, MemArena_t* memArena, void (*platformMessageHandler)( void ), void (*platformRenderHandler)( void ) )
{
   game->memArena = memArena;

   PixelBuffer_Create( &( game->pixelBuffer ), memArena, SCREEN_WIDTH, SCREEN_HEIGHT );

   game->platformMessageHandler = platformMessageHandler;
   game->platformRenderHandler = platformRenderHandler;
}

void Game_Run( Game_t* game )
{
   game->shutdown = False;

   while ( !game->shutdown )
   {
      // TODO
      //Clock_StartFrame( &( game->clock ) );
      //Input_ResetState( &( game->input ) );

      game->platformMessageHandler();

      PixelBuffer_ClearColor( game->pixelBuffer, 0 );

      // TODO
      //Game_Tic( game );

      game->platformRenderHandler();

      // TODO
      //Clock_EndFrame( &game->clock );
   }
}

void Game_Stop( Game_t* game )
{
   game->shutdown = True;
}
