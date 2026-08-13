#include "game.h"
#include "platform.h"
#include "display.h"

void Game_Render( Game_t* game )
{
   Display_Fill( game->display, 0 );

   // TODO: draw this in the correct place based on the game state
   Display_DrawTileMapViewport( game->display, game->tileMap, game->tileMapViewport, 0, 0 );

   Platform_RenderDisplayBuffer( game->display );
}
