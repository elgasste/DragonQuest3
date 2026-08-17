#include <stdlib.h>

#include "input.h"
#include "unity.h"

void setUp( void ) {}
void tearDown( void ) {}

void* MemArena_Alloc( MemArena_t* arena, size_t size )
{
   UNUSED_PARAM( arena );
   return malloc( size );
}

void MemArena_Free( MemArena_t* arena, void* mem )
{
   UNUSED_PARAM( arena );
   free( mem );
}

void test_Input_Init_InitializesAllButtonStatesToFalse( void )
{
   Input_t* input;
   u32 i;

   input = Input_Create( NULL );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->pressed );
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->released );
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->down );
   }

   Input_Free( input, NULL );
}

void test_Input_ResetAllStates_ResetsAllButtonStatesToFalse( void )
{
   Input_t* input;
   u32 i;

   input = Input_Create( NULL );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      Input_PressButton( input, (InputButton_t)i );
   }

   Input_ResetAllStates( input );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->pressed );
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->released );
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->down );
   }

   Input_Free( input, NULL );
}

void test_Input_ResetPressStates_ResetsPressedAndReleasedStatesButLeavesDownAlone( void )
{
   Input_t* input;
   u32 i;

   input = Input_Create( NULL );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      Input_PressButton( input, (InputButton_t)i );
   }

   Input_ResetPressStates( input );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->pressed );
      TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, (InputButton_t)i )->released );
      TEST_ASSERT_EQUAL( True, Input_GetButtonState( input, (InputButton_t)i )->down );
   }

   Input_Free( input, NULL );
}

void test_Input_PressButton_SetsPressedAndDownStates( void )
{
   Input_t* input;

   input = Input_Create( NULL );

   Input_PressButton( input, InputButton_Start );
   TEST_ASSERT_EQUAL( True, Input_GetButtonState( input, InputButton_Start )->pressed );
   TEST_ASSERT_EQUAL( True, Input_GetButtonState( input, InputButton_Start )->down );
   TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, InputButton_Start )->released );

   Input_Free( input, NULL );
}

void test_Input_ReleaseButton_SetsReleasedAndDownStates( void )
{
   Input_t* input;

   input = Input_Create( NULL );
   Input_PressButton( input, InputButton_Select );

   Input_ReleaseButton( input, InputButton_Select );
   TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, InputButton_Select )->pressed );
   TEST_ASSERT_EQUAL( False, Input_GetButtonState( input, InputButton_Select )->down );
   TEST_ASSERT_EQUAL( True, Input_GetButtonState( input, InputButton_Select )->released );

   Input_Free( input, NULL );
}

void test_Input_IsButtonDown_ReturnsTrueIfButtonIsDown( void )
{
   Input_t* input;

   input = Input_Create( NULL );
   TEST_ASSERT_EQUAL( False, Input_IsButtonDown( input, InputButton_A ) );

   Input_PressButton( input, InputButton_A );
   TEST_ASSERT_EQUAL( True, Input_IsButtonDown( input, InputButton_A ) );

   Input_ReleaseButton( input, InputButton_A );
   TEST_ASSERT_EQUAL( False, Input_IsButtonDown( input, InputButton_A ) );

   Input_Free( input, NULL );
}

void test_Input_IsButtonDown_ReturnsFalseIfButtonIsNotDown( void )
{
   Input_t* input;

   input = Input_Create( NULL );
   TEST_ASSERT_EQUAL( False, Input_IsButtonDown( input, InputButton_B ) );

   Input_PressButton( input, InputButton_B );
   TEST_ASSERT_EQUAL( True, Input_IsButtonDown( input, InputButton_B ) );

   Input_ReleaseButton( input, InputButton_B );
   TEST_ASSERT_EQUAL( False, Input_IsButtonDown( input, InputButton_B ) );

   Input_Free( input, NULL );
}

void test_Input_AnyButtonPressed_ReturnsTrueIfAnyButtonIsPressed( void )
{
   Input_t* input;

   input = Input_Create( NULL );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( input ) );

   Input_PressButton( input, InputButton_A );
   TEST_ASSERT_EQUAL( True, Input_AnyButtonPressed( input ) );

   Input_ResetPressStates( input );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( input ) );

   Input_PressButton( input, InputButton_B );
   TEST_ASSERT_EQUAL( True, Input_AnyButtonPressed( input ) );

   Input_Free( input, NULL );
}

void test_Input_AnyButtonPressed_ReturnsFalseIfNoButtonsArePressed( void )
{
   Input_t* input;

   input = Input_Create( NULL );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( input ) );

   Input_PressButton( input, InputButton_A );
   TEST_ASSERT_EQUAL( True, Input_AnyButtonPressed( input ) );

   Input_ReleaseButton( input, InputButton_A );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( input ) );

   Input_Free( input, NULL );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Input_Init_InitializesAllButtonStatesToFalse );
   RUN_TEST( test_Input_ResetAllStates_ResetsAllButtonStatesToFalse );

   RUN_TEST( test_Input_ResetPressStates_ResetsPressedAndReleasedStatesButLeavesDownAlone );

   RUN_TEST( test_Input_PressButton_SetsPressedAndDownStates );

   RUN_TEST( test_Input_ReleaseButton_SetsReleasedAndDownStates );

   RUN_TEST( test_Input_IsButtonDown_ReturnsTrueIfButtonIsDown );
   RUN_TEST( test_Input_IsButtonDown_ReturnsFalseIfButtonIsNotDown );

   RUN_TEST( test_Input_AnyButtonPressed_ReturnsTrueIfAnyButtonIsPressed );
   RUN_TEST( test_Input_AnyButtonPressed_ReturnsFalseIfNoButtonsArePressed );

   return UNITY_END();
}
