#include "utility.h"

i32 Utility_FloorDiv32i( i32 value, i32 divisor )
{
   if ( divisor == 0 )
   {
      return 0;
   }

   if ( value >= 0 )
   {
      return value / divisor;
   }

   return -( ( -value + divisor - 1 ) / divisor );
}

b32 Utility_RectsOverlap( Vector4i32_t rect1, Vector4i32_t rect2 )
{
   return !( rect1.x + rect1.w <= rect2.x ||
             rect2.x + rect2.w <= rect1.x ||
             rect1.y + rect1.h <= rect2.y ||
             rect2.y + rect2.h <= rect1.y );
}
