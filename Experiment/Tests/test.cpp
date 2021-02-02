#define CATCH_CONFIG_MAIN

// testing files
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"
#include "../source/problem.h"

// empirical headers
#include "base/vector.h"

// library includes
#include <algorithm>
#include <string>

// To run: clang++ -std=c++17 -I ../../../Empirical/source/ test.cpp -o test; ./test

void PrintVec(const emp::vector<double> &v, const std::string s)
{
  std::cerr << s << ": ";
  for(auto x : v) {std::cerr << (double) x << ",";}
  std::cerr << std::endl;
}

TEST_CASE("Problem class initialization", "[initialization]")
{
  // first round of tests
  emp::vector<double> tar(1.0,100);
  emp::vector<double> tar_d(1.0,100);
  const double credit = 0.0;
  Diagnostic diagnostic(tar, credit);

  // make sure we are saving the vector and can find the same one
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(tar));
  // make sure we can get the same vector contents with differnt vars
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(tar_d));
  // make sure that credit is set
  REQUIRE(credit == diagnostic.GetCredit());

  // switch the target vector
  emp::vector<double> targ(2.0, 100);
  emp::vector<double> targ1(2.0, 100);
  diagnostic.SetTarget(targ);

  //make sure the target vector is switched
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(targ));
  // make sure we get the same vector contents with different vars
  REQUIRE_THAT(diagnostic.GetTarget(), Catch::Matchers::Equals(targ1));
}

TEST_CASE("Problem class exploration function", "[exploration]")
{
  // set up our genome and diagnostic to score with structured exploitation
  const size_t size = 10; const double cred = 0.0;
  emp::vector<double> g(size);
  emp::vector<double> correct(size);
  Diagnostic diag(g, cred);


  // create decending vector
  // <10, 9,..., 1>
  g = {10,9,8,7,6,5,4,3,2,1};
  correct = {10,9,8,7,6,5,4,3,2,1};
  // create diagnostic & test function with descending order
  emp::vector<double> score = diag.Exploration(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // make sure that ascending order vector is calcualted correctly
  // {1,2,3,4,5,6,7,8,9,10}
  std::reverse(g.begin(), g.end());
  // create correct vector & get score
  correct = {cred,cred,cred,cred,cred,cred,cred,cred,cred,10};
  score = diag.Exploration(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // middle to end scoring is correct
  g = {10,9,8,7,6,100,4,3,2,1};
  // create correct vector & get score
  correct = {cred, cred, cred, cred, cred, 100, 4, 3, 2, 1};
  score = diag.Exploration(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // early middle to later middle scoring is correct
  g = {10,9,100,7,6,5,10,3,2,1};
  // create correct vector & get score
  correct = {cred,cred,100,7,6,5,cred,cred,cred,cred};
  score = diag.Exploration(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // all the same values
  // <1,1,1,1,1,1,1,1,1,1>
  g = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  score = diag.Exploration(g);
  //create correct vector
  correct = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));
}

TEST_CASE("Problem class structured exploitation function", "[struct-exploit]")
{
  // set up our genome and diagnostic to score with structured exploitation
  const size_t size = 10; const double cred = 0.0;
  emp::vector<double> g(size);
  emp::vector<double> correct(size);
  emp::vector<double> score(size);
  Diagnostic diag(g, cred);

  // create decending vector
  g = {10,9,8,7,6,5,4,3,2,1};
  correct = {10,9,8,7,6,5,4,3,2,1};
  // create diagnostic & test function with descending order
  score = diag.StructExploitation(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // Making sure that a ascending order vector is calculated correctly
  // <1, 2,..., 10>
  std::reverse(g.begin(), g.end());
  score = diag.StructExploitation(g);
  // create correct vector
  correct = {1,cred,cred,cred,cred,cred,cred,cred,cred,cred};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // Checking accending order when last element out of order
  // <10,9,8,7,6,10,4,3,2,10>
  std::reverse(g.begin(), g.end());
  g.back() = size;
  score = diag.StructExploitation(g);
  // create correct vector
  correct = {10, 9, 8, 7, 6, 5, 4, 3, 2, cred};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // checking accending vector middle to end
  // <10,9,8,7,6,10,4,3,2,1>
  g.back() = 1;
  g[size/2] = size;
  score = diag.StructExploitation(g);
  // create correct vector
  correct = {10, 9, 8, 7, 6, cred, cred, cred, cred, cred};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // checking for accending vector middle to end
  g = {1,9,8,7,6,5,4,3,2,1};
  score = diag.StructExploitation(g);
  //create correct vector
  correct = {1, cred, cred, cred, cred, cred, cred, cred, cred,cred};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // all the same values
  g = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  score = diag.StructExploitation(g);
  //create correct vector
  correct = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));
}

TEST_CASE("Problem class contradictory ecology function", "[contra-ecology]")
{
  // set up our genome and diagnostic to score with structured exploitation
  const size_t size = 10; const double cred = 0.0;
  emp::vector<double> g(size);
  emp::vector<double> correct(size);
  emp::vector<double> score(size);
  Diagnostic diag(g, cred);


  // create decending vector, start the optimum
  g = {10,9,8,7,6,5,4,3,2,1};
  correct = {10,1,2,3,4,5,6,7,8,9};
  // create diagnostic & test function with descending order
  score = diag.ContraEcology(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // end of vector is optimum
  g = {10,9,8,7,6,5,4,3,2,20};
  correct = {10,11,12,13,14,15,16,17,18,20};
  // create diagnostic & test function with descending order
  score = diag.ContraEcology(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // start and end of vector is optimum
  g = {20,9,8,7,6,5,4,3,2,20};
  correct = {20,11,12,13,14,15,16,17,18,20};
  // create diagnostic & test function with descending order
  score = diag.ContraEcology(g);
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));


  // all the same values
  g = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  score = diag.ContraEcology(g);
  //create correct vector
  correct = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
  // PrintVec(g, "g"); PrintVec(score, "s"); PrintVec(correct, "c");
  REQUIRE_THAT(score, Catch::Matchers::Equals(correct));
}