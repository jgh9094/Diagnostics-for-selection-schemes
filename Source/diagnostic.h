/// Set of diagnostic problems being used in research project
#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

///< standard headers
#include <algorithm>

///< empirical headers
#include "emp/base/vector.hpp"

class Diagnostic
{
  public:
    //typename for target vector
    using phenotype_t = emp::vector<double>;
    using genotype_t = emp::vector<double>;
    using opti_t = emp::vector<bool>;

  public:

    Diagnostic(double t_, double c) : target(t_), max_cred(c) {cred_set = true;}

    Diagnostic() {;}

    // getters
    double GetTarget() const {return target;}
    double GetCredit() const {return max_cred;}

    //setters
    void SetTarget(double t_) {target = t_;}
    void SetCredit(double c_) {cred_set = true; max_cred = c_;}


    ///< Functions that deal with diagnostic problem scoring

    /**
     * Multi-valley Crossing function:
     *
     * Solutions are pressured to cross valleys of different widths at each trait.
     * We use the penalty vector supplied to calculate the penalty at each valley.
     * We use the difference between the gene value and penalty value to determine
     * the reduction when calculating the trait value.
     *
     * phenotype[i] = genotype[i] - penalty
     *
     * @param g Genotype from organism being evaluated.
     * @param max_credit This value is the maximum credit assigned when structure broken
     *
     * @return phenotype vector that is calculated from 'g'.
     */
    phenotype_t MultiValleyCrossing(const genotype_t & g, const phenotype_t & peaks, double & dips_start, double & dips_end);

    /**
     * Multi-path Exploration function:
     *
     * Solutions are pressured to optimize genomes in descending order.
     * With the caveat that solutions are resposible for optimizizing from start to end.
     * To calculate phenotype vector, first we find the maximum gene value i*.
     * Once i* is found, every position after (include i* position) for phenotype vector is set to genome value associated with that position.
     * This only continues until descending order is broken.
     * Once order is broken, remaining values are set to max error (include other phenotype vector values prior to i* if applicable).
     * Note solutions are responsible for optimizing from the start of genome.
     *
     * @param g Genotype from organism being evaluated.
     * @return phenotype vector that is calculated from 'g'.
     */
    phenotype_t MultiPathExploration(const genotype_t & g);

    /**
     * Exploitation function:
     *
     * All genes in genome evaluated treated as independent optimization tasks.
     * In other words, there is no interactions between genes.
     * The phenotype vector is just the genome, with the caveat that genes cannot go over associated target value.
     *
     * @param g Genotype from organism being evaluated.
     *
     * @return phenotype vector that is calculated from 'g'.
     */
    phenotype_t ExploitationRate(const genotype_t & g);

    /**
     * Weak Phenotypic Diversity Ecology function:
     *
     * All genes are associated with a single type of resource.
     * Solutions are pressured to optimize one maximaly, while other genes drift if they are not the maximum one.
     * To calculate phenotype vector, first we find the maximum gene value i*.
     * Once i* is found, phenotype vector value at every other position every other is 0.
     * Score vector value where i* is found is i*.
     *
     * @param g Genotype from organism being evaluated.
     *
     * @return phenotype vector that is calculated from 'g'.
     */
    phenotype_t ContradictoryObjectives(const genotype_t & g);

    /**
     * Ordered Exploitation function:
     *
     * All genes in genome expected to follow descending order for evaluation.
     * While in decending order, phenotype vector value at positon 'i' is gene value at position 'i'.
     * If descending order is broken, max error assigned at position descending order is broken in phenotype vector, and after.
     * Assuming that genes in genome cannot go over the target vector per position in genome.
     * Note that this problem explicitly forces optimization from start to end.
     *
     * @param g Genotype from organism being evaluated.
     *
     * @return phenotype vector that is calculated from 'g'.
     */
    phenotype_t OrderedExploitation(const genotype_t & g);


    ///< Functions that deal with interpretation of phenotype vectors

    /**
     * Optimized Vector:
     *
     * Will return a boolean vector that lists what genes are optimized relative to the target vector.
     * A gene is optimized if it meets the target value accuracy % requirement.
     *
     * @param g Genotype from organism being evaluated.
     * @param acc This value is the accuracy % needed to be considered optimized
     *
     * @return boolean vector that displays optimized traits.
    */
    opti_t OptimizedVector(const genotype_t & g, const double acc);

