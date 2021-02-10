#define CATCH_CONFIG_MAIN

// testing files
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"
#include "../source/org.h"

// empirical headers
#include "base/vector.h"

// library includes
#include <algorithm>
#include <string>

// In Tests directory, to run:
// clang++ -std=c++17 -I ../../../Empirical/source/ org-test.cpp -o org-test; ./org-test

TEST_CASE("Initialization functions","[init]")
{
  // initialize vars needed for orgs
  const size_t M = 10;
  emp::vector<double> start{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  emp::vector<double> x{1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0};

  // create both orgs
  Org a(M);
  Org b(x);

  // make sure that they have the correct genome to start
  REQUIRE_THAT(a.GetGenome(), Catch::Matchers::Equals(start));
  REQUIRE_THAT(b.GetGenome(), Catch::Matchers::Equals(x));

  // make sure that they have the correct M
  REQUIRE(a.GetM() == M);
  REQUIRE(b.GetM() == M);
}

TEST_CASE("Aggregate scoring functions", "[score]")
{
  // initialize vars needed for orgs
  const size_t M10 = 10; const size_t M11 = 11;
  emp::vector<double> x10{1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
  emp::vector<double> y11{0.0,10.0,20.0,30.0,40.0,50.0,60.0,70.0,80.0,90.0,100.0};

  // create orgs + set their score vectors
  Org a(M10); Org b(M11);
  a.SetScore(x10); b.SetScore(y11);

  // check if score vectors are correct
  REQUIRE_THAT(a.GetScore(), Catch::Matchers::Equals(x10));
  REQUIRE_THAT(b.GetScore(), Catch::Matchers::Equals(y11));

  // make sure that they have the correct M
  REQUIRE(a.GetM() == M10);
  REQUIRE(b.GetM() == M11);

  // now lets aggregate stuff and see if its correct
  a.AggregateScore(); b.AggregateScore();

  // check if they are the same
  REQUIRE(a.GetAggregate() == 55.0);
  REQUIRE(b.GetAggregate() == 550.0);
}

TEST_CASE("Optimal scoring functions", "[opti]")
{
  // initialize vars needed for orgs
  const size_t M10 = 10; const size_t M11 = 11;
  emp::vector<bool> x10{false,false,true,true,false,true,false,true,false,true};
  emp::vector<bool> y11{true,false,false,true,true,false,true,false,true,false,true};

  // create orgs + set their optimal vectors
  Org a(M10); Org b(M11);
  a.SetOptimal(x10); b.SetOptimal(y11);

  // check if optimal vectors are set correctly
  REQUIRE_THAT(a.GetOptimal(), Catch::Matchers::Equals(x10));
  REQUIRE_THAT(b.GetOptimal(), Catch::Matchers::Equals(y11));

  // have orgs count their optimal objectives
  a.CountOptimized(); b.CountOptimized();

  // check if they are the same
  REQUIRE(a.GetCount() == 5);
  REQUIRE(b.GetCount() == 6);


  // check if the solution optimal vector positions match original
  for(size_t i = 0; i < x10.size(); ++i){REQUIRE(a.OptimizedAt(i) == x10[i]);}
  for(size_t i = 0; i < y11.size(); ++i){REQUIRE(b.OptimizedAt(i) == y11[i]);}
}

TEST_CASE("Simulating birth offspring", "[birth]")
{
  // initialize vars needed for orgs
  const size_t M10 = 10; const size_t M11 = 11;
  // score vectors
  emp::vector<double> start{0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  emp::vector<double> x10{1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0};
  emp::vector<double> y11{0.0,10.0,20.0,30.0,40.0,50.0,60.0,70.0,80.0,90.0,100.0};
  // optimal vectors
  emp::vector<bool> bx0{false,false,false,false,false,false,false,false,false,false};
  emp::vector<bool> bx10{false,false,true,true,false,true,false,true,false,true};
  emp::vector<bool> by11{true,false,false,true,true,false,true,false,true,false,true};

  // create parent orgs
  Org a(M10); Org b(x10); Org c(y11);

  // vector size checks
  REQUIRE(a.GetM() == M10);
  REQUIRE(b.GetM() == M10);
  REQUIRE(c.GetM() == M11);

  // make sure correct vectors are stored
  REQUIRE_THAT(a.GetGenome(), Catch::Matchers::Equals(start));
  REQUIRE_THAT(b.GetGenome(), Catch::Matchers::Equals(x10));
  REQUIRE_THAT(c.GetGenome(), Catch::Matchers::Equals(y11));

  // set optimal and score vector
  a.SetScore(start); b.SetScore(x10); c.SetScore(y11);
  a.SetOptimal(bx0); b.SetOptimal(bx10); c.SetOptimal(by11);

  // make sure things are set correctly
  REQUIRE_THAT(a.GetScore(), Catch::Matchers::Equals(start));
  REQUIRE_THAT(b.GetScore(), Catch::Matchers::Equals(x10));
  REQUIRE_THAT(c.GetScore(), Catch::Matchers::Equals(y11));

  REQUIRE_THAT(a.GetOptimal(), Catch::Matchers::Equals(bx0));
  REQUIRE_THAT(b.GetOptimal(), Catch::Matchers::Equals(bx10));
  REQUIRE_THAT(c.GetOptimal(), Catch::Matchers::Equals(by11));

  // calculate the data needed (evaluation phase of ea)
  a.AggregateScore(); b.AggregateScore(); c.AggregateScore();
  a.CountOptimized(); b.CountOptimized(); c.CountOptimized();

  // make sure that things are caluclated correctly
  REQUIRE(a.GetAggregate() == 0.0);
  REQUIRE(b.GetAggregate() == 55.0);
  REQUIRE(c.GetAggregate() == 550.0);

  REQUIRE(a.GetCount() == 0.0);
  REQUIRE(b.GetCount() == 5.0);
  REQUIRE(c.GetCount() == 6.0);


  // create offspring solution (a,b,c) => (x,y,z)
  Org x(a.GetGenome()); Org y(b.GetGenome()); Org z(c.GetGenome());

  // x -> reset, y -> inherit from parent, z -> reset
  x.Reset(); y.MeClone(); z.Reset();

  // genome checks first
  REQUIRE_THAT(x.GetGenome(), Catch::Matchers::Equals(start));
  REQUIRE_THAT(y.GetGenome(), Catch::Matchers::Equals(x10));
  REQUIRE_THAT(z.GetGenome(), Catch::Matchers::Equals(y11));

  // check that internal guards are reset correctly for x z
  REQUIRE(!x.GetScored());
  REQUIRE(!x.GetOpti());
  REQUIRE(!x.GetCounted());
  REQUIRE(!x.GetAggregated());
  REQUIRE(!x.GetClone());

  REQUIRE(!z.GetScored());
  REQUIRE(!z.GetOpti());
  REQUIRE(!z.GetCounted());
  REQUIRE(!z.GetAggregated());
  REQUIRE(!z.GetClone());

  // inherit stuff for y and check if correctly set
  y.Inherit(b.GetScore(), b.GetOptimal(), b.GetCount(), b.GetAggregate());

  REQUIRE_THAT(y.GetScore(), Catch::Matchers::Equals(x10));
  REQUIRE(y.GetScored());
  REQUIRE_THAT(y.GetOptimal(), Catch::Matchers::Equals(bx10));
  REQUIRE(y.GetOpti());
  REQUIRE(y.GetCount() == 5.0);
  REQUIRE(y.GetCounted());
  REQUIRE(y.GetAggregate() == 55.0);
  REQUIRE(y.GetAggregated());
  REQUIRE(y.GetClone());
}