#if !defined( MOCK_TILE_MAP_H )
#define MOCK_TILE_MAP_H

#include "../../include/tile_map.h"

typedef struct Tile_t
{
   u32 textureIndex;
   b32 isPassable;
}
Tile_t;

typedef struct TileMapPortal_t
{
   u32 sourceTileIndex;
   u32 destinationTileMapId;
   u32 destinationTileIndex;
}
TileMapPortal_t;

typedef struct TileMap_t
{
   TileMapInfo_t info;
   Tile_t* tiles;
}
TileMap_t;

#endif // MOCK_TILE_MAP_H
