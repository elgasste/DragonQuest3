#include <string.h>

#include "mem_arena.h"
#include "platform.h"
#include "player.h"

struct Player_t
{
   char name[PLAYER_NAME_BUFFER_SIZE];
};

size_t Player_GetStructSize( void )
{
   return sizeof( Player_t );
}

Player_t* Player_Create( MemArena_t* arena )
{
   Player_t* player;

   player = (Player_t*)MemArena_AllocMem( arena, sizeof( Player_t ) );
   strcpy_s( player->name, PLAYER_NAME_MAX_LENGTH, "JDoe" );

   return player;
}

void Player_Free( MemArena_t* arena, Player_t* player )
{
   MemArena_FreeMem( arena, player );
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
