#if !defined( DISPLAY_H )
#define DISPLAY_H

#include "common.h"
#include "vector.h"

typedef struct MemArena_t MemArena_t;
typedef struct PixelBuffer_t PixelBuffer_t;
typedef struct TileMap_t TileMap_t;

typedef struct Display_t
{
   PixelBuffer_t* buffer;
}
Display_t;

void Display_Init( Display_t* display, MemArena_t* memArena, u32 w, u32 h );
void Display_Cleanup( Display_t* display, MemArena_t* memArena );
void Display_Fill( Display_t* display, u32 color );
void Display_DrawRect( Display_t* display, i32 x, i32 y, i32 w, i32 h, u32 color );
void Display_DrawVector4i( Display_t* display, Vector4i32_t rect, u32 color );
void Display_DrawBuffer( Display_t* display, u32* buffer, u32 bufferW, u32 bufferH, i32 displayX, i32 displayY );
void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, Vector4i32_t viewport, i32 displayX, i32 displayY );

#endif // DISPLAY_H
