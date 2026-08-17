#if !defined ( INPUT_H )
#define INPUT_H

#include "common.h"

typedef struct MemArena_t MemArena_t;

typedef enum InputButton_t
{
   InputButton_A = 0,
   InputButton_B,
   InputButton_Select,
   InputButton_Start,
   InputButton_Up,
   InputButton_Down,
   InputButton_Left,
   InputButton_Right,

   InputButton_Count
}
InputButton_t;

typedef struct InputButtonState_t
{
   b32 pressed;
   b32 released;
   b32 down;
}
InputButtonState_t;

typedef struct Input_t Input_t;
Input_t* Input_Create( MemArena_t* memArena );
void Input_Free( Input_t* input, MemArena_t* memArena );

const InputButtonState_t* Input_GetButtonState( Input_t* input, InputButton_t button );

void Input_ResetAllStates( Input_t* input );
void Input_ResetPressStates( Input_t* input );
void Input_PressButton( Input_t* input, InputButton_t button );
void Input_ReleaseButton( Input_t* input, InputButton_t button );
b32 Input_IsButtonDown( Input_t* input, InputButton_t button );
b32 Input_AnyButtonPressed( Input_t* input );

#endif // INPUT_H
