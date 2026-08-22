#include <stdio.h>
#include <string.h>

#include "file.h"
#include "game.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile.h"
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
   game->gameData = (GameData_t*)MemArena_AllocMem( game->memArena, sizeof( GameData_t ) );
   game->gameData->file = (File_t*)MemArena_AllocMem( game->memArena, sizeof( File_t ) );

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
   if ( game->tileMap )
   {
      TileMap_Free( game->tileMap, game->memArena );
      MemArena_FreeMem( game->memArena, game->tileMap );
      game->tileMap = 0;
   }

   game->tileMap = TileMap_CreateFromGameData( game->memArena, game->gameData, id );
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
   game->tileTextureSet = TileTextureSet_CreateFromGameData( game->memArena, game->gameData );
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

   // next series of bytes are a mapping of tile map IDs to chunk offsets
   tileMapOffsetsOffset = game->gameData->offsets.tileMaps + sizeof( u32 );
   if ( (i32)( tileMapOffsetsOffset + ( game->gameData->tileMapCount * sizeof( GameDataTileMapOffset_t ) ) ) > game->gameData->file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map chunk offsets." );
      return False;
   }

   game->gameData->tileMapOffsets = (GameDataTileMapOffset_t*)MemArena_AllocMem( game->memArena, game->gameData->tileMapCount * sizeof( GameDataTileMapOffset_t ) );

   for ( i = 0; i < game->gameData->tileMapCount; i++ )
   {
      Platform_ReadFileBytes( game->gameData->file, (u8*)&( game->gameData->tileMapOffsets[i] ), sizeof( GameDataTileMapOffset_t ) );

      if ( game->gameData->offsets.tileMaps + game->gameData->tileMapOffsets[i].offset >= game->gameData->file->size )
      {
         Platform_FatalError( "game data file has an invalid tile map offset." );
         return False;
      }
   }

   return True;
}
