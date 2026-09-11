#include "entity.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "npc.h"
#include "sprite.h"

struct Npc_t
{
   Entity_t* entity;
   b32 wanders;
};

size_t Npc_GetStructSize( void )
{
   return sizeof( Npc_t );
}

void Npc_LoadFromGameData( Npc_t* npc, MemArena_t *memArena, GameData_t *gameData, i32 gameDataOffset, ActiveSpriteTextureSet_t* textureSet )
{
   NpcInfo_t info;
   File_t* file;
   ActiveSprite_t* sprite;

   file = GameData_GetFile( gameData );
   Platform_FileSeek( file, gameDataOffset, 0 );

   if ( gameDataOffset + (i32)sizeof( NpcInfo_t ) > file->size )
   {
      Platform_FatalError( "game data file is too small to contain NPC info." );
      return;
   }

   Platform_ReadFileBytes( file, (u8*)&info, sizeof( NpcInfo_t ) );

   npc->wanders = info.wanders;

   sprite = ActiveSprite_Create( memArena, textureSet );
   ActiveSprite_SetTextureIndex( sprite, info.spriteTextureIndex );
   ActiveSprite_SetDirection( sprite, info.direction );

   npc->entity = Entity_Create( memArena, sprite );
   Entity_SetTileIndex( npc->entity, info.tileIndex );
   Entity_SetSize( npc->entity, info.w, info.h );
   Entity_SetSpriteOffset( npc->entity, info.spriteOffsetX, info.spriteOffsetY );
}

void Npc_Free( Npc_t* npc, MemArena_t* memArena )
{
   Entity_Free( npc->entity, memArena );
}

Entity_t* Npc_GetEntity( Npc_t* npc )
{
   return npc->entity;
}

b32 Npc_GetWanders( Npc_t* npc )
{
   return npc->wanders;
}

void Npc_SetWanders( Npc_t* npc, b32 wanders )
{
   npc->wanders = wanders;
}
