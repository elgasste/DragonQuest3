#if !defined( SCREEN_H )
#define SCREEN_H

#include "common.h"

typedef struct MemArena_t MemArena_t;
typedef struct PixelBuffer_t PixelBuffer_t;

typedef struct Screen_t
{
   PixelBuffer_t* buffer;
}
Screen_t;

void Screen_Init( Screen_t* screen, MemArena_t* memArena, u32 w, u32 h );

#endif // SCREEN_H
