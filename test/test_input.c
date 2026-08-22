#include <stdlib.h>

#include "input.h"
#include "unity.h"

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

internal void AssertButtonState( Input_t* input, InputButton_t button, b32 pressed, b32 released, b32 down )
{
   const InputButtonState_t* state = Input_GetButtonState( input, button );

   TEST_ASSERT_EQUAL_INT( pressed, state->pressed );
   TEST_ASSERT_EQUAL_INT( released, state->released );
   TEST_ASSERT_EQUAL_INT( down, state->down );
}

void setUp( void ) {}
void tearDown( void ) {}

void test_Input_GetStructSize_ReturnsNonZeroSize( void )
{
   TEST_ASSERT_GREATER_THAN_UINT( 0, Input_GetStructSize() );
}

void test_Input_Create_InitializesEveryButtonState( void )
{
   u32 button;
   Input_t* input;

   input = Input_Create( 0 );

   TEST_ASSERT_NOT_NULL( input );
   for ( button = 0; button < InputButton_Count; button++ )
   {
      AssertButtonState( input, (InputButton_t)button, False, False, False );
   }

   Input_Free( input, 0 );
}

void test_Input_PressButton_SetsPressedAndDown( void )
{
   Input_t* input;

   input = Input_Create( 0 );

   Input_PressButton( input, InputButton_A );
   AssertButtonState( input, InputButton_A, True, False, True );
   TEST_ASSERT_TRUE( Input_IsButtonDown( input, InputButton_A ) );
   TEST_ASSERT_TRUE( Input_AnyButtonPressed( input ) );

   Input_Free( input, 0 );
}

void test_Input_PressButton_AlreadyDownDoesNotRepeatPress( void )
{
   Input_t* input;

   input = Input_Create( 0 );
   Input_PressButton( input, InputButton_A );
   Input_ResetPressStates( input );

   Input_PressButton( input, InputButton_A );
   AssertButtonState( input, InputButton_A, False, False, True );
   TEST_ASSERT_FALSE( Input_AnyButtonPressed( input ) );

   Input_Free( input, 0 );
}

void test_Input_ReleaseButton_SetsReleasedAndUp( void )
{
   Input_t* input;

   input = Input_Create( 0 );
   Input_PressButton( input, InputButton_B );

   Input_ReleaseButton( input, InputButton_B );
   AssertButtonState( input, InputButton_B, False, True, False );
   TEST_ASSERT_FALSE( Input_IsButtonDown( input, InputButton_B ) );

   Input_Free( input, 0 );
}

void test_Input_ReleaseButton_AlreadyUpDoesNotRepeatRelease( void )
{
   Input_t* input;

   input = Input_Create( 0 );

   Input_ReleaseButton( input, InputButton_B );
   AssertButtonState( input, InputButton_B, False, False, False );

   Input_Free( input, 0 );
}

void test_Input_ResetPressStates_ClearsEdgesAndPreservesHeldButtons( void )
{
   Input_t* input;

   input = Input_Create( 0 );
   Input_PressButton( input, InputButton_Left );

   Input_ResetPressStates( input );
   AssertButtonState( input, InputButton_Left, False, False, True );
   TEST_ASSERT_FALSE( Input_AnyButtonPressed( input ) );

   Input_ReleaseButton( input, InputButton_Left );
   Input_ResetPressStates( input );
   AssertButtonState( input, InputButton_Left, False, False, False );

   Input_Free( input, 0 );
}

void test_Input_ResetAllStates_ClearsAllButtons( void )
{
   u32 button;
   Input_t* input;

   input = Input_Create( 0 );
   Input_PressButton( input, InputButton_A );
   Input_PressButton( input, InputButton_Right );
   Input_ReleaseButton( input, InputButton_A );
   Input_ResetAllStates( input );

   for ( button = 0; button < InputButton_Count; button++ )
   {
      AssertButtonState( input, (InputButton_t)button, False, False, False );
   }
   TEST_ASSERT_FALSE( Input_AnyButtonPressed( input ) );

   Input_Free( input, 0 );
}

void test_Input_AnyButtonPressed_TracksEachButtonIndependently( void )
{
   Input_t* input;

   input = Input_Create( 0 );
   TEST_ASSERT_FALSE( Input_AnyButtonPressed( input ) );
   Input_PressButton( input, InputButton_Count - 1 );
   TEST_ASSERT_TRUE( Input_AnyButtonPressed( input ) );
   Input_ResetPressStates( input );
   TEST_ASSERT_FALSE( Input_AnyButtonPressed( input ) );

   Input_Free( input, 0 );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Input_GetStructSize_ReturnsNonZeroSize );

   RUN_TEST( test_Input_Create_InitializesEveryButtonState );

   RUN_TEST( test_Input_PressButton_SetsPressedAndDown );
   RUN_TEST( test_Input_PressButton_AlreadyDownDoesNotRepeatPress );

   RUN_TEST( test_Input_ReleaseButton_SetsReleasedAndUp );
   RUN_TEST( test_Input_ReleaseButton_AlreadyUpDoesNotRepeatRelease );

   RUN_TEST( test_Input_ResetPressStates_ClearsEdgesAndPreservesHeldButtons );

   RUN_TEST( test_Input_ResetAllStates_ClearsAllButtons );
   
   RUN_TEST( test_Input_AnyButtonPressed_TracksEachButtonIndependently );

   return UNITY_END();
}
