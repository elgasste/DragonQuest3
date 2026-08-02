#if !defined( GAME_H )
#define GAME_H

#include "common.h"

// TODO: define these values somewhere else
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

// 512 MB
#define GAME_MEMORY_SIZE   1024 * 1024 * 512

typedef struct MemArena_t MemArena_t;
typedef struct PixelBuffer_t PixelBuffer_t;

typedef struct Game_t
{
   MemArena_t* memArena;
   PixelBuffer_t* pixelBuffer;

   void (*platformMessageHandler)( void );
   void (*platformRender)( void );

   b32 shutdown;
}
Game_t;

void Game_Create( Game_t* game, MemArena_t* memArena, void (*platformMessageHandler)( void ), void (*platformRender)( void ) );
void Game_Destroy( Game_t* game );
void Game_Run( Game_t* game );
void Game_Stop( Game_t* game );

#endif // GAME_H
