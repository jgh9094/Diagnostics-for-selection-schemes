#define CATCH_CONFIG_MAIN

// testing files
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"
#include "../source/selection.h"

// empirical headers
#include "base/vector.h"
#include "tools/Random.h"
#include "tools/random_utils.h"

// library includes
#include <algorithm>
#include <string>
#include <cmath>
#include <map>
#include <set>

// const vars for test
constexpr size_t SEED = 17;

constexpr double PNORM_EXP = 2.0;
constexpr double POW_EXP = 0.5;

constexpr double SIMP_EXP = 2.0;
constexpr double SIMP_ERR = -1.0;

constexpr size_t NEAR_K = 3;
constexpr double DOUB_K = 3.0;

constexpr double FIT_ERR = -1.0;

constexpr size_t LAMBDA = 20;
constexpr size_t L_RUNS = 5000;


// get unique elements in vector
emp::vector<size_t> Unique(const emp::vector<size_t> & v)
{
  emp::vector<size_t> u;
  std::set<size_t> s;

  for(const auto & i : v){s.insert(i);}
  for(const auto & i : s){u.push_back(i);}

  return u;
}

// check if vector x is a subset of vector y
bool Subset(const emp::vector<size_t> & x, const emp::vector<size_t> & y)
{
  // get the unique elements from each vector
  auto xs = Unique(x);
  // auto ys = Unique(y);

  // make sure we can find every element in x, in y
  for(const auto & v : x)
  {
    if(!std::binary_search(y.begin(), y.end(), v)){return false;}
  }

  return true;
}

// In Tests directory, to run:
// clang++ -std=c++17 -I ../../../Empirical/source/ selection-test.cpp -o selection-test; ./selection-test

TEST_CASE("Selection class Pnorm function", "[initialization]")
{
  // all the vars we will be altering
  const size_t size = 5; double res; double exp;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  emp::vector<double> x(size); emp::vector<double> y(size);
  Selection select(random);


  // first argument of std::pow calculated through wolfram alpha

  // just checking simple x vector computation
  x = {1.0,1.0,1.0,1.0,1.0};
  y = {0.0,0.0,0.0,0.0,0.0};
  res = select.Pnorm(x,y,PNORM_EXP);
  exp = std::pow(5.0, POW_EXP);
  REQUIRE(res == Approx(exp));
  // check that the reveserse also holds
  res = select.Pnorm(y,x,PNORM_EXP);
  REQUIRE(res == Approx(exp));


  // another bigger simple x vector computation
  x = {10.0,10.0,10.0,10.0,10.0};
  y = {0.0,0.0,0.0,0.0,0.0};
  res = select.Pnorm(x,y,PNORM_EXP);
  exp = std::pow(500.0, POW_EXP);
  REQUIRE(res == Approx(exp));
  // check that the reveserse also holds
  res = select.Pnorm(y,x,PNORM_EXP);
  REQUIRE(res == Approx(exp));


  // compute difference accuracy between vector x & y
  x = {10.0,10.0,10.0,10.0,10.0};
  y = {5.0,5.0,5.0,5.0,5.0};
  res = select.Pnorm(x,y,PNORM_EXP);
  exp = std::pow(125.0, POW_EXP);
  REQUIRE(res == Approx(exp));
  // check that the reveserse also holds
  res = select.Pnorm(y,x,PNORM_EXP);
  REQUIRE(res == Approx(exp));


  // check with small precision abnormalities
  x = {10.0,10.0,10.0,10.0,10.0};
  y = {5.5,5.5,5.5,5.5,5.5};
  res = select.Pnorm(x,y,PNORM_EXP);
  exp = std::pow(101.25, POW_EXP);
  REQUIRE(res == Approx(exp));
  // check that the reveserse also holds
  res = select.Pnorm(y,x,PNORM_EXP);
  REQUIRE(res == Approx(exp));

  random.Delete();
}

