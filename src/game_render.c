#include "display.h"
#include "game.h"
#include "platform.h"

internal void GameRender_DrawPlayer( Game_t* game );

void Game_Render( Game_t* game )
{
   Display_t* display;
   TileTextureSet_t* tileTextureSet;
   TileMap_t* tileMap;
   Vector4i32_t viewport;

   display = Game_GetDisplay( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   tileMap = Game_GetTileMap( game );
   viewport = Game_GetTileMapViewport( game );
   
   Display_Fill( display, 0 );

   // TODO: draw this in the correct place based on the game state
   Display_DrawTileMapViewport( display, tileMap, tileTextureSet, viewport, 0, 0 );

   GameRender_DrawPlayer( game );

   Platform_RenderDisplayBuffer( display );
}

internal void GameRender_DrawPlayer( Game_t* game )
{
   Display_t* display;
   Vector4i32_t viewport;
   Vector4i32_t playerRect;

   display = Game_GetDisplay( game );
   viewport = Game_GetTileMapViewport( game );
   playerRect = Game_GetPlayerRect( game );

   Display_DrawRect( display, playerRect.x - viewport.x, playerRect.y - viewport.y, playerRect.w, playerRect.h, 0x00666666u );
}
