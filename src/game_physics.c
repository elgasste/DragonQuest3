#include <stdlib.h>

#include "clock.h"
#include "entity.h"
#include "game.h"
#include "npc.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "utility.h"

internal i32 GamePhysics_GetPixelMovement( i32 velocity, r32 frameSeconds, u32 frameCount );
internal b32 GamePhysics_RectCollidesWithNonPassableTile( TileMap_t* tileMap, Vector4i32_t rect, u32 tileSize );
internal b32 GamePhysics_RectCollidesWithNpc( TileMap_t* tileMap, Entity_t* movingEntity, Vector4i32_t rect );
internal void GamePhysics_TicEntity( Game_t* game, Entity_t* entity, b32 isPlayer );

void Game_TicPhysics( Game_t* game )
{
   u32 i;
   TileMap_t* tileMap;

   tileMap = Game_GetTileMap( game );
   GamePhysics_TicEntity( game, Game_GetPlayerEntity( game ), True );

   for ( i = 0; i < TileMap_GetNpcCount( tileMap ); i++ )
   {
      GamePhysics_TicEntity( game, Npc_GetEntity( TileMap_GetNpc( tileMap, i ) ), False );
   }
}

internal void GamePhysics_TicEntity( Game_t* game, Entity_t* entity, b32 isPlayer )
{
   i32 mapWidth, mapHeight, maxX, maxY, moveX, moveY, stepX, stepY, steps;
   u32 tileSize, prevTileIndex;
   r32 frameSeconds;
   Vector4i32_t entityRect;
   i32 velocityX, velocityY;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;

   frameSeconds = Clock_GetFrameSec( Game_GetClock( game ) );
   tileMap = Game_GetTileMap( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   entityRect = Entity_GetRect( entity );
   if ( isPlayer )
   {
      velocityX = Entity_GetVelocity( entity ).x;
      velocityY = Entity_GetVelocity( entity ).y;
   }
   else
   {
      velocityX = Entity_GetVelocityX( entity );
      velocityY = Entity_GetVelocityY( entity );
   }

   tileSize = TileTextureSet_GetTileSize( tileTextureSet ) * WORLD_UNITS_PER_PIXEL;
   moveX = GamePhysics_GetPixelMovement( velocityX, frameSeconds, Clock_GetFrameCount( Game_GetClock( game ) ) );
   moveY = GamePhysics_GetPixelMovement( velocityY, frameSeconds, Clock_GetFrameCount( Game_GetClock( game ) ) );
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
         entityRect.x += stepX * WORLD_UNITS_PER_PIXEL;
         if ( GamePhysics_RectCollidesWithNonPassableTile( tileMap, entityRect, tileSize ) ||
              GamePhysics_RectCollidesWithNpc( tileMap, entity, entityRect ) ||
              ( !isPlayer && Utility_RectsOverlap( entityRect, Entity_GetRect( Game_GetPlayerEntity( game ) ) ) ) )
         {
            entityRect.x -= stepX * WORLD_UNITS_PER_PIXEL;
         }
      }

      if ( step < abs( moveY ) )
      {
         entityRect.y += stepY * WORLD_UNITS_PER_PIXEL;
         if ( GamePhysics_RectCollidesWithNonPassableTile( tileMap, entityRect, tileSize ) ||
              GamePhysics_RectCollidesWithNpc( tileMap, entity, entityRect ) ||
              ( !isPlayer && Utility_RectsOverlap( entityRect, Entity_GetRect( Game_GetPlayerEntity( game ) ) ) ) )
         {
            entityRect.y -= stepY * WORLD_UNITS_PER_PIXEL;
         }
      }
   }

   Entity_SetPosition( entity, entityRect.x, entityRect.y );

   // clamp the player to the edge of the map as well, if it doesn't wrap
   if ( !TileMap_GetWraps( tileMap ) )
   {
      mapWidth = (i32)( TileMap_GetTilesX( tileMap ) * TileTextureSet_GetTileSize( tileTextureSet ) ) * WORLD_UNITS_PER_PIXEL;
      mapHeight = (i32)( TileMap_GetTilesY( tileMap ) * TileTextureSet_GetTileSize( tileTextureSet ) ) * WORLD_UNITS_PER_PIXEL;
      maxX = mapWidth - entityRect.w;
      maxY = mapHeight - entityRect.h;

      if ( maxX < 0 )
      {
         maxX = 0;
      }
      if ( maxY < 0 )
      {
         maxY = 0;
      }

      if ( entityRect.x < 0 )
      {
         entityRect.x = 0;
      }
      else if ( entityRect.x > maxX )
      {
         entityRect.x = maxX;
      }

      if ( entityRect.y < 0 )
      {
         entityRect.y = 0;
      }
      else if ( entityRect.y > maxY )
      {
         entityRect.y = maxY;
      }
   }
   else
   {
      TileMap_WrapEntityPosition( tileMap, entity );
      entityRect = Entity_GetRect( entity );
   }

   prevTileIndex = Entity_GetTileIndex( entity );

   Entity_SetPosition( entity, entityRect.x, entityRect.y );
   Entity_SetTileIndex( entity, TileMap_GetTileIndexForEntity( tileMap, entity ) );
   if ( isPlayer )
   {
      Entity_SetVelocity( entity, 0, 0 );
   }

   if ( isPlayer && prevTileIndex != Entity_GetTileIndex( entity ) )
   {
      Game_OnPlayerTileIndexChanged( game, Entity_GetTileIndex( entity ) );
   }
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

#if defined( _WIN32 )
   if ( g_winDebugFlags.noClip )
   {
      return False;
   }
#endif

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

internal b32 GamePhysics_RectCollidesWithNpc( TileMap_t* tileMap, Entity_t* movingEntity, Vector4i32_t rect )
{
   u32 i;
   Vector4i32_t npcRect;
   Entity_t* npcEntity;

#if defined( _WIN32 )
   if ( g_winDebugFlags.noClip )
   {
      return False;
   }
#endif

   for ( i = 0; i < TileMap_GetNpcCount( tileMap ); i++ )
   {
      npcEntity = Npc_GetEntity( TileMap_GetNpc( tileMap, i ) );
      if ( npcEntity == movingEntity )
      {
         continue;
      }
      npcRect = Entity_GetRect( npcEntity );

      if ( Utility_RectsOverlap( rect, npcRect ) )
      {
         return True;
      }
   }

   return False;
}
