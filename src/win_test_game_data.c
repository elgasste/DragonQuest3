#include <stdio.h>

#include "game.h"
#include "game_data.h"
#include "platform.h"
#include "version.h"
#include "win_common.h"

internal b32 WriteTestGameDataHeader( HANDLE hFile );
internal b32 WriteTestGameDataTileTextures( HANDLE hFile );

// TODO: we can get rid of this once we have a proper game editor
void WriteTestGameDataFile( const char* filePath )
{
   HANDLE hFile;
   char msg[STRING_SIZE_DEFAULT];

   hFile = CreateFileA( filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file: %lu", GetLastError() );
      Platform_FatalError( msg );
   }

   if ( !WriteTestGameDataHeader( hFile ) )
   {
      CloseHandle( hFile );
      return;
   }
   if ( !WriteTestGameDataTileTextures( hFile ) )
   {
      CloseHandle( hFile );
      return;
   }

   CloseHandle( hFile );
}

internal b32 WriteTestGameDataHeader( HANDLE hFile )
{
   DWORD bytesWritten;
   BOOL result;
   GameDataHeader_t header;
   u32 i;
   char msg[STRING_SIZE_DEFAULT];

   for ( i = 0; i < 4; i++ )
   {
      header.magic[i] = GAME_DATA_MAGIC[i];
   }
   header.version.major = GAME_VERSION_MAJOR;
   header.version.minor = GAME_VERSION_MINOR;
   header.version.maint = GAME_VERSION_MAINT;
   header.tileTexturesHeaderOffset = sizeof( GameDataHeader_t );

   bytesWritten = 0;
   result = WriteFile( hFile, &header, sizeof( GameDataHeader_t ), &bytesWritten, NULL );

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

internal b32 WriteTestGameDataTileTextures( HANDLE hFile )
{
   DWORD bytesWritten;
   BOOL result;
   u32 pixel, tilePixels, tileIndex, pixelIndex;
   u32 tileColors[5];
   GameDataTileTexturesHeader_t tileTexturesHeader;
   char msg[STRING_SIZE_DEFAULT];

   // write the header
   tileTexturesHeader.count = 5;
   tileTexturesHeader.tileSize = 16;
   tileTexturesHeader.texturesOffset = sizeof( GameDataHeader_t ) + sizeof( GameDataTileTexturesHeader_t );

   bytesWritten = 0;
   result = WriteFile( hFile, &tileTexturesHeader, sizeof( GameDataTileTexturesHeader_t ), &bytesWritten, NULL );

   if ( !result )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile textures header: %lu", GetLastError() );
      Platform_FatalError( msg );
      return False;
   }
   else if ( bytesWritten != sizeof( GameDataTileTexturesHeader_t ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to write test game data file tile textures header: wrote %lu of %lu bytes", bytesWritten, sizeof( GameDataTileTexturesHeader_t ) );
      Platform_FatalError( msg );
      return False;
   }

   // write the tile textures
   tileColors[0] = RGB( 0, 0, 0 );
   tileColors[1] = RGB( 255, 255, 255 );
   tileColors[2] = RGB( 255, 0, 0 );
   tileColors[3] = RGB( 0, 0, 255 );
   tileColors[4] = RGB( 0, 255, 0 );
   tilePixels = tileTexturesHeader.tileSize * tileTexturesHeader.tileSize;

   for ( tileIndex = 0; tileIndex < tileTexturesHeader.count; tileIndex++ )
   {
      for ( pixelIndex = 0; pixelIndex < tilePixels; pixelIndex++ )
      {
         pixel = tileColors[tileIndex];
         bytesWritten = 0;
         result = WriteFile( hFile, &pixel, sizeof( u32 ), &bytesWritten, NULL );

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
