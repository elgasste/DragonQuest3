#include "mocks/mock_tile.h"

#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "game_data.h"
#include "tile_map.h"
#include "unity.h"
#include "vector.h"

typedef struct TestTileMapData_t
{
   u32 id;
   u32 tilesX;
   u32 tilesY;
   b32 wraps;
   Tile_t* tiles;
}
TestTileMapData_t;

typedef struct TestTileMap_t
{
   TestTileMapData_t data;
   Vector4i32_t viewportUnits;
   Vector4i32_t viewportPixels;
}
TestTileMap_t;

global u8 g_fileData[256];
global size_t g_filePosition;
global File_t g_file;
global u32 g_fatalErrorCount;
global GameDataFileOffsets_t g_fileOffsets;
global GameDataObjectOffset_t g_tileMapOffset;

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   free( mem );
}

u32 Tile_GetStructSize( void )
{
   return sizeof( Tile_t );
}

GameDataFileOffsets_t GameData_GetFileOffsets( GameData_t* gameData )
{
   UNUSED_PARAM( gameData );
   return g_fileOffsets;
}

u32 GameData_GetTileMapCount( GameData_t* gameData )
{
   UNUSED_PARAM( gameData );
   return 1;
}

GameDataObjectOffset_t GameData_GetTileMapOffset( GameData_t* gameData, u32 tileMapId )
{
   UNUSED_PARAM( gameData );
   UNUSED_PARAM( tileMapId );
   return g_tileMapOffset;
}

File_t* GameData_GetFile( GameData_t* gameData )
{
   UNUSED_PARAM( gameData );
   return &g_file;
}

void Platform_FileSeek( File_t* file, i32 offset, i32 origin )
{
   UNUSED_PARAM( file );
   UNUSED_PARAM( origin );
   g_filePosition = (size_t)offset;
}

void Platform_ReadFileBytes( File_t* file, u8* buffer, size_t size )
{
   UNUSED_PARAM( file );
   memcpy( buffer, g_fileData + g_filePosition, size );
   g_filePosition += size;
}

void Platform_FatalError( const char* msg )
{
   UNUSED_PARAM( msg );
   g_fatalErrorCount++;
}

internal void SetUpMapFixture( TestTileMapData_t map, Tile_t* tiles )
{
   memcpy( g_fileData, &map, sizeof( map ) );
   memcpy( g_fileData + sizeof( map ), tiles, map.tilesX * map.tilesY * sizeof( Tile_t ) );
   g_file.size = (i32)( sizeof( map ) + map.tilesX * map.tilesY * sizeof( Tile_t ) );
   g_filePosition = 0;
   g_fatalErrorCount = 0;
   g_fileOffsets.tileMaps = 0;
   g_tileMapOffset.id = map.id;
   g_tileMapOffset.offset = 0;
}

internal TileMap_t* LoadMap( u32 id )
{
   return TileMap_CreateFromGameData( (MemArena_t*)1, (GameData_t*)1, id );
}

void setUp( void )
{
   memset( g_fileData, 0, sizeof( g_fileData ) );
   g_filePosition = 0;
   g_file.size = 0;
   g_fatalErrorCount = 0;
   g_fileOffsets.tileMaps = 0;
   g_tileMapOffset.id = 1;
   g_tileMapOffset.offset = 0;
}

void tearDown( void ) {}

void test_TileMap_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_UINT( 0, TileMap_GetStructSize() );
}

void test_TileMap_CreateFromGameData_LoadsMapAndTiles( void )
{
   Tile_t expectedTiles[4] = { { 1 }, { 2 }, { 3 }, { 4 } };
   TestTileMapData_t map = { 7, 2, 2, False, 0 };
   TileMap_t* tileMap;

   SetUpMapFixture( map, expectedTiles );
   tileMap = LoadMap( 7 );

   TEST_ASSERT_NOT_NULL( tileMap );
   TEST_ASSERT_EQUAL_UINT( 7, TileMap_GetId( tileMap ) );
   TEST_ASSERT_EQUAL_UINT( 2, TileMap_GetTilesX( tileMap ) );
   TEST_ASSERT_EQUAL_UINT( 2, TileMap_GetTilesY( tileMap ) );
   TEST_ASSERT_FALSE( TileMap_GetWraps( tileMap ) );
   TEST_ASSERT_EQUAL_UINT( 1, TileMap_GetTile( tileMap, 0, 0 )->textureIndex );
   TEST_ASSERT_EQUAL_UINT( 4, TileMap_GetTile( tileMap, 1, 1 )->textureIndex );

   TileMap_Free( tileMap, (MemArena_t*)1 );
}

