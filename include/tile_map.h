#if !defined( TILE_MAP_H )
#define TILE_MAP_H

#include "common.h"

typedef struct TileTextureSet_t
{
   u32 count;
   u32 tileSize;
   u32* textures;
}
TileTextureSet_t;

typedef struct Tile_t
{
   u32 textureIndex;
}
Tile_t;

typedef struct TileMap_t
{
   u32 id;
   u32 w;
   u32 h;
   b32 wraps;
   Tile_t* tiles;
   TileTextureSet_t* tileTextureSet;
}
TileMap_t;

#endif // TILE_MAP_H
