#include <stdio.h>
#include <stdlib.h>

#include "direction.h"
#include "game_data.h"
#include "platform.h"
#include "version.h"
#include "win_common.h"

PACKED_STRUCT
typedef struct TileTextureSetInfoMock_t
{
   u32 count;
   u32 tileSize;
}
TileTextureSetInfoMock_t;
END_PACKED_STRUCT

typedef struct TileTextureSetMock_t
{
   TileTextureSetInfoMock_t info;
   u32* textures;
}
TileTextureSetMock_t;

PACKED_STRUCT
typedef struct ActiveSpriteTextureSetInfoMock_t
{
   u32 count;
   u32 frameSize;
   u32 frameCount;
}
ActiveSpriteTextureSetInfoMock_t;
END_PACKED_STRUCT

typedef struct ActiveSpriteTextureSetMock_t
{
   ActiveSpriteTextureSetInfoMock_t info;
   u32* textures;
}
ActiveSpriteTextureSetMock_t;

typedef struct TileMock_t
{
   u32 textureIndex;
   b32 isPassable;
}
TileMock_t;

PACKED_STRUCT
typedef struct TileMapPortalMock_t
{
   u32 sourceTileIndex;
   u32 destinationTileMapId;
   u32 destinationTileIndex;
}
TileMapPortalMock_t;
END_PACKED_STRUCT

typedef struct TileMapInfoMock_t
{
   u32 id;
   u32 tilesX;
   u32 tilesY;
   b32 wraps;
   u32 portalCount;
}
TileMapInfoMock_t;

typedef struct TileMapMock_t
{
   TileMapInfoMock_t info;
   TileMock_t* tiles;
   TileMapPortalMock_t* portals;
}
TileMapMock_t;

internal TileTextureSetMock_t* CreateTestTileTextureSet( void );
internal ActiveSpriteTextureSetMock_t* CreateTestActiveSpriteTextureSet( void );
internal TileMapMock_t* CreateTestTileMaps( u32* tileMapCount );
internal b32 WriteTestGameDataHeader( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* textureSet, ActiveSpriteTextureSetMock_t* activeSpriteTextureSet );
internal b32 WriteTestGameDataTileTextureSet( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* textureSet );
internal b32 WriteTestGameDataActiveSpriteTextureSet( HANDLE hFile, DWORD* filePos, ActiveSpriteTextureSetMock_t* textureSet );
internal b32 WriteTestGameDataTileMaps( HANDLE hFile, DWORD* filePos, TileMapMock_t* tileMaps, u32 tileMapCount );

void WriteTestGameDataFile( const char* filePath )
{
   HANDLE hFile;
   DWORD filePos;
   TileTextureSetMock_t* tileTextureSet;
   ActiveSpriteTextureSetMock_t* activeSpriteTextureSet;
   TileMapMock_t* tileMaps;
   u32 tileMapCount, i;
   char msg[STRING_SIZE_DEFAULT];

   hFile = CreateFileA( filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
   filePos = 0;

   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to create test game data file: %lu", GetLastError() );
      Platform_FatalError( msg );
   }

   tileTextureSet = CreateTestTileTextureSet();
   activeSpriteTextureSet = CreateTestActiveSpriteTextureSet();
   tileMapCount = 0;
   tileMaps = CreateTestTileMaps( &tileMapCount );

   if ( !WriteTestGameDataHeader( hFile, &filePos, tileTextureSet, activeSpriteTextureSet ) )
   {
      Platform_FatalError( "failed to write test game data file header." );
   }
   if ( !WriteTestGameDataTileTextureSet( hFile, &filePos, tileTextureSet ) )
   {
      Platform_FatalError( "failed to write test game data file tile texture set." );
   }
   if ( !WriteTestGameDataActiveSpriteTextureSet( hFile, &filePos, activeSpriteTextureSet ) )
   {
      Platform_FatalError( "failed to write test game data file active sprite texture set." );
   }
   if ( !WriteTestGameDataTileMaps( hFile, &filePos, tileMaps, tileMapCount ) )
   {
      Platform_FatalError( "failed to write test game data file tile maps." );
   }

   free( tileTextureSet->textures );
   free( tileTextureSet );
   free( activeSpriteTextureSet->textures );
   free( activeSpriteTextureSet );
   for ( i = 0; i < tileMapCount; i++ )
   {
      free( tileMaps[i].tiles );
      free( tileMaps[i].portals );
   }
   free( tileMaps );

   CloseHandle( hFile );
}

