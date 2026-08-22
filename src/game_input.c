#include "game.h"
#include "input.h"
#include "tile_map.h"
#include "tile_texture_set.h"

// TODO: this is all temporary
void Game_HandleInput( Game_t* game )
{
   b32 wraps;
   u32 tilesX, tilesY, tileSize;
   Input_t* input;
   TileTextureSet_t* tileTextureSet;
   TileMap_t* tileMap;
   Vector4i32_t playerRect;

   input = Game_GetInput( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   tileMap = Game_GetTileMap( game );
   playerRect = Game_GetPlayerRect( game );

   wraps = TileMap_GetWraps( tileMap );
   tilesX = TileMap_GetTilesX( tileMap );
   tilesY = TileMap_GetTilesY( tileMap );
   tileSize = TileTextureSet_GetTileSize( tileTextureSet );

   if ( Input_IsButtonDown( input, InputButton_Left ) )
   {
      playerRect.x -= 1;
      if ( playerRect.x < 0 )
      {
         if ( wraps )
            playerRect.x = ( tilesX * tileSize ) - 1;
         else
            playerRect.x = 0;
      }
   }
   if ( Input_IsButtonDown( input, InputButton_Up ) )
   {
      playerRect.y -= 1;
      if ( playerRect.y < 0 )
      {
         if ( wraps )
            playerRect.y = ( tilesY * tileSize ) - 1;
         else
            playerRect.y = 0;
      }
   }
   if ( Input_IsButtonDown( input, InputButton_Right ) )
   {
      playerRect.x += 1;
      if ( playerRect.x + playerRect.w > (i32)( tilesX * tileSize ) )
      {
         if ( wraps )
         {
            if ( playerRect.x > (i32)( tilesX * tileSize ) )
               playerRect.x = 0;
         }
         else
            playerRect.x = ( tilesX * tileSize ) - playerRect.w;
      }
   }
   if ( Input_IsButtonDown( input, InputButton_Down ) )
   {
      playerRect.y += 1;
      if ( playerRect.y + playerRect.h > (i32)( tilesY * tileSize ) )
      {
         if ( wraps )
         {
            if ( playerRect.y > (i32)( tilesY * tileSize ) )
               playerRect.y = 0;
         }
         else
            playerRect.y = ( tilesY * tileSize ) - playerRect.h;
      }
   }

   Game_SetPlayerRect( game, playerRect );
}