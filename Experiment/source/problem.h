/// These are the digital organisms we are using for the diagnostic experiments

#ifndef DIA_PRO_H
#define DIA_PRO_H

///< standard headers
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <string>
#include <utility>

///< empirical headers
#include "base/vector.h"

class Diagnostic
{
  public:
    //typename for target vector
    using target_t = emp::vector<double>;
    using score_t = emp::vector<double>;
    using genome_t = emp::vector<double>;

  public:

    Diagnostic(target_t & t_)
    {
      target.resize(t_.size());
      std::copy(t_.begin(), t_.end(), target.begin());
    }

    Diagnostic() {;}

    // getters
    target_t GetTarget() const {return target;}
    // const target_t & GetTarget() const {return target;}

    //setters
    void SetTarget(target_t & t) {target.clear(); target.resize(t.size()); std::copy(t.begin(), t.end(), target.begin());}

    // exploration function
    score_t Exploration(const genome_t & g);

    /**
     * Exploration function. All positions in genome evaluated independently.
     * This means that we just return the same vector, in principle.
     * Assuming that genes in genome cannot go over the target.
     *
     * @param g Genome from organism being evaluated.
     * @return score vector that is calculated from 'g'.
     */
    score_t Exploitation(const genome_t & g);

    // ecological niche function
    score_t EcologyNiche(const genome_t & g);

    /**
     * Structured Exploitation function.
     * All genes in genome expect to follow descending order for evaluation.
     * If descending order is broken, max error assigned to every position after broken.
     * Assuming that genes in genome cannot go over the target.
     *
     * @param g Genome from organism being evaluated.
     * @param max_error This value is the maximum error assigned when structure broken
     * @return score vector that is calculated from 'g'.
     */
    score_t StructExploitation(const genome_t & g, double max_error);

  private:
    // variable that holds target vector
    target_t target;
};



// exploration function
Diagnostic::score_t Diagnostic::Exploration(const genome_t & g)
{
  // quick checks
  emp_assert(g.size() == target.size(), g.size());

  // intialize vector with size g
  score_t score(g.size());




  return score;
}


Diagnostic::score_t Diagnostic::Exploitation(const genome_t & g)
{
  // quick checks
  emp_assert(g.size() == target.size(), g.size());

  // intialize vector with size g
  score_t score(g.size());

  for(size_t i = 0; i < g.size(); ++i)
  {
    score[i] = g[i];
  }

  return score;
}


// ecological niche function
Diagnostic::score_t Diagnostic::EcologyNiche(const genome_t & g)
{
  score_t score;


  return score;
}

Diagnostic::score_t Diagnostic::StructExploitation(const genome_t & g, double max_error)
{
  // intialize vector with size g
  score_t score(g.size());

  // using stl algorithm library
  auto it = std::is_sorted_until(g.begin(), g.end(), std::greater_equal<>());

  // if sorted
  if(it == g.end()) {std::copy(g.begin(), g.end(), score.begin());}

  else
  {
    size_t cutoff = (it - g.begin());

    // everything up to unsorted
    for(size_t i = 0; i < cutoff; ++i) {score[i] = g[i];}

    // everything after unsorted
    for(size_t i = cutoff; i < score.size(); ++i) {score[i] = max_error;}
  }

  return score;
}


#endif