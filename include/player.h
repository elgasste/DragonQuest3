#if !defined( PLAYER_H )
#define PLAYER_H

#include "common.h"

#define PLAYER_NAME_BUFFER_SIZE  9
#define PLAYER_NAME_MAX_LENGTH  ( PLAYER_NAME_BUFFER_SIZE - 1 )

typedef struct MemArena_t MemArena_t;

typedef struct Player_t Player_t;
size_t Player_GetStructSize( void );
Player_t* Player_Create( MemArena_t* arena );
void Player_Free( MemArena_t* arena, Player_t* player );

const char* Player_GetName( const Player_t* player );

void Player_SetName( Player_t* player, const char* name );

#endif // PLAYER_H
