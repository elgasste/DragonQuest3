#if !defined ( TILE_TEXTURE_SET_H )
#define TILE_TEXTURE_SET_H

#include "common.h"

typedef MemArena_t MemArena_t;

typedef struct TileTextureSet_t
{
   u32 count;
   u32 tileSize;
   u32* textures;
}
TileTextureSet_t;

void TileTextureSet_Cleanup( TileTextureSet_t* tileTextureSet, MemArena_t* memArena );

#endif // TILE_TEXTURE_SET_H
