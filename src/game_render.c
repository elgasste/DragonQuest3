#include "display.h"
#include "entity.h"
#include "game.h"
#include "platform.h"
#include "tile_map.h"

internal void GameRender_DrawPlayer( Game_t* game );

void Game_Render( Game_t* game )
{
   Display_t* display;
   TileTextureSet_t* tileTextureSet;
   TileMap_t* tileMap;

   display = Game_GetDisplay( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   tileMap = Game_GetTileMap( game );
   
   Display_Fill( display, 0 );

   // TODO: draw this in the correct place based on the game state
   Display_DrawTileMapViewport( display, tileMap, tileTextureSet, 0, 0 );

   GameRender_DrawPlayer( game );

   Platform_RenderDisplayBuffer( display );
}

internal void GameRender_DrawPlayer( Game_t* game )
{
   Display_t* display;
   TileMap_t* tileMap;
   Vector4i32_t viewportInPixels;
   Vector4i32_t playerRect;
   Entity_t* playerEntity;

   display = Game_GetDisplay( game );
   tileMap = Game_GetTileMap( game );
   viewportInPixels = TileMap_GetViewportInPixels( tileMap );
   playerEntity = Game_GetPlayerEntity( game );
   playerRect = Entity_GetRect( playerEntity );

   playerRect.x /= WORLD_UNITS_PER_PIXEL;
   playerRect.y /= WORLD_UNITS_PER_PIXEL;
   playerRect.w /= WORLD_UNITS_PER_PIXEL;
   playerRect.h /= WORLD_UNITS_PER_PIXEL;

   Display_DrawRect( display, playerRect.x - viewportInPixels.x, playerRect.y - viewportInPixels.y, playerRect.w, playerRect.h, 0x00990000u );
}
