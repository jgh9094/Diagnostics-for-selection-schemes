/// Set of diagnostic problems being used in research project

// TODO
// Exploration []
// Exploitation [X]
// ContraEcology [X]
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
    using opti_t = emp::vector<bool>;

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

    ///< Functions that deal with diagnostic problem scoring

    /**
     * Exploitation function:
     *
     * Solutions are pressured to optimize genomes in descending order.
     * With the caveat that solutions are resposible for optimizizing from start to end.
     * To calculate score vector, first we find the maximum gene value i*.
     * Once i* is found, every position after (include i* position) for score vector is set to genome value associated with that position.
     * This only continues until descending order is broken.
     * Once order is broken, remaining values are set to max error (include other score vector values prior to i* if applicable).
     * Note solutions are responsible for optimizing from the start of genome.
     *
     * @param g Genome from organism being evaluated.
     * @param max_error This value is the maximum error assigned when structure broken
     *
     * @return score vector that is calculated from 'g'.
     */
    score_t Exploration(const genome_t & g, double max_error);

    /**
     * Exploitation function:
     *
     * All genes in genome evaluated treated as independent optimization tasks.
     * In other words, there is no interactions between genes.
     * The score vector is just the genome, with the caveat that genes cannot go over associated target value.
     *
     * @param g Genome from organism being evaluated.
     *
     * @return score vector that is calculated from 'g'.
     */
    score_t Exploitation(const genome_t & g);

    /**
     * Contradicting Ecology function:
     *
     * All genes are associated with a single type of resource.
     * Solutions are pressured to optimize one maximaly, while keeping others close to 0.
     * To calculate score vector, first we find the maximum gene value i*.
     * Once i* is found, score vector value at position i is i* minus gene value at position i.
     * Score vector value where i* is found is just i*.
     *
     * @param g Genome from organism being evaluated.
     * @param max_error This value is the maximum error assigned when structure broken
     *
     * @return score vector that is calculated from 'g'.
     */
    score_t ContraEcology(const genome_t & g);

    /**
     * Structured Exploitation function:
     *
     * All genes in genome expected to follow descending order for evaluation.
     * While in decending order, score vector value at positon 'i' is gene value at position 'i'.
     * If descending order is broken, max error assigned at position descending order is broken in score vector, and after.
     * Assuming that genes in genome cannot go over the target vector per position in genome.
     * Note that this problem explicitly forces optimization from start to end.
     *
     * @param g Genome from organism being evaluated.
     * @param max_error This value is the maximum error assigned when structure broken
     *
     * @return score vector that is calculated from 'g'.
     */
    score_t StructExploitation(const genome_t & g, double max_error);


    ///< Functions that deal with interpretation of score vectors

    /**
     * Optimized Vector:
     *
     * Will return a boolean vector that lists what genes are optimized relative to the target vector.
     * A gene is optimized if it meets the target value accuracy % requirement.
     *
     * @param g Genome from organism being evaluated.
     * @param acc This value is the accuracy % needed to be considered optimized
     *
     * @return boolean vector that displays optimized traits.
    */
    opti_t OptimizedVector(const genome_t & g, const double & acc);

  private:
    // variable that holds target vector
    target_t target;
};

///< diagnostic problem implementations

Diagnostic::score_t Diagnostic::Exploration(const genome_t & g, double max_error)
{
  // intialize vector with size g
  score_t score(g.size());

  // find max value position
  auto opti_it = std::max_element(g.begin(), g.end());
  size_t opti = (opti_it - g.begin());

  // find where order breaks
  size_t sort = (std::is_sorted_until(opti_it, g.end(), std::greater_equal<>()) - g.begin());

  // left of optimal value found
  for(size_t i = 0; i < opti; ++i) {score[i] = max_error;}
  // middle of optimal value till order broken
  for(size_t i = opti; i < sort; ++i) {score[i] = g[i];}
  // right of order broken
  for(size_t i = sort; i < score.size(); ++i) {score[i] = max_error;}

  return score;
}
Diagnostic::score_t Diagnostic::Exploitation(const genome_t & g)
{
  // intialize vector with size g
  score_t score(g.size());

  // copy genome into score vector
  std::copy(g.begin(), g.end(), score.begin());

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

///< score vector interpretation implementations

Diagnostic::opti_t Diagnostic::OptimizedVector(const genome_t & g, const double & acc)
{
  // quick checks
  emp_assert(g.size() > 0, g.size());
  emp_assert(g.size() == target.size(), target.size());
  emp_assert(0.0 < acc, acc);
  emp_assert(acc <= 1.0, acc);

  // initialize optimized vector with all false
  opti_t optimize(g.size(), false);

  // iterate through genome and check optimality
  for(size_t i = 0; i < g.size(); ++i)
  {
    if((acc * target[i]) <= g[i]) {optimize[i] = true;}
  }

  return optimize;
}

#endif