#include <string.h>

#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "version.h"

internal void GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize );

void Game_LoadFromFile( Game_t* game, const char* filePath )
{
   u32 fileSize;
   u8 *fileContents;

   fileContents = Platform_LoadFileToMemory( filePath, game->memArena, &fileSize );
   if ( !fileContents )
   {
      Platform_FatalError( "failed to load game data file into memory." );
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
      Platform_FatalError( "game data file is too small to contain a valid header." );
      return;
   }

   header = (GameDataHeader_t*)fileContents;

   if ( strncmp( header->magic, GAME_DATA_MAGIC, 4 ) != 0 )
   {
      Platform_FatalError( "game data file has an invalid magic number." );
      return;
   }
   else if ( header->version.major != GAME_VERSION_MAJOR ||
             header->version.minor != GAME_VERSION_MINOR ||
             header->version.maint != GAME_VERSION_MAINT )
   {
      Platform_FatalError( "game data file has an incompatible version." );
   }
}
