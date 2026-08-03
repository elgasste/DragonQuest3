#include "platform_ops.h"
#include "win_common.h"

u64 PlatformOps_GetMicros( void )
{
   LARGE_INTEGER ticks;

   QueryPerformanceCounter( &ticks );
   return (u64)( ( (r64)( ticks.QuadPart ) / (r64)( g_winGlobals.performanceFrequency.QuadPart ) ) * (u64)1000000 );
}

void PlatformOps_HandleMessages( void )
{
   MSG msg;

   while ( PeekMessageA( &msg, g_winGlobals.hWndMain, 0, 0, PM_REMOVE ) )
   {
      TranslateMessage( &msg );
      DispatchMessageA( &msg );
   }
}

void PlatformOps_RenderScreenBuffer( void )
{
   InvalidateRect( g_winGlobals.hWndMain, 0, FALSE );
}

void PlatformOps_SleepMs( u32 ms )
{
   Sleep( (DWORD)ms );
}
