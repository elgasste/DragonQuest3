#include "input.h"
#include "unity.h"

void setUp( void ) {}
void tearDown( void ) {}

void test_Input_Init_InitializesAllButtonStatesToFalse( void )
{
   Input_t input;
   u32 i;

   Input_Init( &input );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      TEST_ASSERT_EQUAL( False, input.buttonStates[i].pressed );
      TEST_ASSERT_EQUAL( False, input.buttonStates[i].released );
      TEST_ASSERT_EQUAL( False, input.buttonStates[i].down );
   }
}

void test_Input_ResetPressStates_ResetsPressedAndReleasedStatesButLeavesDownAlone( void )
{
   Input_t input;
   u32 i;

   Input_Init( &input );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      input.buttonStates[i].pressed = True;
      input.buttonStates[i].released = True;
      input.buttonStates[i].down = True;
   }

   Input_ResetPressStates( &input );
   for ( i = 0; i < InputButton_Count; i++ )
   {
      TEST_ASSERT_EQUAL( False, input.buttonStates[i].pressed );
      TEST_ASSERT_EQUAL( False, input.buttonStates[i].released );
      TEST_ASSERT_EQUAL( True, input.buttonStates[i].down );
   }
}

void test_Input_PressButton_SetsPressedAndDownStates( void )
{
   Input_t input;

   Input_Init( &input );

   Input_PressButton( &input, InputButton_Start );
   TEST_ASSERT_EQUAL( True, input.buttonStates[InputButton_Start].pressed );
   TEST_ASSERT_EQUAL( True, input.buttonStates[InputButton_Start].down );
   TEST_ASSERT_EQUAL( False, input.buttonStates[InputButton_Start].released );
}

void test_Input_ReleaseButton_SetsReleasedAndDownStates( void )
{
   Input_t input;

   Input_Init( &input );
   Input_PressButton( &input, InputButton_Select );

   Input_ReleaseButton( &input, InputButton_Select );
   TEST_ASSERT_EQUAL( False, input.buttonStates[InputButton_Select].pressed );
   TEST_ASSERT_EQUAL( False, input.buttonStates[InputButton_Select].down );
   TEST_ASSERT_EQUAL( True, input.buttonStates[InputButton_Select].released );
}

void test_Input_AnyButtonPressed_ReturnsTrueIfAnyButtonIsPressed( void )
{
   Input_t input;

   Input_Init( &input );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( &input ) );

   Input_PressButton( &input, InputButton_A );
   TEST_ASSERT_EQUAL( True, Input_AnyButtonPressed( &input ) );

   Input_ResetPressStates( &input );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( &input ) );

   Input_PressButton( &input, InputButton_B );
   TEST_ASSERT_EQUAL( True, Input_AnyButtonPressed( &input ) );
}

void test_Input_AnyButtonPressed_ReturnsFalseIfNoButtonsArePressed( void )
{
   Input_t input;

   Input_Init( &input );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( &input ) );

   Input_PressButton( &input, InputButton_A );
   TEST_ASSERT_EQUAL( True, Input_AnyButtonPressed( &input ) );

   Input_ReleaseButton( &input, InputButton_A );
   TEST_ASSERT_EQUAL( False, Input_AnyButtonPressed( &input ) );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Input_Init_InitializesAllButtonStatesToFalse );

   RUN_TEST( test_Input_ResetPressStates_ResetsPressedAndReleasedStatesButLeavesDownAlone );

   RUN_TEST( test_Input_PressButton_SetsPressedAndDownStates );

   RUN_TEST( test_Input_ReleaseButton_SetsReleasedAndDownStates );

   RUN_TEST( test_Input_AnyButtonPressed_ReturnsTrueIfAnyButtonIsPressed );
   RUN_TEST( test_Input_AnyButtonPressed_ReturnsFalseIfNoButtonsArePressed );

   return UNITY_END();
}
