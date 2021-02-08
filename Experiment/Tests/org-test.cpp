#define CATCH_CONFIG_MAIN

// testing files
#include "/mnt/c/Users/josex/Desktop/Research/Repos/Catch/catch.hpp"
#include "../source/problem.h"

// empirical headers
#include "base/vector.h"

// library includes
#include <algorithm>
#include <string>

// In Tests directory, to run:
// clang++ -std=c++17 -I ../../../Empirical/source/ org-test.cpp -o org-test; ./org-test

template <class T>
void PrintVec(const emp::vector<T> &v, const std::string s)
{
  std::cerr << s << ": ";
  for(auto x : v) {std::cerr << (double) x << ",";}
  std::cerr << std::endl;
}
