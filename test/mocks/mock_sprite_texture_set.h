#if !defined( MOCK_SPRITE_TEXTURE_SET_H )
#define MOCK_SPRITE_TEXTURE_SET_H

#include "../../include/sprite_texture_set.h"

typedef struct ActiveSpriteTextureSet_t
{
   u32 count;
   u32 frameSize;
   u32 frameCount;
   u32* textures;
}
ActiveSpriteTextureSet_t;

#endif // MOCK_SPRITE_TEXTURE_SET_H
