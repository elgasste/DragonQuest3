#if !defined( SCREEN_H )
#define SCREEN_H

#include "common.h"
#include "vector.h"

typedef struct MemArena_t MemArena_t;
typedef struct PixelBuffer_t PixelBuffer_t;

typedef struct Screen_t
{
   PixelBuffer_t* buffer;
}
Screen_t;

void Screen_Init( Screen_t* screen, MemArena_t* memArena, u32 w, u32 h );
void Screen_Fill( Screen_t* screen, u32 color );
void Screen_DrawRect( Screen_t* screen, Vector4i32_t rect, u32 color );

#endif // SCREEN_H
