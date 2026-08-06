#include "clock.h"
#include "platform.h"

void Clock_Init( Clock_t* clock, u32 fps )
{
   Clock_SetFps( clock, fps );

   clock->frameStartMicro = 0;
   clock->absoluteStartMicro = 0;
   clock->absoluteEndMicro = 0;
   clock->lastframeMicro = 0;

   clock->frameCount = 0;
   clock->lagFrameCount = 0;

   clock->hasStarted = False;
}

void Clock_SetFps( Clock_t* clock, u32 fps )
{
   clock->fps = fps;
   clock->frameMicroSec = 1000000 / (u64)fps;
   clock->frameSec = 1.0f / (r32)fps;
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

   if ( elapsedMicro <= clock->frameMicroSec )
   {
      Platform_SleepMs( (u32)( ( clock->frameMicroSec - elapsedMicro ) / 1000 ) );
   }
   else
   {
      clock->lagFrameCount++;
   }
}
