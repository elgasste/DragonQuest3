#include "mem_arena.h"
#include "tile_texture_set.h"

void TileTextureSet_Cleanup( TileTextureSet_t* tileTextureSet, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileTextureSet->textures );
}
