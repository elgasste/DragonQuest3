#if !defined( SPRITE_H )
#define SPRITE_H

#include "common.h"

#define ACTIVE_SPRITE_FRAME_DURATION_SEC_DEFAULT      0.5f

typedef struct MemArena_t MemArena_t;
typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;

typedef struct ActiveSprite_t ActiveSprite_t;
size_t ActiveSprite_GetStructSize( void );
ActiveSprite_t* ActiveSprite_Create( MemArena_t* memArena, ActiveSpriteTextureSet_t* textureSet );
void ActiveSprite_Free( ActiveSprite_t* activeSprite, MemArena_t* memArena );

ActiveSpriteTextureSet_t* ActiveSprite_GetTextureSet( ActiveSprite_t* activeSprite );
Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite );
u32 ActiveSprite_GetFrameIndex( ActiveSprite_t* activeSprite );
r32 ActiveSprite_GetFrameDurationSec( ActiveSprite_t* activeSprite );

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir );
void ActiveSprite_SetFrameIndex( ActiveSprite_t* activeSprite, u32 frameIndex );
void ActiveSprite_SetFrameDurationSec( ActiveSprite_t* activeSprite, r32 frameDurationSec );

void ActiveSprite_Tic( ActiveSprite_t* activeSprite, r32 deltaSec );

#endif // SPRITE_H
