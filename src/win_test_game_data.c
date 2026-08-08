#include <stdio.h>

#include "game.h"
#include "game_data.h"
#include "platform.h"
#include "version.h"
#include "win_common.h"

internal b32 WriteTestGameDataHeader( HANDLE hFile );
internal b32 WriteTestGameDataTileTexturesHeader( HANDLE hFile );

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
   if ( !WriteTestGameDataTileTexturesHeader( hFile ) )
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

internal b32 WriteTestGameDataTileTexturesHeader( HANDLE hFile )
{
   DWORD bytesWritten;
   BOOL result;
   GameDataTileTexturesHeader_t tileTexturesHeader;
   char msg[STRING_SIZE_DEFAULT];

   tileTexturesHeader.count = 0;
   tileTexturesHeader.tileSize = 16;
   tileTexturesHeader.texturesOffset = 0;

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

   return True;
}
