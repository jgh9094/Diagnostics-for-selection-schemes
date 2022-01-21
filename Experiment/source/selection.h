/// These are the selection schemes we are using for this project
/// Will be broken up according to my thoughts on how $$ components work
/// These selection functions are selecting on the assumption that the problem is a maximization problem

#ifndef SEL_H
#define SEL_H

///< standard headers
#include <algorithm>
#include <map>
#include <utility>
#include <cmath>
#include <numeric>
#include <set>

///< empirical headers
#include "base/vector.h"
#include "tools/Random.h"
#include "tools/random_utils.h"

///< constant vars
constexpr size_t DRIFT_SIZE = 1;
constexpr double ERROR_VALD = -1.0;

class Selection
{
  // object types we are using in this class
  public:
    // vector of any ids
    using ids_t = emp::vector<size_t>;
    // vector type of org score
    using score_t = emp::vector<double>;
    // matrix type of org with multiple scores
    using fmatrix_t = emp::vector<score_t>;
    // vector holding population genomes
    using gmatrix_t = emp::vector<emp::vector<double>>;
    // map holding population id groupings by fitness (keys in decending order)
    using fitgp_t = std::map<double, ids_t, std::greater<double>>;
    // sorted score vector w/ position id and score
    using sorted_t = emp::vector<std::pair<size_t,double>>;
    // vector of double vectors for K neighborhoods
    using neigh_t = emp::vector<score_t>;
    // vector of vector size_t for Pareto grouping
    using pareto_g_t = emp::vector<emp::vector<size_t>>;
    // pairing of position_id and fitness associated with id
    using id_f_t = std::pair<size_t,double>;


  public:

    Selection(emp::Ptr<emp::Random> rng = nullptr) : random(rng) {emp_assert(rng);}

    ///< sanity check functions

    // make sure all ids are unique
    template <class T>
    bool ParetoUnique(const emp::vector<emp::vector<T>> & v, const size_t & size)
    {
      std::set<T> check;
      for(const emp::vector<T> & u : v)
      {
        for(T x : u)
        {
          check.insert(x);
        }
      }

      return size == check.size();
    }

    bool ParetoNonZero(const score_t & score)
    {
      for(const auto & s : score)
      {
        if(s==0.0)
        {
          return false;
        }
      }
      return true;
    }

    ///< helper functions

    // distance function between two values
    double Distance(double a, double b) {return std::abs(a - b);}

    // p-norm function between two vector subtractions and the exponent for p
    double Pnorm(const score_t & x, const score_t & y, const double exp);

    // similarity matrix generator
    fmatrix_t SimilarityMatrix(const gmatrix_t & genome, const double exp);

    // print vectors
    template <class T>
    void PrintVec(const emp::vector<T> &v, const std::string s)
    {
      std::cout << s << ": ";
      for(auto x : v) {std::cout << (double) x << ",";}
      std::cout << std::endl;
    }

    // does A dominate B?
    // Is A > B?
    // Is A partially greater than B?
    bool PartiallyGreater(const score_t & A, const score_t & B);

    // construct a similarity matrix with current phenotypes and archive
    fmatrix_t NoveltySimilarityMatrix(const fmatrix_t & phenos, const std::deque<score_t> & archive, const double exp);


    ///< population structure

    /**
     * K-Nearest Neighbor Structure:
     *
     * This function will return a vector of vectors, where
     * each individual vector corresponds to that solutions nearest neighbors
     *
     * In the event of ties, we take right neighbor score.
     *
     * @param score Vector containing all solution scores.
     * @param K Size of neighborhood.
     *
     * @return Vector of vector scores.
     */
    neigh_t FitNearestN(const score_t & score, const size_t K);

