#include <stdio.h>

#include "file.h"
#include "mem_arena.h"
#include "platform.h"
#include "win_common.h"

void Platform_Log( const char* msg )
{
   HANDLE hFile;
   SYSTEMTIME now;
   DWORD bytesToWrite;
   BOOL result;
   char timestamp[32];
   char timestampedMsg[STRING_SIZE_LARGE];
   char err[STRING_SIZE_DEFAULT];

   GetLocalTime( &now );
   snprintf(
      timestamp,
      sizeof( timestamp ),
      "[%04u-%02u-%02u %02u:%02u:%02u] ",
      now.wYear,
      now.wMonth,
      now.wDay,
      now.wHour,
      now.wMinute,
      now.wSecond
   );

   snprintf( timestampedMsg, STRING_SIZE_LARGE, "%s: %s\n", timestamp, msg );
   bytesToWrite = (DWORD)strlen( timestampedMsg );

   hFile = CreateFileA( g_winGlobals.logFilePath, FILE_APPEND_DATA, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

   if ( hFile == INVALID_HANDLE_VALUE )
   {
      snprintf( err, STRING_SIZE_DEFAULT, "Error opening log file: %lu", GetLastError() );
      MessageBoxA( 0, err, "Error", MB_OK | MB_ICONERROR );
      return;
   }

   result = WriteFile( hFile, timestampedMsg, bytesToWrite, 0, 0 );

   if ( !result )
   {
      snprintf( err, STRING_SIZE_DEFAULT, "Error writing to log file: %lu", GetLastError() );
      MessageBoxA( 0, err, "Error", MB_OK | MB_ICONERROR );
   }

   CloseHandle( hFile );
}

void Platform_FatalError( const char* msg )
{
   char err[STRING_SIZE_DEFAULT];
   snprintf( err, STRING_SIZE_DEFAULT, "Fatal error: %s", msg );
   Platform_Log( err );
   MessageBoxA( 0, err, "Error", MB_OK | MB_ICONERROR );
   exit( 1 );
}

u64 Platform_GetMicros( void )
{
   LARGE_INTEGER ticks;

   QueryPerformanceCounter( &ticks );
   return (u64)( ( (r64)( ticks.QuadPart ) / (r64)( g_winGlobals.performanceFrequency.QuadPart ) ) * (u64)1000000 );
}

void Platform_HandleMessages( Game_t* game )
{
   MSG msg;

   UNUSED_PARAM( game );

   while ( PeekMessageA( &msg, g_winGlobals.hWndMain, 0, 0, PM_REMOVE ) )
   {
      TranslateMessage( &msg );
      DispatchMessageA( &msg );
   }
}

void Platform_RenderDisplayBuffer( Display_t* display )
{
   UNUSED_PARAM( display );
   
   InvalidateRect( g_winGlobals.hWndMain, 0, FALSE );
}

void Platform_SleepMs( u32 ms )
{
   Sleep( (DWORD)ms );
}

u8* Platform_LoadFileToMemory( const char* filePath, MemArena_t* memArena, u32* bytesRead )
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
      Platform_FatalError( msg );
      return 0;
   }

   if ( !GetFileSizeEx( hFile, &fileSize ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not get file size: %lu\n", GetLastError() );
      Platform_FatalError( msg );
      return 0;
   }

   memArenaResult = MemArena_Alloc( memArena, (void**)&buffer, (u32)( fileSize.QuadPart ) );
   if ( memArenaResult != MemArenaResult_Success )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not allocate memory for file buffer: %s", MemArena_GetErrorMessage( memArenaResult ) );
      Platform_FatalError( msg );
      return 0;
   }

   if ( !ReadFile( hFile, buffer, (u32)( fileSize.QuadPart ), &bytesReadFromFile, NULL ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not read file: %lu\n", GetLastError() );
      Platform_FatalError( msg );
      return 0;
   }

   CloseHandle( hFile );

   if ( bytesReadFromFile != (u32)( fileSize.QuadPart ) )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "could not read file: read %lu of %llu bytes\n", bytesReadFromFile, fileSize.QuadPart );
      Platform_FatalError( msg );
      return 0;
   }

   if ( bytesRead )
   {
      *bytesRead = bytesReadFromFile;
   }

   return buffer;
}

void Platform_Rand_Seed( u32 seed )
{
   srand( seed );
}

u32 Platform_Rand_u32Ranged( u32 min, u32 max )
{
   return min + (u32)( rand() % ( max - min + 1 ) );
}

void Platform_OpenFile( File_t* file, const char* filePath )
{
   FILE *fileStream;
   errno_t err;
   char msg[STRING_SIZE_DEFAULT];

   fileStream = 0;
   err = fopen_s( &fileStream, filePath, "rb" );
   if ( err != 0 || !fileStream )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to open file stream for '%s': %d", filePath, err );
      Platform_FatalError( msg );
   }

   file->stream = (void*)fileStream;
   
   fseek( fileStream, 0, SEEK_END );
   file->size = ftell( fileStream );
   fseek( fileStream, 0, SEEK_SET );
}

void Platform_CloseFile( File_t* file )
{
   if ( file )
   {
      fclose( (FILE*)file->stream );
   }
}

void Platform_ReadFileBytes( File_t* file, u8 *buffer, size_t size )
{
   size_t bytesRead;
   char msg[STRING_SIZE_DEFAULT];

   bytesRead = fread( buffer, size, 1, (FILE*)file->stream );
   if ( bytesRead != 1 )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to read from file stream: read %zu bytes", bytesRead );
      Platform_FatalError( msg );
   }
}

void Platform_FileSeek( File_t* file, i32 offset, i32 origin )
{
   int result;
   char msg[STRING_SIZE_DEFAULT];

   result = fseek( (FILE*)file->stream, offset, origin );
   if ( result != 0 )
   {
      snprintf( msg, STRING_SIZE_DEFAULT, "failed to seek in file stream: fseek returned %d", result );
      Platform_FatalError( msg );
   }
}
