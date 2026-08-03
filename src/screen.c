#include "mem_arena.h"
#include "pixel_buffer.h"
#include "screen.h"

void Screen_Init( Screen_t* screen, MemArena_t* memArena, u32 w, u32 h )
{
   PixelBuffer_Create( &( screen->buffer ), memArena, w, h );
}

void Screen_Fill( Screen_t* screen, u32 color )
{
   PixelBuffer_ClearColor( screen->buffer, color );
}
