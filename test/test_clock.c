#include <stdlib.h>

#include "clock.h"
#include "unity.h"

global u64 g_platformMicros;
global u32 g_platformSleepMs;
global u32 g_platformSleepCallCount;

void* MemArena_AllocMem( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   return malloc( size );
}

void MemArena_FreeMem( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   free( mem );
}

u64 Platform_GetMicros( void )
{
   return g_platformMicros;
}

void Platform_SleepMs( u32 ms )
{
   g_platformSleepMs = ms;
   g_platformSleepCallCount++;
}

void setUp( void )
{
   g_platformMicros = 0;
   g_platformSleepMs = 0;
   g_platformSleepCallCount = 0;
}

void tearDown( void ) {}

void test_Clock_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_UINT( 0, Clock_GetStructSize() );
}

void test_Clock_Create_InitializesClockState( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   TEST_ASSERT_NOT_NULL( clock );
   TEST_ASSERT_EQUAL_UINT( 60, Clock_GetFps( clock ) );
   TEST_ASSERT_EQUAL_FLOAT( 1.0f / 60.0f, Clock_GetFrameSec( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, (u32)Clock_GetAbsoluteStartMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, (u32)Clock_GetAbsoluteEndMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, (u32)Clock_GetLastFrameMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, Clock_GetLagFrameCount( clock ) );

   Clock_Free( clock, 0 );
}

void test_Clock_SetFps_UpdatesFrameRateValues( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 30 );

   Clock_SetFps( clock, 60 );
   TEST_ASSERT_EQUAL_UINT( 60, Clock_GetFps( clock ) );
   TEST_ASSERT_EQUAL_FLOAT( 1.0f / 60.0f, Clock_GetFrameSec( clock ) );

   Clock_Free( clock, 0 );
}

void test_Clock_StartFrame_FirstFrameInitializesAbsoluteTimes( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   g_platformMicros = 100;

   Clock_StartFrame( clock );
   TEST_ASSERT_EQUAL_UINT( 100, (u32)Clock_GetAbsoluteStartMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 100, (u32)Clock_GetAbsoluteEndMicro( clock ) );

   Clock_Free( clock, 0 );
}

void test_Clock_StartFrame_LaterFramesPreserveAbsoluteStartTime( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   g_platformMicros = 100;
   Clock_StartFrame( clock );
   g_platformMicros = 200;

   Clock_StartFrame( clock );
   TEST_ASSERT_EQUAL_UINT( 100, (u32)Clock_GetAbsoluteStartMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 100, (u32)Clock_GetAbsoluteEndMicro( clock ) );

   Clock_Free( clock, 0 );
}

void test_Clock_EndFrame_NormalFrameUpdatesTimingAndSleeps( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   g_platformMicros = 1000;
   Clock_StartFrame( clock );
   g_platformMicros = 2000;

   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL_UINT( 2000, (u32)Clock_GetAbsoluteEndMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1000, (u32)Clock_GetLastFrameMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, Clock_GetLagFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 15, g_platformSleepMs );
   TEST_ASSERT_EQUAL_UINT( 1, g_platformSleepCallCount );

   Clock_Free( clock, 0 );
}

void test_Clock_EndFrame_ExactFrameDurationDoesNotCountAsLag( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   g_platformMicros = 1000;
   Clock_StartFrame( clock );
   g_platformMicros = 17666;

   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL_UINT( 16666, (u32)Clock_GetLastFrameMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, Clock_GetLagFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, g_platformSleepMs );
   TEST_ASSERT_EQUAL_UINT( 1, g_platformSleepCallCount );

   Clock_Free( clock, 0 );
}

void test_Clock_EndFrame_LaggingFrameIncrementsLagCountWithoutSleeping( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   g_platformMicros = 1000;
   Clock_StartFrame( clock );
   g_platformMicros = 17667;

   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL_UINT( 16667, (u32)Clock_GetLastFrameMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1, Clock_GetLagFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 0, g_platformSleepCallCount );

   Clock_Free( clock, 0 );
}

void test_Clock_EndFrame_TracksMultipleFramesAndLags( void )
{
   Clock_t* clock;

   clock = Clock_Create( 0, 60 );
   g_platformMicros = 1000;
   Clock_StartFrame( clock );
   g_platformMicros = 2000;
   Clock_EndFrame( clock );
   g_platformMicros = 3000;
   Clock_StartFrame( clock );
   g_platformMicros = 20000;

   Clock_EndFrame( clock );
   TEST_ASSERT_EQUAL_UINT( 2, Clock_GetFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1, Clock_GetLagFrameCount( clock ) );
   TEST_ASSERT_EQUAL_UINT( 17000, (u32)Clock_GetLastFrameMicro( clock ) );
   TEST_ASSERT_EQUAL_UINT( 1, g_platformSleepCallCount );

   Clock_Free( clock, 0 );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Clock_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_Clock_Create_InitializesClockState );

   RUN_TEST( test_Clock_SetFps_UpdatesFrameRateValues );

   RUN_TEST( test_Clock_StartFrame_FirstFrameInitializesAbsoluteTimes );
   RUN_TEST( test_Clock_StartFrame_LaterFramesPreserveAbsoluteStartTime );
   
   RUN_TEST( test_Clock_EndFrame_NormalFrameUpdatesTimingAndSleeps );
   RUN_TEST( test_Clock_EndFrame_ExactFrameDurationDoesNotCountAsLag );
   RUN_TEST( test_Clock_EndFrame_LaggingFrameIncrementsLagCountWithoutSleeping );
   RUN_TEST( test_Clock_EndFrame_TracksMultipleFramesAndLags );

   return UNITY_END();
}
