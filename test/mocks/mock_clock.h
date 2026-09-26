#if !defined( MOCK_CLOCK_H )
#define MOCK_CLOCK_H

#include "../../include/common.h"

typedef struct MemArena_t MemArena_t;

typedef struct Clock_t
{
   u32 frameCount;
}
Clock_t;

Clock_t* Clock_Create( MemArena_t* memArena );
void Clock_Free( Clock_t* clock, MemArena_t* memArena );
void Clock_StartFrame( Clock_t* clock );
void Clock_EndFrame( Clock_t* clock );

#endif // MOCK_CLOCK_H
