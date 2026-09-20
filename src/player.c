#include <string.h>

#include "sprite.h"
#include "entity.h"
#include "mem_arena.h"
#include "platform.h"
#include "player.h"

struct Player_t
{
   Entity_t* entity;

   char name[PLAYER_NAME_BUFFER_SIZE];
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

void Player_SetName( Player_t* player, const char* name )
{
   if ( strlen( name ) > PLAYER_NAME_MAX_LENGTH )
   {
      Platform_FatalError( "Player name is too long" );
      return;
   }
   
   strcpy_s( player->name, PLAYER_NAME_BUFFER_SIZE, name );
}
