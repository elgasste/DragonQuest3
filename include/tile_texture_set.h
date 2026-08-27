#if !defined ( TILE_TEXTURE_SET_H )
#define TILE_TEXTURE_SET_H

#include "common.h"
#include "platform.h"

typedef struct GameData_t GameData_t;
typedef struct MemArena_t MemArena_t;

PACKED_STRUCT
typedef struct TileTextureSetInfo_t
{
   u32 count;
   u32 tileSize;
}
TileTextureSetInfo_t;
END_PACKED_STRUCT

typedef struct TileTextureSet_t TileTextureSet_t;
size_t TileTextureSet_GetStructSize( void );
TileTextureSet_t* TileTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData );
void TileTextureSet_Free( TileTextureSet_t* tileTextureSet, MemArena_t* memArena );

u32 TileTextureSet_GetCount( TileTextureSet_t* tileTextureSet );
u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet );
u32* TileTextureSet_GetTexture( TileTextureSet_t* tileTextureSet, u32 index );

#endif // TILE_TEXTURE_SET_H
