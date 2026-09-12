#if !defined( UTILITY_H )
#define UTILITY_H

#include "common.h"
#include "vector.h"

i32 Utility_FloorDiv32i( i32 value, i32 divisor );
b32 Utility_RectsOverlap( Vector4i32_t rect1, Vector4i32_t rect2 );

#endif // UTILITY_H
