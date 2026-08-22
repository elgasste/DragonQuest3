#if !defined( MOCK_INPUT_H )
#define MOCK_INPUT_H

#include "../../include/input.h"

typedef struct Input_t
{
   InputButtonState_t buttonStates[InputButton_Count];
}
Input_t;

#endif // MOCK_INPUT_H
