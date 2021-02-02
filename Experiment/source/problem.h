/// Set of diagnostic problems being used in research project
#ifndef PROBLEM_H
#define PROBLEM_H

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

    Diagnostic(target_t & t_, double c) : max_cred(c)
    {
      cred_set = true;
      target.resize(t_.size());
      std::copy(t_.begin(), t_.end(), target.begin());
    }

    Diagnostic() {;}

    // getters
    target_t GetTarget() const {return target;}
    double GetCredit() const {return max_cred;}

    //setters
    void SetTarget(target_t & t) {target.clear(); target.resize(t.size()); std::copy(t.begin(), t.end(), target.begin());}
    void SetCredit(double c) {cred_set = true; max_cred = c;}


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
     * @param max_credit This value is the maximum credit assigned when structure broken
     *
     * @return score vector that is calculated from 'g'.
     */
    score_t Exploration(const genome_t & g);

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
     * @param max_credit This value is the maximum credit assigned when structure broken
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
     * @param max_credit This value is the maximum credit assigned when structure broken
     *
     * @return score vector that is calculated from 'g'.
     */
    score_t StructExploitation(const genome_t & g);


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
    opti_t OptimizedVector(const genome_t & g, const double acc);

  private:
    // holds vector of target objective values
    target_t target;

    // holds maximum credit allowed for error
    double max_cred;
    // max credit set?
    bool cred_set = false;
};

///< diagnostic problem implementations

Diagnostic::score_t Diagnostic::Exploration(const genome_t & g)
{
  // quick checks
  emp_assert(0 < g.size()); emp_assert(cred_set);

  // intialize vector with size g
  score_t score(g.size());

  // find max value position
  auto opti_it = std::max_element(g.begin(), g.end());
  size_t opti = std::distance(g.begin(), opti_it);

  // find where order breaks
  size_t sort = std::distance(g.begin(), std::is_sorted_until(opti_it, g.end(), std::greater<>()));

  // left of optimal value found
  for(size_t i = 0; i < opti; ++i) {score[i] = max_cred;}
  // middle of optimal value till order broken
  for(size_t i = opti; i < sort; ++i) {score[i] = g[i];}
  // right of order broken
  for(size_t i = sort; i < score.size(); ++i) {score[i] = max_cred;}

  return score;
}
Diagnostic::score_t Diagnostic::Exploitation(const genome_t & g)
{
  // quick checks
  emp_assert(0 < g.size());

  // intialize vector with size g
  score_t score(g.size());

  // copy genome into score vector
  std::copy(g.begin(), g.end(), score.begin());

  return score;
}
Diagnostic::score_t Diagnostic::ContraEcology(const genome_t & g)
{
  // quick checks
  emp_assert(g.size() > 0);

  // intialize score vector
  score_t score(g.size());

  // find max value position
  size_t max_v = std::distance(g.begin(), std::max_element(g.begin(), g.end()));

  // set all score vector values except where max value is
  for(size_t i = 0; i < score.size(); ++i)
  {
    if(i == max_v) {continue;}
    score[i] = g[max_v] - g[i];
  }

  score[max_v] = g[max_v];

  return score;
}
Diagnostic::score_t Diagnostic::StructExploitation(const genome_t & g)
{
  // quick checks
  emp_assert(g.size() > 0); emp_assert(cred_set);

  // intialize vector with size g
  score_t score(g.size());

  // find where descending order breaks
  auto it = std::is_sorted_until(g.begin(), g.end(), std::greater<>());

  // if sorted, return same vector
  if(it == g.end()) {std::copy(g.begin(), g.end(), score.begin());}

  // else fill in appropiately
  else
  {
    // calculate cutoff point where descending order is broken
    size_t cutoff = std::distance(g.begin(), it);

    // everything up to unsorted
    for(size_t i = 0; i < cutoff; ++i) {score[i] = g[i];}

    // everything after unsorted
    for(size_t i = cutoff; i < score.size(); ++i) {score[i] = max_cred;}
  }

  return score;
}

///< score vector interpretation implementations

Diagnostic::opti_t Diagnostic::OptimizedVector(const genome_t & g, const double acc)
{
  // quick checks
  emp_assert(g.size() > 0);
  emp_assert(g.size() == target.size());
  emp_assert(0.0 < acc);
  emp_assert(acc <= 1.0);

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