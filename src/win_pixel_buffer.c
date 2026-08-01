#include "win_common.h"
#include "win_pixel_buffer.h"

void WinPixelBuffer_Init( WinPixelBuffer_t* buffer, u32 w, u32 h )
{
   buffer->w = w;
   buffer->h = h;
   buffer->mem = (u32*)calloc( w * h, sizeof( u32 ) );
}

void WinPixelBuffer_CleanUp( WinPixelBuffer_t* buffer )
{
   free( buffer->mem );
   buffer->mem = 0;
   buffer->w = 0;
   buffer->h = 0;
}
