/// These are the digital organisms we are using for the diagnostic experiments

#ifndef DIA_ORG_H
#define DIA_ORG_H

///< standard headers
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <string>
#include <utility>

///< empirical headers
#include "base/vector.h"

///< static variables
constexpr double START = 0.0;

class Org
{

public:
  // typname for genotype of solution
  using geno_t = emp::vector<double>;
  // typname for genotype of solution
  using pheno_t = emp::vector<double>;
  // typename for vector of bools
  using opti_t = emp::vector<bool>;

public:
  // for initial population
  Org(size_t traits)
  {
    ;
  }
};

#endif