#if !defined( CLOCK_H )
#define CLOCK_H

#include "common.h"
typedef struct MemArena_t MemArena_t;

typedef struct Clock_t Clock_t;
size_t Clock_GetStructSize( void );
Clock_t* Clock_Create( MemArena_t* memArena );
void Clock_Free( Clock_t* clock, MemArena_t* memArena );

u32 Clock_GetFps( Clock_t *clock );
r32 Clock_GetFrameSec( Clock_t *clock );
u64 Clock_GetAbsoluteStartMicro( Clock_t *clock );
u64 Clock_GetAbsoluteEndMicro( Clock_t *clock );
u64 Clock_GetLastFrameMicro( Clock_t *clock );
u32 Clock_GetFrameCount( Clock_t *clock );
u32 Clock_GetLagFrameCount( Clock_t *clock );

void Clock_Init( Clock_t* clock, u32 fps );
void Clock_SetFps( Clock_t* clock, u32 fps );
void Clock_StartFrame( Clock_t* clock );
void Clock_EndFrame( Clock_t* clock );

#endif // CLOCK_H
