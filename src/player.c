#include <string.h>

#include "entity.h"
#include "mem_arena.h"
#include "platform.h"
#include "player.h"
#include "sprite.h"

struct Player_t
{
   Entity_t* entity;

   char name[PLAYER_NAME_BUFFER_SIZE];

   PlayerMovement_t moveHistory[PLAYER_MOVE_HISTORY_SIZE];
   u32 movementChainIndex;
   b32 chainNextPlayer;
};

size_t Player_GetStructSize( void )
{
   return sizeof( Player_t );
}

void Player_Init( Player_t* player, MemArena_t* arena, ActiveSpriteTextureSet_t* textureSet, Vector2i32_t size, Vector2i32_t spriteOffset )
{
   ActiveSprite_t* sprite;

   strcpy_s( player->name, PLAYER_NAME_MAX_LENGTH, "JDoe" );

   sprite = ActiveSprite_Create( arena, textureSet );

   player->entity = Entity_Create( arena, sprite );
   Entity_SetTileIndex( player->entity, 0 );
   Entity_SetPosition( player->entity, 0, 0 );
   Entity_SetVelocity( player->entity, 0, 0 );
   Entity_SetSize( player->entity, size.x, size.y );
   Entity_SetSpriteOffset( player->entity, spriteOffset.x, spriteOffset.y );

   Player_ResetChaining( player );
}

void Player_Free( MemArena_t* arena, Player_t* player )
{
   Entity_Free(  player->entity, arena );
}

Entity_t* Player_GetEntity( const Player_t* player )
{
   return player->entity;
}

const char* Player_GetName( const Player_t* player )
{
   return player->name;
}

b32 Player_GetChainNextPlayer( const Player_t* player )
{
   return player->chainNextPlayer;
}

PlayerMovement_t Player_GetMovement( Player_t* player, u32 index )
{
   return player->moveHistory[index];
}

u32 Player_GetMovementChainIndex( const Player_t* player )
{
   return player->movementChainIndex;
}

void Player_SetName( Player_t* player, const char* name )
{
   if ( strlen( name ) > PLAYER_NAME_MAX_LENGTH )
   {
      Platform_FatalError( "Player name is too long" );
      return;
   }
   
   strcpy_s( player->name, PLAYER_NAME_BUFFER_SIZE, name );
}

void Player_SetChainNextPlayer( Player_t* player, b32 chainNextPlayer )
{
   player->chainNextPlayer = chainNextPlayer;
}

void Player_OffsetMovementHistory( Player_t* player, i32 offsetX, i32 offsetY )
{
   u32 i;

   for ( i = 0; i < PLAYER_MOVE_HISTORY_SIZE; i++ )
   {
      player->moveHistory[i].newPos.x += offsetX;
      player->moveHistory[i].newPos.y += offsetY;
   }
}

void Player_ResetChaining( Player_t* player )
{
   player->chainNextPlayer = False;
   player->movementChainIndex = 0;
}

void Player_AddMovement( Player_t* player, PlayerMovement_t movement )
{
   player->moveHistory[player->movementChainIndex].newPos.x = movement.newPos.x;
   player->moveHistory[player->movementChainIndex].newPos.y = movement.newPos.y;
   player->moveHistory[player->movementChainIndex].newDir = movement.newDir;
   player->movementChainIndex++;

   if ( player->movementChainIndex >= PLAYER_MOVE_HISTORY_SIZE )
   {
      player->chainNextPlayer = True;
      player->movementChainIndex = 0;
   }
}
