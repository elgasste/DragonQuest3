#include "clock.h"
#include "display.h"
#include "game.h"
#include "input.h"
#include "mem_arena.h"
#include "platform.h"
#include "unity.h"

void setUp( void ) {}
void tearDown( void ) {}

void Clock_Init( Clock_t* clock, u32 fps )
{
   UNUSED_PARAM( clock );
   UNUSED_PARAM( fps );
}

void Clock_StartFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );
}

void Clock_EndFrame( Clock_t* clock )
{
   UNUSED_PARAM( clock );
}

void Display_Init( Display_t* display, MemArena_t* memArena, u32 w, u32 h )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( memArena );
   UNUSED_PARAM( w );
   UNUSED_PARAM( h );
}

void Display_Cleanup( Display_t* display, MemArena_t* memArena )
{
   UNUSED_PARAM( display );
   UNUSED_PARAM( memArena );
}

void Game_LoadGameData( Game_t* game, const char* gameDataFilePath )
{
   UNUSED_PARAM( game );
   UNUSED_PARAM( gameDataFilePath );
}

void GameData_Cleanup( GameData_t* gameData, MemArena_t* memArena )
{
   UNUSED_PARAM( gameData );
   UNUSED_PARAM( memArena );
}

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( memArena );
}

void TileTextureSet_Cleanup( TileTextureSet_t* tileTextureSet, MemArena_t* memArena )
{
   UNUSED_PARAM( tileTextureSet );
   UNUSED_PARAM( memArena );
}

void Game_LoadTileMapFromId( Game_t* game, u32 id )
{
   UNUSED_PARAM( game );
   UNUSED_PARAM( id );
}

void Game_Render( Game_t* game )
{
   UNUSED_PARAM( game );
}

void Input_Init( Input_t* input )
{
   UNUSED_PARAM( input );
}

void Input_ResetPressStates( Input_t* input )
{
   UNUSED_PARAM( input );
}

const char* MemArena_GetErrorMessage( MemArenaResult_t result )
{
   UNUSED_PARAM( result );
   return "stubbed memory arena error";
}

MemArenaResult_t MemArena_Alloc( MemArena_t* arena, void** user, size_t size )
{
   UNUSED_PARAM( arena );
   UNUSED_PARAM( user );
   UNUSED_PARAM( size );
   return MemArenaResult_OutOfMemory;
}

void MemArena_Free( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   UNUSED_PARAM( mem );
}

void Platform_FatalError( const char* message )
{
   UNUSED_PARAM( message );
}

void Platform_HandleMessages( Game_t* game )
{
   UNUSED_PARAM( game );
}

void Platform_CloseFile( File_t* file )
{
   UNUSED_PARAM( file );
}

int main( void )
{
   UNITY_BEGIN();

   return UNITY_END();
}
