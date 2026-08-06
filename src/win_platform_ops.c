#include <stdio.h>

#include "mem_arena.h"
#include "platform_ops.h"
#include "win_common.h"

void PlatformOps_Log( const char* msg )
{
   HANDLE hFile;
   DWORD bytesToWrite;
   BOOL result;
   char err[STRING_SIZE_DEFAULT];

   bytesToWrite = (DWORD)strlen( msg );

   hFile = CreateFileA( g_winGlobals.logFilePath, FILE_APPEND_DATA, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( err, STRING_SIZE_DEFAULT, "Error opening log file: %lu", GetLastError() );
      MessageBoxA( 0, err, "Error", MB_OK | MB_ICONERROR );
      return;
   }

   result = WriteFile( hFile, msg, bytesToWrite, 0, 0 );

   if ( !result )
   {
      snprintf( err, STRING_SIZE_DEFAULT, "Error writing to log file: %lu", GetLastError() );
      MessageBoxA( 0, err, "Error", MB_OK | MB_ICONERROR );
   }

   CloseHandle( hFile );
}

void PlatformOps_FatalError( const char* msg )
{
   char err[STRING_SIZE_DEFAULT];
   snprintf( err, STRING_SIZE_DEFAULT, "Fatal error: %s", msg );
   PlatformOps_Log( err );
   MessageBoxA( 0, err, "Error", MB_OK | MB_ICONERROR );
   exit( 1 );
}

u64 PlatformOps_GetMicros( void )
{
   LARGE_INTEGER ticks;

   QueryPerformanceCounter( &ticks );
   return (u64)( ( (r64)( ticks.QuadPart ) / (r64)( g_winGlobals.performanceFrequency.QuadPart ) ) * (u64)1000000 );
}

void PlatformOps_HandleMessages( Game_t* game )
{
   MSG msg;

   UNUSED_PARAM( game );

   while ( PeekMessageA( &msg, g_winGlobals.hWndMain, 0, 0, PM_REMOVE ) )
   {
      TranslateMessage( &msg );
      DispatchMessageA( &msg );
   }
}

void PlatformOps_RenderScreenBuffer( Screen_t* screen )
{
   UNUSED_PARAM( screen );
   
   InvalidateRect( g_winGlobals.hWndMain, 0, FALSE );
}

void PlatformOps_SleepMs( u32 ms )
{
   Sleep( (DWORD)ms );
}

u8* PlatformOps_LoadFileToMemory( const char* filePath, MemArena_t* memArena, u32* bytesRead )
{
   HANDLE hFile;
   LARGE_INTEGER fileSize;
   DWORD bytesReadFromFile;
   u8* buffer;
   MemArenaResult_t memArenaResult;
   char msg[STRING_SIZE_DEFAULT];

   hFile = CreateFile( filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not open file: %lu\n", GetLastError() );
      PlatformOps_FatalError( msg );
      return 0;
   }

   if ( !GetFileSizeEx( hFile, &fileSize ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not get file size: %lu\n", GetLastError() );
      PlatformOps_FatalError( msg );
      return 0;
   }

   memArenaResult = MemArena_Alloc( memArena, (void**)&buffer, (u32)( fileSize.QuadPart ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not allocate memory for file buffer: %s", MemArena_GetErrorMessage( memArenaResult ) );
      PlatformOps_FatalError( msg );
      return 0;
   }

   if ( !ReadFile( hFile, buffer, (u32)( fileSize.QuadPart ), &bytesReadFromFile, NULL ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not read file: %lu\n", GetLastError() );
      PlatformOps_FatalError( msg );
      return 0;
   }

   CloseHandle( hFile );

   if ( bytesReadFromFile != (u32)( fileSize.QuadPart ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not read file: read %lu of %llu bytes\n", bytesReadFromFile, fileSize.QuadPart );
      PlatformOps_FatalError( msg );
      return 0;
   }

   if ( bytesRead )
   {
      *bytesRead = bytesReadFromFile;
   }

   return buffer;
}
