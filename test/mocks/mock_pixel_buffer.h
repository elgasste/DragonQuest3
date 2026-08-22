#if !defined( MOCK_PIXEL_BUFFER_H )
#define MOCK_PIXEL_BUFFER_H

#include "../../include/common.h"

typedef struct MemArena_t MemArena_t;

typedef struct PixelBuffer_t
{
   u32 w;
   u32 h;
   u32* mem;
}
PixelBuffer_t;

PixelBuffer_t* PixelBuffer_Create( MemArena_t* memArena, u32 w, u32 h );
void PixelBuffer_Free( PixelBuffer_t* buffer, MemArena_t* memArena );

u32 PixelBuffer_GetWidth( PixelBuffer_t* buffer );
u32 PixelBuffer_GetHeight( PixelBuffer_t* buffer );
u32* PixelBuffer_GetPixels( PixelBuffer_t* buffer );

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color );

#endif // MOCK_PIXEL_BUFFER_H
