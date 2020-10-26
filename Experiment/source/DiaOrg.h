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

class DiaOrg
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
  DiaOrg(size_t traits)
  {
    // Initialize everything in a solution
    genome.resize(traits, START);
    performance.resize(traits, START);
    M = traits;
  }

  // for each generation after 0
  DiaOrg(geno_t g_) {genome = g_;}

  // ask Charles lol :|
  DiaOrg(const DiaOrg &) = default;
  DiaOrg(DiaOrg &&) = default;
  ~DiaOrg() { ; }
  DiaOrg &operator=(const DiaOrg &) = default;
  DiaOrg &operator=(DiaOrg &&) = default;

  /* functions for computing fitness and optimization counts */

  // calculate and save aggregate fitness
  double AggregateFitness();
  // is this trait optimized?
  bool Optimized(const size_t & trt, const double & target, const double & thrsh);
  // return the optmized count if calculated
  size_t OptimizedCount();
  // calculate number of optimized traits
  void CalculateOptimizedTrt(const pheno_t & target, const double & trsh);

  /* functions for computing performance based on diagnostic */

  // calculate performance with respect to exploitation
  void Exploitation(const pheno_t & target);
  // calculate performance with respect to structured exploitation
  void Structured(const pheno_t & target);
  // calculate performance with respect to contradictory traits
  void Contradiction(const pheno_t & target);
  // calculate performance with respect to Exploration
  void Exploration(const pheno_t & target);

  /* getters */

  // const + reference to
  const geno_t & GetGenome() const { return genome; }
  const pheno_t & GetPheno() const { return performance; }
  // reference to
  geno_t & GetGenome() { return genome; }
  pheno_t & GetPheno() { return performance; }
  // get const aggregate fitness
  double GetAggFit() const { return fit_agg; }
  // get const individual trait performance
  double GetPerf(const size_t & i) const
  {
    emp_assert(i >= 0, i);
    emp_assert(i < performance.size(), i);
    return performance[i];
  }
  // get const trait
  double GetTrait(const size_t & i) const
  {
    emp_assert(i >= 0, i);
    emp_assert(i < genome.size(), i);
    return genome[i];
  }
  // get const clone bool value
  const bool & GetClone() const {return clone;}


  /* setters */

  // set genome
  void SetGenome(geno_t g) { genome = g; }
  // set trait
  void SetTrait(const size_t & i, const double & _t)
  {
    emp_assert(i >= 0, i);
    emp_assert(i < genome.size(), i);
    genome[i] = _t;
  }
  // set performance
  void SetPerf(const size_t & i, const double & _s)
  {
    emp_assert(i >= 0, i);
    emp_assert(i < performance.size(), i);
    performance[i] = _s;
  }
  // Re-initialize everything in a solution
  void Reset(const size_t & traits);

  /* clone stuff */
  void Inherit(double & agg, pheno_t & per, size_t & m, size_t & trt_cnt);

private:
  // aggregate fitness
  double fit_agg = 0.0;
  // optimized trait count
  size_t trait_cnt = 0;
  // trait vector
  geno_t genome;
  // performance vector
  pheno_t performance;
  // are we similar to the parent
  bool clone = false;
  // number of traits
  size_t M = 0;
  // is optimal trait count calculated?
  bool counted = false;
  // is the aggregate calculated
  bool aggregate = false;
};

/* functions for computing fitness and optimization counts */

// calculate and save aggregate fitness
double DiaOrg::AggregateFitness()
{
  emp_assert(performance.size() == M, performance.size());

  if(!aggregate)
  {
    fit_agg = std::accumulate(performance.begin(), performance.end(), START);
    aggregate = true;
  }

  return fit_agg;
}

// is this trait optimized?
bool DiaOrg::Optimized(const size_t & trt, const double & target, const double & thrsh)
{
  // quick checks
  emp_assert(genome.size() == M, genome.size());
  emp_assert(thrsh >= 0, thrsh);
  emp_assert(trt >= 0, trt);
  emp_assert(trt < M, trt);

  return std::abs(genome[trt] - target) <= thrsh;
}

// return the optmized count if calculated
size_t DiaOrg::OptimizedCount()
{
  // some quick checks
  emp_assert(counted == true, counted);
  return trait_cnt;
}

// calculate number of optimized traits
void DiaOrg::CalculateOptimizedTrt(const pheno_t & target, const double & trsh)
{
  // quick checks
  emp_assert(target.size() == M, target.size());
  emp_assert(genome.size() == M, genome.size());
  emp_assert(trait_cnt == 0, trait_cnt);
  emp_assert(trsh >= 0.0, trsh);
  emp_assert(counted == false, counted);

  // go through the target vector and see if everything is met
  for(size_t i = 0; i < M; ++i)
  {
    // is this trait optimized?
    if(std::abs(target[i] - genome[i]) <= (target[i] * trsh)) {trait_cnt++;}
  }

  counted = true;
}

/* functions for computing performance based on diagnostic */

// calculate performance with respect to exploitation
void DiaOrg::Exploitation(const pheno_t & target)
{
  emp_assert(target.size() == M, target.size());
  emp_assert(genome.size() == M, genome.size());
  emp_assert(performance.size() == M, performance.size());

  for(size_t i = 0; i < M; ++i) { performance[i] = genome[i]; }
}

// calculate performance with respect to structured exploitation
void DiaOrg::Structured(const pheno_t & target)
{
  emp_assert(target.size() == M, target.size());

}

// calculate performance with respect to contradictory traits
void DiaOrg::Contradiction(const pheno_t & target)
{
  emp_assert(target.size() == M, target.size());

}

// calculate performance with respect to Exploration
void DiaOrg::Exploration(const pheno_t & target)
{
  emp_assert(target.size() == M, target.size());

}

/* setters */

// Re-initialize everything in a solution
// genome assumed to be already set
void DiaOrg::Reset(const size_t & traits)
{
  // quick checks
  emp_assert(traits > 0, traits);
  // we do not reset clones because everything is already set
  emp_assert(clone == false, clone);

  performance.resize(traits, START);
  fit_agg = 0.0;
  trait_cnt = 0;
  clone = false;
  counted = false;
  aggregate = false;
  M = traits;
}

/* clone stuff */

// inherit everything from parent solution
// genome assumed to be already set
void DiaOrg::Inherit(double & agg, pheno_t & per, size_t & m, size_t & trt_cnt)
{
  // quick checks
  emp_assert(agg >= 0.0, agg);
  emp_assert(m > 0, m);
  emp_assert(per.size() > 0, per.size());
  emp_assert(trt_cnt >= 0, trt_cnt);

  fit_agg = agg;
  performance = per;
  trait_cnt = trt_cnt;
  clone = true;
  M = m;
  counted = true;
  aggregate = true;
}

#endif