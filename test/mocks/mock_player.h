#if !defined( MOCK_PLAYER_H )
#define MOCK_PLAYER_H

#include "../../include/player.h"

typedef struct Entity_t Entity_t;

typedef struct Player_t
{
   Entity_t* entity;
   char name[PLAYER_NAME_BUFFER_SIZE];
}
Player_t;

#endif // MOCK_PLAYER_H
