#include <stdio.h>
#include <string.h>

#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "version.h"

internal b32 GameData_VerifyHeaderAndVersion( u8* fileContents, u32 fileSize );
internal b32 GameData_LoadTileTextureSet( Game_t* game, u8* fileContents, u32 fileSize, u32 tileTextureSetOffset );
internal b32 GameData_LoadTileMaps( Game_t* game, u8* fileContents, u32 fileSize, u32 tileMapsOffset );

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

   if ( !GameData_LoadTileTextureSet( game, fileContents, fileSize, header->tileTextureSetOffset ) )
   {
      MemArena_Free( game->memArena, fileContents );
      return;
   }

   if ( !GameData_LoadTileMaps( game, fileContents, fileSize, header->tileMapsOffset ) )
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

internal b32 GameData_LoadTileTextureSet( Game_t* game, u8* fileContents, u32 fileSize, u32 tileTextureSetOffset )
{
   u32 i, pixelCount, expectedTexturesSize;
   TileTextureSet_t* fileTextureSet;
   u32* fileTextures;
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   if ( ( tileTextureSetOffset + sizeof( TileTextureSet_t ) ) > fileSize )
   {
      Platform_FatalError( "game data file is too small to contain a valid tile texture set." );
      return False;
   }

   fileTextureSet = (TileTextureSet_t*)( fileContents + tileTextureSetOffset );
   pixelCount = fileTextureSet->count * fileTextureSet->tileSize * fileTextureSet->tileSize;

   if ( fileTextureSet->count > 0 )
   {
      expectedTexturesSize = pixelCount * sizeof( u32 );
      if ( ( tileTextureSetOffset + sizeof( TileTextureSet_t ) + expectedTexturesSize ) > fileSize )
      {
         Platform_FatalError( "game data file is too small to contain all tile textures." );
         return False;
      }
   }

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

internal b32 GameData_LoadTileMaps( Game_t* game, u8* fileContents, u32 fileSize, u32 tileMapsOffset )
{
   u32 i;
   size_t totalTileMapsSize;
   u8 *filePos;
   TileMap_t *fileTileMap;
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   if ( ( tileMapsOffset + sizeof( u32 ) ) > fileSize )
   {
      Platform_FatalError( "game data file is too small to contain any tile maps." );
      return False;
   }

   game->tileMapCount = *(u32*)( fileContents + tileMapsOffset );
   filePos = fileContents + tileMapsOffset + sizeof( u32 );
   totalTileMapsSize = 0;

   // first pass: calculate how much space we need to allocate for all the tile maps
   for ( i = 0; i < game->tileMapCount; i++ )
   {
      if ( ( filePos + sizeof( TileMap_t ) ) > fileContents + fileSize )
      {
         Platform_FatalError( "game data file is too small to contain all tile maps." );
         return False;
      }

      fileTileMap = (TileMap_t*)filePos;
      totalTileMapsSize += sizeof( TileMap_t );
      filePos += sizeof( TileMap_t );

      if ( ( filePos + ( sizeof( Tile_t ) * fileTileMap->w * fileTileMap->h ) ) > fileContents + fileSize )
      {
         Platform_FatalError( "game data file is too small to contain all tiles for a tile map." );
         return False;
      }

      totalTileMapsSize += sizeof( Tile_t ) * fileTileMap->w * fileTileMap->h;
      filePos += sizeof( Tile_t ) * fileTileMap->w * fileTileMap->h;
   }

   // second pass: allocate memory
   memArenaResult = MemArena_Alloc( game->memArena, (void**)&( game->tileMaps ), sizeof( TileMap_t ) * game->tileMapCount );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to allocate memory for tile maps: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return False;
   }

   filePos = fileContents + tileMapsOffset + sizeof( u32 );
   for ( i = 0; i < game->tileMapCount; i++ )
   {
      fileTileMap = (TileMap_t*)filePos;
      game->tileMaps[i].id = fileTileMap->id;
      game->tileMaps[i].w = fileTileMap->w;
      game->tileMaps[i].h = fileTileMap->h;
      game->tileMaps[i].tileTextureSet = game->tileTextureSet;

      filePos += sizeof( TileMap_t );

      size_t tileCount = fileTileMap->w * fileTileMap->h;
      game->tileMaps[i].tiles = (Tile_t*)filePos;

      filePos += sizeof( Tile_t ) * tileCount;
   }

   return True;
}
