#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_data.h"
#include "platform.h"
#include "tile_map.h"
#include "version.h"
#include "win_common.h"

internal b32 WriteTestGameDataHeader( HANDLE hFile, GameDataHeader_t* header, DWORD* filePos );
internal void InsertUpdatedTestGameDataHeader( const char* filePath, GameDataHeader_t* header );
internal b32 WriteTestGameDataTileTextureSet( HANDLE hFile, DWORD* filePos );
internal b32 WriteTestGameDataTileMaps( HANDLE hFile, DWORD* filePos );

void WriteTestGameDataFile( const char* filePath )
{
   HANDLE hFile;
   DWORD filePos;
   GameDataHeader_t header;
   char msg[STRING_SIZE_DEFAULT];

   hFile = CreateFileA( filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
   filePos = 0;

   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file: %lu", GetLastError() );
      Platform_FatalError( msg );
   }

   if ( !WriteTestGameDataHeader( hFile, &header, &filePos ) )
   {
      CloseHandle( hFile );
      return;
   }

   header.tileTextureSetOffset = filePos;
   if ( !WriteTestGameDataTileTextureSet( hFile, &filePos ) )
   {
      CloseHandle( hFile );
      return;
   }

   header.tileMapsOffset = filePos;
   if ( !WriteTestGameDataTileMaps( hFile, &filePos ) )
   {
      CloseHandle( hFile );
      return;
   }

   CloseHandle( hFile );

   InsertUpdatedTestGameDataHeader( filePath, &header );
}

