#include "game.h"
#include "mem_arena.h"
#include "platform_ops.h"

void Game_LoadFromFile( Game_t* game, const char* filePath )
{
   u32 fileSize;
   u8 *fileContents;

   fileContents = PlatformOps_LoadFileToMemory( filePath, game->memArena, &fileSize );

   // TODO: load stuff!

   MemArena_Free( game->memArena, fileContents );
}
