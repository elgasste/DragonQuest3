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

typedef struct ActiveSpriteTextureSet_t ActiveSpriteTextureSet_t;
typedef struct Clock_t Clock_t;
typedef struct Display_t Display_t;
typedef struct Entity_t Entity_t;
typedef struct GameData_t GameData_t;
typedef struct Input_t Input_t;
typedef struct MemArena_t MemArena_t;
typedef struct TileMap_t TileMap_t;
typedef struct TileTextureSet_t TileTextureSet_t;

// game.c
typedef struct Game_t Game_t;
size_t Game_GetStructSize( void );
Game_t* Game_Create( MemArena_t* memArena, const char* gameDataFilePath );
void Game_Free( Game_t* game, MemArena_t* memArena );

Clock_t* Game_GetClock( Game_t* game );
Input_t* Game_GetInput( Game_t* game );
Display_t* Game_GetDisplay( Game_t* game );
GameData_t* Game_GetGameData( Game_t* game );
TileTextureSet_t* Game_GetTileTextureSet( Game_t* game );
ActiveSpriteTextureSet_t* Game_GetActiveSpriteTextureSet( Game_t* game );
TileMap_t* Game_GetTileMap( Game_t* game );
Entity_t* Game_GetPlayerEntity( Game_t* game );

void Game_SetPlayerRect( Game_t* game, Vector4i32_t playerRect );

void Game_Run( Game_t* game );
void Game_Stop( Game_t* game );

// game_input.c
void Game_HandleInput( Game_t* game );

// game_physics.c
void Game_TicPhysics( Game_t* game );

// game_render.c
void Game_Render( Game_t* game );

#endif // GAME_H
