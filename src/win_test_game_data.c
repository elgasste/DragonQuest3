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

internal TileTextureSetMock_t* CreateTestTileTextureSet( void )
{
   u32 pixel, tilePixels, tileIndex, pixelIndex;
   u32 tileColors[5];
   TileTextureSetMock_t* textureSet;

   textureSet = (TileTextureSetMock_t*)malloc( sizeof( TileTextureSetMock_t ) );
   textureSet->count = 5;
   textureSet->tileSize = 16;
   textureSet->textures = (u32*)malloc( textureSet->count * textureSet->tileSize * textureSet->tileSize * sizeof( u32 ) );

   tileColors[0] = RGB( 0, 0, 0 );
   tileColors[1] = RGB( 255, 255, 255 );
   tileColors[2] = RGB( 255, 0, 0 );
   tileColors[3] = RGB( 0, 0, 255 );
   tileColors[4] = RGB( 0, 255, 0 );
   tilePixels = textureSet->tileSize * textureSet->tileSize;

   for ( tileIndex = 0; tileIndex < textureSet->count; tileIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
      {
         pixel = tileColors[tileIndex];
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
      curTileMap->tiles[i].textureIndex = Platform_Rand_u32Ranged( 0, 4 ) % 5;
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
      curTileMap->tiles[i].textureIndex = Platform_Rand_u32Ranged( 0, 4 ) % 5;
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
