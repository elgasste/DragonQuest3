#include <stdio.h>

#include "platform_ops.h"
#include "win_common.h"

void PlatformOps_FatalError( const char* msg )
{
   char errorMsg[STRING_SIZE_DEFAULT];
   snprintf( errorMsg, STRING_SIZE_DEFAULT, "Fatal error: %s", msg );
   MessageBoxA( 0, msg, "Error", MB_OK | MB_ICONERROR );
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