void test_TileMap_GetTile_ReturnsTilesInRowMajorOrder( void )
{
   Tile_t expectedTiles[6] = { { 10 }, { 20 }, { 30 }, { 40 }, { 50 }, { 60 } };
   TestTileMapData_t map = { 3, 3, 2, True, 0 };
   TileMap_t* tileMap;

   SetUpMapFixture( map, expectedTiles );
   tileMap = LoadMap( 3 );

   TEST_ASSERT_EQUAL_UINT( 10, TileMap_GetTile( tileMap, 0, 0 )->textureIndex );
   TEST_ASSERT_EQUAL_UINT( 30, TileMap_GetTile( tileMap, 2, 0 )->textureIndex );
   TEST_ASSERT_EQUAL_UINT( 40, TileMap_GetTile( tileMap, 0, 1 )->textureIndex );
   TEST_ASSERT_EQUAL_UINT( 60, TileMap_GetTile( tileMap, 2, 1 )->textureIndex );

   TileMap_Free( tileMap, (MemArena_t*)1 );
}

void test_TileMap_AnchorViewport_ClampsNonWrappingMapAtTopLeft( void )
{
   TestTileMap_t map = { { 1, 20, 15, False, 0 }, { 0, 0, 160 * WORLD_UNITS_PER_PIXEL, 120 * WORLD_UNITS_PER_PIXEL } };

   TileMap_AnchorViewportToPointUnits( (TileMap_t*)&map, 0, 0, 16 );

   TEST_ASSERT_EQUAL_INT( 0, map.viewportUnits.x );
   TEST_ASSERT_EQUAL_INT( 0, map.viewportUnits.y );
}

void test_TileMap_AnchorViewport_ClampsNonWrappingMapAtBottomRight( void )
{
   TestTileMap_t map = { { 1, 20, 15, False, 0 }, { 0, 0, 160 * WORLD_UNITS_PER_PIXEL, 120 * WORLD_UNITS_PER_PIXEL } };

   TileMap_AnchorViewportToPointUnits( (TileMap_t*)&map, 319 * WORLD_UNITS_PER_PIXEL, 239 * WORLD_UNITS_PER_PIXEL, 16 );

   TEST_ASSERT_EQUAL_INT( 160 * WORLD_UNITS_PER_PIXEL, map.viewportUnits.x );
   TEST_ASSERT_EQUAL_INT( 120 * WORLD_UNITS_PER_PIXEL, map.viewportUnits.y );
}

void test_TileMap_AnchorViewport_CentersSmallNonWrappingMap( void )
{
   TestTileMap_t map = { { 1, 4, 3, False, 0 }, { 0, 0, 160 * WORLD_UNITS_PER_PIXEL, 120 * WORLD_UNITS_PER_PIXEL } };

   TileMap_AnchorViewportToPointUnits( (TileMap_t*)&map, 32 * WORLD_UNITS_PER_PIXEL, 24 * WORLD_UNITS_PER_PIXEL, 16 );

   TEST_ASSERT_EQUAL_INT( -48 * WORLD_UNITS_PER_PIXEL, map.viewportUnits.x );
   TEST_ASSERT_EQUAL_INT( -36 * WORLD_UNITS_PER_PIXEL, map.viewportUnits.y );
}

void test_TileMap_AnchorViewport_AllowsWrappingMapToMoveBeyondEdges( void )
{
   TestTileMap_t map = { { 1, 20, 15, True, 0 }, { 0, 0, 160 * WORLD_UNITS_PER_PIXEL, 120 * WORLD_UNITS_PER_PIXEL } };

   TileMap_AnchorViewportToPointUnits( (TileMap_t*)&map, 0, 0, 16 );

   TEST_ASSERT_EQUAL_INT( -80 * WORLD_UNITS_PER_PIXEL, map.viewportUnits.x );
   TEST_ASSERT_EQUAL_INT( -60 * WORLD_UNITS_PER_PIXEL, map.viewportUnits.y );
}

