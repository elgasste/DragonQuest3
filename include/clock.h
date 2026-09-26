#if !defined( CLOCK_H )
#define CLOCK_H

#include "common.h"

#define CLOCK_FPS             60
#define CLOCK_FRAME_SEC       ( 1.0f / (r32)CLOCK_FPS )
#define CLOCK_FRAME_MICROSEC  ( 1000000 / (u64)CLOCK_FPS )

typedef struct MemArena_t MemArena_t;

typedef struct Clock_t Clock_t;
size_t Clock_GetStructSize( void );
Clock_t* Clock_Create( MemArena_t* memArena );
void Clock_Free( Clock_t* clock, MemArena_t* memArena );

u64 Clock_GetAbsoluteStartMicro( Clock_t *clock );
u64 Clock_GetAbsoluteEndMicro( Clock_t *clock );
u64 Clock_GetLastFrameMicro( Clock_t *clock );
u32 Clock_GetFrameCount( Clock_t *clock );
u32 Clock_GetLagFrameCount( Clock_t *clock );

void Clock_StartFrame( Clock_t* clock );
void Clock_EndFrame( Clock_t* clock );

#endif // CLOCK_H
