#include "unity.h"

#include "utility.h"

void setUp( void ) {}

void tearDown( void ) {}

void test_Utility_FloorDiv_ReturnsQuotientForExactDivision( void )
{
   TEST_ASSERT_EQUAL_INT32( 4, Utility_FloorDiv( 20, 5 ) );
   TEST_ASSERT_EQUAL_INT32( -4, Utility_FloorDiv( -20, 5 ) );
}

void test_Utility_FloorDiv_RoundsNegativeQuotientDown( void )
{
   TEST_ASSERT_EQUAL_INT32( -5, Utility_FloorDiv( -21, 5 ) );
   TEST_ASSERT_EQUAL_INT32( -5, Utility_FloorDiv( -25, 5 ) );
}

void test_Utility_FloorDiv_HandlesValuesBetweenZeroAndDivisor( void )
{
   TEST_ASSERT_EQUAL_INT32( 0, Utility_FloorDiv( 0, 5 ) );
   TEST_ASSERT_EQUAL_INT32( 0, Utility_FloorDiv( 4, 5 ) );
   TEST_ASSERT_EQUAL_INT32( -1, Utility_FloorDiv( -1, 5 ) );
}

void test_Utility_FloorDiv_ReturnsZeroForZeroDivisor( void )
{
   TEST_ASSERT_EQUAL_INT32( 0, Utility_FloorDiv( 10, 0 ) );
   TEST_ASSERT_EQUAL_INT32( 0, Utility_FloorDiv( -10, 0 ) );
}

void test_Utility_FloorDiv_HandlesIntegerBoundaryValues( void )
{
   TEST_ASSERT_EQUAL_INT32( 429496729, Utility_FloorDiv( 2147483647, 5 ) );
   TEST_ASSERT_EQUAL_INT32( -429496728, Utility_FloorDiv( -2147483640, 5 ) );
}

int main( void )
{
   UNITY_BEGIN();

   RUN_TEST( test_Utility_FloorDiv_ReturnsQuotientForExactDivision );
   RUN_TEST( test_Utility_FloorDiv_RoundsNegativeQuotientDown );
   RUN_TEST( test_Utility_FloorDiv_HandlesValuesBetweenZeroAndDivisor );
   RUN_TEST( test_Utility_FloorDiv_ReturnsZeroForZeroDivisor );
   RUN_TEST( test_Utility_FloorDiv_HandlesIntegerBoundaryValues );

   return UNITY_END();
}