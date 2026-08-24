#if !defined( TILE_MAP_H )
#define TILE_MAP_H

#include "common.h"

typedef struct GameData_t GameData_t;
typedef struct MemArena_t MemArena_t;
typedef struct Tile_t Tile_t;
typedef struct Vector4i32_t Vector4i32_t;

typedef struct TileMapData_t
{
   u32 id;
   u32 tilesX;
   u32 tilesY;
   b32 wraps;
   Tile_t* tiles;
}
TileMapData_t;

typedef struct TileMap_t TileMap_t;
size_t TileMap_GetStructSize( void );
TileMap_t *TileMap_CreateFromGameData(MemArena_t *memArena, GameData_t *gameData, u32 tileMapId);
void TileMap_Free( TileMap_t* tileMap, MemArena_t* memArena );

u32 TileMap_GetId( TileMap_t* tileMap );
u32 TileMap_GetTilesX( TileMap_t* tileMap );
u32 TileMap_GetTilesY( TileMap_t* tileMap );
b32 TileMap_GetWraps( TileMap_t* tileMap );
Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 x, u32 y );

void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, Vector4i32_t* viewport, u32 x, u32 y, u32 tileSize );

#endif // TILE_MAP_H
