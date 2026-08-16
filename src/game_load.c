#include <stdio.h>
#include <string.h>

#include "file.h"
#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "version.h"

PACKED_STRUCT
typedef struct GameDataMetaData_t
{
   char magic[4];
   GameDataVersion_t version;
   i32 tileTextureSetOffset;
   i32 tileMapsOffset;
}
GameDataMetaData_t;
END_PACKED_STRUCT

internal b32 GameData_LoadMetaData( GameData_t* gameData );
internal b32 GameData_LoadTileTextureSet( Game_t* game );
internal b32 GameData_LoadTileMapOffsets( Game_t* game );

void Game_LoadGameData( Game_t* game, const char* gameDataFilePath )
{
   game->gameData = (GameData_t*)MemArena_Alloc( game->memArena, sizeof( GameData_t ) );
   game->gameData->file = (File_t*)MemArena_Alloc( game->memArena, sizeof( File_t ) );

   Platform_OpenFile( game->gameData->file, gameDataFilePath );

   if ( !GameData_LoadMetaData( game->gameData ) ||
        !GameData_LoadTileTextureSet( game ) ||
        !GameData_LoadTileMapOffsets( game ) )
   {
      Platform_FatalError( "failed to load game data." );
      return;
   }
}

void Game_LoadTileMapFromId( Game_t* game, u32 id )
{
   u32 i;
   i32 tileMapOffset, tilesOffset, tileCount;
   char msg[STRING_SIZE_DEFAULT];

   if ( game->tileMap )
   {
      TileMap_Cleanup( game->tileMap, game->memArena );
      MemArena_Free( game->memArena, game->tileMap );
      game->tileMap = 0;
   }

   for ( i = 0; i < game->gameData->tileMapCount; i++ )
   {
      if ( game->gameData->tileMapFileOffsets[i].id == id )
      {
         tileMapOffset = game->gameData->tileMapFileOffsets[i].offset;
         if ( (i32)( tileMapOffset + sizeof( TileMap_t ) ) > game->gameData->file->size )
         {
            Platform_FatalError( "game data file is too small to contain the requested tile map header." );
            return;
         }

         game->tileMap = (TileMap_t*)MemArena_Alloc( game->memArena, sizeof( TileMap_t ) );
         Platform_FileSeek( game->gameData->file, tileMapOffset, 0 );
         Platform_ReadFileBytes( game->gameData->file, (u8*)( game->tileMap ), sizeof( TileMap_t ) );
         game->tileMap->tiles = 0;

         tileCount = (i32)( game->tileMap->tilesX * game->tileMap->tilesY );
         tilesOffset = tileMapOffset + sizeof( TileMap_t );
         if ( tilesOffset + (i32)( tileCount * sizeof( Tile_t ) ) > game->gameData->file->size )
         {
            Platform_FatalError( "game data file is too small to contain all the requested tile map tiles." );
            return;
         }

         game->tileMap->tiles = (Tile_t*)MemArena_Alloc( game->memArena, tileCount * sizeof( Tile_t ) );
         Platform_FileSeek( game->gameData->file, tilesOffset, 0 );
         Platform_ReadFileBytes( game->gameData->file, (u8*)( game->tileMap->tiles ), tileCount * sizeof( Tile_t ) );
         game->tileMap->tileTextureSet = game->tileTextureSet;

         return;
      }
   }

   snprintf( msg, STRING_SIZE_DEFAULT, "failed to load tile map with ID %u: not found in game data file.", id );
   Platform_FatalError( msg );
}

