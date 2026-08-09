#if !defined( SCREEN_H )
#define SCREEN_H

#include "common.h"
#include "vector.h"

typedef struct MemArena_t MemArena_t;
typedef struct PixelBuffer_t PixelBuffer_t;
typedef struct TileMap_t TileMap_t;

typedef struct Screen_t
{
   PixelBuffer_t* buffer;
}
Screen_t;

void Screen_Init( Screen_t* screen, MemArena_t* memArena, u32 w, u32 h );
void Screen_Fill( Screen_t* screen, u32 color );
void Screen_DrawRect( Screen_t* screen, Vector4i32_t rect, u32 color );
void Screen_DrawBuffer( Screen_t* screen, u32* buffer, u32 bufferW, u32 bufferH, i32 screenX, i32 screenY );
void Screen_DrawTileMapViewport( Screen_t* screen, TileMap_t* tileMap, Vector4i32_t viewport, i32 screenX, i32 screenY );

#endif // SCREEN_H
