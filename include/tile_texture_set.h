#if !defined ( TILE_TEXTURE_SET_H )
#define TILE_TEXTURE_SET_H

#include "common.h"

typedef struct GameData_t GameData_t;
typedef struct MemArena_t MemArena_t;

typedef struct TileTextureSet_t TileTextureSet_t;
size_t TileTextureSet_GetStructSize( void );
TileTextureSet_t* TileTextureSet_CreateFromGameData( MemArena_t* memArena, GameData_t* gameData );
void TileTextureSet_Free( TileTextureSet_t* tileTextureSet, MemArena_t* memArena );

u32 TileTextureSet_GetCount( TileTextureSet_t* tileTextureSet );
u32 TileTextureSet_GetTileSize( TileTextureSet_t* tileTextureSet );
u32* TileTextureSet_GetTexture( TileTextureSet_t* tileTextureSet, u32 index );

void TileTextureSet_SetTexture( TileTextureSet_t* tileTextureSet, u32 index, const u32* textureData );

#endif // TILE_TEXTURE_SET_H
