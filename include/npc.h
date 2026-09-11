#if !defined( NPC_H )
#define NPC_H

#include "common.h"
#include "platform.h"

typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;
typedef struct Entity_t Entity_t;
typedef struct GameData_t GameData_t;
typedef struct MemArena_t MemArena_t;

PACKED_STRUCT
typedef struct NpcInfo_t
{
   u32 tileIndex;
   i32 w;
   i32 h;
   i32 spriteOffsetX;
   i32 spriteOffsetY;
   i32 direction;
   u32 spriteTextureIndex;
   b32 wanders;
}
NpcInfo_t;
END_PACKED_STRUCT

typedef struct Npc_t Npc_t;
size_t Npc_GetStructSize( void );
void Npc_LoadFromGameData( Npc_t* npc, MemArena_t *memArena, GameData_t *gameData, i32 gameDataOffset, ActiveSpriteTextureSet_t* textureSet );
void Npc_Free( Npc_t* npc, MemArena_t* memArena );

Entity_t* Npc_GetEntity( Npc_t* npc );
b32 Npc_GetWanders( Npc_t* npc );

void Npc_SetWanders( Npc_t* npc, b32 wanders );

#endif // NPC_H
