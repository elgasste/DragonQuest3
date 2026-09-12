#if !defined( MOCK_NPC_H )
#define MOCK_NPC_H

#include "../../include/npc.h"

typedef struct Entity_t Entity_t;

typedef struct Npc_t
{
   Entity_t* entity;
   b32 wanders;
   b32 isWandering;
   b32 isPausing;
   r32 actionSeconds;
   r32 elapsedSeconds;
}
Npc_t;

#endif // MOCK_NPC_H
