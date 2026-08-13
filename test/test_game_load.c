#include "file.h"
#include "game.h"
#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "unity.h"

void setUp( void ) {}
void tearDown( void ) {}

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

void Platform_OpenFile( File_t* file, const char* filePath )
{
   UNUSED_PARAM( file );
   UNUSED_PARAM( filePath );
}

void Platform_ReadFileBytes( File_t* file, u8* buffer, size_t size )
{
   UNUSED_PARAM( file );
   UNUSED_PARAM( buffer );
   UNUSED_PARAM( size );
}

void Platform_FileSeek( File_t* file, i32 offset, i32 origin )
{
   UNUSED_PARAM( file );
   UNUSED_PARAM( offset );
   UNUSED_PARAM( origin );
}

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   UNUSED_PARAM( tileMap );
   UNUSED_PARAM( memArena );
}

int main( void )
{
   UNITY_BEGIN();

   return UNITY_END();
}
