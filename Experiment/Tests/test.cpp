#define CATCH_CONFIG_MAIN

// testing files
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"
#include "../source/problem.h"

// empirical headers
#include "base/vector.h"

// library includes
#include <algorithm>
#include <string>

// To run: clang++ -std=c++17 -I ../../../Empirical/source/ test.cpp -o test; ./test -l -t -s

void PrintVec(const emp::vector<double> &v, const std::string s)
{
  std::cerr << s << ": ";
  for(auto x : v) {std::cerr << x << ",";}
  std::cerr << std::endl;
}

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

TEST_CASE( "Problem class structured exploitation function", "[struct-exploit]")
{
  // set up our genome and diagnostic to score with structured exploitation
  const size_t size = 10;
  const double error = -1.0;
  emp::vector<double> g(size);
  emp::vector<double> correct(size);


  /* Making sure that a decending vector is calculated correctly */
  for(size_t i = size; i > 0; --i) {g[size-i] = i;}
  // create diagnostic + test function
  Diagnostic diag;
  emp::vector<double> score = diag.StructExploitation(g, error);
  // checking
  REQUIRE_THAT(score, Catch::Matchers::Equals(g));


  /* Making sure that a decending sorted vector is calculated correctly */
  std::reverse(g.begin(), g.end());
  score = diag.StructExploitation(g, error);
  // create correct vector
  correct[0] = g[0];
  for(size_t i = 1; i < correct.size(); ++i) {correct[i] = error;}
  // checking
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  /* Checking accending vector last element out of order */
  std::reverse(g.begin(), g.end());
  g.back() = size;
  score = diag.StructExploitation(g, error);
  // create correct vector
  correct.clear(); correct.resize(size);
  std::copy(g.begin(), g.end(), correct.begin());
  correct.back() = error;
  // make sure we get the correct vector
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  /* Checking accending vector middle & last element out of order */
  g[size/2] = size;
  score = diag.StructExploitation(g, error);
  // create correct vector
  for(size_t i = size/2; i < correct.size(); ++i) {correct[i] = error;}
  // make sure we get the correct vector
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));

  // debug
  // PrintVec(g, "g");
  // PrintVec(correct, "c");
  // PrintVec(score, "s");
}