TEST_CASE("Selection class similarity matrix function","[similarity]")
{
  // all the vars we will be altering
  const size_t size = 5; const size_t pop = 4; double diff;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  emp::vector<double> x(size); emp::vector<double> y(size);
  emp::vector<double> u(size); emp::vector<double> v(size);
  emp::vector<emp::vector<double>> tmat(size-1);
  emp::vector<emp::vector<double>> mat(size-1);
  emp::vector<emp::vector<double>> exp(size-1);
  Selection select(random);


  // create genomes
  x = {1,1,1,1,1}; y = {1,1,1,1,1}; u = {1,1,1,1,1}; v = {1,1,1,1,1};
  // create genome matrix
  mat = {x,y,u,v};
  // test function
  tmat = select.SimilarityMatrix(mat, SIMP_EXP);
  // create expected return (0's gained from premature calculation)
  exp = {{SIMP_ERR,SIMP_ERR,SIMP_ERR,SIMP_ERR},{0,SIMP_ERR,SIMP_ERR,SIMP_ERR},{0,0,SIMP_ERR,SIMP_ERR},{0,0,0,SIMP_ERR}};
  // level one checks
  REQUIRE(tmat.size() == exp.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i){REQUIRE(tmat[i].size() == exp[i].size());}
  // level three checks
  for(size_t i = 0; i < pop; ++i){REQUIRE_THAT(tmat[i], Catch::Matchers::Equals(exp[i]));}


  // alter second genome
  // create genomes
  x = {1,1,1,1,1}; y = {0,0,0,0,0}; u = {1,1,1,1,1}; v = {1,1,1,1,1};
  // create genome matrix
  mat = {x,y,u,v};
  // test function
  tmat = select.SimilarityMatrix(mat, SIMP_EXP);
  // create expected return (0's gained from premature calculation)
  diff = std::pow(5.0, 0.5);
  exp = {{SIMP_ERR,SIMP_ERR,SIMP_ERR,SIMP_ERR},{diff,SIMP_ERR,SIMP_ERR,SIMP_ERR},{0,diff,SIMP_ERR,SIMP_ERR},{0,diff,0,SIMP_ERR}};
  // level one checks
  REQUIRE(tmat.size() == exp.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i){REQUIRE(tmat[i].size() == exp[i].size());}
  // level three checks
  for(size_t i = 0; i < pop; ++i){REQUIRE_THAT(tmat[i], Catch::Matchers::Equals(exp[i]));}


  // alter third genome
  // create genomes
  x = {1,1,1,1,1}; y = {1,1,1,1,1}; u = {0,0,0,0,0}; v = {1,1,1,1,1};
  // create genome matrix
  mat = {x,y,u,v};
  // test function
  tmat = select.SimilarityMatrix(mat, SIMP_EXP);
  // create expected return (0's gained from premature calculation)
  diff = std::pow(5.0, 0.5);
  exp = {{SIMP_ERR,SIMP_ERR,SIMP_ERR,SIMP_ERR},{0,SIMP_ERR,SIMP_ERR,SIMP_ERR},{diff,diff,SIMP_ERR,SIMP_ERR},{0,0,diff,SIMP_ERR}};
  // level one checks
  REQUIRE(tmat.size() == exp.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i){REQUIRE(tmat[i].size() == exp[i].size());}
  // level three checks
  for(size_t i = 0; i < pop; ++i){REQUIRE_THAT(tmat[i], Catch::Matchers::Equals(exp[i]));}


  // alter last genome
  // create genomes
  x = {1,1,1,1,1}; y = {1,1,1,1,1}; u = {1,1,1,1,1}; v = {0,0,0,0,0};
  // create genome matrix
  mat = {x,y,u,v};
  // test function
  tmat = select.SimilarityMatrix(mat, SIMP_EXP);
  // create expected return (0's gained from premature calculation)
  diff = std::pow(5.0, 0.5);
  exp = {{SIMP_ERR,SIMP_ERR,SIMP_ERR,SIMP_ERR},{0,SIMP_ERR,SIMP_ERR,SIMP_ERR},{0,0,SIMP_ERR,SIMP_ERR},{diff,diff,diff,SIMP_ERR}};
  // level one checks
  REQUIRE(tmat.size() == exp.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i){REQUIRE(tmat[i].size() == exp[i].size());}
  // level three checks
  for(size_t i = 0; i < pop; ++i){REQUIRE_THAT(tmat[i], Catch::Matchers::Equals(exp[i]));}

  random.Delete();
}