internal u32 LandscapeTilePixel( u32 tileIndex, u32 x, u32 y )
{
   u32 speckle = ( ( x * 7 + y * 13 ) % 5 == 0 );

   if ( tileIndex == 0 ) // grass
   {
      if ( speckle )
      {
         return 0xFF000000u | RGB( 60, 140, 50 );
      }
      return 0xFF000000u | RGB( 80, 170, 70 );
   }
   else if ( tileIndex == 1 ) // dirt
   {
      if ( speckle )
      {
         return 0xFF000000u | RGB( 110, 80, 50 );
      }
      return 0xFF000000u | RGB( 140, 100, 65 );
   }
   else if ( tileIndex == 2 ) // sand
   {
      if ( speckle )
      {
         return 0xFF000000u | RGB( 220, 195, 130 );
      }
      return 0xFF000000u | RGB( 235, 210, 150 );
   }
   else if ( tileIndex == 3 ) // water
   {
      if ( ( x + ( y % 4 ) ) % 6 < 2 )
      {
         return 0xFF000000u | RGB( 60, 110, 200 );
      }
      return 0xFF000000u | RGB( 80, 140, 220 );
   }
   else if ( tileIndex == 4 ) // stone
   {
      if ( speckle )
      {
         return 0xFF000000u | RGB( 110, 110, 115 );
      }
      return 0xFF000000u | RGB( 140, 140, 145 );
   }
   else if ( tileIndex == 5 ) // path
   {
      if ( speckle )
      {
         return 0xFF000000u | RGB( 190, 160, 110 );
      }
      return 0xFF000000u | RGB( 205, 175, 125 );
   }
   else if ( tileIndex == 6 ) // snow
   {
      if ( speckle )
      {
         return 0xFF000000u | RGB( 225, 230, 235 );
      }
      return 0xFF000000u | RGB( 245, 248, 250 );
   }
   else if ( tileIndex == 7 ) // tree
   {
      if ( y < 6 )
      {
         return ( speckle ) ? ( 0xFF000000u | RGB( 40, 110, 45 ) ) : ( 0xFF000000u | RGB( 55, 130, 55 ) );
      }
      if ( x >= 6 && x <= 9 )
      {
         return 0xFF000000u | RGB( 90, 60, 35 );
      }
      return 0xFF000000u | RGB( 80, 170, 70 );
   }
   else if ( tileIndex == 8 ) // bush
   {
      if ( x >= 3 && x <= 12 && y >= 4 && y <= 11 )
      {
         return ( speckle ) ? ( 0xFF000000u | RGB( 50, 120, 55 ) ) : ( 0xFF000000u | RGB( 65, 140, 60 ) );
      }
      return 0xFF000000u | RGB( 80, 170, 70 );
   }
   else // mountain
   {
      if ( y + ( x / 2 ) < 10 )
      {
         return 0xFF000000u | RGB( 130, 130, 135 );
      }
      return 0xFF000000u | RGB( 100, 100, 105 );
   }
}

internal TileTextureSetMock_t* CreateTestTileTextureSet( void )
{
   u32 pixel, tilePixels, tileIndex, pixelIndex, x, y;
   TileTextureSetMock_t* textureSet;

   textureSet = (TileTextureSetMock_t*)malloc( sizeof( TileTextureSetMock_t ) );
   textureSet->info.count = 11;
   textureSet->info.tileSize = 16;
   textureSet->textures = (u32*)malloc( textureSet->info.count * textureSet->info.tileSize * textureSet->info.tileSize * sizeof( u32 ) );

   tilePixels = textureSet->info.tileSize * textureSet->info.tileSize;

   for ( tileIndex = 0; tileIndex < textureSet->info.count - 1; tileIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
      {
         x = pixelIndex % textureSet->info.tileSize;
         y = pixelIndex / textureSet->info.tileSize;
         pixel = LandscapeTilePixel( tileIndex, x, y );
         textureSet->textures[tileIndex * tilePixels + pixelIndex] = pixel;
      }
   }

   // this is for portals
   for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
   {
      x = pixelIndex % textureSet->info.tileSize;
      y = pixelIndex / textureSet->info.tileSize;
      pixel = 0xFFFF0000u;
      textureSet->textures[( textureSet->info.count - 1 ) * tilePixels + pixelIndex] = pixel;
   }

   return textureSet;
}

