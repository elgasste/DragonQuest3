#include <string.h>

#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"
#include "version.h"

PACKED_STRUCT
typedef struct GameDataMetaData_t
{
   char magic[4];
   GameDataVersion_t version;
   GameDataFileOffsets_t fileOffsets;
}
GameDataMetaData_t;
END_PACKED_STRUCT

struct GameData_t
{
   File_t* file;
   GameDataVersion_t version;
   GameDataFileOffsets_t fileOffsets;

   GameDataObjectOffset_t* tileMapOffsets;
   u32 tileMapCount;
};

internal b32 GameData_LoadMetaData( GameData_t* gameData );
internal b32 GameData_LoadTileMapOffsets( GameData_t* gameData, MemArena_t* memArena );

size_t GameData_GetStructSize( void )
{
   return sizeof( GameData_t );
}

GameData_t* GameData_Create( MemArena_t* memArena, const char* filePath )
{
   GameData_t* gameData = (GameData_t*)MemArena_AllocMem( memArena, sizeof( GameData_t ) );

   gameData->file = (File_t*)MemArena_AllocMem( memArena, sizeof( File_t ) );
   Platform_OpenFile(gameData->file, filePath );

   if ( !GameData_LoadMetaData( gameData ) ||
        !GameData_LoadTileMapOffsets( gameData, memArena ) )
   {
      Platform_FatalError( "failed to load game data." );
      return 0;
   }

   // gameData->tileMapOffsets = 0;
   // gameData->tileMapCount = 0;

   return gameData;
}

void GameData_Free( GameData_t* gameData, MemArena_t* memArena )
{
   Platform_CloseFile( gameData->file );

   MemArena_FreeMem( memArena, gameData->tileMapOffsets );
   MemArena_FreeMem( memArena, gameData->file );
   MemArena_FreeMem( memArena, gameData );
}

File_t* GameData_GetFile( GameData_t* gameData )
{
   return gameData->file;
}

GameDataVersion_t GameData_GetVersion( GameData_t* gameData )
{
   return gameData->version;
}

GameDataFileOffsets_t GameData_GetFileOffsets( GameData_t* gameData )
{
   return gameData->fileOffsets;
}

GameDataObjectOffset_t GameData_GetTileMapOffset( GameData_t* gameData, u32 tileMapId )
{
   u32 i;
   GameDataObjectOffset_t notFoundOffset;

   for ( i = 0; i < gameData->tileMapCount; i++ )
   {
      if ( gameData->tileMapOffsets[i].id == tileMapId )
      {
         return gameData->tileMapOffsets[i];
      }
   }

   notFoundOffset.id = 0;
   notFoundOffset.offset = -1;
   return notFoundOffset;
}

u32 GameData_GetTileMapCount( GameData_t* gameData )
{
   return gameData->tileMapCount;
}

void GameData_SetTileMapOffsets( GameData_t* gameData, GameDataObjectOffset_t* offsets, u32 count )
{
   gameData->tileMapOffsets = offsets;
   gameData->tileMapCount = count;
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
   else if ( metaData.fileOffsets.tileTextureSet >= file->size )
   {
      Platform_FatalError( "game data file has an invalid tile texture set offset." );
      return False;
   }
   else if ( metaData.fileOffsets.tileMaps >= file->size )
   {
      Platform_FatalError( "game data file has an invalid tile maps offset." );
      return False;
   }

   gameData->version = metaData.version;
   gameData->fileOffsets = metaData.fileOffsets;

   return True;
}

internal b32 GameData_LoadTileMapOffsets( GameData_t* gameData, MemArena_t* memArena )
{
   u32 tileMapOffsetsOffset, tileMapCount, i;

   if ( (i32)( gameData->fileOffsets.tileMaps + sizeof( u32 ) ) > gameData->file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map metadata." );
      return False;
   }

   // first 4 bytes are the number of tile maps
   Platform_FileSeek( gameData->file, gameData->fileOffsets.tileMaps, 0 );
   Platform_ReadFileBytes( gameData->file, (u8*)&( tileMapCount ), sizeof( u32 ) );

   // next series of bytes are a mapping of tile map IDs to chunk offsets
   tileMapOffsetsOffset = gameData->fileOffsets.tileMaps + sizeof( u32 );
   if ( (i32)( tileMapOffsetsOffset + ( tileMapCount * sizeof( GameDataObjectOffset_t ) ) ) > gameData->file->size )
   {
      Platform_FatalError( "game data file is too small to contain tile map chunk offsets." );
      return False;
   }

   gameData->tileMapOffsets = (GameDataObjectOffset_t*)MemArena_AllocMem( memArena, tileMapCount * sizeof( GameDataObjectOffset_t ) );

   for ( i = 0; i < tileMapCount; i++ )
   {
      Platform_ReadFileBytes( gameData->file, (u8*)&( gameData->tileMapOffsets[i] ), sizeof( GameDataObjectOffset_t ) );

      if ( gameData->fileOffsets.tileMaps + gameData->tileMapOffsets[i].offset >= gameData->file->size )
      {
         Platform_FatalError( "game data file has an invalid tile map offset." );
         return False;
      }
   }

   return True;
}
