#if !defined( WIN_PIXEL_BUFFER_H )
#define WIN_PIXEL_BUFFER_H

#include "common.h"

typedef struct WinPixelBuffer_t
{
   u32 w;
   u32 h;
   u32* mem;
}
WinPixelBuffer_t;

void WinPixelBuffer_Init( WinPixelBuffer_t* buffer, u32 w, u32 h );
void WinPixelBuffer_CleanUp( WinPixelBuffer_t* buffer );

#endif // WIN_PIXEL_BUFFER_H
