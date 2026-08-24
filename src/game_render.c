#include "display.h"
#include "entity.h"
#include "game.h"
#include "platform.h"

internal void GameRender_DrawPlayer( Game_t* game );

void Game_Render( Game_t* game )
{
   Display_t* display;
   TileTextureSet_t* tileTextureSet;
   TileMap_t* tileMap;
   Vector4i32_t viewportUnits;

   display = Game_GetDisplay( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   tileMap = Game_GetTileMap( game );
   viewportUnits = Game_GetTileMapViewportUnits( game );
   
   Display_Fill( display, 0 );

   // TODO: draw this in the correct place based on the game state
   Display_DrawTileMapViewport( display, tileMap, tileTextureSet, viewportUnits, 0, 0 );

   GameRender_DrawPlayer( game );

   Platform_RenderDisplayBuffer( display );
}

internal void GameRender_DrawPlayer( Game_t* game )
{
   Display_t* display;
   Vector4i32_t viewportUnits;
   Vector4i32_t playerRect;
   Entity_t* playerEntity;

   display = Game_GetDisplay( game );
   viewportUnits = Game_GetTileMapViewportUnits( game );
   playerEntity = Game_GetPlayerEntity( game );
   playerRect = Entity_GetRect( playerEntity );

   // TODO: it seems silly we have to do this for every frame, we should probably store
   // a separate version of this viewport in pixels. same with the player rect.
   viewportUnits.x /= WORLD_UNITS_PER_PIXEL;
   viewportUnits.y /= WORLD_UNITS_PER_PIXEL;
   viewportUnits.w /= WORLD_UNITS_PER_PIXEL;
   viewportUnits.h /= WORLD_UNITS_PER_PIXEL;
   playerRect.x /= WORLD_UNITS_PER_PIXEL;
   playerRect.y /= WORLD_UNITS_PER_PIXEL;
   playerRect.w /= WORLD_UNITS_PER_PIXEL;
   playerRect.h /= WORLD_UNITS_PER_PIXEL;

   Display_DrawRect( display, playerRect.x - viewportUnits.x, playerRect.y - viewportUnits.y, playerRect.w, playerRect.h, 0x00666666u );
}
