#include "game.h"
#include "platform.h"
#include "screen.h"

void Game_Render( Game_t* game )
{
   Screen_Fill( game->screen, 0 );
   Platform_RenderScreenBuffer( game->screen );
}