    /**
     * K-Nearest Neighbor Structure for Pairwise Euclidean Distances
     *
     * This function will return a vector of vectors, where
     * each individual vector corresponds to that solutions nearest neighbors
     *
     * In the event of ties, we take the right neighbor score.
     *
     * @param dmat Matrix containing the pairwise distances.
     * @param K Size of nighborhood.
     *
     * @return Vector of vector scores.
     */
    neigh_t EuclideanNearestN(const fmatrix_t & dmat, const size_t K);

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

    /**
     * Pareto Front Group Structure:
     *
     * This function will return a vector of vectors, where
     * each individual vector corresponds to the Pareto group that
     * each solution belongs to. The first vector consists of the first
     * Pareto front, and so on.
     *
     * @param score Vector containing all solution phenotypes.
     *
     * @return Vector of vector groupings.
     */
    pareto_g_t ParetoFrontGroups(const fmatrix_t & scores);


    /**
     * K-Nearest Neighbor Structure for Pairwise Euclidean Distances for Novelty Search
     *
     * This function will return a vector of vectors, where
     * each individual vector corresponds to that solutions nearest neighbors.
     * We are given a matrix with both population and archive (in that order.)
     * We must only find neighbors for the first POP_SIZE vectors in the matrix given.
     *
     * In the event of ties, we take the right neighbor score.
     *
     * @param dmat Matrix containing the pairwise distances.
     * @param K Size of nighborhood.
     * @param N Number of neighborhoods we need to find
     *
     * @return Vector of vector scores.
     */
    neigh_t NoveltySearchNearestN(const fmatrix_t & dmat, const size_t K, const size_t N);

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
    score_t FitnessSharing(const fmatrix_t & dmat, const score_t & score, const double alph, const double sig);

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

    /**
     * Pareto Fitness Sharing:
     *
     * Fitness sharing is applied within grouping.
     *
     * @param groups  Vector containing Pareto groups (earlier ones are better).
     * @param phenos  Vector of phenotypes
     * @param alpha   Alpha value used for fitness sharing
     * @param beta    Beta value used for fitness sharing
     *
     * @return Void.
     */
    score_t ParetoFitness(const pareto_g_t & groups, const fmatrix_t & phenos, const double alpha, const double sigma, const double low, const double high);

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
    ids_t MLSelect(const size_t mu, const size_t lambda, const fitgp_t & group);

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

    /**
     * Epsilon Lexicase Selector:
     *
     * This function will iterate through individual testcases.
     * While filtering down the population on each one, only taking the top performers.
     * The top performers must be some within some distance of the top performance to pass.
     *
     * In the event of ties on the last testcase being used, a solution will be selected at random.
     *
     * @param mscore Matrix of solution fitnesses (must be the same amount of fitnesses per solution)(mscore.size => # of orgs).
     * @param epsi Epsilon threshold value.
     * @param M Number of traits we are expecting.
     *
     * @return A single winning solution id.
     */
    size_t EpsiLexicase(const fmatrix_t & mscore, const double epsi, const size_t M);

    /**
     * Stochastic Remainder Selector:
     *
     * This function will implement the selector for stochastic remainder selection with replacement.
     *
     * We start by checking if there exist any solutions with a score greater than 0.
     * If there isn't we assign all solutions a dummy score of 1.0, and allow seleciton to continue.
     * If there are, then we proceed with placing them into a set (to keep them sorted high to low).
     *
     * After we go through each solution in the set, and divde all fitness by the population fitness mean.
     * Next, we iterate through each solutinon and add them as parrents according the interger portion
     * of thier fitness score. The decimal portion of their adjusted fitness is the probability of them
     * being selected again as parent. We loop through the population until enough parents are selected.
     *
     * @param mscore vector of solution fitnesses.
     *
     * @return A vector with parent ids.
     */
    ids_t StochasticRemainder(const score_t & scores);

  private:

    // random pointer from world.h
    emp::Ptr<emp::Random> random;
};

///< population structure

