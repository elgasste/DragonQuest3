#include <stdio.h>
#include <stdlib.h>

#include "game_data.h"
#include "platform.h"
#include "version.h"
#include "win_common.h"

typedef struct TileTextureSetMock_t
{
   u32 count;
   u32 tileSize;
   u32* textures;
}
TileTextureSetMock_t;

typedef struct TileMock_t
{
   u32 textureIndex;
}
TileMock_t;

typedef struct TileMapMock_t
{
   u32 id;
   u32 tilesX;
   u32 tilesY;
   b32 wraps;
   TileMock_t* tiles;
}
TileMapMock_t;

internal TileTextureSetMock_t* CreateTestTileTextureSet( void );
internal TileMapMock_t* CreateTestTileMaps( u32* tileMapCount );
internal b32 WriteTestGameDataHeader( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* textureSet );
internal b32 WriteTestGameDataTileTextureSet( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* textureSet );
internal b32 WriteTestGameDataTileMaps( HANDLE hFile, DWORD* filePos, TileMapMock_t* tileMaps, u32 tileMapCount );

void WriteTestGameDataFile( const char* filePath )
{
   HANDLE hFile;
   DWORD filePos;
   TileTextureSetMock_t* textureSet;
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

   textureSet = CreateTestTileTextureSet();
   tileMapCount = 0;
   tileMaps = CreateTestTileMaps( &tileMapCount );

   if ( !WriteTestGameDataHeader( hFile, &filePos, textureSet ) )
   {
      Platform_FatalError( "failed to write test game data file header." );
   }
   if ( !WriteTestGameDataTileTextureSet( hFile, &filePos, textureSet ) )
   {
      Platform_FatalError( "failed to write test game data file tile texture set." );
   }
   if ( !WriteTestGameDataTileMaps( hFile, &filePos, tileMaps, tileMapCount ) )
   {
      Platform_FatalError( "failed to write test game data file tile maps." );
   }

   free( textureSet->textures );
   free( textureSet );
   for ( i = 0; i < tileMapCount; i++ )
   {
      free( tileMaps[i].tiles );
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
         return RGB( 60, 140, 50 );
      }
      return RGB( 80, 170, 70 );
   }
   else if ( tileIndex == 1 ) // dirt
   {
      if ( speckle )
      {
         return RGB( 110, 80, 50 );
      }
      return RGB( 140, 100, 65 );
   }
   else if ( tileIndex == 2 ) // sand
   {
      if ( speckle )
      {
         return RGB( 220, 195, 130 );
      }
      return RGB( 235, 210, 150 );
   }
   else if ( tileIndex == 3 ) // water
   {
      if ( ( x + ( y % 4 ) ) % 6 < 2 )
      {
         return RGB( 60, 110, 200 );
      }
      return RGB( 80, 140, 220 );
   }
   else if ( tileIndex == 4 ) // stone
   {
      if ( speckle )
      {
         return RGB( 110, 110, 115 );
      }
      return RGB( 140, 140, 145 );
   }
   else if ( tileIndex == 5 ) // path
   {
      if ( speckle )
      {
         return RGB( 190, 160, 110 );
      }
      return RGB( 205, 175, 125 );
   }
   else if ( tileIndex == 6 ) // snow
   {
      if ( speckle )
      {
         return RGB( 225, 230, 235 );
      }
      return RGB( 245, 248, 250 );
   }
   else if ( tileIndex == 7 ) // tree
   {
      if ( y < 6 )
      {
         return ( speckle ) ? RGB( 40, 110, 45 ) : RGB( 55, 130, 55 );
      }
      if ( x >= 6 && x <= 9 )
      {
         return RGB( 90, 60, 35 );
      }
      return RGB( 80, 170, 70 );
   }
   else if ( tileIndex == 8 ) // bush
   {
      if ( x >= 3 && x <= 12 && y >= 4 && y <= 11 )
      {
         return ( speckle ) ? RGB( 50, 120, 55 ) : RGB( 65, 140, 60 );
      }
      return RGB( 80, 170, 70 );
   }
   else // mountain
   {
      if ( y + ( x / 2 ) < 10 )
      {
         return RGB( 130, 130, 135 );
      }
      return RGB( 100, 100, 105 );
   }
}

internal TileTextureSetMock_t* CreateTestTileTextureSet( void )
{
   u32 pixel, tilePixels, tileIndex, pixelIndex, x, y;
   TileTextureSetMock_t* textureSet;

   textureSet = (TileTextureSetMock_t*)malloc( sizeof( TileTextureSetMock_t ) );
   textureSet->count = 10;
   textureSet->tileSize = 16;
   textureSet->textures = (u32*)malloc( textureSet->count * textureSet->tileSize * textureSet->tileSize * sizeof( u32 ) );

   tilePixels = textureSet->tileSize * textureSet->tileSize;

   for ( tileIndex = 0; tileIndex < textureSet->count; tileIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
      {
         x = pixelIndex % textureSet->tileSize;
         y = pixelIndex / textureSet->tileSize;
         pixel = LandscapeTilePixel( tileIndex, x, y );
         textureSet->textures[tileIndex * tilePixels + pixelIndex] = pixel;
      }
   }

   return textureSet;
}

