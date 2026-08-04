#include "game.h"
#include "platform_ops.h"
#include "screen.h"

void Game_Render( Game_t* game )
{
   Screen_Fill( game->screen, 0 );
   PlatformOps_RenderScreenBuffer( game->screen );
}