internal u32* CreateArrowTileTexture( u32 tileSize, u32 color, Direction_t dir )
{
   i32 center, forward, side, shaftHalfWidth;
   u32* texture;

   texture = (u32*)malloc( tileSize * tileSize * sizeof( u32 ) );
   center = (i32)( tileSize / 2 );
   shaftHalfWidth = (i32)( tileSize / 8 );
   if ( shaftHalfWidth < 1 )
   {
      shaftHalfWidth = 1;
   }

   for ( u32 y = 0; y < tileSize; y++ )
   {
      for ( u32 x = 0; x < tileSize; x++ )
      {
         switch ( dir )
         {
            case Direction_Left:
               forward = center - (i32)x;
               side = (i32)y - center;
               break;
            case Direction_Up:
               forward = center - (i32)y;
               side = (i32)x - center;
               break;
            case Direction_Right:
               forward = (i32)x - center;
               side = (i32)y - center;
               break;
            case Direction_Down:
               forward = (i32)y - center;
               side = (i32)x - center;
               break;
            default:
               forward = -1;
               side = 0;
               break;
         }

         if ( ( forward >= -(i32)( tileSize / 2 ) && abs( side ) <= shaftHalfWidth ) ||
              ( forward >= 0 && abs( side ) <= ( (i32)( tileSize / 2 ) - forward ) ) )
         {
            texture[y * tileSize + x] = color;
         }
         else
         {
            texture[y * tileSize + x] = 0;
         }
      }
   }

   return texture;
}

internal ActiveSpriteTextureSetMock_t* CreateTestActiveSpriteTextureSet( void )
{
   u32 spriteIndex, color1, color2;
   u32 *spriteTexture, *arrowTexture1, *arrowTexture2;
   Direction_t dir;
   ActiveSpriteTextureSetMock_t* textureSet;

   textureSet = (ActiveSpriteTextureSetMock_t*)malloc( sizeof( ActiveSpriteTextureSetMock_t ) );
   textureSet->info.count = 2;
   textureSet->info.frameSize = 16;
   textureSet->info.frameCount = 2;
   textureSet->textures = (u32*)malloc( textureSet->info.count * textureSet->info.frameSize * textureSet->info.frameSize * textureSet->info.frameCount * Direction_Count * sizeof( u32 ) );

   for ( spriteIndex = 0; spriteIndex < textureSet->info.count; spriteIndex++ )
   {
      spriteTexture = &textureSet->textures[spriteIndex * textureSet->info.frameSize * textureSet->info.frameSize * textureSet->info.frameCount * Direction_Count];

      switch( spriteIndex )
      {
         case 0: color1 = 0xFF000000u | RGB( 255, 0, 0 ); color2 = 0xFF000000u | RGB( 200, 0, 0 ); break;
         case 1: color1 = 0xFF000000u | RGB( 0, 255, 0 ); color2 = 0xFF000000u | RGB( 0, 200, 0 ); break;
         default: color1 = 0xFF000000u | RGB( 255, 255, 255 ); color2 = 0xFF000000u | RGB( 200, 200, 200 ); break;
      }

      for ( dir = 0; dir < Direction_Count; dir++ )
      {
         arrowTexture1 = CreateArrowTileTexture( textureSet->info.frameSize, color1, dir );
         arrowTexture2 = CreateArrowTileTexture( textureSet->info.frameSize, color2, dir );

         memcpy( &spriteTexture[dir * textureSet->info.frameSize * textureSet->info.frameSize * textureSet->info.frameCount], arrowTexture1, textureSet->info.frameSize * textureSet->info.frameSize * sizeof( u32 ) );
         memcpy( &spriteTexture[dir * textureSet->info.frameSize * textureSet->info.frameSize * textureSet->info.frameCount + ( textureSet->info.frameSize * textureSet->info.frameSize )], arrowTexture2, textureSet->info.frameSize * textureSet->info.frameSize * sizeof( u32 ) );

         free( arrowTexture1 );
         free( arrowTexture2 );
      }
   }

   return textureSet;
}

