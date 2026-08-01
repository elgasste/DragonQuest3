#include <stdio.h>

#include "game.h"
#include "mem_arena.h"
#include "pixel_buffer.h"

void Game_Create( Game_t* game )
{
   MemArenaResult_t result;
   char msg[STRING_SIZE_DEFAULT];

   // TODO: define this value somewhere else
   result = MemArena_Create( &( game->memArena ), 1024 * 1024 * 512 ); // 512 MB
   if ( result != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "Failed to create memory arena for game object: %s", MemArena_GetErrorMessage( result ) );
      FatalError( msg );
   }

   PixelBuffer_Create( &( game->pixelBuffer ), game->memArena, SCREEN_WIDTH, SCREEN_HEIGHT );
}

void Game_Destroy( Game_t* game )
{
   MemArena_Destroy( &( game->memArena ) );
   game->memArena = 0;
}