Selection::neigh_t Selection::FitNearestN(const score_t & score, const size_t K)
{
  // quick checks
  emp_assert(0 < score.size()); emp_assert(K < score.size());

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

  emp_assert(order.size() == score.size());

  // walk through sorted vector and create the k neighborhood per score value
  for(size_t i = 0; i < order.size(); ++i)
  {
    emp::vector<double> neigh;
    int left = i - 1; int right = i + 1;

    // generate the neighborhood
    while(neigh.size() != K)
    {
      //quick checks
      emp_assert(!(left < 0 && order.size() <= right));

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
        const double ld = Distance(order[i].second, order[left].second);
        const double rd = Distance(order[i].second, order[right].second);

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
      ids_t p{i};
      group[score[i]] = p;
    }
    else{group[score[i]].push_back(i);}
  }

  return group;
}

Selection::neigh_t Selection::EuclideanNearestN(const fmatrix_t & dmat, const size_t K)
{
  // quick checks
  emp_assert(0 < dmat.size()); emp_assert(K < dmat.size()); emp_assert(0 <= K);

  // create group vector returning
  neigh_t group(dmat.size());

  // iterate through the distance vector
  for(size_t i = 0; i < dmat.size(); ++i)
  {
    // quick checks
    emp_assert(dmat.size() == dmat[i].size());

    // container to hold distances
    std::multiset<double> distance;

    // grab values from lower diagonal
    for(size_t j = 0; j < dmat[i].size(); ++j)
    {
      // ignore i == j case
      if(i == j) {continue;}

      // only look at lower diagnoal
      if(j < i)
      {
        emp_assert(dmat[i][j] != ERROR_VALD);
        distance.insert(dmat[i][j]);
      }
      else
      {
        emp_assert(dmat[j][i] != ERROR_VALD);
        distance.insert(dmat[j][i]);
      }
    }

    // make sure we have all the required data
    emp_assert(distance.size() == dmat.size() - 1);

    // grab the k nearest neighbors
    auto it = distance.begin();
    for(size_t k = 0; k < K; ++k) {group[i].push_back(*it++);}

    emp_assert(group[i].size() == K);
  }

  return group;
}

Selection::pareto_g_t Selection::ParetoFrontGroups(const fmatrix_t & scores)
{
  // quick checks
  emp_assert(0 < scores.size());

  // will hold the Pareto groups
  pareto_g_t groups;
  // initialize current candidates
  ids_t current(scores.size());
  std::iota(current.begin(), current.end(), 0);

  // loop through current while there are solutions to be placed
  while(0 < current.size())
  {
    // holds current front
    ids_t front;
    // holds next iteration of candidate ids
    ids_t next_current;

    // loop through pairs for comparisons
    for(size_t i = 0; i < current.size(); ++i)
    {
      bool dominated = false;
      for(size_t j = 0; j < current.size(); ++j)
      {
        if(i == j) {continue;}

        // check if any j dominates i
        if(PartiallyGreater(scores[current[j]], scores[current[i]]))
        {
          dominated = true;
          break;
        }
      }
      // if i is not dominated by anyone we add it to the front
      if(!dominated)
      {
        front.push_back(current[i]);
      }
      else
      {
        next_current.push_back(current[i]);
      }
    }

    current.clear();
    current = next_current;
    groups.push_back(front);
  }

  // quick checks
  // make sure that all ids are accounted for
  emp_assert(ParetoUnique(groups, scores.size()));

  return groups;
}

