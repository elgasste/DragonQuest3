#if !defined( GAME_H )
#define GAME_H

#include "common.h"

// TODO: define these values somewhere else
#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

// 512 MB
#define GAME_MEMORY_SIZE   1024 * 1024 * 512

// TODO: this should probably also go somewhere else
#define GAME_DEFAULT_FPS   60

typedef struct MemArena_t MemArena_t;
typedef struct Clock_t Clock_t;
typedef struct Input_t Input_t;
typedef struct Screen_t Screen_t;

typedef struct Game_t
{
   MemArena_t* memArena;
   Clock_t* clock;
   Input_t* input;
   Screen_t* screen;

   b32 shutdown;
}
Game_t;

// game.c
void Game_Init( Game_t* game, MemArena_t* memArena );
void Game_Run( Game_t* game );
void Game_Stop( Game_t* game );

// game_render.c
void Game_Render( Game_t* game );

#endif // GAME_H
