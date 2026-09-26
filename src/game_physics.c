#include <stdlib.h>

#include "clock.h"
#include "entity.h"
#include "game.h"
#include "npc.h"
#include "player.h"
#include "sprite.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "utility.h"

internal i32 GamePhysics_GetPixelMovement( i32 velocity, r32 frameSeconds, u32 frameCount );
internal b32 GamePhysics_RectCollidesWithNonPassableTile( TileMap_t* tileMap, Vector4i32_t rect, u32 tileSize );
internal b32 GamePhysics_RectCollidesWithNpc( TileMap_t* tileMap, Entity_t* movingEntity, Vector4i32_t rect );
internal void GamePhysics_TicEntity( Game_t* game, Entity_t* entity, b32 isPlayer );
internal void GamePhysics_ChainPlayers( Game_t* game );
internal void GamePhysics_AdjustPlayerHistoriesForWrap( Game_t* game, Vector4i32_t previousRect, Vector4i32_t currentRect );

void Game_TicPhysics( Game_t* game )
{
   u32 i;
   TileMap_t* tileMap;
   Player_t* activePlayer;
   Vector4i32_t playerRectPrev, playerRectNew;

   activePlayer = Game_GetActivePlayer( game );
   playerRectPrev = Entity_GetRect( Player_GetEntity( activePlayer ) );

   tileMap = Game_GetTileMap( game );
   GamePhysics_TicEntity( game, Player_GetEntity( activePlayer ), True );

   for ( i = 0; i < TileMap_GetNpcCount( tileMap ); i++ )
   {
      GamePhysics_TicEntity( game, Npc_GetEntity( TileMap_GetNpc( tileMap, i ) ), False );
   }

   playerRectNew = Entity_GetRect( Player_GetEntity( activePlayer ) );
   if ( TileMap_GetWraps( tileMap ) )
   {
      GamePhysics_AdjustPlayerHistoriesForWrap( game, playerRectPrev, playerRectNew );
   }
   if ( playerRectPrev.x != playerRectNew.x || playerRectPrev.y != playerRectNew.y )
   {
      GamePhysics_ChainPlayers( game );
   }
}

internal void GamePhysics_TicEntity( Game_t* game, Entity_t* entity, b32 isPlayer )
{
   i32 mapWidth, mapHeight, maxX, maxY, moveX, moveY, stepX, stepY, steps;
   u32 tileSize, prevTileIndex;
   Vector4i32_t entityRect;
   Vector2i32_t velocity;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;

   tileMap = Game_GetTileMap( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   entityRect = Entity_GetRect( entity );
   velocity = Entity_GetVelocity( entity );

#if defined( _WIN32 )
   if ( isPlayer && g_winDebugFlags.moveFast )
   {
      if ( velocity.x != 0 )
      {
         velocity.x = ( velocity.x < 0 ) ? -( 180 * WORLD_UNITS_PER_PIXEL ) : ( 180 * WORLD_UNITS_PER_PIXEL );
      }
      if ( velocity.y != 0 )
      {
         velocity.y = ( velocity.y < 0 ) ? -( 180 * WORLD_UNITS_PER_PIXEL ) : ( 180 * WORLD_UNITS_PER_PIXEL );
      }
   }
#endif

   tileSize = TileTextureSet_GetTileSize( tileTextureSet ) * WORLD_UNITS_PER_PIXEL;
   moveX = GamePhysics_GetPixelMovement( velocity.x, CLOCK_FRAME_SEC, Clock_GetFrameCount( Game_GetClock( game ) ) );
   moveY = GamePhysics_GetPixelMovement( velocity.y, CLOCK_FRAME_SEC, Clock_GetFrameCount( Game_GetClock( game ) ) );
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
              ( !isPlayer && Utility_RectsOverlap( entityRect, Entity_GetRect( Game_GetActivePlayerEntity( game ) ) ) ) )
         {
            entityRect.x -= stepX * WORLD_UNITS_PER_PIXEL;
         }
      }

      if ( step < abs( moveY ) )
      {
         entityRect.y += stepY * WORLD_UNITS_PER_PIXEL;
         if ( GamePhysics_RectCollidesWithNonPassableTile( tileMap, entityRect, tileSize ) ||
              GamePhysics_RectCollidesWithNpc( tileMap, entity, entityRect ) ||
              ( !isPlayer && Utility_RectsOverlap( entityRect, Entity_GetRect( Game_GetActivePlayerEntity( game ) ) ) ) )
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

internal void GamePhysics_ChainPlayers( Game_t* game )
{
   u32 i, playerCount, *playerOrder;
   Player_t *frontPlayer, *backPlayer;
   Entity_t* entity;
   Vector4i32_t rect;
   PlayerMovement_t movement;

   playerCount = Game_GetPlayerCount( game );
   playerOrder = Game_GetPlayerOrder( game );
   
   // always update the front player
   frontPlayer = Game_GetPlayer( game, playerOrder[0] );
   entity = Player_GetEntity( frontPlayer );
   rect = Entity_GetRect( entity );
   movement.newPos.x = rect.x;
   movement.newPos.y = rect.y;
   movement.newDir = ActiveSprite_GetDirection( Entity_GetSprite( entity ) );
   Player_AddMovement( frontPlayer, movement );

   // now update any trailing players
   for ( i = 1; i < playerCount; i++ )
   {
      if ( !Player_GetChainNextPlayer( frontPlayer ) )
      {
         break;
      }

      backPlayer = Game_GetPlayer( game, playerOrder[i] );
      entity = Player_GetEntity( backPlayer );
      movement = Player_GetMovement( frontPlayer, Player_GetMovementChainIndex( backPlayer ) );
      Player_AddMovement( backPlayer, movement );
      Entity_SetPosition( entity, movement.newPos.x, movement.newPos.y );
      ActiveSprite_SetDirection( Entity_GetSprite( entity ), movement.newDir );

      frontPlayer = backPlayer;
   }
}

internal void GamePhysics_AdjustPlayerHistoriesForWrap( Game_t* game, Vector4i32_t previousRect, Vector4i32_t currentRect )
{
   i32 mapWidth, mapHeight, offsetX, offsetY;
   u32 i, tileSize;
   TileMap_t* tileMap;
   TileTextureSet_t* tileTextureSet;

   tileMap = Game_GetTileMap( game );
   tileTextureSet = Game_GetTileTextureSet( game );
   tileSize = TileTextureSet_GetTileSize( tileTextureSet );
   mapWidth = (i32)( TileMap_GetTilesX( tileMap ) * tileSize ) * WORLD_UNITS_PER_PIXEL;
   mapHeight = (i32)( TileMap_GetTilesY( tileMap ) * tileSize ) * WORLD_UNITS_PER_PIXEL;
   offsetX = 0;
   offsetY = 0;

   if ( previousRect.x - currentRect.x > mapWidth / 2 )
   {
      offsetX = -mapWidth;
   }
   else if ( currentRect.x - previousRect.x > mapWidth / 2 )
   {
      offsetX = mapWidth;
   }

   if ( previousRect.y - currentRect.y > mapHeight / 2 )
   {
      offsetY = -mapHeight;
   }
   else if ( currentRect.y - previousRect.y > mapHeight / 2 )
   {
      offsetY = mapHeight;
   }

   if ( offsetX == 0 && offsetY == 0 )
   {
      return;
   }

   for ( i = 0; i < Game_GetPlayerCount( game ); i++ )
   {
      Player_OffsetMovementHistory( Game_GetPlayer( game, i ), offsetX, offsetY );
   }
}
