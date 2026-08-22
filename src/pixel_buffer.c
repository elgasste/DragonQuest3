#include "mem_arena.h"
#include "pixel_buffer.h"
#include "platform.h"

struct PixelBuffer_t
{
   u32 w;
   u32 h;
   u32* mem;
};

size_t PixelBuffer_GetStructSize( void )
{
   return sizeof( PixelBuffer_t );
}

PixelBuffer_t* PixelBuffer_Create( MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_t* buffer = MemArena_AllocMem( memArena, sizeof( PixelBuffer_t ) );
   buffer->w = w;
   buffer->h = h;
   buffer->mem = (u32*)MemArena_AllocMem( memArena, w * h * sizeof( u32 ) );
   return buffer;
}

void PixelBuffer_Free( PixelBuffer_t* buffer, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, buffer->mem );
   MemArena_FreeMem( memArena, buffer );
}

u32 PixelBuffer_GetWidth( PixelBuffer_t* buffer )
{
   return buffer->w;
}

u32 PixelBuffer_GetHeight( PixelBuffer_t* buffer )
{
   return buffer->h;
}

u32* PixelBuffer_GetPixels( PixelBuffer_t* buffer )
{
   return buffer->mem;
}

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color )
{
   size_t pixels, i;

   pixels = buffer->w * buffer->h;
   for ( i = 0; i < pixels; i++ )
   {
      buffer->mem[i] = color;
   }
}
