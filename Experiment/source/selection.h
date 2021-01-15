/// These are the selection schemes we are using for this project
/// Will be broken up according to my thoughts on how $$ components work

#ifndef SEL_H
#define SEL_H

///< standard headers
#include <algorithm>

///< empirical headers
#include "base/vector.h"
#include "tools/Random.h"

///< constant vars
constexpr size_t DRIFT_SIZE = 1;

class Selection
{
  public:
    // vector type of parent ids selected
    using parent_t = emp::vector<size_t>;
    // vector type of org score
    using score_t = emp::vector<double>;
    // matrix type of org with multiple scorea
    using matrix_t = emp::vector<emp::vector<double>>;
    // vector holding population genomes
    using genome_t = emp::vector<emp::vector<double>>;
    // vector holding population groupings
    using group_t = emp::vector<emp::vector<size_t>>;

  public:

    Selection(emp::Ptr<emp::Random> rng = nullptr) : random(rng) {emp_assert(rng);}

    ///< population structure

    /**
     * K-Nearest Neighbor Structure:
     *
     * This function will return a vector of vectors, where
     * each individual vector corresponds to that solutions nearest neighbor
     *
     * In the event of ties, a solution will be selected at random.
     *
     * @param score Vector containing all solution scores.
     *
     * @return Vector with parent id's that are selected.
     */
    group_t KNearestN(const score_t & score);


    ///< fitness transformation

    /**
     * Fitness Sharing Transformation:
     *
     * Fitness transformation function inspired from:
     * B. Sareni and L. Krahenbuhl, “Fitness sharing and niching methods revisited,”
     * in IEEE Transactions on Evolutionary Computation, vol. 2, no. 3, pp. 97-106, Sept. 1998.
     *
     *
     * @param genome Vector containing all genomes.
     * @param score Vector containing all solution scores.
     * @param alph Shape of sharing function.
     * @param sig Similarity threshold.
     *
     * @return Vector with parent id's that are selected.
     */
    score_t FitnessSharing(const genome_t & genome, const score_t & score, const double alph, const double sig);

    /**
     * Novelty Fitness Transformation:
     *
     * This function will transform original fitness values as novelty scores.
     * Each score has its associated neighbors (2nd argument).
     *
     *
     * @param score Vector containing all solution scores.
     * @param neigh Vector containing neighboring positions in score (per solution)
     *
     * @return Vector with transformed scores.
     */
    score_t Novelty(const score_t & score, const group_t & neigh);


    ///< selector functions

    /**
     * (μ,λ) Elite Selector:
     *
     * This function finds the top 'm' performing solutions.
     * Once the top 'm' solutions are found, each will be placed (l/m) times in the parent list.
     *
     * @param top Number of top performing solutions to pick.
     * @param pops Population size.
     * @param score Vector holding the population score.
     *
     * @return Vector with parent id's that are selected.
     */
    parent_t MLElite(const size_t mu, const size_t lambda, const score_t & score);

    /**
     * Tournament Selector:
     *
     * This function holds tournaments for 't' solutions randomly picked from the population.
     * In the event of ties, a solution will be selected randomly from all solutions that tie.
     *
     * @param t Tournament size.
     * @param score Vector holding the population score.
     *
     * @return Parent id of solution that won the tournament.
     */

    size_t Tournament(const size_t t, const score_t & score);

    /**
     * Drift Selector:
     *
     * This function will return a random solution as a parent.
     *
     *
     * @param score Vector containing all solution scores.
     *
     * @return Parent id that is selected.
     */
    size_t Drift(const size_t size);


    ///< helper functions


  private:

    // random pointer from world.h
    emp::Ptr<emp::Random> random;

};

///< population structure

Selection::group_t Selection::KNearestN(const score_t & score)
{
  // quick checks

  group_t group;



  return group;
}

///< fitness transformation

Selection::score_t Selection::FitnessSharing(const genome_t & genome, const score_t & score, const double alph, const double sig)
{
  // quick checks


  score_t tscore;


  return tscore;
}
Selection::score_t Selection::Novelty(const score_t & score, const group_t & neigh)
{
  // quick checks


  score_t tscore;


  return tscore;
}


///< selector functions

Selection::parent_t Selection::MLElite(const size_t mu, const size_t lambda, const score_t & score)
{
  // quick checks

  parent_t win;




  return win;
}
size_t Selection::Tournament(const size_t t, const score_t & score)
{
  // quick checks
  emp_assert(t > 0); emp_assert(0 < score.size());

  // get tournament ids
  auto tour = emp::Choose(*random, score.size(), t);

  auto win = std::max_element(tour.begin(), tour.end(),
              [score](double const lhs, double const rhs)
              {
                return score[lhs] < score[rhs];
              });

  return std::distance(tour.begin(), win);
}
size_t Selection::Drift(const size_t size)
{
  // quick checks
  emp_assert(size > 0);

  // return a random org id
  auto win = emp::Choose(*random, size, DRIFT_SIZE);
  emp_assert(win.size() == DRIFT_SIZE);

  return win[0];
}

#endif