  private:
    // holds vector of target objective values
    double target = 0.0;
    // holds maximum credit allowed for error
    double max_cred = 0.0;
    // max credit set?
    bool cred_set = false;
};

///< diagnostic problem implementations

Diagnostic::phenotype_t Diagnostic::ExploitationRate(const genotype_t & g)
{
  // quick checks
  emp_assert(0 < g.size());

  // intialize vector with size g
  phenotype_t phenotype = g;

  return phenotype;
}

Diagnostic::phenotype_t Diagnostic::OrderedExploitation(const genotype_t & g)
{
  // quick checks
  emp_assert(g.size() > 0); emp_assert(cred_set);

  // find where descending order breaks
  const auto it = std::is_sorted_until(g.begin(), g.end(), std::greater<>());

  // if sorted, return same vector
  if(it == g.end())
  {
    phenotype_t phenotype = g;
    return phenotype;
  }
  // else fill in appropiately
  else
  {
    // initialize phenotype to the same size
    phenotype_t phenotype(g.size());

    // calculate cutoff point where descending order is broken
    const size_t cutoff = std::distance(g.begin(), it);

    // everything up to unsorted
    for(size_t i = 0; i < cutoff; ++i) {phenotype[i] = g[i];}

    // everything after unsorted
    for(size_t i = cutoff; i < phenotype.size(); ++i) {phenotype[i] = max_cred;}

    return phenotype;
  }
}

Diagnostic::phenotype_t Diagnostic::ContradictoryObjectives(const genotype_t & g)
{
  // quick checks
  emp_assert(g.size() > 0);

  // intialize phenotype vector
  phenotype_t phenotype(g.size());

  // find max value position
  const size_t max_gene = std::distance(g.begin(), std::max_element(g.begin(), g.end()));

  // set all phenotype vector values
  for(size_t i = 0; i < phenotype.size(); ++i)
  {
    if(i == max_gene) {phenotype[i] = g[max_gene];}
    else {phenotype[i] = max_cred;}
  }

  return phenotype;
}

Diagnostic::phenotype_t Diagnostic::MultiPathExploration(const genotype_t & g)
{
  // quick checks
  emp_assert(0 < g.size()); emp_assert(cred_set);

  // intialize vector with size g
  phenotype_t phenotype(g.size());

  // find max value position
  const auto opti_it = std::max_element(g.begin(), g.end());
  const size_t opti = std::distance(g.begin(), opti_it);

  // find where order breaks
  const size_t sort = std::distance(g.begin(), std::is_sorted_until(opti_it, g.end(), std::greater<>()));

  // left of optimal value found
  for(size_t i = 0; i < opti; ++i) {phenotype[i] = max_cred;}
  // middle of optimal value till order broken
  for(size_t i = opti; i < sort; ++i) {phenotype[i] = g[i];}
  // right of order broken
  for(size_t i = sort; i < phenotype.size(); ++i) {phenotype[i] = max_cred;}

  return phenotype;
}

Diagnostic::phenotype_t Diagnostic::MultiValleyCrossing(const genotype_t & g, const phenotype_t & peaks, double & dips_start, double & dips_end)
{
  // quick checks
  emp_assert(g.size() > 0); emp_assert(cred_set);
  emp_assert(peaks.size() > 0);

  // intialize vector with size g
  phenotype_t phenotype(g.size());

  for(size_t i = 0; i < g.size(); ++i)
  {
    if (g[i] <= dips_start || g[i] >= dips_end)
    {
      phenotype[i] = g[i];
    }
    else
    {
      phenotype[i] = 2.0 * peaks[static_cast<size_t>(g[i])] - g[i];
    }
  }

  return phenotype;
}

///< phenotype vector interpretation implementations

Diagnostic::opti_t Diagnostic::OptimizedVector(const genotype_t & g, const double acc)
{
  // quick checks
  emp_assert(g.size() > 0);
  emp_assert(0.0 < target);
  emp_assert(0.0 < acc);
  emp_assert(acc <= 1.0);

  // initialize optimized vector with all false
  opti_t optimize(g.size(), false);
  const double threshold = acc * target;

  // iterate through genome and check optimality
  for(size_t i = 0; i < g.size(); ++i)
  {
    if(threshold <= g[i]) {optimize[i] = true;}
  }

  return optimize;
}

#endif