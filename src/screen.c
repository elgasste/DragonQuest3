#include "mem_arena.h"
#include "pixel_buffer.h"
#include "screen.h"
#include "tile_map.h"

void Screen_Init( Screen_t* screen, MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_Create( &( screen->buffer ), memArena, w, h );
}

void Screen_Fill( Screen_t* screen, u32 color )
{
   PixelBuffer_ClearColor( screen->buffer, color );
}

void Screen_DrawRect( Screen_t* screen, Vector4i32_t rect, u32 color )
{
   i32 r, b, row, col;
   PixelBuffer_t* buffer;
   u32* mem;

   buffer = screen->buffer;
   r = rect.x + rect.w;
   b = rect.y + rect.h;

   // make sure the rect is even on the screen
   if ( rect.x >= (i32)( buffer->w ) || rect.y >= (i32)( buffer->h ) || r <= 0 || b <= 0 )
   {
      return;
   }

   if ( rect.x < 0 )
   {
      // the left side is off the screen
      rect.w += rect.x;
      rect.x = 0;
   }
   if ( r > (i32)( buffer->w ) )
   {
      // the right side is off the screen
      rect.w -= ( r - buffer->w );
      r = buffer->w;
   }

   if ( rect.y < 0 )
   {
      // the top side is off the screen
      rect.h += rect.y;
      rect.y = 0;
   }
   if ( b > (i32)( buffer->h ) )
   {
      // the bottom side is off the screen
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

void Screen_DrawBuffer( Screen_t* screen, u32* buffer, u32 bufferW, u32 bufferH, i32 screenX, i32 screenY )
{
   i32 screenR, screenB, row, col;
   u32 bufferOffsetL, bufferOffsetR, bufferOffsetT, bufferOffsetB;
   PixelBuffer_t* screenBuffer;
   u32* screenMem;

   screenBuffer = screen->buffer;
   screenR = screenX + (i32)bufferW;
   screenB = screenY + (i32)bufferH;

   // make sure the rect is even on the screen
   if ( screenX >= (i32)( screenBuffer->w ) || screenY >= (i32)( screenBuffer->h ) || screenR <= 0 || screenB <= 0 )
   {
      return;
   }

   bufferOffsetL = 0;
   bufferOffsetR = 0;
   if ( screenX < 0 )
   {
      // the left side is off the screen
      bufferOffsetL += -screenX;
      screenX = 0;
   }
   if ( screenR > (i32)( screenBuffer->w ) )
   {
      // the right side is off the screen
      bufferOffsetR += ( screenR - screenBuffer->w );
   }

   bufferOffsetT = 0;
   bufferOffsetB = 0;
   if ( screenY < 0 )
   {
      // the top side is off the screen
      bufferOffsetT += -screenY;
      screenY = 0;
   }
   if ( screenB > (i32)( screenBuffer->h ) )
   {
      // the bottom side is off the screen
      bufferOffsetB += ( screenB - screenBuffer->h );
   }

   screenMem = screenBuffer->mem + ( ( screenY * screenBuffer->w ) + screenX );
   for ( row = bufferOffsetT; row < (i32)( bufferH - bufferOffsetB ); row++ )
   {
      for ( col = bufferOffsetL; col < (i32)( bufferW - bufferOffsetR ); col++ )
      {
         *screenMem = buffer[( row * bufferW ) + col];
         screenMem++;
      }

      screenMem += ( screenBuffer->w - ( bufferW - bufferOffsetL - bufferOffsetR ) );
   }
}

void Screen_DrawTileMapViewport( Screen_t* screen, TileMap_t* tileMap, Vector4i32_t viewport, i32 screenX, i32 screenY )
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
         drawX = screenX + ( tileWorldX - viewport.x );
         drawY = screenY + ( tileWorldY - viewport.y );
         Screen_DrawBuffer( screen, texture, tileSize, tileSize, drawX, drawY );
      }
   }

}
