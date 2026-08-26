#include "utility.h"

i32 Utility_FloorDiv( i32 value, i32 divisor )
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
