#include <stdlib.h>

#include "pixel_buffer.h"

void PixelBuffer_Init( PixelBuffer_t* buffer, u32 w, u32 h )
{
   buffer->w = w;
   buffer->h = h;
   // TODO: use memory arena
   buffer->mem = (u32*)calloc( w * h, sizeof( u32 ) );
}

void PixelBuffer_CleanUp( PixelBuffer_t* buffer )
{
   free( buffer->mem );
   buffer->mem = 0;
   buffer->w = 0;
   buffer->h = 0;
}
