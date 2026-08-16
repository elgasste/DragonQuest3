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
   i32 tileMaps;
}
GameDataFileOffsets_t;
END_PACKED_STRUCT

PACKED_STRUCT
typedef struct GameDataTileMapOffset_t
{
   u32 id;
   i32 offset;
}
GameDataTileMapOffset_t;
END_PACKED_STRUCT

typedef struct GameData_t
{
   File_t* file;
   GameDataVersion_t version;
   GameDataFileOffsets_t offsets;

   GameDataTileMapOffset_t* tileMapOffsets;
   u32 tileMapCount;
}
GameData_t;

void GameData_Cleanup( GameData_t* gameData, MemArena_t* memArena );

#endif // GAME_DATA_H
