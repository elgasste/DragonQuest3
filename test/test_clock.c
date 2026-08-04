#include "clock.h"
#include "platform_ops.h"
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

u64 PlatformOps_GetMicros( void )
{
   g_platformOpsGetMicrosCall.callCount++;
   return g_platformOpsGetMicrosCall.returnValue;
}

void PlatformOps_SleepMs( u32 ms )
{
   g_platformOpsSleepMsCall.ms = ms;
   g_platformOpsSleepMsCall.callCount++;
}

void test_Clock_Init_InitializesClockWithCorrectParameters( void )
{
   u64 expectedFrameMicroSec;
   r32 expectedFrameSec;
   Clock_t clock;

   expectedFrameMicroSec = 1000000 / 30;
   expectedFrameSec = 1.0f / 30.0f;

   Clock_Init( &clock, 30 );

   TEST_ASSERT_EQUAL( 30, clock.fps );
   TEST_ASSERT_EQUAL( expectedFrameMicroSec, clock.frameMicroSec );
   TEST_ASSERT_EQUAL( expectedFrameSec, clock.frameSec );
   TEST_ASSERT_EQUAL( 0, clock.frameStartMicro );
   TEST_ASSERT_EQUAL( 0, clock.absoluteStartMicro );
   TEST_ASSERT_EQUAL( 0, clock.lastframeMicro );
   TEST_ASSERT_EQUAL( 0, clock.frameCount );
   TEST_ASSERT_EQUAL( 0, clock.lagFrameCount );
   TEST_ASSERT_EQUAL( False, clock.hasStarted );
}

void test_Clock_SetFps_UpdatesFrameMicroSecAndFrameSec( void )
{
   u64 expectedFrameMicroSec;
   r32 expectedFrameSec;
   Clock_t clock;

   Clock_Init( &clock, 30 );

   expectedFrameMicroSec = 1000000 / 60;
   expectedFrameSec = 1.0f / 60.0f;

   Clock_SetFps( &clock, 60 );

   TEST_ASSERT_EQUAL( 60, clock.fps );
   TEST_ASSERT_EQUAL( expectedFrameMicroSec, clock.frameMicroSec );
   TEST_ASSERT_EQUAL( expectedFrameSec, clock.frameSec );
}

void test_Clock_StartFrame_ClockIsMarkedAsStarted( void )
{
   Clock_t clock;

   Clock_Init( &clock, 30 );
   TEST_ASSERT_EQUAL( False, clock.hasStarted );

   Clock_StartFrame( &clock );
   TEST_ASSERT_EQUAL( True, clock.hasStarted );
}

void test_Clock_StartFrame_InitializesStartMicroValues( void )
{
   Clock_t clock;

   Clock_Init( &clock, 30 );   
   g_platformOpsGetMicrosCall.returnValue = 100;

   Clock_StartFrame( &clock );
   TEST_ASSERT_EQUAL( 1, g_platformOpsGetMicrosCall.callCount );
   TEST_ASSERT_EQUAL( 100, clock.frameStartMicro );
   TEST_ASSERT_EQUAL( 100, clock.absoluteStartMicro );
   TEST_ASSERT_EQUAL( 100, clock.absoluteEndMicro );
}

void test_Clock_EndFrame_UpdatesParametersCorrectly( void )
{
   Clock_t clock;

   Clock_Init( &clock, 60 );
   g_platformOpsGetMicrosCall.returnValue = 100;

   Clock_StartFrame( &clock );
   TEST_ASSERT_EQUAL( 1, g_platformOpsGetMicrosCall.callCount );
   TEST_ASSERT_EQUAL( 0, clock.frameCount );
   TEST_ASSERT_EQUAL( 100, clock.frameStartMicro );
   TEST_ASSERT_EQUAL( 100, clock.absoluteEndMicro );

   g_platformOpsGetMicrosCall.returnValue = 150;
   Clock_EndFrame( &clock );
   TEST_ASSERT_EQUAL( 2, g_platformOpsGetMicrosCall.callCount );
   TEST_ASSERT_EQUAL( 1, clock.frameCount );
   TEST_ASSERT_EQUAL( 150, clock.absoluteEndMicro );
   TEST_ASSERT_EQUAL( 50, clock.lastframeMicro );
}

void test_Clock_EndFrame_NonLagFrameSleepsForCorrectDuration( void )
{
   u64 frameMicroSec;
   Clock_t clock;

   Clock_Init( &clock, 60 );
   frameMicroSec = 1000000 / 60;  // 16,666

   Clock_StartFrame( &clock );
   g_platformOpsGetMicrosCall.returnValue = 10000;

   Clock_EndFrame( &clock );
   TEST_ASSERT_EQUAL( 1, clock.frameCount );
   TEST_ASSERT_EQUAL( 0, clock.lagFrameCount );
   TEST_ASSERT_EQUAL( 1, g_platformOpsSleepMsCall.callCount );
   TEST_ASSERT_EQUAL( (u32)( frameMicroSec - 10000 ) / 1000, g_platformOpsSleepMsCall.ms );
}

void test_Clock_EndFrame_LagFrameDoesNotSleepAndUpdatesLagFrameCount( void )
{
   u64 frameMicroSec;
   Clock_t clock;

   Clock_Init( &clock, 60 );
   frameMicroSec = 1000000 / 60;  // 16,666

   Clock_StartFrame( &clock );
   g_platformOpsGetMicrosCall.returnValue = 17000;

   Clock_EndFrame( &clock );
   TEST_ASSERT_EQUAL( 1, clock.frameCount );
   TEST_ASSERT_EQUAL( 1, clock.lagFrameCount );
   TEST_ASSERT_EQUAL( 0, g_platformOpsSleepMsCall.callCount );
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
