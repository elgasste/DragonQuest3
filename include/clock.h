#if !defined( CLOCK_H )
#define CLOCK_H

#include "common.h"

typedef struct Clock_t
{
   u32 fps;
   u64 frameMicroSec;
   r32 frameSec;

   u64 frameStartMicro;
   u64 absoluteStartMicro;
   u64 absoluteEndMicro;
   u64 lastframeMicro;

   u32 frameCount;
   u32 lagFrameCount;

   b32 hasStarted;
}
Clock_t;

void Clock_Init( Clock_t* clock, u32 fps );
void Clock_SetFps( Clock_t* clock, u32 fps );
void Clock_StartFrame( Clock_t* clock );
void Clock_EndFrame( Clock_t* clock );

#endif // CLOCK_H
