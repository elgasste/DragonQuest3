#if !defined( DISPLAY_H )
#define DISPLAY_H

#include "common.h"
#include "vector.h"

typedef struct Display_t Display_t;
typedef struct MemArena_t MemArena_t;
typedef struct TileMap_t TileMap_t;

Display_t* Display_Create( MemArena_t* memArena, u32 w, u32 h );
void Display_Free( Display_t* display, MemArena_t* memArena );

u32 Display_GetWidth( Display_t* display );
u32 Display_GetHeight( Display_t* display );
const u32* Display_GetPixels( Display_t* display );

void Display_Fill( Display_t* display, u32 color );
void Display_DrawRect( Display_t* display, i32 x, i32 y, i32 w, i32 h, u32 color );
void Display_DrawVector4i( Display_t* display, Vector4i32_t rect, u32 color );
void Display_DrawBuffer( Display_t* display, u32* buffer, u32 bufferW, u32 bufferH, i32 displayX, i32 displayY );
void Display_DrawTileMapViewport( Display_t* display, TileMap_t* tileMap, Vector4i32_t viewport, i32 displayX, i32 displayY );

#endif // DISPLAY_H
