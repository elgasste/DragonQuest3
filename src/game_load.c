#include <stdio.h>
#include <string.h>

#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "version.h"

internal b32 GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize );
internal b32 GameData_VerifyTileTexturesHeader( u8* fileContents, u32 fileSize, u32 tileTexturesHeaderOffset );
internal b32 GameData_LoadTileTextures( Game_t* game, u8* fileContents, u32 tileTexturesHeaderOffset );

void Game_LoadFromFile( Game_t* game, const char* filePath )
{
   u32 fileSize;
   u8 *fileContents;
   GameDataHeader_t* header;

   fileContents = Platform_LoadFileToMemory( filePath, game->memArena, &fileSize );
   if ( !fileContents )
   {
      Platform_FatalError( "failed to load game data file into memory." );
      return;
   }

   if ( !GameData_VerifyHeaderAndVersion( fileContents, fileSize ) )
   {
      MemArena_Free( game->memArena, fileContents );
      return;
   }

   header = (GameDataHeader_t*)fileContents;
   if ( !GameData_VerifyTileTexturesHeader( fileContents, fileSize, header->tileTexturesHeaderOffset ) )
   {
      MemArena_Free( game->memArena, fileContents );
      return;
   }

   if ( !GameData_LoadTileTextures( game, fileContents, header->tileTexturesHeaderOffset ) )
   {
      MemArena_Free( game->memArena, fileContents );
      return;
   }

   MemArena_Free( game->memArena, fileContents );
}

internal b32 GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize )
{
   GameDataHeader_t* header;

   if ( fileSize < sizeof( GameDataHeader_t ) )
   {
      Platform_FatalError( "game data file is too small to contain a valid header." );
      return False;
   }

   header = (GameDataHeader_t*)fileContents;

   if ( strncmp( header->magic, GAME_DATA_MAGIC, 4 ) != 0 )
   {
      Platform_FatalError( "game data file has an invalid magic number." );
      return False;
   }
   else if ( header->version.major != GAME_VERSION_MAJOR ||
             header->version.minor != GAME_VERSION_MINOR ||
             header->version.maint != GAME_VERSION_MAINT )
   {
      Platform_FatalError( "game data file has an incompatible version." );
      return False;
   }

   if ( header->tileTexturesHeaderOffset >= fileSize )
   {
      Platform_FatalError( "game data file has an invalid tile textures offset." );
      return False;
   }

   return True;
}

internal b32 GameData_VerifyTileTexturesHeader( u8* fileContents, u32 fileSize, u32 tileTexturesHeaderOffset )
{
   u32 expectedTexturesSize;
   GameDataTileTexturesHeader_t* tileTexturesHeader;

   if ( ( tileTexturesHeaderOffset + sizeof( GameDataTileTexturesHeader_t ) ) > fileSize )
   {
      Platform_FatalError( "game data file is too small to contain a valid tile textures header." );
      return False;
   }

   tileTexturesHeader = (GameDataTileTexturesHeader_t*)( fileContents + tileTexturesHeaderOffset );

   if ( tileTexturesHeader->texturesOffset >= fileSize )
   {
      Platform_FatalError( "game data file has an invalid textures offset." );
      return False;
   }

   if ( tileTexturesHeader->count > 0 )
   {
      expectedTexturesSize = tileTexturesHeader->count * tileTexturesHeader->tileSize * tileTexturesHeader->tileSize * sizeof( u32 );
      if ( ( tileTexturesHeader->texturesOffset + expectedTexturesSize ) > fileSize )
      {
         Platform_FatalError( "game data file is too small to contain all tile textures." );
         return False;
      }
   }

   return True;
}

internal b32 GameData_LoadTileTextures( Game_t* game, u8* fileContents, u32 tileTexturesHeaderOffset )
{
   GameDataTileTexturesHeader_t* tileTexturesHeader;
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   tileTexturesHeader = (GameDataTileTexturesHeader_t*)( fileContents + tileTexturesHeaderOffset );

   game->tileSize = tileTexturesHeader->tileSize;
   game->tileTextureCount = tileTexturesHeader->count;

   memArenaResult = MemArena_Alloc( game->memArena, (void**)&( game->tileTextures ), tileTexturesHeader->count * tileTexturesHeader->tileSize * tileTexturesHeader->tileSize * sizeof( u32 ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to allocate memory for tile textures: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return False;
   }

   return True;
}
