/// These are the selection schemes we are using for this project
/// Will be broken up according to my thoughts on how $$ components work

#ifndef SEL_H
#define SEL_H

///< standard headers
#include <algorithm>
#include <map>
#include <utility>
#include <cmath>

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
    // matrix type of org with multiple scores
    using matrix_t = emp::vector<emp::vector<double>>;
    // vector holding population genomes
    using genome_t = emp::vector<emp::vector<double>>;
    // vector holding population groupings
    using group_t = emp::vector<emp::vector<size_t>>;
    // map holding population id groupings by fitness (keys in decending order)
    using fitgp_t = std::map<double, parent_t, std::greater<int>>;
    // sorted score vector w/ position id and score
    using sorted_t = emp::vector<std::pair<size_t,double>>;
    // vector of double vectors for K neighborhoods
    using kneigh_t = emp::vector<emp::vector<double>>;

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
     * @return Vector of vector ids groups as nearest neighbors.
     */
    kneigh_t FitNearestN(const score_t & score, const size_t K);

    /**
     * Fitness Group Structure:
     *
     * This function will return a map of double,vector pairing where
     * each vector corresponds to solution ids share the same fitness
     *
     * @param score Vector containing all solution scores.
     *
     * @return Map grouping population ids by fitness (keys (fitness) in decending order)
     */
    fitgp_t FitnessGroup(const score_t & score);


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
     * @param group Map with fitness,vector id pairs (descending order expected). Doesn't have to be fitness but some double value in best to worst order.
     *
     * @return Vector with parent id's that are selected.
     */
    parent_t MLSelect(const size_t mu, const size_t lambda, const fitgp_t & group);

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

    // distance function between two values
    double Distance(double a, double b) {return std::abs(a - b);}


  private:

    // random pointer from world.h
    emp::Ptr<emp::Random> random;
};

///< population structure

Selection::kneigh_t Selection::FitNearestN(const score_t & score, const size_t K)
{
  // quick checks
  emp_assert(0 < score.size()); emp_assert(score.size()-1 < K);

  // create group vector returning
  kneigh_t group(score.size());

  // create vector to sort scores and generate score neighborhood pairings
  // <position id in orginal score vector, original score vector value>
  sorted_t order;

  // populate the vector with pairings + sort it based on scores
  for(size_t i = 0; i < score.size(); ++i){order.emplace_back(i, score[i]);}
  std::sort(order.begin(), order.end(), [](const auto &left, const auto &right) {
    return left.second < right.second;
  });

  // walk through sorted vector and create the k neighborhood per score value
  for(size_t i = 0; i < order.size(); ++i)
  {
    emp::vector<double> neigh;
    int left = i - 1; int right = i + 1;

    // generate the neighborhood
    while(neigh.size() != K)
    {
      //quick checks
      emp_assert((left < 0) && (order.size() <= right));

      // i = 0
      if(left < 0)
      {
        neigh.push_back(order[right].second);
        ++right;
      }
      // i = score.size() - 1
      else if (order.size() <= right)
      {
        neigh.push_back(order[left].second);
        --left;
      }
      // in the middle
      else
      {
        // look at distances
        double ld = Distance(order[i].second, order[left].second);
        double rd = Distance(order[i].second, order[right].second);

        // left is smaller distance
        if(ld < rd) {neigh.push_back(order[left].second); --left;}
        else {neigh.push_back(order[right].second); ++right;}
      }
    }

    emp_assert(neigh.size() == K);
    // we now have the add vector into right location
    group[order[i].first] = neigh;
  }

  return group;
}
Selection::fitgp_t Selection::FitnessGroup(const score_t & score)
{
  // quick checks
  emp_assert(0 < score.size());

  // place all solutions in map based on score
  fitgp_t group;
  for(size_t i = 0; i < score.size(); ++i)
  {
    // didn't find in group
    if(group.find(score[i]) == group.end())
    {
      parent_t p{i};
      group[score[i]] = p;
    }
    else{group[score[i]].push_back(i);}
  }

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

Selection::parent_t Selection::MLSelect(const size_t mu, const size_t lambda, const fitgp_t & group)
{
  // quick checks
  emp_assert(0 < mu); emp_assert(0 < lambda);
  emp_assert(mu <= lambda); emp_assert(0 < group.size());

  // go through the ordered scores and get our top mu solutions
  parent_t topmu;
  for(auto & g : group)
  {
    auto gt = g.second;
    emp::Shuffle(*random, gt);
    for(auto id : gt)
    {
      topmu.push_back(id);
      if(topmu.size() == mu) {break;}
    }

    if(topmu.size() == mu) {break;}
  }

  // insert the correct amount of ids
  parent_t parent;
  size_t ml = mu / lambda;
  for(auto id : topmu)
  {
    for(size_t i = 0; i < ml; ++i){parent.push_back(id);}
  }

  emp_assert(parent.size() == lambda);

  return parent;
}
size_t Selection::Tournament(const size_t t, const score_t & score)
{
  // quick checks
  emp_assert(0 < t); emp_assert(0 < score.size());
  emp_assert(t <= score.size());

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