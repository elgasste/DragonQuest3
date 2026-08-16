#include "game.h"
#include "platform.h"
#include "display.h"

internal void GameRender_DrawPlayer( Game_t* game );

void Game_Render( Game_t* game )
{
   Display_Fill( game->display, 0 );

   // TODO: draw this in the correct place based on the game state
   Display_DrawTileMapViewport( game->display, game->tileMap, game->tileMapViewport, 0, 0 );

   GameRender_DrawPlayer( game );

   Platform_RenderDisplayBuffer( game->display );
}

internal void GameRender_DrawPlayer( Game_t* game )
{
   Display_DrawRect( game->display, game->playerRect.x - game->tileMapViewport.x, game->playerRect.y - game->tileMapViewport.y, game->playerRect.w, game->playerRect.h, 0x00666666u );
}
