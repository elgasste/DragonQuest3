#include "animation.h"
#include "mem_arena.h"
#include "platform.h"

internal void Animation_Tic( Animation_t* anim, r32 deltaTime );

struct Animation_t
{
    AnimationType_t type;
    r32 duration;
    r32 elapsed;

    void (*finishedCallback)( void* callbackData1, void* callbackData2 );
    void* callbackData1;
    void* callbackData2;
};

size_t Animation_GetStructSize( void )
{
    return sizeof( Animation_t );
}

internal void Animation_Tic( Animation_t* anim, r32 deltaTime )
{
   // TODO: handle different animation types
   anim->elapsed += deltaTime;
}

struct AnimationChain_t
{
   Animation_t* animations;
   u32 maxAnimations;
   u32 count;

   u32 curAnimation;
   r32 isRunning;

   void (*finishedCallback)( void* callbackData1, void* callbackData2 );
   void* callbackData1;
   void* callbackData2;
};

size_t AnimationChain_GetStructSize( void )
{
   return sizeof( AnimationChain_t );
}

AnimationChain_t* AnimationChain_Create( MemArena_t* memArena, u32 maxAnimations )
{
    AnimationChain_t* chain = (AnimationChain_t*)MemArena_AllocMem( memArena, sizeof( AnimationChain_t ) );
    chain->animations = (Animation_t*)MemArena_AllocMem( memArena, sizeof( Animation_t ) * maxAnimations );
    chain->maxAnimations = maxAnimations;
    AnimationChain_Reset( chain );
    return chain;
}

void AnimationChain_Free( AnimationChain_t* chain, MemArena_t* memArena )
{
    MemArena_FreeMem( memArena, chain->animations );
    MemArena_FreeMem( memArena, chain );
}

u32 AnimationChain_GetMaxAnimations( AnimationChain_t* chain )
{
    return chain->maxAnimations;
}

u32 AnimationChain_GetCount( AnimationChain_t* chain )
{
    return chain->count;
}

r32 AnimationChain_GetIsRunning( AnimationChain_t* chain )
{
   return chain->isRunning;
}

void AnimationChain_Reset( AnimationChain_t* chain )
{
   chain->count = 0;
   chain->curAnimation = 0;
   chain->isRunning = False;
   chain->finishedCallback = 0;
   chain->callbackData1 = 0;
   chain->callbackData2 = 0;
}

void AnimationChain_Push( AnimationChain_t* chain, AnimationType_t type, r32 duration, void (*finishedCallback)( void* callbackData1, void* callbackData2 ), void* callbackData1, void* callbackData2 )
{
   Animation_t* anim;

   if ( chain->count >= chain->maxAnimations )
   {
      Platform_FatalError( "AnimationChain_Push: Exceeded max animations" );
      return;
   }

   anim = &chain->animations[ chain->count ];
   anim->type = type;
   anim->duration = duration;
   anim->elapsed = 0.0f;
   anim->finishedCallback = finishedCallback;
   anim->callbackData1 = callbackData1;
   anim->callbackData2 = callbackData2;
   chain->count++;
}

void AnimationChain_Start( AnimationChain_t* chain, void (*finishedCallback)( void* callbackData1, void* callbackData2 ), void* callbackData1, void* callbackData2 )
{
   chain->isRunning = True;
   chain->finishedCallback = finishedCallback;
   chain->callbackData1 = callbackData1;
   chain->callbackData2 = callbackData2;
}

void AnimationChain_Tic( AnimationChain_t* chain, r32 deltaTime )
{
   Animation_t* anim;

   if ( !chain->isRunning )
   {
      return;
   }

   if ( chain->curAnimation >= chain->count )
   {
      chain->isRunning = False;
      return;
   }

   anim = &chain->animations[ chain->curAnimation ];
   Animation_Tic( anim, deltaTime );

   if ( anim->elapsed >= anim->duration )
   {
      if ( anim->finishedCallback )
      {
         anim->finishedCallback( anim->callbackData1, anim->callbackData2 );
      }

      chain->curAnimation++;

      if ( chain->curAnimation >= chain->count )
      {
         chain->isRunning = False;
         if ( chain->finishedCallback )
         {
            chain->finishedCallback( chain->callbackData1, chain->callbackData2 );
         }
      }
   }
}
