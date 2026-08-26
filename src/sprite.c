#include "direction.h"
#include "mem_arena.h"
#include "sprite.h"
#include "sprite_texture_set.h"

struct ActiveSprite_t
{
   ActiveSpriteTextureSet_t* textureSet;
   Direction_t dir;
   u32 frameIndex;
};

size_t ActiveSprite_GetStructSize( void )
{
    return sizeof( ActiveSprite_t );
}

ActiveSprite_t* ActiveSprite_Create( MemArena_t* memArena, ActiveSpriteTextureSet_t* textureSet )
{
   ActiveSprite_t* sprite;

    sprite = (ActiveSprite_t*)MemArena_AllocMem( memArena, sizeof( ActiveSprite_t ) );
   sprite->textureSet = textureSet;
   sprite->dir = 0;
   sprite->frameIndex = 0;

   return sprite;
}

void ActiveSprite_Free( ActiveSprite_t* activeSprite, MemArena_t* memArena )
{
    MemArena_FreeMem( memArena, activeSprite );
}

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite )
{
   return activeSprite->dir;
}

u32 ActiveSprite_GetFrameIndex( ActiveSprite_t* activeSprite )
{
    return activeSprite->frameIndex;
}

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir )
{
    activeSprite->dir = dir;
}

void ActiveSprite_SetFrameIndex( ActiveSprite_t* activeSprite, u32 frameIndex )
{
    activeSprite->frameIndex = frameIndex;
}
