#if !defined( PLATFORM_OPS_H )
#define PLATFORM_OPS_H

#include "common.h"

typedef struct Game_t Game_t;
typedef struct Screen_t Screen_t;

void PlatformOps_FatalError( const char* msg );
u64 PlatformOps_GetMicros( void );
void PlatformOps_HandleMessages( Game_t* game );
void PlatformOps_RenderScreenBuffer( Screen_t* screen );
void PlatformOps_SleepMs( u32 ms );

#endif // PLATFORM_OPS_H
