#include "mem_arena.h"
#include "pixel_buffer.h"
#include "display.h"
#include "tile.h"
#include "tile_map.h"
#include "tile_texture_set.h"

struct Display_t
{
   PixelBuffer_t* buffer;
};

internal i32 Display_FloorDiv( i32 value, i32 divisor );
internal void Display_DrawWrappedTileMapViewport( Display_t* display, TileMap_t* tileMap, TileTextureSet_t* tileTextureSet, Vector4i32_t viewport, i32 displayX, i32 displayY, u32 tilesX, u32 tilesY, u32 tileSize );

size_t Display_GetStructSize( void )
{
   return sizeof( Display_t );
}

Display_t* Display_Create( MemArena_t* memArena, u32 w, u32 h )
{
   Display_t* display = (Display_t*)MemArena_AllocMem( memArena, sizeof( Display_t ) );
   display->buffer = PixelBuffer_Create( memArena, w, h );
   return display;
}

void Display_Free( Display_t* display, MemArena_t* memArena )
{
   PixelBuffer_Free( display->buffer, memArena );
   MemArena_FreeMem( memArena, display );
}

u32 Display_GetWidth( Display_t* display )
{
   return PixelBuffer_GetWidth( display->buffer );
}

u32 Display_GetHeight( Display_t* display )
{
   return PixelBuffer_GetHeight( display->buffer );
}

const u32* Display_GetPixels( Display_t* display )
{
   return PixelBuffer_GetPixels( display->buffer );
}

void Display_Fill( Display_t* display, u32 color )
{
   PixelBuffer_ClearColor( display->buffer, color );
}

void Display_DrawRect( Display_t* display, i32 x, i32 y, i32 w, i32 h, u32 color )
{
   i32 r, b, row, col;
   u32 bufferW, bufferH;
   PixelBuffer_t* buffer;
   u32* mem;

   buffer = display->buffer;
   bufferW = PixelBuffer_GetWidth( buffer );
   bufferH = PixelBuffer_GetHeight( buffer );
   r = x + w;
   b = y + h;

   // make sure the rect is even on the display
   if ( x >= (i32)( bufferW ) || y >= (i32)( bufferH ) || r <= 0 || b <= 0 )
   {
      return;
   }

   if ( x < 0 )
   {
      // the left side is off the display
      w += x;
      x = 0;
   }
   if ( r > (i32)( bufferW ) )
   {
      // the right side is off the display
      w -= ( r - bufferW );
      r = bufferW;
   }

   if ( y < 0 )
   {
      // the top side is off the display
      h += y;
      y = 0;
   }
   if ( b > (i32)( bufferH ) )
   {
      // the bottom side is off the display
      h -= ( b - bufferH );
      b = bufferH;
   }

   mem = PixelBuffer_GetPixels( display->buffer ) + ( ( y * bufferW ) + x );
   for ( row = 0; row < h; row++ )
   {
      for ( col = 0; col < w; col++ )
      {
         *mem = color;
         mem++;
      }

      mem += ( bufferW - w );
   }
}

void Display_DrawVector4i( Display_t* display, Vector4i32_t rect, u32 color )
{
   Display_DrawRect( display, rect.x, rect.y, rect.w, rect.h, color );
}

void Display_DrawBuffer( Display_t* display, u32* buffer, u32 bufferW, u32 bufferH, i32 displayX, i32 displayY )
{
   i32 displayR, displayB, row, col;
   u32 displayBufferW, displayBufferH, bufferOffsetL, bufferOffsetR, bufferOffsetT, bufferOffsetB;
   PixelBuffer_t* displayBuffer;
   u32* displayMem;

   displayBuffer = display->buffer;
   displayBufferW = PixelBuffer_GetWidth( displayBuffer );
   displayBufferH = PixelBuffer_GetHeight( displayBuffer );
   displayR = displayX + (i32)bufferW;
   displayB = displayY + (i32)bufferH;

   // make sure the rect is even on the display
   if ( displayX >= (i32)( displayBufferW ) || displayY >= (i32)( displayBufferH ) || displayR <= 0 || displayB <= 0 )
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
   if ( displayR > (i32)( displayBufferW ) )
   {
      // the right side is off the display
      bufferOffsetR += ( displayR - displayBufferW );
   }

   bufferOffsetT = 0;
   bufferOffsetB = 0;
   if ( displayY < 0 )
   {
      // the top side is off the display
      bufferOffsetT += -displayY;
      displayY = 0;
   }
   if ( displayB > (i32)( displayBufferH ) )
   {
      // the bottom side is off the display
      bufferOffsetB += ( displayB - displayBufferH );
   }

   displayMem = PixelBuffer_GetPixels( display->buffer ) + ( ( displayY * displayBufferW ) + displayX );
   for ( row = bufferOffsetT; row < (i32)( bufferH - bufferOffsetB ); row++ )
   {
      for ( col = bufferOffsetL; col < (i32)( bufferW - bufferOffsetR ); col++ )
      {
         *displayMem = buffer[( row * bufferW ) + col];
         displayMem++;
      }

      displayMem += ( displayBufferW - ( bufferW - bufferOffsetL - bufferOffsetR ) );
   }
}