internal TileMapMock_t* CreateTestTileMaps( u32* tileMapCount )
{
   u32 i, index;
   TileMapMock_t *tileMaps, *curTileMap;

   *tileMapCount = 5;
   tileMaps = (TileMapMock_t*)malloc( *tileMapCount * sizeof( TileMapMock_t ) );

   // 0: 10x10 checkerboard, no wrapping
   curTileMap = tileMaps;
   curTileMap->info.id = 0;
   curTileMap->info.tilesX = 10;
   curTileMap->info.tilesY = 10;
   curTileMap->info.wraps = False;

   curTileMap->info.portalCount = 1;
   curTileMap->portals = (TileMapPortalMock_t*)malloc( curTileMap->info.portalCount * sizeof( TileMapPortalMock_t ) );
   curTileMap->portals[0].sourceTileIndex = 74;
   curTileMap->portals[0].destinationTileMapId = 2;
   curTileMap->portals[0].destinationTileIndex = 0;

   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->info.tilesX * curTileMap->info.tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->info.tilesX * curTileMap->info.tilesY; i++ )
   {
      if ( i == 74 )
      {
         curTileMap->tiles[i].textureIndex = 10; // portal tile
      }
      else if ( ( i / curTileMap->info.tilesX ) % 2 == 0 )
      {
         if ( ( i % curTileMap->info.tilesX ) % 2 == 0 )
         {
            curTileMap->tiles[i].textureIndex = 1; // white
         }
         else
         {
            curTileMap->tiles[i].textureIndex = 0; // black
         }
      }
      else
      {
         if ( ( i % curTileMap->info.tilesX ) % 2 == 0 )
         {
            curTileMap->tiles[i].textureIndex = 0; // black
         }
         else
         {
            curTileMap->tiles[i].textureIndex = 1; // white
         }
      }

      curTileMap->tiles[i].isPassable = True;
   }

   // 1: 256x256 random, wrapping
   curTileMap++;
   curTileMap->info.id = 1;
   curTileMap->info.tilesX = 256;
   curTileMap->info.tilesY = 256;
   curTileMap->info.wraps = True;

   curTileMap->info.portalCount = 1;
   curTileMap->portals = (TileMapPortalMock_t*)malloc( curTileMap->info.portalCount * sizeof( TileMapPortalMock_t ) );
   curTileMap->portals[0].sourceTileIndex = 5130;
   curTileMap->portals[0].destinationTileMapId = 0;
   curTileMap->portals[0].destinationTileIndex = 38;

   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->info.tilesX * curTileMap->info.tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->info.tilesX * curTileMap->info.tilesY; i++ )
   {
      // random
      index = Platform_Rand_u32Ranged( 0, 9 );
      curTileMap->tiles[i].textureIndex = index;
      curTileMap->tiles[i].isPassable = index == 7 ? False : True;

      if ( i == 5130 )
      {
         curTileMap->tiles[i].textureIndex = 10;
         curTileMap->tiles[i].isPassable = True;
      }
   }

   // make the edges all the same so we can test wrapping
   for ( i = 0; i < curTileMap->info.tilesX; i++ )
   {
      curTileMap->tiles[i].textureIndex = 2; // top edge
      curTileMap->tiles[i].isPassable = True;
      curTileMap->tiles[( curTileMap->info.tilesY - 1 ) * curTileMap->info.tilesX + i].textureIndex = 2; // bottom edge
      curTileMap->tiles[( curTileMap->info.tilesY - 1 ) * curTileMap->info.tilesX + i].isPassable = True;
   }
   for ( i = 0; i < curTileMap->info.tilesY; i++ )
   {
      curTileMap->tiles[i * curTileMap->info.tilesX].textureIndex = 2; // left edge
      curTileMap->tiles[i * curTileMap->info.tilesX].isPassable = True;
      curTileMap->tiles[i * curTileMap->info.tilesX + ( curTileMap->info.tilesX - 1 )].textureIndex = 2; // right edge
      curTileMap->tiles[i * curTileMap->info.tilesX + ( curTileMap->info.tilesX - 1 )].isPassable = True;
   }

   // 2: 128x128 random, no wrapping
   curTileMap++;
   curTileMap->info.id = 2;
   curTileMap->info.tilesX = 128;
   curTileMap->info.tilesY = 128;
   curTileMap->info.wraps = False;

   curTileMap->info.portalCount = 1;
   curTileMap->portals = (TileMapPortalMock_t*)malloc( curTileMap->info.portalCount * sizeof( TileMapPortalMock_t ) );
   curTileMap->portals[0].sourceTileIndex = 2320;
   curTileMap->portals[0].destinationTileMapId = 4;
   curTileMap->portals[0].destinationTileIndex = 10000;

   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->info.tilesX * curTileMap->info.tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->info.tilesX * curTileMap->info.tilesY; i++ )
   {
      if ( i == 2320 )
      {
         // portal
         curTileMap->tiles[i].textureIndex = 10;
         curTileMap->tiles[i].isPassable = True;
      }
      else
      {
         // random
         index = Platform_Rand_u32Ranged( 0, 9 );
         curTileMap->tiles[i].textureIndex = index;
         curTileMap->tiles[i].isPassable = index == 7 ? False : True;
      }
   }

   // 3: 3x3, no wrapping
   curTileMap++;
   curTileMap->info.id = 3;
   curTileMap->info.tilesX = 3;
   curTileMap->info.tilesY = 3;
   curTileMap->info.wraps = False;
   
   curTileMap->info.portalCount = 2;
   curTileMap->portals = (TileMapPortalMock_t*)malloc( curTileMap->info.portalCount * sizeof( TileMapPortalMock_t ) );
   curTileMap->portals[0].sourceTileIndex = 0;
   curTileMap->portals[0].destinationTileMapId = 1;
   curTileMap->portals[0].destinationTileIndex = 10000;
   curTileMap->portals[1].sourceTileIndex = 2;
   curTileMap->portals[1].destinationTileMapId = 1;
   curTileMap->portals[1].destinationTileIndex = 5140;

   curTileMap->tiles = 0;
   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->info.tilesX * curTileMap->info.tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->info.tilesX * curTileMap->info.tilesY; i++ )
   {
      if ( i == 0 || i == 2 )
      {
         // portal
         curTileMap->tiles[i].textureIndex = 10;
      }
      else
      {
         curTileMap->tiles[i].textureIndex = 1;
      }

      curTileMap->tiles[i].isPassable = True;
   }

   // 4: 256x256 random, no wrapping
   curTileMap++;
   curTileMap->info.id = 4;
   curTileMap->info.tilesX = 256;
   curTileMap->info.tilesY = 256;
   curTileMap->info.wraps = False;
   
   curTileMap->info.portalCount = 1;
   curTileMap->portals = (TileMapPortalMock_t*)malloc( curTileMap->info.portalCount * sizeof( TileMapPortalMock_t ) );
   curTileMap->portals[0].sourceTileIndex = 1555;
   curTileMap->portals[0].destinationTileMapId = 3;
   curTileMap->portals[0].destinationTileIndex = 8;

   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->info.tilesX * curTileMap->info.tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->info.tilesX * curTileMap->info.tilesY; i++ )
   {
      if ( i == 1555 )
      {
         // portal
         curTileMap->tiles[i].textureIndex = 10;
         curTileMap->tiles[i].isPassable = True;
      }
      else
      {
         // random
         index = Platform_Rand_u32Ranged( 0, 9 );
         curTileMap->tiles[i].textureIndex = index;
         curTileMap->tiles[i].isPassable = index == 7 ? False : True;
      }
   }

   return tileMaps;
}

