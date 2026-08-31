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

u32 AnimationChain_GetMaxAnimations( AnimationChain_t* chain );
u32 AnimationChain_GetCount( AnimationChain_t* chain );
r32 AnimationChain_GetIsRunning( AnimationChain_t* chain );

void AnimationChain_Reset( AnimationChain_t* chain );
void AnimationChain_Push( AnimationChain_t* chain, AnimationType_t type, r32 duration, void (*finishedCallback)( void* callbackData1, void* callbackData2 ), void* callbackData1, void* callbackData2 );
void AnimationChain_Start( AnimationChain_t* chain, void (*finishedCallback)( void* callbackData1, void* callbackData2 ), void* callbackData1, void* callbackData2 );
void AnimationChain_Tic( AnimationChain_t* chain, r32 deltaTime );

#endif // ANIMATION_H
