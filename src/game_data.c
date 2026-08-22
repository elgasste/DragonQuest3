#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"

struct GameData_t
{
   File_t* file;
   GameDataVersion_t version;
   GameDataFileOffsets_t offsets;

   GameDataObjectOffset_t* tileMapOffsets;
   u32 tileMapCount;
};

size_t GameData_GetStructSize( void )
{
   return sizeof( GameData_t );
}

GameData_t* GameData_Create( MemArena_t* memArena, const char* filePath )
{
   GameData_t* gameData = (GameData_t*)MemArena_AllocMem( memArena, sizeof( GameData_t ) );

   gameData->file = (File_t*)MemArena_AllocMem( memArena, sizeof( File_t ) );
   Platform_OpenFile(gameData->file, filePath );

   gameData->tileMapOffsets = 0;
   gameData->tileMapCount = 0;

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
   return gameData->offsets;
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

void GameData_SetVersion( GameData_t* gameData, GameDataVersion_t version )
{
   gameData->version = version;
}

void GameData_SetFileOffsets( GameData_t* gameData, GameDataFileOffsets_t offsets )
{
   gameData->offsets = offsets;
}

void GameData_SetTileMapOffsets( GameData_t* gameData, GameDataObjectOffset_t* offsets, u32 count )
{
   gameData->tileMapOffsets = offsets;
   gameData->tileMapCount = count;
}
