#if !defined( MOCK_GAME_DATA_H )
#define MOCK_GAME_DATA_H

#include "../../include/game_data.h"

typedef struct GameData_t
{
   File_t* file;
   GameDataVersion_t version;
   GameDataFileOffsets_t fileOffsets;

   GameDataObjectOffset_t* tileMapOffsets;
   u32 tileMapCount;
}
GameData_t;

#endif // MOCK_GAME_DATA_H
