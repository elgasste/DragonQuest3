#include <stdio.h>

#include "entity.h"
#include "file.h"
#include "game_data.h"
#include "mem_arena.h"
#include "tile_map.h"
#include "utility.h"

PACKED_STRUCT
struct Tile_t
{
   u32 textureIndex;
   b32 isPassable;
};
END_PACKED_STRUCT

size_t Tile_GetStructSize( void )
{
   return sizeof( Tile_t );
}

u32 Tile_GetTextureIndex( Tile_t* tile )
{
   return tile->textureIndex;
}

b32 Tile_GetIsPassable( Tile_t* tile )
{
   return tile->isPassable;
}

void Tile_SetTextureIndex( Tile_t* tile, u32 textureIndex )
{
   tile->textureIndex = textureIndex;
}

void Tile_SetIsPassable( Tile_t* tile, b32 isPassable )
{
   tile->isPassable = isPassable;
}

PACKED_STRUCT
struct TileMapPortal_t
{
   u32 sourceTileIndex;
   u32 destinationTileMapId;
   u32 destinationTileIndex;
};
END_PACKED_STRUCT

size_t TileMapPortal_GetStructSize( void )
{
   return sizeof( TileMapPortal_t );
}

u32 TileMapPortal_GetSourceTileIndex( TileMapPortal_t* portal )
{
   return portal->sourceTileIndex;
}

u32 TileMapPortal_GetDestinationTileMapId( TileMapPortal_t* portal )
{
   return portal->destinationTileMapId;
}

u32 TileMapPortal_GetDestinationTileIndex( TileMapPortal_t* portal )
{
   return portal->destinationTileIndex;
}

struct TileMap_t
{
   TileMapInfo_t info;
   Tile_t* tiles;
   TileMapPortal_t* portals;

   u32 tileSizePixels;
   Vector4i32_t viewportInUnits;
   Vector4i32_t viewportInPixels;
};

size_t TileMap_GetStructSize( void )
{
   return sizeof( TileMap_t );
}

TileMap_t* TileMap_CreateFromGameData( MemArena_t *memArena, GameData_t* gameData, u32 tileMapId, u32 tileSizePixels )
{
   u32 tileMapCount, i;
   i32 chunkOffset, tileMapOffset, tileCount, tilesOffset;
   TileMap_t *tileMap;
   GameDataFileOffsets_t fileOffsets;
   GameDataObjectOffset_t tileMapFileOffset;
   File_t* file;
   u8 *tiles, *portals;
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
         if ( (i32)( tileMapOffset + sizeof( TileMapInfo_t ) ) > file->size )
         {
            Platform_FatalError( "game data file is too small to contain the requested tile map." );
            return 0;
         }

         tileMap = (TileMap_t*)MemArena_AllocMem( memArena, TileMap_GetStructSize() );
         tileMap->tileSizePixels = tileSizePixels;
         tileMap->portals = 0;
         Platform_FileSeek( file, tileMapOffset, 0 );
         Platform_ReadFileBytes( file, (u8*)( &tileMap->info ), sizeof( TileMapInfo_t ) );

         tileCount = (i32)( tileMap->info.tilesX * tileMap->info.tilesY );
         tilesOffset = tileMapOffset + sizeof( TileMapInfo_t );
         if ( tilesOffset + (i32)( tileCount * sizeof( Tile_t ) ) > file->size )
         {
            Platform_FatalError( "game data file is too small to contain all the requested tile map tiles." );
            MemArena_FreeMem( memArena, tileMap );
            return 0;
         }

         tiles = (u8*)MemArena_AllocMem( memArena, tileCount * sizeof( Tile_t ) );
         Platform_FileSeek( file, tilesOffset, 0 );
         Platform_ReadFileBytes( file, tiles, tileCount * sizeof( Tile_t ) );
         tileMap->tiles = (Tile_t*)tiles;

         if ( tileMap->info.portalCount > 0 )
         {
            portals = (u8*)MemArena_AllocMem( memArena, tileMap->info.portalCount * sizeof( TileMapPortal_t ) );
            Platform_FileSeek( file, tilesOffset + tileCount * sizeof( Tile_t ), 0 );
            Platform_ReadFileBytes( file, portals, tileMap->info.portalCount * sizeof( TileMapPortal_t ) );
            tileMap->portals = (TileMapPortal_t*)portals;
         }

         return tileMap;
      }
   }

   snprintf( msg, STRING_SIZE_DEFAULT, "failed to load tile map with ID %u: not found in game data file.", tileMapId );
   Platform_FatalError( msg );
   return 0;
}