Selection::neigh_t Selection::NoveltySearchNearestN(const fmatrix_t & dmat, const size_t K, const size_t N)
{
// quick checks
  emp_assert(0 < dmat.size()); emp_assert(0 < N);
  emp_assert(K < N); emp_assert(0 <= K);
  emp_assert(N <= dmat.size());

  // create group vector returning
  neigh_t group(N);

  // iterate through the distance vector
  for(size_t i = 0; i < N; ++i)
  {
    // quick checks
    emp_assert(dmat.size() == dmat[i].size());

    // container to hold distances
    std::multiset<double> distance;

    // grab values from lower diagonal
    for(size_t j = 0; j < dmat[i].size(); ++j)
    {
      // ignore i == j case
      if(i == j) {continue;}

      // only look at lower diagnoal
      if(j < i)
      {
        emp_assert(dmat[i][j] != ERROR_VALD);
        distance.insert(dmat[i][j]);
      }
      else
      {
        emp_assert(dmat[j][i] != ERROR_VALD);
        distance.insert(dmat[j][i]);
      }
    }

    // make sure we have all the required data
    emp_assert(distance.size() == dmat.size() - 1);

    // grab the k nearest neighbors
    auto it = distance.begin();
    for(size_t k = 0; k < K; ++k) {group[i].push_back(*it++);}

    emp_assert(group[i].size() == K);
  }

  return group;
}


///< fitness transformation >///

