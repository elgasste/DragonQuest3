#if !defined( PIXEL_BUFFER_H )
#define PIXEL_BUFFER_H

#include "common.h"

typedef struct MemArena_t MemArena_t;

typedef struct PixelBuffer_t
{
   u32 w;
   u32 h;
   u32* mem;
}
PixelBuffer_t;

void PixelBuffer_Create( PixelBuffer_t** pBuffer, MemArena_t* memArena, u32 w, u32 h );
void PixelBuffer_Cleanup( PixelBuffer_t* buffer, MemArena_t* memArena );
void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color );

#endif // PIXEL_BUFFER_H
