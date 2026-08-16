#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"

void GameData_Cleanup( GameData_t* gameData, MemArena_t* memArena )
{
   Platform_CloseFile( gameData->file );

   MemArena_Free( memArena, gameData->file );
   MemArena_Free( memArena, gameData->tileMapOffsets );
}