internal b32 GameData_LoadMetaData( GameData_t* gameData )
{
   GameDataMetaData_t metaData;

   if ( gameData->file->size < (i32)sizeof( GameDataMetaData_t ) )
   {
      Platform_FatalError( "game data file is too small to contain meta data." );
      return False;
   }

   Platform_FileSeek( gameData->file, 0, 0 );
   Platform_ReadFileBytes( gameData->file, (u8*)&metaData, sizeof( GameDataMetaData_t ) );

   if ( strncmp( metaData.magic, GAME_DATA_MAGIC, 4 ) != 0 )
   {
      Platform_FatalError( "game data file has an invalid magic number." );
      return False;
   }

   if ( metaData.version.major != GAME_VERSION_MAJOR ||
        metaData.version.minor != GAME_VERSION_MINOR ||
        metaData.version.maint != GAME_VERSION_MAINT )
   {
      Platform_FatalError( "game data file has an incompatible version." );
      return False;
   }
   else if ( metaData.tileTextureSetOffset >= gameData->file->size )
   {
      Platform_FatalError( "game data file has an invalid tile texture set offset." );
      return False;
   }
   else if ( metaData.tileMapsOffset >= gameData->file->size )
   {
      Platform_FatalError( "game data file has an invalid tile maps offset." );
      return False;
   }

   gameData->version = metaData.version;
   gameData->offsets.tileTextureSet = metaData.tileTextureSetOffset;
   gameData->offsets.tileMaps = metaData.tileMapsOffset;
   return True;
}

internal b32 GameData_LoadTileTextureSet( Game_t* game )
{
   i32 tileTextureSetOffset, textureDataOffset, textureDataSize;

   tileTextureSetOffset = game->gameData->offsets.tileTextureSet;
   if ( (i32)( tileTextureSetOffset + sizeof( TileTextureSet_t ) ) > game->gameData->file->size )
   {
      Platform_FatalError( "game data file is too small to contain a valid tile texture set header." );
      return False;
   }

   game->tileTextureSet = (TileTextureSet_t*)MemArena_Alloc( game->memArena, sizeof( TileTextureSet_t ) );
   Platform_FileSeek( game->gameData->file, tileTextureSetOffset, 0 );
   Platform_ReadFileBytes( game->gameData->file, (u8*)( game->tileTextureSet ), sizeof( TileTextureSet_t ) );
   game->tileTextureSet->textures = 0;
   textureDataOffset = tileTextureSetOffset + sizeof( TileTextureSet_t );

   if ( game->tileTextureSet->count > 0 )
   {
      textureDataSize = game->tileTextureSet->count * game->tileTextureSet->tileSize * game->tileTextureSet->tileSize * sizeof( u32 );
      if ( ( textureDataOffset + textureDataSize ) > game->gameData->file->size )
      {
         Platform_FatalError( "game data file is too small to contain all tile textures." );
         return False;
      }

      game->tileTextureSet->textures = (u32*)MemArena_Alloc( game->memArena, textureDataSize );
      Platform_FileSeek( game->gameData->file, textureDataOffset, 0 );
      Platform_ReadFileBytes( game->gameData->file, (u8*)( game->tileTextureSet->textures ), textureDataSize );
   }

   return True;
}

// TODD: should we move all the verification stuff out of here to make loading faster?
// maybe we can have a separate function do a sanity check when the game loads?
internal b32 GameData_LoadTileMapOffsets( Game_t* game )
{
   u32 tileMapOffsetsOffset, i;

   if ( (i32)( game->gameData->offsets.tileMaps + sizeof( u32 ) ) > game->gameData->file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map metadata." );
      return False;
   }

   // first 4 bytes are the number of tile maps
   Platform_FileSeek( game->gameData->file, game->gameData->offsets.tileMaps, 0 );
   Platform_ReadFileBytes( game->gameData->file, (u8*)&( game->gameData->tileMapCount ), sizeof( u32 ) );

   // next series of bytes are a mapping of tile map IDs to file offsets
   tileMapOffsetsOffset = game->gameData->offsets.tileMaps + sizeof( u32 );
   if ( (i32)( tileMapOffsetsOffset + ( game->gameData->tileMapCount * sizeof( GameDataTileMapFileOffset_t ) ) ) > game->gameData->file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map file offsets." );
      return False;
   }

   game->gameData->tileMapFileOffsets = (GameDataTileMapFileOffset_t*)MemArena_Alloc( game->memArena, game->gameData->tileMapCount * sizeof( GameDataTileMapFileOffset_t ) );

   for ( i = 0; i < game->gameData->tileMapCount; i++ )
   {
      Platform_ReadFileBytes( game->gameData->file, (u8*)&( game->gameData->tileMapFileOffsets[i] ), sizeof( GameDataTileMapFileOffset_t ) );

      if ( game->gameData->tileMapFileOffsets[i].offset >= game->gameData->file->size )
      {
         Platform_FatalError( "game data file has an invalid tile map offset." );
         return False;
      }
   }

   return True;
}
