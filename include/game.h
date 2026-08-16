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
typedef struct File_t File_t;
typedef struct GameData_t GameData_t;
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
   GameData_t* gameData;

   TileTextureSet_t* tileTextureSet;

   TileMap_t *tileMap;
   Vector4i32_t tileMapViewport;

   // TODO: this is the player, temporarily
   Vector4i32_t playerRect;

   b32 shutdown;
}
Game_t;

// game.c
void Game_Create( Game_t** pGame, MemArena_t* memArena, const char* gameDataFilePath );
void Game_Destroy( Game_t** pGame );
void Game_Run( Game_t* game );
void Game_Stop( Game_t* game );

// game_load.c
void Game_LoadGameData( Game_t* game, const char* gameDataFilePath );
void Game_LoadTileMapFromId( Game_t* game, u32 id );

// game_input.c
void Game_HandleInput( Game_t* game );

// game_render.c
void Game_Render( Game_t* game );

#endif // GAME_H
