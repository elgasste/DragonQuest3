#if !defined( TILE_MAP_H )
#define TILE_MAP_H

#include "common.h"
#include "platform.h"
#include "vector.h"
#include "direction.h"

typedef struct Entity_t Entity_t;
typedef struct GameData_t GameData_t;
typedef struct MemArena_t MemArena_t;

typedef struct Tile_t Tile_t;
size_t Tile_GetStructSize( void );

u32 Tile_GetTextureIndex( Tile_t* tile );
b32 Tile_GetIsPassable( Tile_t* tile );

void Tile_SetTextureIndex( Tile_t* tile, u32 textureIndex );
void Tile_SetIsPassable( Tile_t* tile, b32 isPassable );

typedef struct TileMapPortal_t TileMapPortal_t;
size_t TileMapPortal_GetStructSize( void );

u32 TileMapPortal_GetSourceTileIndex( TileMapPortal_t* portal );
u32 TileMapPortal_GetDestinationTileMapId( TileMapPortal_t* portal );
u32 TileMapPortal_GetDestinationTileIndex( TileMapPortal_t* portal );
Direction_t TileMapPortal_GetDestinationDir( TileMapPortal_t* portal );

PACKED_STRUCT
typedef struct TileMapInfo_t
{
   u32 id;
   u32 tilesX;
   u32 tilesY;
   b32 wraps;
   u32 portalCount;
}
TileMapInfo_t;
END_PACKED_STRUCT

typedef struct TileMap_t TileMap_t;
size_t TileMap_GetStructSize( void );
TileMap_t *TileMap_CreateFromGameData( MemArena_t *memArena, GameData_t *gameData, u32 tileMapId, u32 tileSizePixels );
void TileMap_Free( TileMap_t* tileMap, MemArena_t* memArena );

u32 TileMap_GetId( TileMap_t* tileMap );
u32 TileMap_GetTilesX( TileMap_t* tileMap );
u32 TileMap_GetTilesY( TileMap_t* tileMap );
b32 TileMap_GetWraps( TileMap_t* tileMap );
u32 TileMap_GetPortalCount( TileMap_t* tileMap );
Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 tileIndex );
TileMapPortal_t* TileMap_GetPortal( TileMap_t* tileMap, u32 tileIndex );
Vector4i32_t TileMap_GetViewportInUnits( TileMap_t* tileMap );
Vector4i32_t TileMap_GetViewportInPixels( TileMap_t* tileMap );

void TileMap_SetViewportInUnits( TileMap_t* tileMap, Vector4i32_t viewportInUnits );
void TileMap_SetViewportInPixels( TileMap_t* tileMap, Vector4i32_t viewportInPixels );

void TileMap_AnchorViewportToPointUnits( TileMap_t* tileMap, u32 x, u32 y );
void TileMap_AnchorViewportToEntity( TileMap_t* tileMap, Entity_t* entity );
u32 TileMap_GetTileIndexForEntity( TileMap_t* tileMap, Entity_t* entity );
void TileMap_WrapEntityPosition( TileMap_t* tileMap, Entity_t* entity );
void TileMap_CenterEntityInTile( TileMap_t* tileMap, Entity_t* entity, u32 tileIndex );

#endif // TILE_MAP_H
