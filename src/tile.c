#include "mem_arena.h"
#include "platform.h"
#include "tile.h"

PACKED_STRUCT
struct Tile_t
{
   u32 textureIndex;
};
END_PACKED_STRUCT

size_t Tile_GetStructSize( void )
{
   return sizeof( Tile_t );
}

Tile_t* Tile_Create( MemArena_t* memArena )
{
   Tile_t* tile = (Tile_t*)MemArena_AllocMem( memArena, sizeof(Tile_t) );
   tile->textureIndex = 0;
   return tile;
}

void Tile_Free( Tile_t* tile, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tile );
}

u32 Tile_GetTextureIndex( Tile_t* tile )
{
   return tile->textureIndex;
}
