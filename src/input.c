#include "input.h"

void Input_Init( Input_t* input )
{
   Input_ResetAllStates( input );
}

void Input_ResetAllStates( Input_t* input )
{
   u32 i;

   for ( i = 0; i < InputButton_Count; i++ )
   {
      input->buttonStates[i].pressed = False;
      input->buttonStates[i].released = False;
      input->buttonStates[i].down = False;
   }
}

void Input_ResetPressStates( Input_t* input )
{
   u32 i;
   InputButtonState_t* state = input->buttonStates;

   for ( i = 0; i < InputButton_Count; i++ )
   {
      state->pressed = False;
      state->released = False;
      state++;
   }
}

void Input_PressButton( Input_t* input, InputButton_t button )
{
   InputButtonState_t* state = &( input->buttonStates[button] );

   if ( !state->down )
   {
      state->down = True;
      state->pressed = True;
      state->released = False;
   }
}

void Input_ReleaseButton( Input_t* input, InputButton_t button )
{
   InputButtonState_t* state = &( input->buttonStates[button] );

   if ( state->down )
   {
      state->down = False;
      state->pressed = False;
      state->released = True;
   }
}

b32 Input_IsButtonDown( Input_t* input, InputButton_t button )
{
   return input->buttonStates[button].down;
}

b32 Input_AnyButtonPressed( Input_t* input )
{
   u32 i;

   for ( i = 0; i < InputButton_Count; i++ )
   {
      if ( input->buttonStates[i].pressed )
      {
         return True;
      }
   }

   return False;
}
