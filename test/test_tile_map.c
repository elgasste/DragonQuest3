#include <stdio.h>

#include "mem_arena.h"
#include "platform.h"
#include "tile_map.h"
#include "unity.h"
#include "vector.h"

void Platform_FatalError( const char* message )
{
   UNUSED_PARAM( message );
}

void setUp( void ) {}
void tearDown( void ) {}

internal void test_TileMap_Cleanup_FreesAllocatedTileMemory( void )
{
   MemArena_t* arena;
   MemArenaResult_t result;
   TileMap_t tileMap;
   Tile_t* tiles;
   MemArenaStats_t stats;

   result = MemArena_Create( &arena, 4096 );
   TEST_ASSERT_EQUAL( MemArenaResult_Success, result );

   tiles = (Tile_t*)MemArena_Alloc( arena, sizeof( Tile_t ) * 16 );
   TEST_ASSERT_NOT_NULL( tiles );

   tileMap.tiles = tiles;
   TileMap_Cleanup( &tileMap, arena );

   stats = MemArena_GetStats( arena );
   TEST_ASSERT_EQUAL( 0, stats.totalAllocatedBlocks );
   TEST_ASSERT_EQUAL( 0, stats.totalAllocatedSpace );

   MemArena_Destroy( &arena );
}

internal void test_TileMap_AnchorViewportToPoint_ClampsToMapBoundsWhenNotWrapping( void )
{
   TileMap_t tileMap;
   Vector4i32_t viewport;

   tileMap.w = 20;
   tileMap.h = 10;
   tileMap.wraps = False;

   viewport.x = 0;
   viewport.y = 0;
   viewport.w = 5;
   viewport.h = 3;

   TileMap_AnchorViewportToPoint( &tileMap, &viewport, 2, 1 );
   TEST_ASSERT_EQUAL( 0, viewport.x );
   TEST_ASSERT_EQUAL( 0, viewport.y );

   TileMap_AnchorViewportToPoint( &tileMap, &viewport, 30, 20 );
   TEST_ASSERT_EQUAL( 15, viewport.x );
   TEST_ASSERT_EQUAL( 7, viewport.y );
}

internal void test_TileMap_AnchorViewportToPoint_DoesNotClampWhenWrapping( void )
{
   TileMap_t tileMap;
   Vector4i32_t viewport;

   tileMap.w = 20;
   tileMap.h = 10;
   tileMap.wraps = True;

   viewport.x = 0;
   viewport.y = 0;
   viewport.w = 5;
   viewport.h = 3;

   TileMap_AnchorViewportToPoint( &tileMap, &viewport, 30, 20 );
   TEST_ASSERT_EQUAL( 28, viewport.x );
   TEST_ASSERT_EQUAL( 19, viewport.y );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_TileMap_Cleanup_FreesAllocatedTileMemory );
   
   RUN_TEST( test_TileMap_AnchorViewportToPoint_ClampsToMapBoundsWhenNotWrapping );
   RUN_TEST( test_TileMap_AnchorViewportToPoint_DoesNotClampWhenWrapping );

   return UNITY_END();
}
