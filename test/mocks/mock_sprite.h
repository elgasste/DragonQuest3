#if !defined( MOCK_SPRITE_H )
#define MOCK_SPRITE_H

#include "../../include/sprite.h"

struct ActiveSprite_t
{
   ActiveSpriteTextureSet_t* textureSet;
   Direction_t dir;
   u32 textureIndex;
};

#endif // MOCK_SPRITE_H
