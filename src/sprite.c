#include "direction.h"
#include "mem_arena.h"
#include "sprite.h"
#include "sprite_texture_set.h"

struct ActiveSprite_t
{
   ActiveSpriteTextureSet_t* textureSet;
   Direction_t dir;
   u32 frameIndex;

   r32 frameSec;
   r32 frameDurationSec;
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

   sprite->frameSec = 0.0f;
   sprite->frameDurationSec = ACTIVE_SPRITE_FRAME_DURATION_SEC_DEFAULT;

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

ActiveSpriteTextureSet_t* ActiveSprite_GetTextureSet( ActiveSprite_t* activeSprite )
{
    return activeSprite->textureSet;
}

u32 ActiveSprite_GetFrameIndex( ActiveSprite_t* activeSprite )
{
    return activeSprite->frameIndex;
}

r32 ActiveSprite_GetFrameDurationSec( ActiveSprite_t* activeSprite )
{
    return activeSprite->frameDurationSec;
}

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir )
{
    activeSprite->dir = dir;
}

void ActiveSprite_SetFrameIndex( ActiveSprite_t* activeSprite, u32 frameIndex )
{
    activeSprite->frameIndex = frameIndex;
}
void ActiveSprite_SetFrameDurationSec( ActiveSprite_t* activeSprite, r32 frameDurationSec )
{
    activeSprite->frameDurationSec = frameDurationSec;
}

void ActiveSprite_Tic( ActiveSprite_t* activeSprite, r32 deltaSec )
{
    activeSprite->frameSec += deltaSec;

    while( activeSprite->frameSec >= activeSprite->frameDurationSec )
    {
        activeSprite->frameSec -= activeSprite->frameDurationSec;
        activeSprite->frameIndex++;
        if ( activeSprite->frameIndex >= ActiveSpriteTextureSet_GetFrameCount( activeSprite->textureSet ) )
        {
            activeSprite->frameIndex = 0;
        }
    }
}
