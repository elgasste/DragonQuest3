#if !defined( PLAYER_H )
#define PLAYER_H

#include "common.h"
#include "vector.h"

#define PLAYER_NAME_BUFFER_SIZE  9
#define PLAYER_NAME_MAX_LENGTH  ( PLAYER_NAME_BUFFER_SIZE - 1 )

typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;
typedef struct Entity_t Entity_t;
typedef struct MemArena_t MemArena_t;

typedef struct Player_t Player_t;
size_t Player_GetStructSize( void );
void Player_Init( Player_t* player, MemArena_t* arena, ActiveSpriteTextureSet_t* textureSet, Vector2i32_t size, Vector2i32_t spriteOffset );
void Player_Free( MemArena_t* arena, Player_t* player );

Entity_t* Player_GetEntity( const Player_t* player );
const char* Player_GetName( const Player_t* player );

void Player_SetName( Player_t* player, const char* name );

#endif // PLAYER_H
