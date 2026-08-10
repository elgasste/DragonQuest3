#include "mem_arena.h"
#include "pixel_buffer.h"
#include "display.h"
#include "tile_map.h"

void Display_Init( Display_t* display, MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_Create( &( display->buffer ), memArena, w, h );
}

void Display_Fill( Display_t* display, u32 color )
{
   PixelBuffer_ClearColor( display->buffer, color );
}

void Display_DrawRect( Display_t* display, Vector4i32_t rect, u32 color )
{
   i32 r, b, row, col;
   PixelBuffer_t* buffer;
   u32* mem;

   buffer = display->buffer;
   r = rect.x + rect.w;
   b = rect.y + rect.h;

   // make sure the rect is even on the display
   if ( rect.x >= (i32)( buffer->w ) || rect.y >= (i32)( buffer->h ) || r <= 0 || b <= 0 )
   {
      return;
   }

   if ( rect.x < 0 )
   {
      // the left side is off the display
      rect.w += rect.x;
      rect.x = 0;
   }
   if ( r > (i32)( buffer->w ) )
   {
      // the right side is off the display
      rect.w -= ( r - buffer->w );
      r = buffer->w;
   }

   if ( rect.y < 0 )
   {
      // the top side is off the display
      rect.h += rect.y;
      rect.y = 0;
   }
   if ( b > (i32)( buffer->h ) )
   {
      // the bottom side is off the display
      rect.h -= ( b - buffer->h );
      b = buffer->h;
   }

   mem = buffer->mem + ( ( rect.y * buffer->w ) + rect.x );
   for ( row = 0; row < rect.h; row++ )
   {
      for ( col = 0; col < rect.w; col++ )
      {
         *mem = color;
         mem++;
      }

      mem += ( buffer->w - rect.w );
   }
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

void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, Vector4i32_t viewport, i32 displayX, i32 displayY )
{
   i32 tileX, tileY, viewportR, viewportB;
   i32 tileWorldX, tileWorldY, drawX, drawY;
   u32 tileSize, tileIndex;
   TileTextureSet_t* textureSet;
   Tile_t* tile;
   u32* texture;

   textureSet = tileMap->tileTextureSet;
   viewportR = viewport.x + viewport.w;
   viewportB = viewport.y + viewport.h;
   tileSize = textureSet->tileSize;

   for ( tileY = 0; tileY < (i32)tileMap->h; tileY++ )
   {
      tileWorldY = tileY * (i32)tileSize;
      if ( tileWorldY >= viewportB )
      {
         break;
      }

      // TODO: this might be inefficient, we could calculate this in advance
      if ( ( tileWorldY + (i32)tileSize ) <= viewport.y )
      {
         continue;
      }

      for ( tileX = 0; tileX < (i32)tileMap->w; tileX++ )
      {
         tileWorldX = tileX * (i32)tileSize;
         if ( tileWorldX >= viewportR )
         {
            break;
         }

         // TODO: this might be inefficient, we could calculate this in advance
         if ( ( tileWorldX + (i32)tileSize ) <= viewport.x )
         {
            continue;
         }

         tileIndex = (u32)tileY * tileMap->w + (u32)tileX;
         tile = &( tileMap->tiles[tileIndex] );

         texture = textureSet->textures + ( tile->textureIndex * tileSize * tileSize );
         drawX = displayX + ( tileWorldX - viewport.x );
         drawY = displayY + ( tileWorldY - viewport.y );
         Display_DrawBuffer( display, texture, tileSize, tileSize, drawX, drawY );
      }
   }

}
