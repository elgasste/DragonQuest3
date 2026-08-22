#if !defined( TILE_H )
#define TILE_H

#include "common.h"

typedef struct MemArena_t MemArena_t;

typedef struct Tile_t Tile_t;
size_t Tile_GetStructSize( void );
Tile_t* Tile_Create( MemArena_t* memArena );
void Tile_Free( Tile_t* tile, MemArena_t* memArena );

u32 Tile_GetTextureIndex( Tile_t* tile );

#endif // TILE_H
