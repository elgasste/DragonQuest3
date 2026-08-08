#if !defined( GAME_H )
#define GAME_H

#include "common.h"

#define GAME_DATA_MAGIC          "DW3D"

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

typedef struct GameDataVersion_t
{
   u8 major;
   u8 minor;
   u8 maint;
}
GameDataVersion_t;

typedef struct GameDataHeader_t
{
   char magic[4];
   GameDataVersion_t version;
}
GameDataHeader_t;

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

// game_load.c
void Game_LoadFromFile( Game_t* game, const char* filePath );

// game_render.c
void Game_Render( Game_t* game );

#endif // GAME_H
