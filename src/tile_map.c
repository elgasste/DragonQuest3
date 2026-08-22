#include <stdio.h>

#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "tile.h"
#include "tile_map.h"
#include "platform.h"
#include "vector.h"

struct TileMap_t
{
   u32 id;
   u32 tilesX;
   u32 tilesY;
   b32 wraps;
   Tile_t* tiles;
};

size_t TileMap_GetStructSize( void )
{
   return sizeof( TileMap_t );
}

TileMap_t* TileMap_CreateFromGameData( MemArena_t *memArena, GameData_t* gameData, u32 tileMapId )
{
   u32 tileMapCount, i;
   i32 chunkOffset, tileMapOffset, tileCount, tilesOffset;
   TileMap_t *tileMap;
   GameDataFileOffsets_t fileOffsets;
   GameDataObjectOffset_t tileMapFileOffset;
   File_t* file;
   u8* tiles;
   char msg[STRING_SIZE_DEFAULT];

   fileOffsets = GameData_GetFileOffsets( gameData );
   tileMapCount = GameData_GetTileMapCount( gameData );
   file = GameData_GetFile( gameData );
   
   for ( i = 0; i < tileMapCount; i++ )
   {
      tileMapFileOffset = GameData_GetTileMapOffset( gameData, tileMapId );

      if ( tileMapFileOffset.id == tileMapId )
      {
         chunkOffset = fileOffsets.tileMaps;
         tileMapOffset = chunkOffset + tileMapFileOffset.offset;
         if ( (i32)( tileMapOffset + TileMap_GetStructSize() ) > file->size )
         {
            Platform_FatalError( "game data file is too small to contain the requested tile map." );
            return 0;
         }

         tileMap = (TileMap_t*)MemArena_AllocMem( memArena, TileMap_GetStructSize() );
         Platform_FileSeek( file, tileMapOffset, 0 );
         Platform_ReadFileBytes( file, (u8*)( tileMap ), sizeof( TileMap_t ) );

         tileCount = (i32)( tileMap->tilesX * tileMap->tilesY );
         tilesOffset = tileMapOffset + sizeof( TileMap_t );
         if ( tilesOffset + (i32)( tileCount * Tile_GetStructSize() ) > file->size )
         {
            Platform_FatalError( "game data file is too small to contain all the requested tile map tiles." );
            return 0;
         }

         tiles = (u8*)MemArena_AllocMem( memArena, tileCount * Tile_GetStructSize() );
         Platform_FileSeek( file, tilesOffset, 0 );
         Platform_ReadFileBytes( file, tiles, tileCount * Tile_GetStructSize() );
         tileMap->tiles = (Tile_t*)tiles;

         return tileMap;
      }
   }

   snprintf( msg, STRING_SIZE_DEFAULT, "failed to load tile map with ID %u: not found in game data file.", tileMapId );
   Platform_FatalError( msg );
   return 0;
}

void TileMap_Free( TileMap_t* tileMap, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, tileMap->tiles );
   MemArena_FreeMem( memArena, tileMap );
}

u32 TileMap_GetId( TileMap_t* tileMap )
{
   return tileMap->id;
}

u32 TileMap_GetTilesX( TileMap_t* tileMap )
{
   return tileMap->tilesX;
}

u32 TileMap_GetTilesY( TileMap_t* tileMap )
{
   return tileMap->tilesY;
}

b32 TileMap_GetWraps( TileMap_t* tileMap )
{
   return tileMap->wraps;
}

Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 x, u32 y )
{
   return (Tile_t*)( (u8*)tileMap->tiles + ( y * tileMap->tilesX + x ) * Tile_GetStructSize() );
}

void TileMap_AnchorViewportToPoint( TileMap_t* tileMap, Vector4i32_t* viewport, u32 x, u32 y, u32 tileSize )
{
   i32 newViewportX, newViewportY, halfViewportW, halfViewportH, tileMapW, tileMapH;

   halfViewportW = (i32)( viewport->w / 2 );
   halfViewportH = (i32)( viewport->h / 2 );

   tileMapW = (i32)tileMap->tilesX * (i32)tileSize;
   tileMapH = (i32)tileMap->tilesY * (i32)tileSize;

   newViewportX = (i32)x - halfViewportW;
   newViewportY = (i32)y - halfViewportH;

   if ( !tileMap->wraps )
   {
      if ( viewport->w >= tileMapW )
      {
         newViewportX = -(i32)( ( viewport->w - tileMapW ) / 2 );
      }
      else if ( newViewportX < 0 )
      {
         newViewportX = 0;
      }
      else if ( newViewportX > ( tileMapW - viewport->w ) )
      {
         newViewportX = (i32)( tileMapW - viewport->w );
      }

      if ( viewport->h >= tileMapH )
      {
         newViewportY = -(i32)( ( viewport->h - tileMapH ) / 2 );
      }
      else if ( newViewportY < 0 )
      {
         newViewportY = 0;
      }
      else if ( newViewportY > ( tileMapH - viewport->h ) )
      {
         newViewportY = (i32)( tileMapH - viewport->h );
      }
   }

   viewport->x = newViewportX;
   viewport->y = newViewportY;
}
