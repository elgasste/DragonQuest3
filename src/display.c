#include "mem_arena.h"
#include "pixel_buffer.h"
#include "display.h"
#include "tile_map.h"
#include "tile_texture_set.h"

void Display_Init( Display_t* display, MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_Create( &( display->buffer ), memArena, w, h );
}

void Display_Cleanup( Display_t* display, MemArena_t* memArena )
{
   PixelBuffer_Cleanup( display->buffer, memArena );
   MemArena_Free( memArena, display->buffer );
}

void Display_Fill( Display_t* display, u32 color )
{
   PixelBuffer_ClearColor( display->buffer, color );
}

void Display_DrawRect( Display_t* display, i32 x, i32 y, i32 w, i32 h, u32 color )
{
   i32 r, b, row, col;
   PixelBuffer_t* buffer;
   u32* mem;

   buffer = display->buffer;
   r = x + w;
   b = y + h;

   // make sure the rect is even on the display
   if ( x >= (i32)( buffer->w ) || y >= (i32)( buffer->h ) || r <= 0 || b <= 0 )
   {
      return;
   }

   if ( x < 0 )
   {
      // the left side is off the display
      w += x;
      x = 0;
   }
   if ( r > (i32)( buffer->w ) )
   {
      // the right side is off the display
      w -= ( r - buffer->w );
      r = buffer->w;
   }

   if ( y < 0 )
   {
      // the top side is off the display
      h += y;
      y = 0;
   }
   if ( b > (i32)( buffer->h ) )
   {
      // the bottom side is off the display
      h -= ( b - buffer->h );
      b = buffer->h;
   }

   mem = buffer->mem + ( ( y * buffer->w ) + x );
   for ( row = 0; row < h; row++ )
   {
      for ( col = 0; col < w; col++ )
      {
         *mem = color;
         mem++;
      }

      mem += ( buffer->w - w );
   }
}

void Display_DrawVector4i( Display_t* display, Vector4i32_t rect, u32 color )
{
   Display_DrawRect( display, rect.x, rect.y, rect.w, rect.h, color );
}

void Display_DrawBuffer( Display_t* display, u32* buffer, u32 bufferW, u32 bufferH, i32 displayX, i32 displayY )
{
   i32 displayR, displayB, row, col;
   u32 bufferOffsetL, bufferOffsetR, bufferOffsetT, bufferOffsetB;
   PixelBuffer_t* displayBuffer;
   u32* displayMem;

   displayBuffer = display->buffer;
   displayR = displayX + (i32)bufferW;
   displayB = displayY + (i32)bufferH;

   // make sure the rect is even on the display
   if ( displayX >= (i32)( displayBuffer->w ) || displayY >= (i32)( displayBuffer->h ) || displayR <= 0 || displayB <= 0 )
   {
      return;
   }

   bufferOffsetL = 0;
   bufferOffsetR = 0;
   if ( displayX < 0 )
   {
      // the left side is off the display
      bufferOffsetL += -displayX;
      displayX = 0;
   }
   if ( displayR > (i32)( displayBuffer->w ) )
   {
      // the right side is off the display
      bufferOffsetR += ( displayR - displayBuffer->w );
   }

   bufferOffsetT = 0;
   bufferOffsetB = 0;
   if ( displayY < 0 )
   {
      // the top side is off the display
      bufferOffsetT += -displayY;
      displayY = 0;
   }
   if ( displayB > (i32)( displayBuffer->h ) )
   {
      // the bottom side is off the display
      bufferOffsetB += ( displayB - displayBuffer->h );
   }

   displayMem = displayBuffer->mem + ( ( displayY * displayBuffer->w ) + displayX );
   for ( row = bufferOffsetT; row < (i32)( bufferH - bufferOffsetB ); row++ )
   {
      for ( col = bufferOffsetL; col < (i32)( bufferW - bufferOffsetR ); col++ )
      {
         *displayMem = buffer[( row * bufferW ) + col];
         displayMem++;
      }

      displayMem += ( displayBuffer->w - ( bufferW - bufferOffsetL - bufferOffsetR ) );
   }
}

// MUFFINS: I need to understand this before I check it in
static i32 Display_FloorDiv( i32 value, i32 divisor )
{
   if ( divisor == 0 )
   {
      return 0;
   }

   if ( value >= 0 )
   {
      return value / divisor;
   }

   return -( ( -value + divisor - 1 ) / divisor );
}

