#if !defined( TILE_MAP_H )
#define TILE_MAP_H

#include "common.h"

typedef struct MemArena_t MemArena_t;
typedef struct TileTextureSet_t TileTextureSet_t;
typedef struct Vector4i32_t Vector4i32_t;

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

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena );
void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, Vector4i32_t* viewport, u32 x, u32 y );

#endif // TILE_MAP_H
