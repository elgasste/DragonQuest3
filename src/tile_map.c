#include "mem_arena.h"
#include "tile_map.h"
#include "vector.h"

void TileMap_Cleanup( TileMap_t* tileMap, MemArena_t* memArena )
{
   MemArena_Free( memArena, tileMap->tiles );
}

void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, Vector4i32_t* viewport, u32 x, u32 y )
{
   i32 newViewportX, newViewportY, halfViewportW, halfViewportH;

   halfViewportW = (i32)( viewport->w / 2 );
   halfViewportH = (i32)( viewport->h / 2 );

   newViewportX = (i32)x - halfViewportW;
   newViewportY = (i32)y - halfViewportH;

   if ( !tileMap->wraps )
   {
      if ( (u32)viewport->w >= tileMap->w )
      {
         newViewportX = (i32)( ( viewport->w - tileMap->w ) / 2 );
      }
      else if ( newViewportX < 0 )
      {
         newViewportX = 0;
      }
      else if ( (u32)newViewportX > ( tileMap->w - viewport->w ) )
      {
         newViewportX = (i32)( tileMap->w - viewport->w );
      }

      if ( (u32)viewport->h >= tileMap->h )
      {
         newViewportY = (i32)( ( viewport->h - tileMap->h ) / 2 );
      }
      else if ( newViewportY < 0 )
      {
         newViewportY = 0;
      }
      else if ( (u32)newViewportY > ( tileMap->h - viewport->h ) )
      {
         newViewportY = (i32)( tileMap->h - viewport->h );
      }
   }

   viewport->x = newViewportX;
   viewport->y = newViewportY;
}
