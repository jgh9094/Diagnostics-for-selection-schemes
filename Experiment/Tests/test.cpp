#define CATCH_CONFIG_MAIN

// testing files
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"
#include "../source/problem.h"

// empirical headers
#include "base/vector.h"

// To run: clang++ -std=c++17 -I ../../../Empirical/source/ test.cpp -o test; ./test -l -t -s

TEST_CASE( "Problem class initialization", "[initialization]" )
{
  // first round of tests
  emp::vector<double> tar(1.0,100);
  emp::vector<double> tar_d(1.0,100);
  Diagnostic diagnostic(tar);

  // make sure we are saving the vector and can find the same one
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(tar));
  // make sure we can get the same vector contents with differnt vars
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(tar_d));

  // switch the target vector
  emp::vector<double> targ(2.0, 100);
  emp::vector<double> targ1(2.0, 100);
  diagnostic.SetTarget(targ);

  //make sure the target vector is switched
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(targ));
  // make sure we get the same vector contents with different vars
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(targ1));


}