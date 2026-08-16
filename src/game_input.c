#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "tile_texture_set.h"

// TODO: this is all temporary
void Game_HandleInput( Game_t* game )
{
   if ( Input_IsButtonDown( game->input, InputButton_Left ) )
   {
      game->playerRect.x -= 1;
   }
   if ( Input_IsButtonDown( game->input, InputButton_Up ) )
   {
      game->playerRect.y -= 1;
   }
   if ( Input_IsButtonDown( game->input, InputButton_Right ) )
   {
      game->playerRect.x += 1;
   }
   if ( Input_IsButtonDown( game->input, InputButton_Down ) )
   {
      game->playerRect.y += 1;
   }

   if ( game->playerRect.x < 0 )
   {
      if ( game->tileMap->wraps )
      {
         game->playerRect.x = ( game->tileMap->tilesX * game->tileTextureSet->tileSize ) - game->playerRect.w;
      }
      else
      {
         game->playerRect.x = 0;
      }
   }
   if ( game->playerRect.y < 0 )
   {
      if ( game->tileMap->wraps )
      {
         game->playerRect.y = ( game->tileMap->tilesY * game->tileTextureSet->tileSize ) - game->playerRect.h;
      }
      else
      {
         game->playerRect.y = 0;
      }
   }
   if ( game->playerRect.x + game->playerRect.w > (i32)( game->tileMap->tilesX * game->tileTextureSet->tileSize ) )
   {
      if ( game->tileMap->wraps )
      {
         game->playerRect.x = 0;
      }
      else
      {
         game->playerRect.x = ( game->tileMap->tilesX * game->tileTextureSet->tileSize ) - game->playerRect.w;
      }
   }
   if ( game->playerRect.y + game->playerRect.h > (i32)( game->tileMap->tilesY * game->tileTextureSet->tileSize ) )
   {
      if ( game->tileMap->wraps )
      {
         game->playerRect.y = 0;
      }
      else
      {
         game->playerRect.y = ( game->tileMap->tilesY * game->tileTextureSet->tileSize ) - game->playerRect.h;
      }
   }
}