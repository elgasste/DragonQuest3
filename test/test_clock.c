#include <stdlib.h>

#include "clock.h"
#include "platform.h"
#include "unity.h"

typedef struct PlatformOpsGetMicrosCall_t
{
   u64 returnValue;
   int callCount;
}
PlatformOpsGetMicrosCall_t;

typedef struct PlatformOpsSleepMsCall_t
{
   u32 ms;
   int callCount;
}
PlatformOpsSleepMsCall_t;

local_persist PlatformOpsGetMicrosCall_t g_platformOpsGetMicrosCall;
local_persist PlatformOpsSleepMsCall_t g_platformOpsSleepMsCall;

void setUp( void )
{
   g_platformOpsGetMicrosCall.returnValue = 0;
   g_platformOpsGetMicrosCall.callCount = 0;

   g_platformOpsSleepMsCall.ms = 0;
   g_platformOpsSleepMsCall.callCount = 0;
}

void tearDown( void ) {}

u64 Platform_GetMicros( void )
{
   g_platformOpsGetMicrosCall.callCount++;
   return g_platformOpsGetMicrosCall.returnValue;
}

void Platform_SleepMs( u32 ms )
{
   g_platformOpsSleepMsCall.ms = ms;
   g_platformOpsSleepMsCall.callCount++;
}

void test_Clock_Init_InitializesClockWithCorrectParameters( void )
{
   r32 expectedFrameSec;
   Clock_t* clock;

   expectedFrameSec = 1.0f / 30.0f;
   clock = (Clock_t*)malloc( Clock_GetSize() );

   Clock_Init( clock, 30 );

   TEST_ASSERT_EQUAL( 30, Clock_GetFps( clock ) );
   TEST_ASSERT_EQUAL( expectedFrameSec, Clock_GetFrameSec( clock ) );
   TEST_ASSERT_EQUAL( 0, Clock_GetAbsoluteStartMicro( clock ) );
   TEST_ASSERT_EQUAL( 0, Clock_GetAbsoluteEndMicro( clock ) );
   TEST_ASSERT_EQUAL( 0, Clock_GetLastFrameMicro( clock ) );
   TEST_ASSERT_EQUAL( 0, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 0, Clock_GetLagFrameCount( clock ) );

   free( clock );
}

void test_Clock_SetFps_UpdatesFrameMicroSecAndFrameSec( void )
{
   r32 expectedFrameSec;
   Clock_t* clock;

   clock = (Clock_t*)malloc( Clock_GetSize() );
   Clock_Init( clock, 30 );

   expectedFrameSec = 1.0f / 60.0f;

   Clock_SetFps( clock, 60 );

   TEST_ASSERT_EQUAL( 60, Clock_GetFps( clock ) );
   TEST_ASSERT_EQUAL( expectedFrameSec, Clock_GetFrameSec( clock ) );

   free( clock );
}

void test_Clock_StartFrame_ClockIsMarkedAsStarted( void )
{
   Clock_t* clock;

   clock = (Clock_t*)malloc( Clock_GetSize() );
   Clock_Init( clock, 30 );
   g_platformOpsGetMicrosCall.returnValue = 100;

   Clock_StartFrame( clock );
   TEST_ASSERT_EQUAL( 100, Clock_GetAbsoluteStartMicro( clock ) );
   TEST_ASSERT_EQUAL( 100, Clock_GetAbsoluteEndMicro( clock ) );

   free( clock );
}

void test_Clock_StartFrame_InitializesStartMicroValues( void )
{
   Clock_t* clock;

   clock = (Clock_t*)malloc( Clock_GetSize() );
   Clock_Init( clock, 30 );
   g_platformOpsGetMicrosCall.returnValue = 100;

   Clock_StartFrame( clock );
   TEST_ASSERT_EQUAL( 1, g_platformOpsGetMicrosCall.callCount );
   TEST_ASSERT_EQUAL( 100, Clock_GetAbsoluteStartMicro( clock ) );
   TEST_ASSERT_EQUAL( 100, Clock_GetAbsoluteEndMicro( clock ) );

   free( clock );
}

void test_Clock_EndFrame_UpdatesParametersCorrectly( void )
{
   Clock_t* clock;

   clock = (Clock_t*)malloc( Clock_GetSize() );
   Clock_Init( clock, 60 );
   g_platformOpsGetMicrosCall.returnValue = 100;

   Clock_StartFrame( clock );
   TEST_ASSERT_EQUAL( 1, g_platformOpsGetMicrosCall.callCount );
   TEST_ASSERT_EQUAL( 0, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 100, Clock_GetAbsoluteEndMicro( clock ) );

   g_platformOpsGetMicrosCall.returnValue = 150;
   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL( 2, g_platformOpsGetMicrosCall.callCount );
   TEST_ASSERT_EQUAL( 1, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 150, Clock_GetAbsoluteEndMicro( clock ) );
   TEST_ASSERT_EQUAL( 50, Clock_GetLastFrameMicro( clock ) );

   free( clock );
}

void test_Clock_EndFrame_NonLagFrameSleepsForCorrectDuration( void )
{
   u64 frameMicroSec;
   Clock_t* clock;

   clock = (Clock_t*)malloc( Clock_GetSize() );
   Clock_Init( clock, 60 );
   frameMicroSec = 1000000 / 60;  // 16,666

   Clock_StartFrame( clock );
   g_platformOpsGetMicrosCall.returnValue = 10000;

   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL( 1, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 0, Clock_GetLagFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 1, g_platformOpsSleepMsCall.callCount );
   TEST_ASSERT_EQUAL( (u32)( frameMicroSec - 10000 ) / 1000, g_platformOpsSleepMsCall.ms );

   free( clock );
}

void test_Clock_EndFrame_LagFrameDoesNotSleepAndUpdatesLagFrameCount( void )
{
   u64 frameMicroSec;
   Clock_t* clock;

   clock = (Clock_t*)malloc( Clock_GetSize() );
   Clock_Init( clock, 60 );
   frameMicroSec = 1000000 / 60;  // 16,666

   Clock_StartFrame( clock );
   g_platformOpsGetMicrosCall.returnValue = 17000;

   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL( 1, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 1, Clock_GetLagFrameCount( clock ) );
   TEST_ASSERT_EQUAL( 0, g_platformOpsSleepMsCall.callCount );

   free( clock );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Clock_Init_InitializesClockWithCorrectParameters );

   RUN_TEST( test_Clock_SetFps_UpdatesFrameMicroSecAndFrameSec );

   RUN_TEST( test_Clock_StartFrame_ClockIsMarkedAsStarted );
   RUN_TEST( test_Clock_StartFrame_InitializesStartMicroValues );

   RUN_TEST( test_Clock_EndFrame_UpdatesParametersCorrectly );
   RUN_TEST( test_Clock_EndFrame_NonLagFrameSleepsForCorrectDuration );
   RUN_TEST( test_Clock_EndFrame_LagFrameDoesNotSleepAndUpdatesLagFrameCount );

   return UNITY_END();
}
