#if !defined( GAME_H )
#define GAME_H

#include "common.h"
#include "vector.h"

// TODO: define these values somewhere else
#define DISPLAY_WIDTH    320
#define DISPLAY_HEIGHT   240

// 32 MB
#define GAME_MEMORY_SIZE   ( 1024 * 1024 * 32 )

// TODO: this should probably also go somewhere else
#define GAME_DEFAULT_FPS   60

typedef struct MemArena_t MemArena_t;
typedef struct Clock_t Clock_t;
typedef struct Input_t Input_t;
typedef struct Display_t Display_t;
typedef struct TileMap_t TileMap_t;
typedef struct TileTextureSet_t TileTextureSet_t;

typedef struct Game_t
{
   MemArena_t* memArena;
   Clock_t* clock;
   Input_t* input;
   Display_t* display;

   TileTextureSet_t* tileTextureSet;

   TileMap_t *tileMaps;
   u32 tileMapCount;
   TileMap_t* currentTileMap;
   Vector4i32_t tileMapViewport;

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
