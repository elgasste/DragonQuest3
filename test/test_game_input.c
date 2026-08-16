#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "unity.h"

void setUp( void ) {}
void tearDown( void ) {}

void test_Game_HandleInput_WrapsPlayerRightWhenPlayerRightEdgeReachesMapEdge( void )
{
   Game_t game = { 0 };
   Input_t input;
   TileTextureSet_t textureSet;
   TileMap_t tileMap;

   Input_Init( &input );
   game.input = &input;

   textureSet.count = 1;
   textureSet.tileSize = 16;
   tileMap.tilesX = 3;
   tileMap.tilesY = 3;
   tileMap.wraps = True;
   tileMap.tileTextureSet = &textureSet;

   game.tileMap = &tileMap;
   game.tileTextureSet = &textureSet;
   game.playerRect.x = ( 3 * 16 ) - 1;
   game.playerRect.y = 10;
   game.playerRect.w = 12;
   game.playerRect.h = 14;

   Input_PressButton( &input, InputButton_Right );
   Game_HandleInput( &game );

   TEST_ASSERT_EQUAL( 0, game.playerRect.x );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Game_HandleInput_WrapsPlayerRightWhenPlayerRightEdgeReachesMapEdge );

   return UNITY_END();
}
