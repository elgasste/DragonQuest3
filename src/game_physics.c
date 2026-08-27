#include <stdlib.h>

#include "clock.h"
#include "entity.h"
#include "game.h"
#include "tile_map.h"
#include "tile_texture_set.h"

internal i32 GamePhysics_GetPixelMovement( i32 velocity, r32 frameSeconds, u32 frameCount );
internal b32 GamePhysics_RectCollidesWithNonPassableTile( TileMap_t* tileMap, Vector4i32_t rect, u32 tileSize );

void Game_TicPhysics( Game_t* game )
{
   i32 mapWidth, mapHeight, maxX, maxY, moveX, moveY, stepX, stepY, steps;
   u32 tileSize;
   r32 frameSeconds;
   Vector4i32_t playerRect;
   Vector2i32_t playerVelocity;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;
   Entity_t* playerEntity;

   frameSeconds = Clock_GetFrameSec( Game_GetClock( game ) );
   playerEntity = Game_GetPlayerEntity( game );
   tileMap = Game_GetTileMap( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   playerRect = Entity_GetRect( playerEntity );
   playerVelocity = Entity_GetVelocity( playerEntity );

   tileSize = TileTextureSet_GetTileSize( tileTextureSet ) * WORLD_UNITS_PER_PIXEL;
   moveX = GamePhysics_GetPixelMovement( playerVelocity.x, frameSeconds, Clock_GetFrameCount( Game_GetClock( game ) ) );
   moveY = GamePhysics_GetPixelMovement( playerVelocity.y, frameSeconds, Clock_GetFrameCount( Game_GetClock( game ) ) );
   stepX = ( moveX < 0 ) ? -1 : 1;
   stepY = ( moveY < 0 ) ? -1 : 1;
   steps = ( abs( moveX ) > abs( moveY ) ) ? abs( moveX ) : abs( moveY );

   // check for collisions with non-passable tiles. the idea for this part came from Maddy Thorson's game, Celeste.
   // instead of setting an entity's new position and doing collision resolution, we just move one pixel at a time
   // and check for collisions. the x and y checks have to be separated, so movement is still allowed in a non-colliding direction.
   for ( i32 step = 0; step < steps; step++ )
   {
      if ( step < abs( moveX ) )
      {
         playerRect.x += stepX * WORLD_UNITS_PER_PIXEL;
         if ( GamePhysics_RectCollidesWithNonPassableTile( tileMap, playerRect, tileSize ) )
         {
            playerRect.x -= stepX * WORLD_UNITS_PER_PIXEL;
         }
      }

      if ( step < abs( moveY ) )
      {
         playerRect.y += stepY * WORLD_UNITS_PER_PIXEL;
         if ( GamePhysics_RectCollidesWithNonPassableTile( tileMap, playerRect, tileSize ) )
         {
            playerRect.y -= stepY * WORLD_UNITS_PER_PIXEL;
         }
      }
   }

   Entity_SetPosition( playerEntity, playerRect.x, playerRect.y );

   // clamp the player to the edge of the map as well, if it doesn't wrap
   if ( !TileMap_GetWraps( tileMap ) )
   {
      mapWidth = (i32)( TileMap_GetTilesX( tileMap ) * TileTextureSet_GetTileSize( tileTextureSet ) ) * WORLD_UNITS_PER_PIXEL;
      mapHeight = (i32)( TileMap_GetTilesY( tileMap ) * TileTextureSet_GetTileSize( tileTextureSet ) ) * WORLD_UNITS_PER_PIXEL;
      maxX = mapWidth - playerRect.w;
      maxY = mapHeight - playerRect.h;

      if ( maxX < 0 )
      {
         maxX = 0;
      }
      if ( maxY < 0 )
      {
         maxY = 0;
      }

      if ( playerRect.x < 0 )
      {
         playerRect.x = 0;
      }
      else if ( playerRect.x > maxX )
      {
         playerRect.x = maxX;
      }

      if ( playerRect.y < 0 )
      {
         playerRect.y = 0;
      }
      else if ( playerRect.y > maxY )
      {
         playerRect.y = maxY;
      }
   }
   else
   {
      TileMap_WrapEntityPosition( tileMap, playerEntity );
      playerRect = Entity_GetRect( playerEntity );
   }

   Entity_SetPosition( playerEntity, playerRect.x, playerRect.y );
   Entity_SetTileIndex( playerEntity, TileMap_GetTileIndexForEntity( tileMap, playerEntity ) );
   Entity_SetVelocity( playerEntity, 0, 0 );
}

internal i32 GamePhysics_GetPixelMovement( i32 velocity, r32 frameSeconds, u32 frameCount )
{
   r32 pixelsPerFrame;
   i32 currentPixels, previousPixels, movement;

   pixelsPerFrame = (r32)abs( velocity ) * frameSeconds / WORLD_UNITS_PER_PIXEL;
   currentPixels = (i32)( pixelsPerFrame * ( frameCount + 1 ) );
   previousPixels = (i32)( pixelsPerFrame * frameCount );
   movement = currentPixels - previousPixels;

   return ( velocity < 0 ) ? -movement : movement;
}

internal b32 GamePhysics_RectCollidesWithNonPassableTile( TileMap_t* tileMap, Vector4i32_t rect, u32 tileSize )
{
   i32 firstTileX, lastTileX, firstTileY, lastTileY;
   i32 tileX, tileY, tilesX, tilesY;
   u32 tileIndex;

   tilesX = (i32)TileMap_GetTilesX( tileMap );
   tilesY = (i32)TileMap_GetTilesY( tileMap );
   firstTileX = rect.x / tileSize;
   lastTileX = ( rect.x + rect.w - 1 ) / tileSize;
   firstTileY = rect.y / tileSize;
   lastTileY = ( rect.y + rect.h - 1 ) / tileSize;

   for ( tileY = firstTileY; tileY <= lastTileY; tileY++ )
   {
      for ( tileX = firstTileX; tileX <= lastTileX; tileX++ )
      {
         if ( TileMap_GetWraps( tileMap ) )
         {
            tileIndex = (u32)( ( ( tileY % tilesY ) + tilesY ) % tilesY ) * (u32)tilesX + (u32)( ( ( tileX % tilesX ) + tilesX ) % tilesX );
         }
         else if ( tileX < 0 || tileX >= tilesX || tileY < 0 || tileY >= tilesY )
         {
            continue;
         }
         else
         {
            tileIndex = (u32)tileY * (u32)tilesX + (u32)tileX;
         }

         if ( !Tile_GetIsPassable( TileMap_GetTile( tileMap, tileIndex ) ) )
         {
            return True;
         }
      }
   }

   return False;
}
