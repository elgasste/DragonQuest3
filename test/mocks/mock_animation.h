#if !defined( MOCK_ANIMATION_H )
#define MOCK_ANIMATION_H

#include "../../include/animation.h"

typedef struct Animation_t
{
   AnimationType_t type;
   r32 duration;
   r32 elapsed;
   void (*finishedCallback)( void* callbackData1, void* callbackData2 );
   void* callbackData1;
   void* callbackData2;
}
Animation_t;

typedef struct AnimationChain_t
{
   Animation_t* animations;
   u32 maxAnimations;
   u32 count;
   u32 curAnimation;
   r32 isRunning;
   void (*finishedCallback)( void* callbackData1, void* callbackData2 );
   void* callbackData1;
   void* callbackData2;
}
AnimationChain_t;

#endif // MOCK_ANIMATION_H
