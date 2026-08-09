#include "game.h"
#include "platform.h"
#include "screen.h"

void Game_Render( Game_t* game )
{
   Screen_Fill( game->screen, 0 );

   // TODO: draw this in the correct place based on the game state
   Screen_DrawTileMapViewport( game->screen, game->currentTileMap, game->tileMapViewport, 0, 0 );

   Platform_RenderScreenBuffer( game->screen );
}