void TileMap_Free( TileMap_t* tileMap, MemArena_t* memArena )
{
   if ( tileMap->portals )
   {
      MemArena_FreeMem( memArena, tileMap->portals );
   }
   MemArena_FreeMem( memArena, tileMap->tiles );
   MemArena_FreeMem( memArena, tileMap );
}

u32 TileMap_GetId( TileMap_t* tileMap )
{
   return tileMap->info.id;
}

u32 TileMap_GetTilesX( TileMap_t* tileMap )
{
   return tileMap->info.tilesX;
}

u32 TileMap_GetTilesY( TileMap_t* tileMap )
{
   return tileMap->info.tilesY;
}

b32 TileMap_GetWraps( TileMap_t* tileMap )
{
   return tileMap->info.wraps;
}

u32 TileMap_GetPortalCount( TileMap_t* tileMap )
{
   return tileMap->info.portalCount;
}

Tile_t* TileMap_GetTile( TileMap_t* tileMap, u32 tileIndex )
{
   return (Tile_t*)( (u8*)tileMap->tiles + tileIndex * sizeof( Tile_t ) );
}

TileMapPortal_t* TileMap_GetPortal( TileMap_t* tileMap, u32 tileIndex )
{
   u32 i;
   TileMapPortal_t* portal;
   
   for ( i = 0; i < tileMap->info.portalCount; i++ )
   {
      portal = &tileMap->portals[i];
      
      if ( portal->sourceTileIndex == tileIndex )
      {
         return portal;
      }
   }

   return 0;
}

Vector4i32_t TileMap_GetViewportInUnits( TileMap_t* tileMap )
{
   return tileMap->viewportInUnits;
}

Vector4i32_t TileMap_GetViewportInPixels( TileMap_t* tileMap )
{
   return tileMap->viewportInPixels;
}

void TileMap_SetViewportInUnits( TileMap_t* tileMap, Vector4i32_t viewportInUnits )
{
   tileMap->viewportInUnits = viewportInUnits;

   tileMap->viewportInPixels.x = viewportInUnits.x / WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInPixels.y = viewportInUnits.y / WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInPixels.w = viewportInUnits.w / WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInPixels.h = viewportInUnits.h / WORLD_UNITS_PER_PIXEL;
}

void TileMap_SetViewportInPixels( TileMap_t* tileMap, Vector4i32_t viewportInPixels )
{
   tileMap->viewportInPixels = viewportInPixels;

   tileMap->viewportInUnits.x = viewportInPixels.x * WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInUnits.y = viewportInPixels.y * WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInUnits.w = viewportInPixels.w * WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInUnits.h = viewportInPixels.h * WORLD_UNITS_PER_PIXEL;
}

