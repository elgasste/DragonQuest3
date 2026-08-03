#if !defined( PLATFORM_OPS_H )
#define PLATFORM_OPS_H

#include "common.h"

u64 PlatformOps_GetMicros();
void PlatformOps_HandleMessages();
void PlatformOps_RenderScreenBuffer();
void PlatformOps_SleepMs( u32 ms );

#endif // PLATFORM_OPS_H
