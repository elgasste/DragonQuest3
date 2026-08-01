#if !defined( PIXEL_BUFFER_H )
#define PIXEL_BUFFER_H

#include "common.h"

typedef struct PixelBuffer_t
{
   u32 w;
   u32 h;
   u32* mem;
}
PixelBuffer_t;

void PixelBuffer_Init( PixelBuffer_t* buffer, u32 w, u32 h );
void PixelBuffer_CleanUp( PixelBuffer_t* buffer );

#endif // PIXEL_BUFFER_H
