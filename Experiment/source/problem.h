/// Set of diagnostic problems being used in research project

// TODO
// Exploration []
// Exploitation []
// ContraEcology []
// StructExploitation [X]


#ifndef DIA_PRO_H
#define DIA_PRO_H

///< standard headers
#include <algorithm>

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

    /**
     * Exploitation function.
     * All genes in genome expected to follow descending order for evaluation.
     * While in decending order, fitness vector score at positon 'i' is gene value at position 'i'.
     * If descending order is broken, max error assigned at position descending order is broken, and after.
     * Assuming that genes in genome cannot go over the target vector per position in genome.
     *
     * @param g Genome from organism being evaluated.
     * @param max_error This value is the maximum error assigned when structure broken
     * @return score vector that is calculated from 'g'.
     */
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

    /**
     * Contradicting Ecology function
     *
     * All genes are associated with a single type of resource.
     * Solutions are pressured to optimize one maximaly, while keeping others close to 0.
     * To find calculate fitness vector, first we find the maximum gene value i*.
     * Once i* is found, fitness vector score at position i is i* minus gene value at position i.
     * Fitness vector score where i* is found is just i*.
     *
     * @param g Genome from organism being evaluated.
     * @param max_error This value is the maximum error assigned when structure broken
     * @return score vector that is calculated from 'g'.
     */
    score_t ContraEcology(const genome_t & g);

    /**
     * Structured Exploitation function.
     *
     * All genes in genome expected to follow descending order for evaluation.
     * While in decending order, fitness vector score at positon 'i' is gene value at position 'i'.
     * If descending order is broken, max error assigned at position descending order is broken in fitness vector, and after.
     * Assuming that genes in genome cannot go over the target vector per position in genome.
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


Diagnostic::score_t Diagnostic::ContraEcology(const genome_t & g)
{
  // intialize score vector
  score_t score(g.size());

  // find max value position
  size_t skip = (std::max_element(g.begin(), g.end()) - g.begin());

  // set all score vector values except where max value is
  for(size_t i = 0; i < score.size(); ++i)
  {
    if(i == skip) {continue;}
    score[i] = g[skip] - g[i];
  }

  score[skip] = g[skip];

  return score;
}

Diagnostic::score_t Diagnostic::StructExploitation(const genome_t & g, double max_error)
{
  // intialize vector with size g
  score_t score(g.size());

  // find where order breaks
  auto it = std::is_sorted_until(g.begin(), g.end(), std::greater_equal<>());

  // if sorted, return same vector
  if(it == g.end()) {std::copy(g.begin(), g.end(), score.begin());}

  // else fill in appropiately
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