#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "tile_texture_set.h"

// TODO: this is all temporary
void Game_HandleInput( Game_t* game )
{
   b32 wraps;
   u32 tilesX, tilesY, tileSize;

   wraps = TileMap_GetWraps( game->tileMap );
   tilesX = TileMap_GetTilesX( game->tileMap );
   tilesY = TileMap_GetTilesY( game->tileMap );
   tileSize = TileTextureSet_GetTileSize( game->tileTextureSet );

   if ( Input_IsButtonDown( game->input, InputButton_Left ) )
   {
      game->playerRect.x -= 1;
      if ( game->playerRect.x < 0 )
      {
         if ( wraps )
            game->playerRect.x = ( tilesX * tileSize ) - 1;
         else
            game->playerRect.x = 0;
      }
   }
   if ( Input_IsButtonDown( game->input, InputButton_Up ) )
   {
      game->playerRect.y -= 1;
      if ( game->playerRect.y < 0 )
      {
         if ( wraps )
            game->playerRect.y = ( tilesY * tileSize ) - 1;
         else
            game->playerRect.y = 0;
      }
   }
   if ( Input_IsButtonDown( game->input, InputButton_Right ) )
   {
      game->playerRect.x += 1;
      if ( game->playerRect.x + game->playerRect.w > (i32)( tilesX * tileSize ) )
      {
         if ( wraps )
         {
            if ( game->playerRect.x > (i32)( tilesX * tileSize ) )
               game->playerRect.x = 0;
         }
         else
            game->playerRect.x = ( tilesX * tileSize ) - game->playerRect.w;
      }
   }
   if ( Input_IsButtonDown( game->input, InputButton_Down ) )
   {
      game->playerRect.y += 1;
      if ( game->playerRect.y + game->playerRect.h > (i32)( tilesY * tileSize ) )
      {
         if ( wraps )
         {
            if ( game->playerRect.y > (i32)( tilesY * tileSize ) )
               game->playerRect.y = 0;
         }
         else
            game->playerRect.y = ( tilesY * tileSize ) - game->playerRect.h;
      }
   }
}