void TileMap_AnchorViewportToPointUnits( TileMap_t* tileMap, u32 x, u32 y )
{
   i32 newViewportX, newViewportY, halfViewportW, halfViewportH, tileMapW, tileMapH;
   Vector4i32_t viewport;

   viewport = tileMap->viewportInUnits;

   halfViewportW = (i32)( viewport.w / 2 );
   halfViewportH = (i32)( viewport.h / 2 );

   tileMapW = (i32)tileMap->info.tilesX * (i32)tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL;
   tileMapH = (i32)tileMap->info.tilesY * (i32)tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL;

   newViewportX = (i32)x - halfViewportW;
   newViewportY = (i32)y - halfViewportH;

   if ( !tileMap->info.wraps )
   {
      if ( viewport.w >= tileMapW )
      {
         newViewportX = -(i32)( ( viewport.w - tileMapW ) / 2 );
      }
      else if ( newViewportX < 0 )
      {
         newViewportX = 0;
      }
      else if ( newViewportX > ( tileMapW - viewport.w ) )
      {
         newViewportX = (i32)( tileMapW - viewport.w );
      }

      if ( viewport.h >= tileMapH )
      {
         newViewportY = -(i32)( ( viewport.h - tileMapH ) / 2 );
      }
      else if ( newViewportY < 0 )
      {
         newViewportY = 0;
      }
      else if ( newViewportY > ( tileMapH - viewport.h ) )
      {
         newViewportY = (i32)( tileMapH - viewport.h );
      }
   }

   tileMap->viewportInUnits.x = newViewportX;
   tileMap->viewportInUnits.y = newViewportY;
   tileMap->viewportInPixels.x = newViewportX / WORLD_UNITS_PER_PIXEL;
   tileMap->viewportInPixels.y = newViewportY / WORLD_UNITS_PER_PIXEL;
}

void TileMap_AnchorViewportToEntity( TileMap_t* tileMap, Entity_t* entity )
{
   Vector4i32_t entityRect;

   entityRect = Entity_GetRect( entity );
   TileMap_AnchorViewportToPointUnits( tileMap, entityRect.x + ( entityRect.w / 2 ), entityRect.y + ( entityRect.h / 2 ) );
}

void TileMap_WrapEntityPosition( TileMap_t* tileMap, Entity_t* entity )
{
   Vector4i32_t entityRect;
   i32 mapWidth, mapHeight;

   entityRect = Entity_GetRect( entity );
   mapWidth = (i32)tileMap->info.tilesX * (i32)tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL;
   mapHeight = (i32)tileMap->info.tilesY * (i32)tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL;

   if ( mapWidth > 0 )
   {
      entityRect.x %= mapWidth;
      if ( entityRect.x < 0 )
      {
         entityRect.x += mapWidth;
      }
   }
   if ( mapHeight > 0 )
   {
      entityRect.y %= mapHeight;
      if ( entityRect.y < 0 )
      {
         entityRect.y += mapHeight;
      }
   }

   Entity_SetPosition( entity, entityRect.x, entityRect.y );
}

u32 TileMap_GetTileIndexForEntity( TileMap_t* tileMap, Entity_t* entity )
{
   Vector4i32_t entityRect;
   i32 tileX, tileY, tilesX, tilesY;

   entityRect = Entity_GetRect( entity );
   tileX = Utility_FloorDiv32i( entityRect.x + ( entityRect.w / 2 ), tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL );
   tileY = Utility_FloorDiv32i( entityRect.y + ( entityRect.h / 2 ), tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL );

   tilesX = (i32)tileMap->info.tilesX;
   tilesY = (i32)tileMap->info.tilesY;
   if ( tileMap->info.wraps )
   {
      tileX %= tilesX;
      tileY %= tilesY;
      if ( tileX < 0 )
      {
         tileX += tilesX;
      }
      if ( tileY < 0 )
      {
         tileY += tilesY;
      }
   }

   return (u32)tileY * tileMap->info.tilesX + (u32)tileX;
}

void TileMap_CenterEntityInTile( TileMap_t* tileMap, Entity_t* entity, u32 tileIndex )
{
   u32 tileX, tileY;
   i32 entityCenterOffsetX, entityCenterOffsetY;
   Vector4i32_t entityRect;

   tileX = tileIndex % tileMap->info.tilesX;
   tileY = tileIndex / tileMap->info.tilesX;

   entityRect = Entity_GetRect( entity );
   entityCenterOffsetX = (i32)( entityRect.w / 2 );
   entityCenterOffsetY = (i32)( entityRect.h / 2 );

   Entity_SetPosition( entity,
      (i32)( tileX * tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL + ( tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL / 2 ) - entityCenterOffsetX ),
      (i32)( tileY * tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL + ( tileMap->tileSizePixels * WORLD_UNITS_PER_PIXEL / 2 ) - entityCenterOffsetY ) );
   Entity_SetTileIndex( entity, tileIndex );
}
