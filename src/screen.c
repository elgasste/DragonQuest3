#include "mem_arena.h"
#include "pixel_buffer.h"
#include "screen.h"

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
