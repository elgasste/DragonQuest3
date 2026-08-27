#if !defined( MOCK_TILE_TEXTURE_SET_H )
#define MOCK_TILE_TEXTURE_SET_H

#include "../../include/tile_texture_set.h"

typedef struct TileTextureSet_t
{
   TileTextureSetInfo_t info;
   u32* textures;
}
TileTextureSet_t;

#endif // MOCK_TILE_TEXTURE_SET_H