internal b32 WriteTestGameDataHeader( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* tileTextureSet, ActiveSpriteTextureSetMock_t* activeSpriteTextureSet )
{
   DWORD bytesWritten;
   BOOL result;
   GameDataVersion_t version;
   GameDataFileOffsets_t offsets;
   char msg[STRING_SIZE_DEFAULT];

   bytesWritten = 0;
   result = WriteFile( hFile, "DW3D", 4, &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file header: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != 4 )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file header: wrote %lu of 4 bytes", bytesWritten );
      Platform_FatalError( msg );
      return False;
   }

   version.major = GAME_VERSION_MAJOR;
   version.minor = GAME_VERSION_MINOR;
   version.maint = GAME_VERSION_MAINT;

   bytesWritten = 0;
   result = WriteFile( hFile, &version, sizeof( GameDataVersion_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file version: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( GameDataVersion_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file version: wrote %lu of %lu bytes", bytesWritten, sizeof( GameDataVersion_t ) );
      Platform_FatalError( msg );
      return False;
   }

   offsets.tileTextureSet = 4 + sizeof( GameDataVersion_t ) + sizeof( GameDataFileOffsets_t );
   offsets.activeSpriteTextureSet = offsets.tileTextureSet + sizeof( TileTextureSetInfoMock_t ) + ( tileTextureSet->info.count * tileTextureSet->info.tileSize * tileTextureSet->info.tileSize * sizeof( u32 ) );
   offsets.tileMaps = offsets.activeSpriteTextureSet + sizeof( ActiveSpriteTextureSetInfoMock_t ) + ( activeSpriteTextureSet->info.count * activeSpriteTextureSet->info.frameSize * activeSpriteTextureSet->info.frameSize * activeSpriteTextureSet->info.frameCount * Direction_Count * sizeof( u32 ) );
   result = WriteFile( hFile, &offsets, sizeof( GameDataFileOffsets_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file offsets: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( GameDataFileOffsets_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file offsets: wrote %lu of %lu bytes", bytesWritten, sizeof( GameDataFileOffsets_t ) );
      Platform_FatalError( msg );
      return False;
   }

   return True;
}

internal b32 WriteTestGameDataTileTextureSet( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* textureSet )
{
   u32 tileIndex, pixelIndex, tilePixels, pixel;
   DWORD bytesWritten;
   BOOL result;
   char msg[STRING_SIZE_DEFAULT];

   bytesWritten = 0;
   result = WriteFile( hFile, &( textureSet->info ), sizeof( TileTextureSetInfoMock_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile texture set info: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( TileTextureSetInfoMock_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile texture set info: wrote %lu of %lu bytes", bytesWritten, sizeof( TileTextureSetInfoMock_t ) );
      Platform_FatalError( msg );
      return False;
   }

   tilePixels = textureSet->info.tileSize * textureSet->info.tileSize;
   
   for ( tileIndex = 0; tileIndex < textureSet->info.count; tileIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
      {
         pixel = textureSet->textures[tileIndex * tilePixels + pixelIndex];
         bytesWritten = 0;
         result = WriteFile( hFile, &pixel, sizeof( u32 ), &bytesWritten, NULL );
         *filePos += bytesWritten;

         if ( !result )
         {
            snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile textures: %lu", GetLastError() );
            Platform_FatalError( msg );
            return False;
         }
         else if ( bytesWritten != sizeof( u32 ) )
         {
            snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile textures: wrote %lu of %lu bytes", bytesWritten, sizeof( u32 ) );
            Platform_FatalError( msg );
            return False;
         }
      }
   }

   return True;
}

internal b32 WriteTestGameDataActiveSpriteTextureSet( HANDLE hFile, DWORD* filePos, ActiveSpriteTextureSetMock_t* textureSet )
{
   u32 spriteIndex, pixelIndex, texturePixels, pixel;
   DWORD bytesWritten;
   BOOL result;
   char msg[STRING_SIZE_DEFAULT];

   bytesWritten = 0;
   result = WriteFile( hFile, &textureSet->info, sizeof( ActiveSpriteTextureSetInfoMock_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file active sprite texture set info: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( ActiveSpriteTextureSetInfoMock_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file active sprite texture set info: wrote %lu of %lu bytes", bytesWritten, sizeof( ActiveSpriteTextureSetInfoMock_t ) );
      Platform_FatalError( msg );
      return False;
   }

   texturePixels = textureSet->info.frameSize * textureSet->info.frameSize * textureSet->info.frameCount * Direction_Count;
   
   for ( spriteIndex = 0; spriteIndex < textureSet->info.count; spriteIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < texturePixels; pixelIndex++ )
      {
         pixel = textureSet->textures[spriteIndex * texturePixels + pixelIndex];
         bytesWritten = 0;
         result = WriteFile( hFile, &pixel, sizeof( u32 ), &bytesWritten, NULL );
         *filePos += bytesWritten;

         if ( !result )
         {
            snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file active sprite textures: %lu", GetLastError() );
            Platform_FatalError( msg );
            return False;
         }
         else if ( bytesWritten != sizeof( u32 ) )
         {
            snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file active sprite textures: wrote %lu of %lu bytes", bytesWritten, sizeof( u32 ) );
            Platform_FatalError( msg );
            return False;
         }
      }
   }

   return True;
}

internal b32 WriteTestGameDataTileMaps( HANDLE hFile, DWORD* filePos, TileMapMock_t* tileMaps, u32 tileMapCount )
{
   u32 i, j, tileAccum, portalAccum;
   DWORD bytesWritten;
   BOOL result;
   TileMock_t* tile;
   TileMapPortalMock_t* portal;
   GameDataObjectOffset_t* offsets;

   bytesWritten = 0;
   result = WriteFile( hFile, &tileMapCount, sizeof( u32 ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      Platform_FatalError( "failed to write test game data file tile maps." );
      return False;
   }
   else if ( bytesWritten != sizeof( u32 ) )
   {
      Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
      return False;
   }

   offsets = (GameDataObjectOffset_t*)malloc( tileMapCount * sizeof( GameDataObjectOffset_t ) );
   tileAccum = 0;
   portalAccum = 0;
   for ( i = 0; i < tileMapCount; i++ )
   {
      offsets[i].id = tileMaps[i].info.id;
      offsets[i].offset = sizeof( u32 )
         + ( tileMapCount * sizeof( GameDataObjectOffset_t ) )
         + ( i * sizeof( TileMapInfoMock_t ) )
         + ( tileAccum * sizeof( TileMock_t ) )
         + ( portalAccum * sizeof( TileMapPortalMock_t ) );
      tileAccum += tileMaps[i].info.tilesX * tileMaps[i].info.tilesY;
      portalAccum += tileMaps[i].info.portalCount;
   }

   result = WriteFile( hFile, offsets, tileMapCount * sizeof( GameDataObjectOffset_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;
   if ( !result )
   {
      Platform_FatalError( "failed to write test game data file tile maps offset table." );
      return False;
   }
   else if ( bytesWritten != tileMapCount * sizeof( GameDataObjectOffset_t ) )
   {
      Platform_FatalError( "failed to write test game data file tile maps offset table: wrote incorrect number of bytes." );
      return False;
   }

   for ( i = 0; i < tileMapCount; i++ )
   {
      bytesWritten = 0;
      result = WriteFile( hFile, &( tileMaps[i].info ), sizeof( TileMapInfoMock_t ), &bytesWritten, NULL );
      *filePos += bytesWritten;

      if ( !result )
      {
         Platform_FatalError( "failed to write test game data file tile maps." );
         return False;
      }
      else if ( bytesWritten != sizeof( TileMapInfoMock_t ) )
      {
         Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
         return False;
      }

      for ( j = 0; j < tileMaps[i].info.tilesX * tileMaps[i].info.tilesY; j++ )
      {
         tile = &( tileMaps[i].tiles[j] );
         
         bytesWritten = 0;
         result = WriteFile( hFile, tile, sizeof( TileMock_t ), &bytesWritten, NULL );
         *filePos += bytesWritten;

         if ( !result )
         {
            Platform_FatalError( "failed to write test game data file tile maps." );
            return False;
         }
         else if ( bytesWritten != sizeof( TileMock_t ) )
         {
            Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
            return False;
         }
      }

      for ( j = 0; j < tileMaps[i].info.portalCount; j++ )
      {
         portal = &( tileMaps[i].portals[j] );
         
         bytesWritten = 0;
         result = WriteFile( hFile, portal, sizeof( TileMapPortalMock_t ), &bytesWritten, NULL );
         *filePos += bytesWritten;

         if ( !result )
         {
            Platform_FatalError( "failed to write test game data file tile map portals." );
            return False;
         }
         else if ( bytesWritten != sizeof( TileMapPortalMock_t ) )
         {
            Platform_FatalError( "failed to write test game data file tile map portals: wrote incorrect number of bytes." );
            return False;
         }
      }
   }

   free( offsets );
   return True;
}
