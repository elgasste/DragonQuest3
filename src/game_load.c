#include <stdio.h>
#include <string.h>

#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "version.h"

internal b32 GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize );
internal b32 GameData_VerifyTileTextureSet( u8* fileContents, u32 fileSize, u32 tileTextureSetOffset );
internal b32 GameData_LoadTileTextureSet( Game_t* game, u8* fileContents, u32 tileTextureSetOffset );

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
   if ( !GameData_VerifyTileTextureSet( fileContents, fileSize, header->tileTextureSetOffset ) )
   {
      MemArena_Free( game->memArena, fileContents );
      return;
   }

   if ( !GameData_LoadTileTextureSet( game, fileContents, header->tileTextureSetOffset ) )
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

   if ( header->tileTextureSetOffset >= fileSize )
   {
      Platform_FatalError( "game data file has an invalid tile textures offset." );
      return False;
   }

   return True;
}

internal b32 GameData_VerifyTileTextureSet( u8* fileContents, u32 fileSize, u32 tileTextureSetOffset )
{
   u32 expectedTexturesSize;
   TileTextureSet_t *textureSet;

   if ( ( tileTextureSetOffset + sizeof( TileTextureSet_t ) ) > fileSize )
   {
      Platform_FatalError( "game data file is too small to contain a valid tile texture set." );
      return False;
   }

   textureSet = (TileTextureSet_t*)( fileContents + tileTextureSetOffset );

   if ( textureSet->count > 0 )
   {
      expectedTexturesSize = textureSet->count * textureSet->tileSize * textureSet->tileSize * sizeof( u32 );
      if ( ( tileTextureSetOffset + sizeof( TileTextureSet_t ) + expectedTexturesSize ) > fileSize )
      {
         Platform_FatalError( "game data file is too small to contain all tile textures." );
         return False;
      }
   }

   return True;
}

internal b32 GameData_LoadTileTextureSet( Game_t* game, u8* fileContents, u32 tileTextureSetOffset )
{
   u32 i, pixelCount;
   TileTextureSet_t* fileTextureSet;
   u32* fileTextures;
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   fileTextureSet = (TileTextureSet_t*)( fileContents + tileTextureSetOffset );
   pixelCount = fileTextureSet->count * fileTextureSet->tileSize * fileTextureSet->tileSize;

   memArenaResult = MemArena_Alloc( game->memArena, (void**)&( game->tileTextureSet ), sizeof( TileTextureSet_t ) + ( sizeof( u32 ) * pixelCount ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to allocate memory for tile texture set: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return False;
   }

   game->tileTextureSet->count = fileTextureSet->count;
   game->tileTextureSet->tileSize = fileTextureSet->tileSize;
   game->tileTextureSet->textures = (u32*)( game->tileTextureSet + 1 );

   fileTextures = (u32*)( fileContents + tileTextureSetOffset + sizeof( TileTextureSet_t ) );
   for ( i = 0; i < pixelCount; i++ )
   {
      game->tileTextureSet->textures[i] = fileTextures[i];
   }

   return True;
}