void test_TileMap_GetViewportPixels_ReturnsStoredValue( void )
{
   TestTileMap_t map = { { 0 }, { 0 }, { 3, 4, 5, 6 } };
   Vector4i32_t viewportPixels;

   viewportPixels = TileMap_GetViewportPixels( (TileMap_t*)&map );

   TEST_ASSERT_EQUAL_INT( 3, viewportPixels.x );
   TEST_ASSERT_EQUAL_INT( 4, viewportPixels.y );
   TEST_ASSERT_EQUAL_INT( 5, viewportPixels.w );
   TEST_ASSERT_EQUAL_INT( 6, viewportPixels.h );
}

void test_TileMap_SetViewportUnits_DerivesViewportPixels( void )
{
   TestTileMap_t map = { { 0 }, { 0 }, { 0 } };
   Vector4i32_t viewportPixels;

   TileMap_SetViewportUnits( (TileMap_t*)&map, (Vector4i32_t){ 10 * WORLD_UNITS_PER_PIXEL, 20 * WORLD_UNITS_PER_PIXEL, 320 * WORLD_UNITS_PER_PIXEL, 240 * WORLD_UNITS_PER_PIXEL } );

   viewportPixels = TileMap_GetViewportPixels( (TileMap_t*)&map );

   TEST_ASSERT_EQUAL_INT( 10, viewportPixels.x );
   TEST_ASSERT_EQUAL_INT( 20, viewportPixels.y );
   TEST_ASSERT_EQUAL_INT( 320, viewportPixels.w );
   TEST_ASSERT_EQUAL_INT( 240, viewportPixels.h );
}

void test_TileMap_SetViewportPixels_DerivesViewportUnits( void )
{
   TestTileMap_t map = { { 0 }, { 0 }, { 0 } };
   Vector4i32_t viewportUnits;

   TileMap_SetViewportPixels( (TileMap_t*)&map, (Vector4i32_t){ 10, 20, 320, 240 } );

   viewportUnits = TileMap_GetViewportUnits( (TileMap_t*)&map );

   TEST_ASSERT_EQUAL_INT( 10 * WORLD_UNITS_PER_PIXEL, viewportUnits.x );
   TEST_ASSERT_EQUAL_INT( 20 * WORLD_UNITS_PER_PIXEL, viewportUnits.y );
   TEST_ASSERT_EQUAL_INT( 320 * WORLD_UNITS_PER_PIXEL, viewportUnits.w );
   TEST_ASSERT_EQUAL_INT( 240 * WORLD_UNITS_PER_PIXEL, viewportUnits.h );
}

void test_TileMap_AnchorViewport_UpdatesViewportPixels( void )
{
   TestTileMap_t map = { { 1, 20, 15, False, 0 }, { 0, 0, 160 * WORLD_UNITS_PER_PIXEL, 120 * WORLD_UNITS_PER_PIXEL }, { 0 } };

   TileMap_AnchorViewportToPointUnits( (TileMap_t*)&map, 319 * WORLD_UNITS_PER_PIXEL, 239 * WORLD_UNITS_PER_PIXEL, 16 );

   TEST_ASSERT_EQUAL_INT( 160, map.viewportPixels.x );
   TEST_ASSERT_EQUAL_INT( 120, map.viewportPixels.y );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_TileMap_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_TileMap_CreateFromGameData_LoadsMapAndTiles );

   RUN_TEST( test_TileMap_GetTile_ReturnsTilesInRowMajorOrder );
   
   RUN_TEST( test_TileMap_AnchorViewport_ClampsNonWrappingMapAtTopLeft );
   RUN_TEST( test_TileMap_AnchorViewport_ClampsNonWrappingMapAtBottomRight );
   RUN_TEST( test_TileMap_AnchorViewport_CentersSmallNonWrappingMap );
   RUN_TEST( test_TileMap_AnchorViewport_AllowsWrappingMapToMoveBeyondEdges );

   RUN_TEST( test_TileMap_GetViewportPixels_ReturnsStoredValue );
   RUN_TEST( test_TileMap_SetViewportUnits_DerivesViewportPixels );
   RUN_TEST( test_TileMap_SetViewportPixels_DerivesViewportUnits );
   RUN_TEST( test_TileMap_AnchorViewport_UpdatesViewportPixels );

   return UNITY_END();
}