TEST_CASE("Selection class fitness nearest neigbor function", "[neighbor]")
{
  // all the vars we will be altering
  const size_t pop = 10;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  emp::vector<emp::vector<double>> neigh(pop);
  emp::vector<emp::vector<double>> output(pop);
  emp::vector<double> score(pop);
  Selection select(random);


  // ascending order
  score = {1,2,3,4,5,6,7,8,9,10};
  // get neighborhood scores
  neigh = select.FitNearestN(score, NEAR_K);
  // build correct vector of fitnesses groups ~ in the event of ties we get the right one
  neigh = {{2,3,4},{3,1,4},{4,2,5},{5,3,6},{6,4,7},{7,5,8},{8,6,9},{9,7,10},{10,8,7},{9,8,7}};
  // get the fuction output
  output = select.FitNearestN(score, NEAR_K);
  // level one checks
  REQUIRE(neigh.size() == output.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i){REQUIRE(output[i].size() == NEAR_K); REQUIRE(neigh[i].size() == output[i].size());}
  // level three checks
  for(size_t i = 0; i < pop; ++i){REQUIRE_THAT(neigh[i], Catch::Matchers::Equals(output[i]));}


  // random order
  score = {10,2,3,5,4,8,7,6,9,1};
  // build correct vector of fitnesses groups ~ the neighborhoods are ordered according to scoring position and value
  //         10       2       3       5       4       8        7       6        9       1
  neigh = {{9,8,7},{3,1,4},{4,2,5},{6,4,7},{5,3,6},{9,7,10},{8,6,9},{7,5,8},{10,8,7},{2,3,4}};
  // get the fuction output
  output = select.FitNearestN(score, NEAR_K);
  // level one checks
  REQUIRE(neigh.size() == output.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i)
  {
    REQUIRE(output[i].size() == NEAR_K);
    REQUIRE(neigh[i].size() == output[i].size());
  }
  // level three checks
  for(size_t i = 0; i < pop; ++i)
  {
    REQUIRE_THAT(neigh[i], Catch::Matchers::Equals(output[i]));
  }


  // redundant scores
  score = {1,1,1,4,4,4,8,8,8,10};
  // build correct vector of fitnesses groups ~ the neighborhoods are ordered according to scoring position and value
  //          1       1       1       4       4       4       8       8        8       10
  neigh = {{1,1,4},{1,1,4},{1,1,4},{4,4,1},{4,4,1},{4,4,1},{8,8,10},{8,8,10},{8,8,10},{8,8,8}};
  // get the fuction output
  output = select.FitNearestN(score, NEAR_K);
  // level one checks
  REQUIRE(neigh.size() == output.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i)
  {
    REQUIRE(output[i].size() == NEAR_K);
    REQUIRE(neigh[i].size() == output[i].size());
  }
  // level three checks
  for(size_t i = 0; i < pop; ++i)
  {
    REQUIRE_THAT(neigh[i], Catch::Matchers::Equals(output[i]));
  }


  // redundant ransdom scores
  score = {10,4,8,1,1,8,4,8,4,1};
  // build correct vector of fitnesses groups ~ the neighborhoods are ordered according to scoring position and value
  //         10       4       8        1       1       8        4       8        4       1
  neigh = {{8,8,8},{4,4,1},{8,8,10},{1,1,4},{1,1,4},{8,8,10},{4,4,1},{8,8,10},{4,4,1},{1,1,4}};
  // get the fuction output
  output = select.FitNearestN(score, NEAR_K);
  // level one checks
  REQUIRE(neigh.size() == output.size());
  // level two checks
  for(size_t i = 0; i < pop; ++i)
  {
    REQUIRE(output[i].size() == NEAR_K);
    REQUIRE(neigh[i].size() == output[i].size());
  }
  // level three checks
  for(size_t i = 0; i < pop; ++i)
  {
    REQUIRE_THAT(neigh[i], Catch::Matchers::Equals(output[i]));
  }

  random.Delete();
}

