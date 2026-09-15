#if !defined( MOCK_PLAYER_H )
#define MOCK_PLAYER_H

#include "../../include/player.h"

typedef struct Player_t
{
   char name[PLAYER_NAME_BUFFER_SIZE];
}
Player_t;

#endif // MOCK_PLAYER_H
