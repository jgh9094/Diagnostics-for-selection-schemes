/// These are the digital organisms we are using for the diagnostic experiments

#ifndef DIA_ORG_H
#define DIA_ORG_H

///< standard headers
#include <functional>
#include <utility>
#include <cmath>
#include <fstream>
#include <string>

///< empirical headers
#include "base/vector.h"

///< static variables
constexpr double START = 0.0;

class DiaOrg {

public:
  // typname for genotype of solution
  using geno_t = emp::vector<double>;
  // typname for genotype of solution
  using pheno_t = emp::vector<double>;
  // typename for vector of bools
  using opti_t = emp::vector<bool>;

public:
  DiaOrg(size_t traits)
  {
    // Initialize everything in a solution
    genome.resize(traits, START);
    performance.resize(traits, START);
    optimized.resize(traits, false);

  }

  // ask Charles lol :|
  DiaOrg(const DiaOrg &) = default;
  DiaOrg(DiaOrg &&) = default;
  ~DiaOrg() { ; }
  DiaOrg & operator=(const DiaOrg &) = default;
  DiaOrg & operator=(DiaOrg &&) = default;

  /* functions for computing fitness and optimization counts */

  // calculate and save aggregate fitness
  double AggreateFitness();
  // check if trait is optimized
  bool Optimized(const double & trt, const double & thresh);
  // cacluate optimized trait count
  size_t OptimizedCount();

  /* functions for computing performance based on diagnostic */

  // calculate performance with respect to exploitation
  void Exploitation();
  // calculate performance with respect to structured exploitation
  void Structured();
  // calculate performance with respect to contradictory traits
  void Contradiction();
  // calculate performance with respect to Exploration
  void Exploration();

  /* getters */

  // const + reference to
  const genome_t & GetGenome() const {return genome;}
  const phenot_t & GetPheno() const {return score;}
  // reference to
  genome_t & GetGenome() { return genome; }
  phenot_t & GetPheno() { return score; }
  // make a copy
  genome_t GetGenome() { return genome; }
  phenot_t GetPheno() { return score; }
  // get const aggregate fitness
  double GetAggFit() const { return total; }
  // get const individual trait performance
  double GetPerf(const size_t & i) const {emp_assert(i >= 0, i); emp_assert(i < performance.size(), i); return performance[i];}
  // get const trait
  double GetTrait(const size_t & i) const {emp_assert(i >= 0, i); emp_assert(i < genome.size(), i); return genome[i];}

  /* setters */

  // set genome
  void SetGenome(genome_t g) { genome = g; }
  // set trait
  void SetTrait(const size_t & i, const double & _t) {emp_assert(i >= 0, i); emp_assert(i < genome.size(), i); genome[i] = _t;}
  // set performance
  void SetPerf(const size_t & i, const double & _s) {emp_assert(i >= 0, i); emp_assert(i < performance.size(), i); performance[i] = _s;}
  void Reset(size_t K_) {
    // Initialize everything in a solution
    genome.resize(traits, START);
    performance.resize(traits, START);
    optimized.resize(traits, false);
    fit_agg = 0.0;
    trait_cnt = 0;
  }


private:

  // aggregate fitness
  double fit_agg = 0;
  // optimized trait count
  size_t trait_cnt = 0;
  // trait vector
  geno_t genome;
  // performance vector
  pheno_t performance;
  // what traits are optimized
  opti_t optimized;
  // are we similar to the parent
  bool different = false;
};

#endif
