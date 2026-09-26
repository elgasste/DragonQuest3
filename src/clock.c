#include "clock.h"
#include "mem_arena.h"
#include "platform.h"

struct Clock_t
{
   u64 frameStartMicro;
   u64 absoluteStartMicro;
   u64 absoluteEndMicro;
   u64 lastframeMicro;

   u32 frameCount;
   u32 lagFrameCount;

   b32 hasStarted;
};

size_t Clock_GetStructSize( void )
{
   return sizeof( Clock_t );
}

Clock_t* Clock_Create( MemArena_t* memArena )
{
   Clock_t* clock;

   clock = (Clock_t*)MemArena_AllocMem( memArena, sizeof( Clock_t ) );

   clock->frameStartMicro = 0;
   clock->absoluteStartMicro = 0;
   clock->absoluteEndMicro = 0;
   clock->lastframeMicro = 0;

   clock->frameCount = 0;
   clock->lagFrameCount = 0;

   clock->hasStarted = False;

   return clock;
}

void Clock_Free( Clock_t* clock, MemArena_t* memArena )
{
   MemArena_FreeMem( memArena, clock );
}

u64 Clock_GetAbsoluteStartMicro( Clock_t *clock )
{
   return clock->absoluteStartMicro;
}

u64 Clock_GetAbsoluteEndMicro( Clock_t *clock )
{
   return clock->absoluteEndMicro;
}

u64 Clock_GetLastFrameMicro( Clock_t *clock )
{
   return clock->lastframeMicro;
}

u32 Clock_GetFrameCount( Clock_t *clock )
{
   return clock->frameCount;
}

u32 Clock_GetLagFrameCount( Clock_t *clock )
{
   return clock->lagFrameCount;
}

void Clock_StartFrame( Clock_t* clock )
{
   clock->frameStartMicro = Platform_GetMicros();

   if ( !clock->hasStarted )
   {
      clock->hasStarted = True;
      clock->absoluteStartMicro = clock->frameStartMicro;
      clock->absoluteEndMicro = clock->frameStartMicro;
   }
}

void Clock_EndFrame( Clock_t* clock )
{
   u64 frameEndMicro, elapsedMicro;

   frameEndMicro = Platform_GetMicros();
   clock->frameCount++;
   clock->absoluteEndMicro = frameEndMicro;
   elapsedMicro = frameEndMicro - clock->frameStartMicro;
   clock->lastframeMicro = elapsedMicro;

   if ( elapsedMicro <= CLOCK_FRAME_MICROSEC )
   {
      Platform_SleepMs( (u32)( ( CLOCK_FRAME_MICROSEC - elapsedMicro ) / 1000 ) );
   }
   else
   {
      clock->lagFrameCount++;
   }
}
