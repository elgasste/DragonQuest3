#include "mem_arena.h"
#include "tile_map.h"
#include "tile_texture_set.h"
#include "vector.h"

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileMap->tiles );
}

void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, Vector4i32_t* viewport, u32 x, u32 y )
{
   i32 newViewportX, newViewportY, halfViewportW, halfViewportH, tileMapW, tileMapH;

   halfViewportW = (i32)( viewport->w / 2 );
   halfViewportH = (i32)( viewport->h / 2 );

   tileMapW = (i32)tileMap->tilesX * (i32)tileMap->tileTextureSet->tileSize;
   tileMapH = (i32)tileMap->tilesY * (i32)tileMap->tileTextureSet->tileSize;

   newViewportX = (i32)x - halfViewportW;
   newViewportY = (i32)y - halfViewportH;

   if ( !tileMap->wraps )
   {
      if ( viewport->w >= tileMapW )
      {
         newViewportX = -(i32)( ( viewport->w - tileMapW ) / 2 );
      }
      else if ( newViewportX < 0 )
      {
         newViewportX = 0;
      }
      else if ( newViewportX > ( tileMapW - viewport->w ) )
      {
         newViewportX = (i32)( tileMapW - viewport->w );
      }

      if ( viewport->h >= tileMapH )
      {
         newViewportY = -(i32)( ( viewport->h - tileMapH ) / 2 );
      }
      else if ( newViewportY < 0 )
      {
         newViewportY = 0;
      }
      else if ( newViewportY > ( tileMapH - viewport->h ) )
      {
         newViewportY = (i32)( tileMapH - viewport->h );
      }
   }

   viewport->x = newViewportX;
   viewport->y = newViewportY;
}
