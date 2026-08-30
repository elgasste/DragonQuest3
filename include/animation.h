#if !defined( ANIMATION_H )
#define ANIMATION_H

#include "common.h"

typedef struct MemArena_t MemArena_t;

typedef enum AnimationType_t
{
   AnimationType_Pause = 0,
   AnimationType_FadeOut,
   AnimationType_FadeIn,

   Animation_Count
}
AnimationType_t;

typedef struct Animation_t Animation_t;
size_t Animation_GetStructSize( void );

typedef struct AnimationChain_t AnimationChain_t;
size_t AnimationChain_GetStructSize( void );
AnimationChain_t* AnimationChain_Create( MemArena_t* memArena, u32 maxAnimations );
void AnimationChain_Free( AnimationChain_t* chain, MemArena_t* memArena );

#endif // ANIMATION_H
