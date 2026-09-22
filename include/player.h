#if !defined( PLAYER_H )
#define PLAYER_H

#include "clock.h"
#include "direction.h"
#include "vector.h"

#define PLAYER_NAME_BUFFER_SIZE     9
#define PLAYER_NAME_MAX_LENGTH      ( PLAYER_NAME_BUFFER_SIZE - 1 )
#define PLAYER_MOVE_HISTORY_SIZE    17

typedef struct PlayerMovement_t
{
   Vector2i32_t newPos;
   Direction_t newDir;
}
PlayerMovement_t;

typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;
typedef struct Entity_t Entity_t;
typedef struct MemArena_t MemArena_t;

typedef struct Player_t Player_t;
size_t Player_GetStructSize( void );
void Player_Init( Player_t* player, MemArena_t* arena, ActiveSpriteTextureSet_t* textureSet, Vector2i32_t size, Vector2i32_t spriteOffset, u32 fps );
void Player_Free( MemArena_t* arena, Player_t* player );

Entity_t* Player_GetEntity( const Player_t* player );
const char* Player_GetName( const Player_t* player );
b32 Player_GetChainNextPlayer( const Player_t* player );
PlayerMovement_t Player_GetMovement( Player_t* player, u32 index );
u32 Player_GetMovementChainIndex( const Player_t* player );

void Player_SetName( Player_t* player, const char* name );
void Player_SetMoveHistoryCountFromFps( Player_t* player, u32 fps );
void Player_SetChainNextPlayer( Player_t* player, b32 chainNextPlayer );

void Player_ResetChaining( Player_t* player );
void Player_AddMovement( Player_t* player, PlayerMovement_t movement );

#endif // PLAYER_H
