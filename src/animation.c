#include "animation.h"
#include "mem_arena.h"

struct Animation_t
{
    AnimationType_t type;
    r32 duration;
    r32 elapsed;
};

size_t Animation_GetStructSize( void )
{
    return sizeof( Animation_t );
}

struct AnimationChain_t
{
    Animation_t* animations;
    u32 maxAnimations;
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
    return chain;
}

void AnimationChain_Free( AnimationChain_t* chain, MemArena_t* memArena )
{
    MemArena_FreeMem( memArena, chain->animations );
    MemArena_FreeMem( memArena, chain );
}
