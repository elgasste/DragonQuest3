#if !defined( PLATFORM_OPS_H )
#define PLATFORM_OPS_H

#include "common.h"

void PlatformOps_FatalError( const char* msg );
u64 PlatformOps_GetMicros( void );
void PlatformOps_HandleMessages( void );
void PlatformOps_RenderScreenBuffer( void );
void PlatformOps_SleepMs( u32 ms );

#endif // PLATFORM_OPS_H
