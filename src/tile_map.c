#include "mem_arena.h"
#include "tile_map.h"

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   MemArena_Free( memArena, tileMap->tiles );
}