internal b32 WriteTestGameDataHeader( HANDLE hFile, GameDataHeader_t* header, DWORD* filePos )
{
   DWORD bytesWritten;
   BOOL result;
   u32 i;
   char msg[STRING_SIZE_DEFAULT];

   for ( i = 0; i < 4; i++ )
   {
      header->magic[i] = GAME_DATA_MAGIC[i];
   }
   header->version.major = GAME_VERSION_MAJOR;
   header->version.minor = GAME_VERSION_MINOR;
   header->version.maint = GAME_VERSION_MAINT;
   header->tileTextureSetOffset = 0;
   header->tileMapsOffset = 0;

   bytesWritten = 0;
   result = WriteFile( hFile, header, sizeof( GameDataHeader_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file header: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( GameDataHeader_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file header: wrote %lu of %lu bytes", bytesWritten, sizeof( GameDataHeader_t ) );
      Platform_FatalError( msg );
      return False;
   }

   return True;
}

internal void InsertUpdatedTestGameDataHeader( const char* filePath, GameDataHeader_t* header )
{
   HANDLE hFile;
   LARGE_INTEGER fileSize;
   DWORD bytesRead, bytesWritten;
   u32 fileSize32;
   u8* fileBytes;
   BOOL result;
   char msg[STRING_SIZE_DEFAULT];

   hFile = CreateFileA( filePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to open test game data file for header update: %lu", GetLastError() );
      Platform_FatalError( msg );
      return;
   }

   GetFileSizeEx( hFile, &fileSize );
   fileSize32 = (u32)fileSize.QuadPart;
   fileBytes = (u8*)malloc( fileSize32 );

   SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
   bytesRead = 0;
   result = ReadFile( hFile, fileBytes, fileSize32, &bytesRead, NULL );
   if ( !result || bytesRead != fileSize32 )
   {
      if ( !result )
      {
         snprintf( msg, STRING_SIZE_DEFAULT, "failed to read test game data file for header update: %lu", GetLastError() );
      }
      else
      {
         snprintf( msg, STRING_SIZE_DEFAULT, "failed to read test game data file for header update: read %lu of %lu bytes", bytesRead, fileSize32 );
      }

      Platform_FatalError( msg );
      CloseHandle( hFile );
      return;
   }

   for ( u32 i = 0; i < sizeof( GameDataHeader_t ); i++ )
   {
      fileBytes[i] = ((u8*)header)[i];
   }

   SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) != INVALID_SET_FILE_POINTER || GetLastError() == NO_ERROR;
   bytesWritten = 0;
   result = WriteFile( hFile, fileBytes, fileSize32, &bytesWritten, NULL );
   if ( !result || bytesWritten != fileSize32 )
   {
      if ( !result )
      {
         snprintf( msg, STRING_SIZE_DEFAULT, "failed to write updated test game data file contents: %lu", GetLastError() );
      }
      else
      {
         snprintf( msg, STRING_SIZE_DEFAULT, "failed to write updated test game data file contents: wrote %lu of %lu bytes", bytesWritten, fileSize32 );
      }

      Platform_FatalError( msg );
      CloseHandle( hFile );
      return;
   }

   free( fileBytes );
   CloseHandle( hFile );
}

internal b32 WriteTestGameDataTileTextureSet( HANDLE hFile, DWORD* filePos )
{
   DWORD bytesWritten;
   BOOL result;
   u32 pixel, tilePixels, tileIndex, pixelIndex;
   u32 tileColors[5];
   TileTextureSet_t textureSet;
   char msg[STRING_SIZE_DEFAULT];

   // write the header
   textureSet.count = 5;
   textureSet.tileSize = 16;

   bytesWritten = 0;
   result = WriteFile( hFile, &textureSet, sizeof( TileTextureSet_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile textures header: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( TileTextureSet_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile textures header: wrote %lu of %lu bytes", bytesWritten, sizeof( TileTextureSet_t ) );
      Platform_FatalError( msg );
      return False;
   }

   // write the tile textures
   tileColors[0] = RGB( 0, 0, 0 );
   tileColors[1] = RGB( 255, 255, 255 );
   tileColors[2] = RGB( 255, 0, 0 );
   tileColors[3] = RGB( 0, 0, 255 );
   tileColors[4] = RGB( 0, 255, 0 );
   tilePixels = textureSet.tileSize * textureSet.tileSize;

   for ( tileIndex = 0; tileIndex < textureSet.count; tileIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
      {
         pixel = tileColors[tileIndex];
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

internal b32 WriteTestGameDataTileMaps( HANDLE hFile, DWORD* filePos )
{
   u32 tileMapCount, i;
   DWORD bytesWritten;
   BOOL result;
   TileMap_t tileMap;
   Tile_t tile;

   // write out the number of tile maps first
   tileMapCount = 2;
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

   // first tile map
   tileMap.id = 0;
   tileMap.w = 10;
   tileMap.h = 10;
   tileMap.wraps = False;
   tileMap.tiles = 0;
   tileMap.tileTextureSet = 0;

   bytesWritten = 0;
   result = WriteFile( hFile, &tileMap, sizeof( TileMap_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      Platform_FatalError( "failed to write test game data file tile maps." );
      return False;
   }
   else if ( bytesWritten != sizeof( TileMap_t ) )
   {
      Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
      return False;
   }

   for ( i = 0; i < tileMap.w * tileMap.h; i++ )
   {
      // checkerboard
      if ( ( i / tileMap.w ) % 2 == 0 )
      {
         if ( ( i % tileMap.w ) % 2 == 0 )
         {
            tile.textureIndex = 1; // white
         }
         else
         {
            tile.textureIndex = 0; // black
         }
      }
      else
      {
         if ( ( i % tileMap.w ) % 2 == 0 )
         {
            tile.textureIndex = 0; // black
         }
         else
         {
            tile.textureIndex = 1; // white
         }
      }

      bytesWritten = 0;
      result = WriteFile( hFile, &tile, sizeof( Tile_t ), &bytesWritten, NULL );
      *filePos += bytesWritten;

      if ( !result )
      {
         Platform_FatalError( "failed to write test game data file tile maps." );
         return False;
      }
      else if ( bytesWritten != sizeof( Tile_t ) )
      {
         Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
         return False;
      }
   }

   // second tile map
   tileMap.id = 1;
   tileMap.w = 256;
   tileMap.h = 256;
   tileMap.wraps = True;
   tileMap.tiles = 0;
   tileMap.tileTextureSet = 0;

   bytesWritten = 0;
   result = WriteFile( hFile, &tileMap, sizeof( TileMap_t ), &bytesWritten, NULL );
   *filePos += bytesWritten;

   if ( !result )
   {
      Platform_FatalError( "failed to write test game data file tile maps." );
      return False;
   }
   else if ( bytesWritten != sizeof( TileMap_t ) )
   {
      Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
      return False;
   }

   for ( i = 0; i < tileMap.w * tileMap.h; i++ )
   {
      // random texture
      tile.textureIndex = Platform_Rand_u32Ranged( 0, 4 ) % 5;

      bytesWritten = 0;
      result = WriteFile( hFile, &tile, sizeof( Tile_t ), &bytesWritten, NULL );
      *filePos += bytesWritten;

      if ( !result )
      {
         Platform_FatalError( "failed to write test game data file tile maps." );
         return False;
      }
      else if ( bytesWritten != sizeof( Tile_t ) )
      {
         Platform_FatalError( "failed to write test game data file tile maps: wrote incorrect number of bytes." );
         return False;
      }
   }

   return True;
}
