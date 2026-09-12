#include "clock.h"
#include "entity.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "npc.h"
#include "platform.h"
#include "sprite.h"

internal void Npc_SetActionSeconds( Npc_t* npc );
internal void Npc_SetPauseSeconds( Npc_t* npc );
internal void Npc_StartWandering( Npc_t* npc );

struct Npc_t
{
   Entity_t* entity;
   b32 wanders;
   b32 isWandering;
   b32 isPausing;

   r32 actionSeconds;
   r32 elapsedSeconds;
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
   Entity_SetPosition( npc->entity, 0, 0 );
   Entity_SetVelocity( npc->entity, 0, 0 );
   Entity_SetSize( npc->entity, info.w, info.h );
   Entity_SetSpriteOffset( npc->entity, info.spriteOffsetX, info.spriteOffsetY );

   npc->elapsedSeconds = 0.0f;
   npc->isWandering = False;
   npc->isPausing = False;
   if ( npc->wanders )
   {
      npc->isWandering = Platform_Rand_u32Ranged( 0, 1 ) ? True : False;
      Npc_SetActionSeconds( npc );
   }
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

void Npc_Tic( Npc_t* npc, Clock_t* clock )
{
   r32 deltaSec;
   Entity_t* entity;
   ActiveSprite_t* sprite;

   entity = Npc_GetEntity( npc );
   sprite = Entity_GetSprite( entity );
   deltaSec = Clock_GetFrameSec( clock );
   ActiveSprite_Tic( sprite, deltaSec );

   if ( !npc->wanders )
   {
      return;
   }

   npc->elapsedSeconds += deltaSec;

   if ( npc->elapsedSeconds > npc->actionSeconds )
   {
      if ( npc->isWandering )
      {
         Entity_SetVelocity( entity, 0, 0 );
         npc->isWandering = False;
         npc->isPausing = True;
         Npc_SetPauseSeconds( npc );
      }
      else if ( npc->isPausing )
      {
         npc->isPausing = False;
         Npc_StartWandering( npc );
      }
      else
      {
         Npc_StartWandering( npc );
      }

      npc->elapsedSeconds = 0.0f;
   }
}

internal void Npc_SetActionSeconds( Npc_t* npc )
{
   npc->actionSeconds = Platform_Rand_u32Ranged( NPC_MIN_ACTION_SECONDS * 1000, NPC_MAX_ACTION_SECONDS * 1000 ) / 1000.0f;
}

internal void Npc_SetPauseSeconds( Npc_t* npc )
{
   npc->actionSeconds = (r32)Platform_Rand_u32Ranged( (u32)( NPC_MIN_PAUSE_SECONDS * 1000 ), (u32)( NPC_MAX_PAUSE_SECONDS * 1000 ) ) / 1000.0f;
}

internal void Npc_StartWandering( Npc_t* npc )
{
   i32 vx, vy;
   b32 movingLeft, movingUp, movingRight, movingDown;
   Vector2i32_t velocity;
   ActiveSprite_t* sprite;
   Direction_t dir;
   Entity_t* entity;

   entity = Npc_GetEntity( npc );
   velocity = Entity_GetVelocity( entity );
   sprite = Entity_GetSprite( entity );
   vx = Platform_Rand_i32Ranged( NPC_MIN_VELOCITY, NPC_MAX_VELOCITY );
   vy = Platform_Rand_i32Ranged( NPC_MIN_VELOCITY, NPC_MAX_VELOCITY );
   movingUp = False;
   movingDown = False;
   movingLeft = False;
   movingRight = False;

   if ( Platform_Rand_u32Ranged( 0, 1 ) == 0 )
   {
      Entity_SetVelocityX( entity, ( Platform_Rand_u32Ranged( 0, 1 ) == 0 ) ? -vx : vx );
      movingLeft = velocity.x < 0;
      movingRight = velocity.x > 0;
   }

   if ( Platform_Rand_u32Ranged( 0, 1 ) == 0 )
   {
      Entity_SetVelocityY( entity, ( Platform_Rand_u32Ranged( 0, 1 ) == 0 ) ? -vy : vy );
      velocity = Entity_GetVelocity( entity );
      movingUp = velocity.y < 0;
      movingDown = velocity.y > 0;
   }

   if ( movingLeft || movingUp || movingRight || movingDown )
   {
      dir = ActiveSprite_GetDirection( sprite );
      if ( dir == Direction_Left && !movingLeft )
      {
         ActiveSprite_SetDirection( sprite, movingUp ? Direction_Up : movingRight ? Direction_Right : Direction_Down );
      }
      else if ( dir == Direction_Up && !movingUp )
      {
         ActiveSprite_SetDirection( sprite, movingRight ? Direction_Right : movingDown ? Direction_Down : Direction_Left );
      }
      else if ( dir == Direction_Right && !movingRight )
      {
         ActiveSprite_SetDirection( sprite, movingDown ? Direction_Down : movingLeft ? Direction_Left : Direction_Up );
      }
      else if ( dir == Direction_Down && !movingDown )
      {
         ActiveSprite_SetDirection( sprite, movingLeft ? Direction_Left : movingUp ? Direction_Up : Direction_Right );
      }
   }

   npc->isWandering = True;
   Npc_SetActionSeconds( npc );
}
