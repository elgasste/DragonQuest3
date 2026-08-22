#if !defined( PIXEL_BUFFER_H )
#define PIXEL_BUFFER_H

#include "common.h"

typedef struct MemArena_t MemArena_t;

typedef struct PixelBuffer_t PixelBuffer_t;
size_t PixelBuffer_GetStructSize( void );
PixelBuffer_t* PixelBuffer_Create( MemArena_t* memArena, u32 w, u32 h );
void PixelBuffer_Free( PixelBuffer_t* buffer, MemArena_t* memArena );

u32 PixelBuffer_GetWidth( PixelBuffer_t* buffer );
u32 PixelBuffer_GetHeight( PixelBuffer_t* buffer );
u32* PixelBuffer_GetPixels( PixelBuffer_t* buffer );

void PixelBuffer_ClearColor( PixelBuffer_t* buffer, u32 color );

#endif // PIXEL_BUFFER_H
