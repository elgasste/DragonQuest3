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
   GameDataFileOffsets_t offsets;
}
GameDataMetaData_t;
END_PACKED_STRUCT

internal b32 GameData_LoadMetaData( GameData_t* gameData );
internal b32 GameData_LoadTileTextureSet( Game_t* game );
internal b32 GameData_LoadTileMapOffsets( Game_t* game );

void Game_LoadGameData( Game_t* game, const char* gameDataFilePath )
{
   game->gameData = GameData_Create( game->memArena, gameDataFilePath );

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
   File_t* file;

   file = GameData_GetFile( gameData );
   if ( file->size < (i32)sizeof( GameDataMetaData_t ) )
   {
      Platform_FatalError( "game data file is too small to contain meta data." );
      return False;
   }

   Platform_FileSeek( file, 0, 0 );
   Platform_ReadFileBytes( file, (u8*)&metaData, sizeof( GameDataMetaData_t ) );

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
   else if ( metaData.offsets.tileTextureSet >= file->size )
   {
      Platform_FatalError( "game data file has an invalid tile texture set offset." );
      return False;
   }
   else if ( metaData.offsets.tileMaps >= file->size )
   {
      Platform_FatalError( "game data file has an invalid tile maps offset." );
      return False;
   }

   GameData_SetVersion( gameData, metaData.version );
   GameData_SetFileOffsets( gameData, metaData.offsets );

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
   u32 tileMapOffsetsOffset, tileMapCount, i;
   GameDataFileOffsets_t fileOffsets;
   GameDataObjectOffset_t* tileMapOffsets;
   File_t* file;

   file = GameData_GetFile( game->gameData );
   fileOffsets = GameData_GetFileOffsets( game->gameData );
   if ( (i32)( fileOffsets.tileMaps + sizeof( u32 ) ) > file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map metadata." );
      return False;
   }

   // first 4 bytes are the number of tile maps
   Platform_FileSeek( file, fileOffsets.tileMaps, 0 );
   Platform_ReadFileBytes( file, (u8*)&( tileMapCount ), sizeof( u32 ) );

   // next series of bytes are a mapping of tile map IDs to chunk offsets
   tileMapOffsetsOffset = fileOffsets.tileMaps + sizeof( u32 );
   if ( (i32)( tileMapOffsetsOffset + ( tileMapCount * sizeof( GameDataObjectOffset_t ) ) ) > file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map chunk offsets." );
      return False;
   }

   tileMapOffsets = (GameDataObjectOffset_t*)MemArena_AllocMem( game->memArena, tileMapCount * sizeof( GameDataObjectOffset_t ) );

   for ( i = 0; i < tileMapCount; i++ )
   {
      Platform_ReadFileBytes( file, (u8*)&( tileMapOffsets[i] ), sizeof( GameDataObjectOffset_t ) );

      if ( fileOffsets.tileMaps + tileMapOffsets[i].offset >= file->size )
      {
         Platform_FatalError( "game data file has an invalid tile map offset." );
         return False;
      }
   }
   
   GameData_SetTileMapOffsets( game->gameData, tileMapOffsets, tileMapCount );

   return True;
}
