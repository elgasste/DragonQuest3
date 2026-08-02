#if !defined( GAME_H )
#define GAME_H

#include "common.h"

// TODO: define these values somewhere else
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

typedef struct MemArena_t MemArena_t;
typedef struct PixelBuffer_t PixelBuffer_t;

typedef struct Game_t
{
   MemArena_t* memArena;
   PixelBuffer_t* pixelBuffer;
}
Game_t;

void Game_Create( Game_t* game );
void Game_Destroy( Game_t* game );

#endif // GAME_H
