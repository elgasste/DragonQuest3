#if !defined( SPRITE_H )
#define SPRITE_H

#include "common.h"

typedef struct MemArena_t MemArena_t;
typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;

typedef struct ActiveSprite_t ActiveSprite_t;
size_t ActiveSprite_GetStructSize( void );
ActiveSprite_t* ActiveSprite_Create( MemArena_t* memArena, ActiveSpriteTextureSet_t* textureSet );
void ActiveSprite_Free( ActiveSprite_t* activeSprite, MemArena_t* memArena );

Direction_t ActiveSprite_GetDirection( ActiveSprite_t* activeSprite );
u32 ActiveSprite_GetFrameIndex( ActiveSprite_t* activeSprite );

void ActiveSprite_SetDirection( ActiveSprite_t* activeSprite, Direction_t dir );
void ActiveSprite_SetFrameIndex( ActiveSprite_t* activeSprite, u32 frameIndex );

#endif // SPRITE_H