TEST_CASE("Selection class fitness group function", "[fit-group]")
{
  // all the vars we will be altering
  const size_t pop = 10;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  std::map<double, emp::vector<size_t>, std::greater<int>> map;
  std::map<double, emp::vector<size_t>, std::greater<int>> output;
  emp::vector<double> score(pop);
  emp::vector<emp::vector<size_t>> fgroups;
  Selection select(random);


  // create a set of scores
  score = {1,2,3,4,5,6,7,8,9,10};
  // build correct vector of fitnesses groups ~ the neighborhoods are ordered according to scoring position and value
  //          1   2   3   4   5   6   7   8   9   10
  fgroups = {{0},{1},{2},{3},{4},{5},{6},{7},{8},{9}};
  // generate the map
  for(size_t i = 0; i < pop; ++i){map[score[i]] = fgroups[i];}
  // get map from function output
  output = select.FitnessGroup(score);
  // gather keys for both maps
  emp::vector<size_t> keys;
  for(const auto & p : map){keys.push_back(p.first);}
  emp::vector<size_t> out_keys;
  for(const auto & p : output){out_keys.push_back(p.first);}
  // level one checks
  REQUIRE(keys.size() == out_keys.size());
  REQUIRE_THAT(keys, Catch::Matchers::Equals(out_keys));
  // loop through and check that the
  for(const auto & k : keys){REQUIRE_THAT(map[k], Catch::Matchers::Equals(output[k]));}


  // redundant scores
  output.clear(); map.clear(); keys.clear(); out_keys.clear();
  score = {1,1,1,2,2,2,3,3,3,7};
  // build correct vector of fitnesses groups ~ the neighborhoods are ordered according to scoring position and value
  //          7     3       2       1
  fgroups = {{9},{6,7,8},{3,4,5},{0,1,2}};
  keys = {7,3,2,1};
  //generate the map
  for(size_t i = 0; i < keys.size(); ++i){map[keys[i]] = fgroups[i];}
  // get map from function output
  output = select.FitnessGroup(score);
  // get map from function output
  for(const auto & p : output){out_keys.push_back(p.first);}
  // level one checks
  REQUIRE(keys.size() == out_keys.size());
  REQUIRE_THAT(keys, Catch::Matchers::Equals(out_keys));
  // loop through and check that the
  for(const auto & k : keys){REQUIRE_THAT(map[k], Catch::Matchers::Equals(output[k]));}


  // redundant and random scores
  output.clear(); map.clear(); keys.clear(); out_keys.clear();
  score = {2,3,1,3,1,2,7,2,1,3};
  // build correct vector of fitnesses groups ~ the neighborhoods are ordered according to scoring position and value
  //          7     3       2       1
  fgroups = {{6},{1,3,9},{0,5,7},{2,4,8}};
  keys = {7,3,2,1};
  //generate the map
  for(size_t i = 0; i < keys.size(); ++i){map[keys[i]] = fgroups[i];}
  // get map from function output
  output = select.FitnessGroup(score);
  // get map from function output
  for(const auto & p : output){out_keys.push_back(p.first);}
  // level one checks
  REQUIRE(keys.size() == out_keys.size());
  REQUIRE_THAT(keys, Catch::Matchers::Equals(out_keys));
  // loop through and check that the
  for(const auto & k : keys){REQUIRE_THAT(map[k], Catch::Matchers::Equals(output[k]));}


  random.Delete();
}

TEST_CASE("Selection class fitness sharing function", "[fit-share]")
{
  // all the vars we will be altering
  const size_t pop = 10;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  emp::vector<double> score(pop);
  emp::vector<double> tscore(pop);
  emp::vector<double> exp(pop);
  emp::vector<emp::vector<double>> dmat;
  Selection select(random);


  // test function with easy set up
  score = {1,1,1};
  // create distance matrix with similar values
  dmat ={ {FIT_ERR, FIT_ERR, FIT_ERR},
          {.5, FIT_ERR, FIT_ERR},
          {.5, .5, FIT_ERR}
        };
  // expected output
  exp = {0.5,0.5,0.5};
  // get fucntion input with alpha=1.0, sigma=1.0
  tscore = select.FitnessSharing(dmat, score, 1.0, 1.0);
  // level 1 checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));


  // test function with easy set up and no fitness sharing applied to score
  score = {1,1,1};
  // create distance matrix with similar values
  dmat ={ {FIT_ERR, FIT_ERR, FIT_ERR},
          {.5, FIT_ERR, FIT_ERR},
          {.5, .5, FIT_ERR}
        };
  // expected output
  exp = {1.0,1.0,1.0};
  // get fucntion input with alpha=1.0, sigma=0.5
  tscore = select.FitnessSharing(dmat, score, 1.0, 0.5);
  // level 1 checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));


  // test function with partial fitness sharing
  score = {1,1,1};
  // create distance matrix with similar values
  dmat ={ {FIT_ERR, FIT_ERR, FIT_ERR},
          {1.0, FIT_ERR, FIT_ERR},
          {.5, .5, FIT_ERR}
        };
  // expected output
  exp = {1.0/1.5,1.0/1.5,1.0/2.0};
  // get fucntion input with alpha=1.0, sigma=1.0
  tscore = select.FitnessSharing(dmat, score, 1.0, 1.0);
  // level 1 checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));


  // test function with partial fitness sharing different scores
  score = {1.0,2.0,3.0};
  // create distance matrix with similar values
  dmat ={ {FIT_ERR, FIT_ERR, FIT_ERR},
          {1.0, FIT_ERR, FIT_ERR},
          {.5, .5, FIT_ERR}
        };
  // expected output
  exp = {1.0/1.5,2.0/1.5,3.0/2.0};
  // get fucntion input with alpha=1.0, sigma=1.0
  tscore = select.FitnessSharing(dmat, score, 1.0, 1.0);
  // level 1 checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));

  random.Delete();
}

