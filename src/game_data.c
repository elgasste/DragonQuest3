#include "game_data.h"
#include "mem_arena.h"
#include "platform.h"

void GameData_Cleanup( GameData_t* gameData, MemArena_t* memArena )
{
   Platform_CloseFile( gameData->file );

   MemArena_FreeMem( memArena, gameData->file );
   MemArena_FreeMem( memArena, gameData->tileMapOffsets );
}
