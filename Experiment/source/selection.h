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
double ERROR_VALD = -1.0;

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
    using neigh_t = emp::vector<emp::vector<double>>;

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
    neigh_t FitNearestN(const score_t & score, const size_t K);

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
     * @param dmat distance matrix for genome pairs, a lower diagnoal matrix is expected.
     * @param score Vector containing all solution scores.
     * @param alph Shape of sharing function.
     * @param sig Similarity threshold.
     *
     * @return Vector with parent id's that are selected.
     */
    score_t FitnessSharing(const matrix_t & dmat, const score_t & score, const double alph, const double sig);

    /**
     * Fitness Sharing: Sharing Function
     *
     * This function will return a score of 0 or 1 - (score/sig)^alph.
     * Depending on score < sig.
     *
     * \param dist single score we are evaluating
     * \param sig similarity threshold we are using
     * \param alph shape of penalty function
     *
     * \return a tranformed fitness score, with fitness sharing integrated within the score
     */
    double SharingFunction(const double dist, const double sig, const double alph);

    /**
     * Novelty Fitness Transformation:
     *
     * This function will transform original fitness values as novelty scores.
     * Each score has its associated neighbors (2nd argument).
     *
     *
     * @param score Vector containing all solution scores.
     * @param neigh Vector containing neighboring positions in score (per solution).
     * @param K Size of each neighborhood.
     * @param exp Exponent we are using for the p norm calculations.
     *
     * @return Vector with transformed scores.
     */
    score_t Novelty(const score_t & score, const neigh_t & neigh, const size_t K);


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

    // p-norm function between two vector subtractions and the exponent for p
    double Pnorm(const score_t & x, const score_t & y, const double exp);

    // similarity matrix generator
    matrix_t SimilarityMatrix(const genome_t & genome, const double exp);


  private:

    // random pointer from world.h
    emp::Ptr<emp::Random> random;
};

///< population structure

Selection::neigh_t Selection::FitNearestN(const score_t & score, const size_t K)
{
  // quick checks
  emp_assert(0 < score.size()); emp_assert(score.size()-1 < K);

  // create group vector returning
  neigh_t group(score.size());

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

      // reached all left neighbors possible for i
      if(left < 0)
      {
        neigh.push_back(order[right].second);
        ++right;
      }
      // reached all right neighbors possible for i
      else if (order.size() <= right)
      {
        neigh.push_back(order[left].second);
        --left;
      }
      // we find the closes neighbor to score i, between left and right
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
    // quick checks
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


///< fitness transformation >///

Selection::score_t Selection::FitnessSharing(const matrix_t & dmat, const score_t & score, const double alph, const double sig)
{
  // quick checks
  emp_assert(dmat.size() == score.size()); emp_assert(0 <= alph); emp_assert(0 <= sig);
  emp_assert(0 < dmat.size()); emp_assert(0 < score.size());

  score_t tscore(score.size());

  for(size_t i = 0; i < dmat.size(); ++i)
  {
    // quick checks
    emp_assert(dmat[i].size() == score.size());

    // mi value that holds niche count (or scaling factor)
    double mi = 0.0;

    for(size_t j = 0; j < dmat.size(); ++j)
    {
      // ignore similar pairs
      if(i == j) {continue;}

      // only look at lower triangular diagonal values
      if(j < i)
      {
        emp_assert(dmat[i][j] != ERROR_VALD);
        mi += SharingFunction(dmat[i][j], sig, alph);
      }
      else
      {
        emp_assert(dmat[j][i] != ERROR_VALD);
        mi += SharingFunction(dmat[j][i], sig, alph);
      }
    }

    tscore[i] = score[i] / mi;
  }

  return tscore;
}
double Selection::SharingFunction(const double dist, const double sig, const double alph)
{
  // quick checks
  emp_assert(0 <= dist); emp_assert(0 <= sig); emp_assert(0 <= alph);

  if(dist < sig)
  {
    // right side of equation
    const double rse = std::pow((dist/sig), alph);
    return 1.0 - rse;
  }

  return 0.0;
}

Selection::score_t Selection::Novelty(const score_t & score, const neigh_t & neigh, const size_t K)
{
  // quick checks
  emp_assert(score.size() == neigh.size());
  emp_assert(0 < score.size()); emp_assert(0 < neigh.size());

  // novelty score transformed from orignial scores
  score_t nscore(score.size());

  // iterate through both score and neighborhood vectors
  for(size_t i = 0; i < score.size(); ++i)
  {
    // quick checks
    emp_assert(neigh[i].size() == K);

    // keep track of distances
    double numer = 0.0;

    // euclidean distance for two points
    for(size_t k = 0; k < K; ++k){numer += Distance(score[i], neigh[i][k]);}

    // calculate the average
    const double denom = static_cast<double>(K);

    nscore[i] = numer / denom;
  }

  return nscore;
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


///< helper functions

double Selection::Pnorm(const score_t & x, const score_t & y, const double exp)
{
  // quick checks
  emp_assert(0 < x.size()); emp_assert(0 < y.size());
  emp_assert(x.size() == y.size()); emp_assert(1 <= exp);

  // subtract one vector from the other and take the exp of that
  score_t diff(x.size());
  for(size_t i = 0; i < x.size(); ++i){diff[i] = std::pow((x[i] - y[i]), exp);}

  double tot = std::accumulate(diff.begin(), diff.end(), 0.0);

  return std::pow(tot, (1.0/exp));
}

Selection::matrix_t Selection::SimilarityMatrix(const genome_t & genome, const double exp)
{
  // quick checks
  emp_assert(1 <= exp); emp_assert(1 < genome.size());

  // generate the matrix, lower diagnonal matrix filled (not include i == j)
  matrix_t similar(genome.size());
  for(auto & s : similar) {s.resize(genome.size(), ERROR_VALD);}

  for(size_t i = 0; i < genome.size(); ++i)
  {
    for(size_t j = 0; j < i; ++j)
    {
      similar[i][j] = Pnorm(genome[i], genome[j], exp);
    }
  }

  return similar;
}

#endif