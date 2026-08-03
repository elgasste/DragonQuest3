#include "platform_ops.h"
#include "win_common.h"

u64 PlatformOps_GetMicros()
{
   LARGE_INTEGER ticks;

   QueryPerformanceCounter( &ticks );
   return (u64)( ( (r64)( ticks.QuadPart ) / (r64)( g_winGlobals.performanceFrequency.QuadPart ) ) * (u64)1000000 );
}

void PlatformOps_SleepMs( u32 ms )
{
   Sleep( (DWORD)ms );
}