void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, TileTextureSet_t* tileTextureSet, Vector4i32_t viewport, i32 displayX, i32 displayY )
{
   i32 tileX, tileY, viewportR, viewportB, tileMapSizeX, tileMapSizeY;
   i32 tileWorldX, tileWorldY, drawX, drawY;
   i32 mapOffsetX, mapOffsetY;
   u32 tilesX, tilesY, tileSize, tileIndex, tileTextureIndex;
   Tile_t* tile;
   u32* texture;
   b32 wraps, smallMapCentered;

   viewportR = viewport.x + viewport.w;
   viewportB = viewport.y + viewport.h;
   tileSize = TileTextureSet_GetTileSize( tileTextureSet );

   mapOffsetX = 0;
   mapOffsetY = 0;
   smallMapCentered = False;

   tilesX = TileMap_GetTilesX( tileMap );
   tilesY = TileMap_GetTilesY( tileMap );
   tileMapSizeX = (i32)( tilesX * tileSize );
   tileMapSizeY = (i32)( tilesY * tileSize );
   wraps = TileMap_GetWraps( tileMap );

   if ( !wraps && viewport.w >= tileMapSizeX && viewport.h >= tileMapSizeY )
   {
      mapOffsetX = ( viewport.w - tileMapSizeX ) / 2;
      mapOffsetY = ( viewport.h - tileMapSizeY ) / 2;
      smallMapCentered = True;
   }

   if ( wraps )
   {
      Display_DrawWrappedTileMapViewport( display, tileMap, tileTextureSet, viewport, displayX, displayY, tilesX, tilesY, tileSize );
      return;
   }

   for ( tileY = 0; tileY < (i32)tilesY; tileY++ )
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

      for ( tileX = 0; tileX < (i32)tilesX; tileX++ )
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

         tileIndex = (u32)tileY * tilesX + (u32)tileX;
         tile = TileMap_GetTile( tileMap, (u32)tileX, (u32)tileY );
         tileTextureIndex = Tile_GetTextureIndex( tile );
         texture = TileTextureSet_GetTexture( tileTextureSet, tileTextureIndex );
         drawX = displayX + ( smallMapCentered ? ( mapOffsetX + tileWorldX ) : ( tileWorldX - viewport.x ) );
         drawY = displayY + ( smallMapCentered ? ( mapOffsetY + tileWorldY ) : ( tileWorldY - viewport.y ) );
         Display_DrawBuffer( display, texture, tileSize, tileSize, drawX, drawY );
      }
   }
}

internal i32 Display_FloorDiv( i32 value, i32 divisor )
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

internal void Display_DrawWrappedTileMapViewport( Display_t* display, TileMap_t* tileMap, TileTextureSet_t* tileTextureSet, Vector4i32_t viewport, i32 displayX, i32 displayY, u32 tilesX, u32 tilesY, u32 tileSize )
{
   i32 tileX, tileY, viewportR, viewportB, tileMapSizeX, tileMapSizeY;
   i32 tileWorldX, tileWorldY, drawX, drawY;
   i32 repeatX, repeatY, repeatStartX, repeatEndX, repeatStartY, repeatEndY;
   u32 tileIndex, tileTextureIndex;
   Tile_t* tile;
   u32* texture;

   viewportR = viewport.x + viewport.w;
   viewportB = viewport.y + viewport.h;
   tileMapSizeX = (i32)( tilesX * tileSize );
   tileMapSizeY = (i32)( tilesY * tileSize );

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

         for ( tileY = 0; tileY < (i32)tilesY; tileY++ )
         {
            tileWorldY = blockWorldY + ( tileY * (i32)tileSize );
            if ( tileWorldY + (i32)tileSize <= viewport.y || tileWorldY >= viewportB )
            {
               continue;
            }

            for ( tileX = 0; tileX < (i32)tilesX; tileX++ )
            {
               tileWorldX = blockWorldX + ( tileX * (i32)tileSize );
               if ( tileWorldX + (i32)tileSize <= viewport.x || tileWorldX >= viewportR )
               {
                  continue;
               }

               tileIndex = (u32)tileY * tilesX + (u32)tileX;
               tile = TileMap_GetTile( tileMap, (u32)tileX, (u32)tileY );
               tileTextureIndex = Tile_GetTextureIndex( tile );

               texture = TileTextureSet_GetTexture( tileTextureSet, tileTextureIndex );
               drawX = displayX + ( tileWorldX - viewport.x );
               drawY = displayY + ( tileWorldY - viewport.y );
               Display_DrawBuffer( display, texture, tileSize, tileSize, drawX, drawY );
            }
         }
      }
   }
}