void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, Vector4i32_t viewport, i32 displayX, i32 displayY )
{
   i32 tileX, tileY, viewportR, viewportB, tileMapSizeX, tileMapSizeY;
   i32 tileWorldX, tileWorldY, drawX, drawY;
   i32 mapOffsetX, mapOffsetY;
   i32 repeatX, repeatY, repeatStartX, repeatEndX, repeatStartY, repeatEndY;
   u32 tileSize, tileIndex;
   TileTextureSet_t* textureSet;
   Tile_t* tile;
   u32* texture;
   b32 smallMapCentered;

   textureSet = tileMap->tileTextureSet;
   viewportR = viewport.x + viewport.w;
   viewportB = viewport.y + viewport.h;
   tileSize = textureSet->tileSize;

   mapOffsetX = 0;
   mapOffsetY = 0;
   smallMapCentered = False;

   tileMapSizeX = (i32)( tileMap->tilesX * tileSize );
   tileMapSizeY = (i32)( tileMap->tilesY * tileSize );

   if ( !tileMap->wraps && viewport.w >= tileMapSizeX && viewport.h >= tileMapSizeY )
   {
      mapOffsetX = ( viewport.w - tileMapSizeX ) / 2;
      mapOffsetY = ( viewport.h - tileMapSizeY ) / 2;
      smallMapCentered = True;
   }

   if ( tileMap->wraps )
   {
      repeatStartX = Display_FloorDiv( viewport.x, tileMapSizeX );
      repeatEndX = Display_FloorDiv( viewportR - 1, tileMapSizeX );
      repeatStartY = Display_FloorDiv( viewport.y, tileMapSizeY );
      repeatEndY = Display_FloorDiv( viewportB - 1, tileMapSizeY );

      for ( repeatY = repeatStartY; repeatY <= repeatEndY; repeatY++ )
      {
         for ( repeatX = repeatStartX; repeatX <= repeatEndX; repeatX++ )
         {
            i32 blockWorldX = repeatX * tileMapSizeX;
            i32 blockWorldY = repeatY * tileMapSizeY;

            for ( tileY = 0; tileY < (i32)tileMap->tilesY; tileY++ )
            {
               tileWorldY = blockWorldY + ( tileY * (i32)tileSize );
               if ( tileWorldY + (i32)tileSize <= viewport.y || tileWorldY >= viewportB )
               {
                  continue;
               }

               for ( tileX = 0; tileX < (i32)tileMap->tilesX; tileX++ )
               {
                  tileWorldX = blockWorldX + ( tileX * (i32)tileSize );
                  if ( tileWorldX + (i32)tileSize <= viewport.x || tileWorldX >= viewportR )
                  {
                     continue;
                  }

                  tileIndex = (u32)tileY * tileMap->tilesX + (u32)tileX;
                  tile = &( tileMap->tiles[tileIndex] );

                  texture = textureSet->textures + ( tile->textureIndex * tileSize * tileSize );
                  drawX = displayX + ( tileWorldX - viewport.x );
                  drawY = displayY + ( tileWorldY - viewport.y );
                  Display_DrawBuffer( display, texture, tileSize, tileSize, drawX, drawY );
               }
            }
         }
      }

      return;
   }

   for ( tileY = 0; tileY < (i32)tileMap->tilesY; tileY++ )
   {
      tileWorldY = tileY * (i32)tileSize;

      if ( !smallMapCentered )
      {
         if ( tileWorldY >= viewportB )
         {
            break;
         }

         if ( ( tileWorldY + (i32)tileSize ) <= viewport.y )
         {
            continue;
         }
      }

      for ( tileX = 0; tileX < (i32)tileMap->tilesX; tileX++ )
      {
         tileWorldX = tileX * (i32)tileSize;

         if ( !smallMapCentered )
         {
            if ( tileWorldX >= viewportR )
            {
               break;
            }

            if ( ( tileWorldX + (i32)tileSize ) <= viewport.x )
            {
               continue;
            }
         }

         tileIndex = (u32)tileY * tileMap->tilesX + (u32)tileX;
         tile = &( tileMap->tiles[tileIndex] );

         texture = textureSet->textures + ( tile->textureIndex * tileSize * tileSize );
         drawX = displayX + ( smallMapCentered ? ( mapOffsetX + tileWorldX ) : ( tileWorldX - viewport.x ) );
         drawY = displayY + ( smallMapCentered ? ( mapOffsetY + tileWorldY ) : ( tileWorldY - viewport.y ) );
         Display_DrawBuffer( display, texture, tileSize, tileSize, drawX, drawY );
      }
   }

}
