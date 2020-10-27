#define CATCH_CONFIG_MAIN
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"

// To run: clear; clang++ -std=c++17 test.cpp -o test; ./test

unsigned int Factorial( unsigned int number ) {
  return number > 1 ? Factorial(number-1)*number : 1;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(0) == 1 );
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}