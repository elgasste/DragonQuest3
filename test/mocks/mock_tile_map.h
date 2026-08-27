#if !defined( MOCK_TILE_MAP_H )
#define MOCK_TILE_MAP_H

#include "../../include/tile_map.h"

typedef struct TileMap_t
{
   TileMapInfo_t info;
   Tile_t* tiles;
}
TileMap_t;

#endif // MOCK_TILE_MAP_H
