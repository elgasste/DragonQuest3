#if !defined( NPC_H )
#define NPC_H

#include "common.h"
#include "direction.h"
#include "platform.h"

#define NPC_MIN_ACTION_SECONDS   1
#define NPC_MAX_ACTION_SECONDS   4
#define NPC_MIN_PAUSE_SECONDS    0.25f
#define NPC_MAX_PAUSE_SECONDS    1.0f
#define NPC_MIN_VELOCITY         ( 8 * WORLD_UNITS_PER_PIXEL )
#define NPC_MAX_VELOCITY         ( 16 * WORLD_UNITS_PER_PIXEL )

typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;
typedef struct Clock_t Clock_t;
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
   Direction_t direction;
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

void Npc_Tic( Npc_t* npc, Clock_t* clock );

#endif // NPC_H
