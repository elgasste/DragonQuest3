#include <string.h>

#include "game.h"
#include "mem_arena.h"
#include "platform_ops.h"

internal void GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize );

void Game_LoadFromFile( Game_t* game, const char* filePath )
{
   u32 fileSize;
   u8 *fileContents;

   fileContents = PlatformOps_LoadFileToMemory( filePath, game->memArena, &fileSize );
   if ( !fileContents )
   {
      PlatformOps_FatalError( "failed to load game data file into memory." );
      return;
   }

   GameData_VerifyHeaderAndVersion( fileContents, fileSize );

   MemArena_Free( game->memArena, fileContents );
}

internal void GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize )
{
   GameDataHeader_t* header;

   if ( fileSize < sizeof( GameDataHeader_t ) )
   {
      PlatformOps_FatalError( "game data file is too small to contain a valid header." );
      return;
   }

   header = (GameDataHeader_t*)fileContents;

   if ( strncmp( header->magic, GAME_DATA_MAGIC, 4 ) != 0 )
   {
      PlatformOps_FatalError( "game data file has an invalid magic number." );
      return;
   }
   else if ( header->version.major != GAME_DATA_VERSION_MAJOR ||
             header->version.minor != GAME_DATA_VERSION_MINOR ||
             header->version.maint != GAME_DATA_VERSION_MAINT )
   {
      PlatformOps_FatalError( "game data file has an incompatible version." );
   }
}