Selection::score_t Selection::FitnessSharing(const fmatrix_t & dmat, const score_t & score, const double alph, const double sig)
{
  // quick checks
  emp_assert(dmat.size() == score.size()); emp_assert(0 <= alph); emp_assert(0 <= sig);
  emp_assert(0 < dmat.size()); emp_assert(0 < score.size());

  score_t tscore(score.size());

  // edge case where K == 0
  if(sig == 0.0)
  {
    std::copy(score.begin(), score.end(), tscore.begin());

    // quick checks and return the score vector unaltered
    emp_assert(score[0] == tscore[0]);
    return tscore;
  }

  for(size_t i = 0; i < dmat.size(); ++i)
  {
    // quick checks
    emp_assert(dmat[i].size() == score.size());

    // mi value that holds niche count (or scaling factor)
    // we can start at 1 because sh(d_ij) (where i == j), will equal 1.0
    double mi = 1.0;

    for(size_t j = 0; j < dmat.size(); ++j)
    {
      // ignore similar pairs, already accounted for
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
  emp_assert(0.0 <= dist); emp_assert(0.0 <= sig); emp_assert(0.0 <= alph);

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
  emp_assert(score.size() == neigh.size()); emp_assert(0 <= K);
  emp_assert(0 < score.size()); emp_assert(0 < neigh.size());

  // novelty score transformed from orignial scores
  score_t nscore(score.size());

  // edge case where K == 0
  if(K == 0)
  {
    std::copy(score.begin(), score.end(), nscore.begin());

    // quick checks and return the score vector unaltered
    emp_assert(score[0] == nscore[0]);
    return nscore;
  }

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
    // std::cout << "Novel: " << numer / denom << std::endl;
    nscore[i] = numer / denom;
  }

  return nscore;
}

Selection::score_t Selection::ParetoFitness(const pareto_g_t & groups, const fmatrix_t & phenos, const double alpha, const double sigma, const double low, const double high)
{
  // quick checks
  emp_assert(0 < groups.size()); emp_assert(0 < phenos.size());
  emp_assert(0 < alpha); emp_assert(0 < sigma);
  emp_assert(0 < high); emp_assert(0 < low);

  // initialize starting fitnessess
  score_t fitness(phenos.size(), 0.0);
  double group_min = high;

  // update fitness accordingly
  for(size_t i = 0; i < groups.size(); ++i)
  {
    if(group_min < 0) {std::cout << "ERROR::group_min=" << group_min << std::endl; exit(-1);}

    // group
    ids_t g = groups[i];

    // if g size is 1 continue
    if(g.size() == 1)
    {
      // std::cout << "g.size() == 1 :: " << group_min << std::endl << std::endl;
      fitness[g[0]] = group_min;
      group_min-= low;
      continue;
    }

    // collect what we need
    score_t group_scores;
    fmatrix_t group_phenos;

    // go through each id
    for(const size_t & id : g)
    {
      group_scores.push_back(group_min);
      group_phenos.push_back(phenos[id]);
    }

    // get similarity matrix
    fmatrix_t sim_mat = SimilarityMatrix(group_phenos, 2.0);
    emp_assert(sim_mat.size() == g.size());
    // apply fitness sharing
    score_t gscores = FitnessSharing(sim_mat, group_scores, alpha, sigma);
    emp_assert(gscores.size() == g.size());

    // std::cout << std::endl;
    // std::cout << "group " << i << ": ";

    // update fitness vector
    // score_t x;
    for(size_t i = 0; i < g.size(); ++i)
    {
      // x.push_back(gscores[i]);
      fitness[g[i]] = gscores[i];
      if(gscores[i] < group_min) {group_min = gscores[i];}
    }

    // std::sort(x.begin(), x.end(), std::greater<double>());
    // for(const auto & y : x) {std::cout << y << ", ";}
    // std::cout << std::endl << std::endl;

    // lower group_min by low
    group_min -= low;
  }

  return fitness;
}


///< selector functions

Selection::ids_t Selection::MLSelect(const size_t mu, const size_t lambda, const fitgp_t & group)
{
  // quick checks
  emp_assert(0 < mu); emp_assert(0 < lambda);
  emp_assert(mu <= lambda); emp_assert(0 < group.size());

  // in the event that we are asking for the whole population
  // just return the vector of ids from 0 to pop size (lambda)
  if(mu == lambda)
  {
    ids_t pop;
    for(const auto & g : group)
    {
      for(const auto & id : g.second)
      {
        pop.push_back(id);
      }
    }

    emp_assert(pop.size() == lambda);
    return pop;
  }

  // go through the ordered scores and get our top mu solutions
  ids_t topmu;
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
  ids_t parent;
  size_t lm = lambda / mu;
  for(auto id : topmu)
  {
    for(size_t i = 0; i < lm; ++i){parent.push_back(id);}
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
  emp::vector<size_t> tour = emp::Choose(*random, score.size(), t);

  // store all scores for the tournament
  emp::vector<double> subscore(t);
  for(size_t i = 0; i < tour.size(); ++i) {subscore[i] = score[tour[i]];}

  // group scores by fitness and position;
  auto group = FitnessGroup(subscore);
  emp::vector<size_t> opt = group.begin()->second;

  //shuffle the vector with best fitness ids
  emp::Shuffle(*random, opt);
  emp_assert(0 < opt.size());

  return tour[opt[0]];
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

size_t Selection::EpsiLexicase(const fmatrix_t & mscore, const double epsi, const size_t M)
{
  // quick checks
  emp_assert(0 < mscore.size()); emp_assert(0 <= epsi); emp_assert(0 < M);

  // create vector of shuffled testcase ids
  ids_t test_id(M);
  std::iota(test_id.begin(), test_id.end(), 0);
  emp::Shuffle(*random, test_id);

  // vector to hold filterd elite solutions
  ids_t filter(mscore.size());
  std::iota(filter.begin(), filter.end(), 0);

  // iterate through testcases until we run out or have a single winner
  size_t tcnt = 0;
  while(tcnt < M && filter.size() != 1)
  {
    // testcase we are randomly evaluating
    size_t testcase = test_id[tcnt];

    // create vector of current filter solutions
    score_t scores(filter.size());
    for(size_t i = 0; i < filter.size(); ++i)
    {
      // make sure each solutions vector is the right size
      emp_assert(mscore[filter[i]].size() == M);
      scores[i] = mscore[filter[i]][testcase];
    }

    // group org ids by performance in descending order
    fitgp_t group = FitnessGroup(scores);

    // update the filter vector with pop ids that are worthy
    ids_t temp_filter = filter;
    filter.clear();
    for(const auto & p : group)
    {
      if(Distance(group.begin()->first, p.first) <= epsi)
      {
        for(auto id : p.second)
        {
          filter.push_back(temp_filter[id]);
        }
      }
      else{break;}
    }

    ++tcnt;
  }

  // Get a random position from the remaining filtered solutions (may be one left too)
  emp_assert(0 < filter.size());
  size_t wid = emp::Choose(*random, filter.size(), 1)[0];

  return filter[wid];
}

Selection::ids_t Selection::StochasticRemainder(const score_t & scores)
{
  // quick checks
  emp_assert(0 < scores.size());

  // average population fitness
  double normalizer = std::accumulate(scores.begin(), scores.end(), 0.0);
  normalizer /= static_cast<double>(scores.size());

  // save all solutions with positions and fitness in set
  // add transformed fitness to the set
  std::set<id_f_t, std::function< bool(id_f_t,id_f_t)>> ordered(
    [] (const id_f_t & lhs, const id_f_t & rhs ) { return std::get<1>(lhs) > std::get<1>(rhs); } ) ;

  for(size_t i = 0; i < scores.size(); ++i)
  {
    if(0.0 < scores[i] / normalizer)
    {
      ordered.insert(std::make_pair(i, scores[i] / normalizer));
    }
  }

  // make sure we have solutions with scores to populate
  if(ordered.size() == 0)
  {
    // parents
    ids_t parents(scores.size());
    std::iota(parents.begin(), parents.end(), 0);
    return parents;
  }

  // parents
  ids_t parents;
  emp_assert(0 == parents.size());

  // continue to push parents back until we are done
  while (parents.size() != scores.size())
  {
    // go through and do expected insertions
    for(const auto & p : ordered)
    {
      // sol_id
      size_t id = std::get<0>(p);
      // sol_fitness
      double fi = std::get<1>(p), intPart, fractPart;;
      fractPart = modf(fi, &intPart);

      // while we can keep adding parents keep going
      for(size_t i = 0; i < static_cast<size_t>(intPart); ++i)
      {
        parents.push_back(id);
        if(parents.size() == scores.size()) {break;}
      }

      if(random->P(fractPart) && parents.size() < scores.size())
      {
        parents.push_back(id);
      }

      if(parents.size() == scores.size()) {break;}
    }
  }
  emp_assert(parents.size() == scores.size());


  return parents;
}


///< helper functions

double Selection::Pnorm(const score_t & x, const score_t & y, const double exp)
{
  // quick checks
  emp_assert(0 < x.size()); emp_assert(0 < y.size());
  emp_assert(x.size() == y.size()); emp_assert(0 <= exp);

  // subtract one vector from the other and take the exp of that
  score_t diff(x.size());
  for(size_t i = 0; i < x.size(); ++i){diff[i] = std::pow((x[i] - y[i]), exp);}

  double tot = std::accumulate(diff.begin(), diff.end(), 0.0);

  return std::pow(tot, (1.0/exp));
}

Selection::fmatrix_t Selection::SimilarityMatrix(const gmatrix_t & genome, const double exp)
{
  // quick checks
  emp_assert(1 <= exp); emp_assert(1 < genome.size());

  // generate the matrix, lower diagnonal matrix filled (not include i == j)
  fmatrix_t similar(genome.size());
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

bool Selection::PartiallyGreater(const score_t & A, const score_t & B)
{
  //quick checks
  emp_assert(A.size() == B.size());

  // check if A > B
  bool strict = false;
  for(size_t i = 0; i < A.size(); ++i)
  {
    // check if A[i] < B[i]
    if(A[i] < B[i]) {return false;}

    // the strictly greater position in A is found
    else if (A[i] > B[i]) {strict = true;}
  }

  return strict;
}

Selection::fmatrix_t Selection::NoveltySimilarityMatrix(const fmatrix_t & phenos, const std::deque<score_t> & archive, const double exp)
{
  // quick checks
  emp_assert(0 < phenos.size()); emp_assert(0 < exp);

  // will hold combined pop phenos and archive of pheno
  fmatrix_t combo;

  for(size_t i = 0; i < phenos.size(); ++i) {combo.push_back(phenos[i]);}
  for(size_t i = 0; i < archive.size(); ++i) {combo.push_back(archive[i]);}

  return SimilarityMatrix(combo,exp);
}

#endif