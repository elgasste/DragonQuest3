#if !defined( GAME_DATA_H )
#define GAME_DATA_H

#include "common.h"
#include "platform.h"

#define GAME_DATA_MAGIC "DW3D"

typedef struct File_t File_t;
typedef struct MemArena_t MemArena_t;

PACKED_STRUCT
typedef struct GameDataVersion_t
{
   u8 major;
   u8 minor;
   u8 maint;
}
GameDataVersion_t;
END_PACKED_STRUCT

PACKED_STRUCT
typedef struct GameDataFileOffsets_t
{
   i32 tileTextureSet;
   i32 activeSpriteTextureSet;
   i32 tileMaps;
}
GameDataFileOffsets_t;
END_PACKED_STRUCT

PACKED_STRUCT
typedef struct GameDataObjectOffset_t
{
   u32 id;
   i32 offset;
}
GameDataObjectOffset_t;
END_PACKED_STRUCT

typedef struct GameData_t GameData_t;
size_t GameData_GetStructSize( void );
GameData_t* GameData_Create( MemArena_t* memArena, const char* filePath );
void GameData_Free( GameData_t* gameData, MemArena_t* memArena );

File_t* GameData_GetFile( GameData_t* gameData );
GameDataVersion_t GameData_GetVersion( GameData_t* gameData );
GameDataFileOffsets_t GameData_GetFileOffsets( GameData_t* gameData );
GameDataObjectOffset_t GameData_GetTileMapOffset( GameData_t* gameData, u32 tileMapId );
u32 GameData_GetTileMapCount( GameData_t* gameData );

void GameData_SetTileMapOffsets( GameData_t* gameData, GameDataObjectOffset_t* offsets, u32 count );

#endif // GAME_DATA_H