TEST_CASE("Selection class novelty scoreing function", "[novelty]")
{
  // all the vars we will be altering
  const size_t pop = 10;
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  emp::vector<double> score(pop);
  emp::vector<double> tscore(pop);
  emp::vector<double> exp(pop);
  emp::vector<emp::vector<double>> neigh;
  Selection select(random);


  // create simple score vector
  score = {1.0,1.0,1.0,1.0,1.0};
  // create neighborhoods
  neigh = {{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0}};
  // create expected output
  exp = {0.0,0.0,0.0,0.0,0.0};
  // get function novelty score
  tscore = select.Novelty(score, neigh, NEAR_K);
  // checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));


  // create ascending score vector
  score = {1.0,2.0,3.0,4.0,5.0};
  // create neighborhoods
  //            1.0           2.0           3.0           4.0       5.0
  neigh = {{2.0,3.0,4.0},{3.0,1.0,4.0},{4.0,2.0,5.0},{5.0,3.0,2.0},{4.0,3.0,2.0}};
  // expected output
  exp = {6.0/DOUB_K,4.0/DOUB_K,4.0/DOUB_K,4.0/DOUB_K,6.0/DOUB_K};
  // get novelty score
  tscore = select.Novelty(score, neigh, NEAR_K);
  // checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));


  // create ascending score vector
  score = {1.0,2.0,3.0,4.0,5.0};
  // create same neighborhoods - don't care about realistic neighborhoods, just computations
  //            1.0           2.0           3.0           4.0            5.0
  neigh = {{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0},{1.0,1.0,1.0}};
  // expected output
  exp = {0.0/DOUB_K,3.0/DOUB_K,6.0/DOUB_K,9.0/DOUB_K,12.0/DOUB_K};
  // get novelty score
  tscore = select.Novelty(score, neigh, NEAR_K);
  // checks
  REQUIRE(exp.size() == tscore.size());
  REQUIRE_THAT(exp, Catch::Matchers::Equals(tscore));

  random.Delete();
}

TEST_CASE("Selection class mu lambda selector function", "[mu-lambda]")
{
  // all the vars we will be altering
  emp::Ptr<emp::Random> random = emp::NewPtr<emp::Random>(SEED);
  std::map<double, emp::vector<size_t>, std::greater<int>> fmap;
  emp::vector<emp::vector<size_t>> groups; emp::vector<emp::vector<size_t>> container;
  emp::vector<emp::vector<size_t>> parent;
  emp::vector<size_t> output;
  emp::vector<size_t> mu{1,2,4,5,10,20};
  emp::vector<double> scores;
  Selection select(random);


  // create keys of fitnesses we are using
  scores = {5.0,4.0,3.0,2.0,1.0};
  // create position ids for each score;
  groups = {{1},{6,7},{11,12},{16,17,18,19},{22,23,24,25,26,27,28,29,30,31,32}};
  // create fitness map
  for(size_t i = 0; i < scores.size(); ++i){fmap[scores[i]] = groups[i];}
  // create expected parent ids to see with per iteration
  //  mu =   1     2           4                  5                                   10                                                        20
  parent = {{1},{1,6,7},{1,6,7,11,12},{1,6,7,11,12,16,17,18,19},{1,6,7,11,12,16,17,18,19,22,23,24,25,26,27,28,29,30,31,32},{1,6,7,11,12,16,17,18,19,22,23,24,25,26,27,28,29,30,31,32}};
  // create a for loop that iterates through the expected parent subsets
  for(size_t i = 0; i < parent.size(); ++i)
  {
    // how many do we wanna test the expected output
    for(size_t j = 0; j < L_RUNS; ++j)
    {
      // expected output
      output = select.MLSelect(mu[i], LAMBDA, fmap);

      // level one checks
      REQUIRE(output.size() == LAMBDA);
      // make sure that output is a subset of expected parent set
      REQUIRE(Subset(output, parent[i]) == true);
      emp::vector<size_t>p{0};
    }
  }

  random.Delete();
}