internal TileMapMock_t* CreateTestTileMaps( u32* tileMapCount )
{
   u32 i;
   TileMapMock_t *tileMaps, *curTileMap;

   *tileMapCount = 4;
   tileMaps = (TileMapMock_t*)malloc( *tileMapCount * sizeof( TileMapMock_t ) );

   // 0: 10x10 checkerboard, no wrapping
   curTileMap = tileMaps;
   curTileMap->id = 0;
   curTileMap->tilesX = 10;
   curTileMap->tilesY = 10;
   curTileMap->wraps = False;
   curTileMap->tiles = 0;
   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->tilesX * curTileMap->tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->tilesX * curTileMap->tilesY; i++ )
   {
      if ( ( i / curTileMap->tilesX ) % 2 == 0 )
      {
         if ( ( i % curTileMap->tilesX ) % 2 == 0 )
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
         if ( ( i % curTileMap->tilesX ) % 2 == 0 )
         {
            curTileMap->tiles[i].textureIndex = 0; // black
         }
         else
         {
            curTileMap->tiles[i].textureIndex = 1; // white
         }
      }
   }

   // 1: 256x256 random, wrapping
   curTileMap++;
   curTileMap->id = 1;
   curTileMap->tilesX = 256;
   curTileMap->tilesY = 256;
   curTileMap->wraps = True;
   curTileMap->tiles = 0;
   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->tilesX * curTileMap->tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->tilesX * curTileMap->tilesY; i++ )
   {
      // random
      curTileMap->tiles[i].textureIndex = Platform_Rand_u32Ranged( 0, 9 );
   }

   // make the edges all the same so we can test wrapping
   for ( i = 0; i < curTileMap->tilesX; i++ )
   {
      curTileMap->tiles[i].textureIndex = 2; // top edge
      curTileMap->tiles[( curTileMap->tilesY - 1 ) * curTileMap->tilesX + i].textureIndex = 2; // bottom edge
   }
   for ( i = 0; i < curTileMap->tilesY; i++ )
   {
      curTileMap->tiles[i * curTileMap->tilesX].textureIndex = 2; // left edge
      curTileMap->tiles[i * curTileMap->tilesX + ( curTileMap->tilesX - 1 )].textureIndex = 2; // right edge
   }

   // 2: 128x128 random, no wrapping
   curTileMap++;
   curTileMap->id = 2;
   curTileMap->tilesX = 128;
   curTileMap->tilesY = 128;
   curTileMap->wraps = False;
   curTileMap->tiles = 0;
   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->tilesX * curTileMap->tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->tilesX * curTileMap->tilesY; i++ )
   {
      // random
      curTileMap->tiles[i].textureIndex = Platform_Rand_u32Ranged( 0, 9 );
   }

   // 3: 3x3, no wrapping
   curTileMap++;
   curTileMap->id = 3;
   curTileMap->tilesX = 3;
   curTileMap->tilesY = 3;
   curTileMap->wraps = False;
   curTileMap->tiles = 0;
   curTileMap->tiles = (TileMock_t*)malloc( curTileMap->tilesX * curTileMap->tilesY * sizeof( TileMock_t ) );

   for ( i = 0; i < curTileMap->tilesX * curTileMap->tilesY; i++ )
   {
      curTileMap->tiles[i].textureIndex = 1;
   }

   return tileMaps;
}

internal b32 WriteTestGameDataHeader( HANDLE hFile, DWORD* filePos, TileTextureSetMock_t* textureSet )
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
   offsets.tileMaps = offsets.tileTextureSet + sizeof( TileTextureSetMock_t ) + ( textureSet->count * textureSet->tileSize * textureSet->tileSize * sizeof( u32 ) );
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
   result = WriteFile( hFile, textureSet, sizeof( TileTextureSetMock_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile texture set header: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( TileTextureSetMock_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile texture set header: wrote %lu of %lu bytes", bytesWritten, sizeof( TileTextureSetMock_t ) );
      Platform_FatalError( msg );
      return False;
   }

   tilePixels = textureSet->tileSize * textureSet->tileSize;
   
   for ( tileIndex = 0; tileIndex < textureSet->count; tileIndex++ )
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

internal b32 WriteTestGameDataTileMaps( HANDLE hFile, DWORD* filePos, TileMapMock_t* tileMaps, u32 tileMapCount )
{
   u32 i, j, tileAccum;
   DWORD bytesWritten;
   BOOL result;
   TileMock_t* tile;
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
   for ( i = 0; i < tileMapCount; i++ )
   {
      offsets[i].id = tileMaps[i].id;
      offsets[i].offset = sizeof( u32 )
         + ( tileMapCount * sizeof( GameDataObjectOffset_t ) )
         + ( i * sizeof( TileMapMock_t ) )
         + ( tileAccum * sizeof( TileMock_t ) );
      tileAccum += tileMaps[i].tilesX * tileMaps[i].tilesY;
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
      result = WriteFile( hFile, &tileMaps[i], sizeof( TileMapMock_t ), &bytesWritten, NULL );
      *filePos += bytesWritten;

      if ( !result )
      {
         Platform_FatalError( "failed to write test game data file tile maps." );
         return False;
      }
      else if ( bytesWritten != sizeof( TileMapMock_t ) )
      {
         Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
         return False;
      }

      for ( j = 0; j < tileMaps[i].tilesX * tileMaps[i].tilesY; j++ )
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
   }

   free( offsets );
   return True;
}
