#if !defined( MOCK_CLOCK_H )
#define MOCK_CLOCK_H

#include "../../include/common.h"

typedef struct MemArena_t MemArena_t;

typedef struct Clock_t
{
   u32 fps;
}
Clock_t;

Clock_t* Clock_Create( MemArena_t* memArena );
void Clock_Free( Clock_t* clock, MemArena_t* memArena );
void Clock_Init( Clock_t* clock, u32 fps );
void Clock_StartFrame( Clock_t* clock );
void Clock_EndFrame( Clock_t* clock );

#endif // MOCK_CLOCK_H
