/// These are the selection schemes we are using for this project
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
#include "emp/base/vector.hpp"
#include "emp/math/Random.hpp"
#include "emp/math/random_utils.hpp"

class Selection
{
  // object types we are using in this class
  public:
    // vector of any ids
    using ids_t = emp::vector<size_t>;
    // vector type of org score
    using phenotype_t = emp::vector<double>;
    // matrix type of org with multiple scores
    using fmatrix_t = emp::vector<phenotype_t>;
    // vector holding population genomes
    using gmatrix_t = emp::vector<emp::vector<double>>;
    // map holding population id groupings by fitness (keys in decending order)
    using fitgp_t = std::map<double, ids_t, std::greater<double>>;
    // sorted score vector w/ position id and score
    using sorted_t = emp::vector<std::pair<size_t,double>>;
    // vector of double vectors for K neighborhoods
    using neigh_t = emp::vector<phenotype_t>;
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

    bool ParetoNonZero(const phenotype_t & score)
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

    // find all similar neighbors given some threshold
    fmatrix_t SimilarNeighbors(const gmatrix_t & mat, const double & threshold, const double exp = 2.0);

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
    bool PartiallyGreater(const phenotype_t & A, const phenotype_t & B);

    // get K smallest sum([x-y])^2 for each position
    fmatrix_t NoveltySearchNearSum(const fmatrix_t & mat, const size_t K, const size_t N, const size_t exp = 2.0);

    // regular summation
    double SumOfSquares(const phenotype_t & x, const phenotype_t & y, const double exp)
    {
      // quick checks
      emp_assert(0 < x.size()); emp_assert(x.size() == y.size());
      emp_assert(0 < exp);

      double sum = 0.0;
      for(size_t i = 0; i < x.size(); ++i) { sum += std::pow((x[i] - y[i]), exp); }
      return sum;
    }

    // smart sum of squares funtion
    // max is the current upper bound for neighbor summation
    // if x,y summation is greater we can stop
    double SmartSOS(const phenotype_t & x, const phenotype_t & y, const double exp, const double max)
    {
      // quick checks
      // max can be 0.0 if exact copies are the neigbors
      emp_assert(0 < x.size()); emp_assert(x.size() == y.size());
      emp_assert(0 < exp); emp_assert(0.0 <= max);

      double sum = 0.0;
      for(size_t i = 0; i < x.size(); ++i)
      {
        sum += std::pow((x[i] - y[i]), exp);

        if(max < sum){return -100.0;}
      }

      return sum;
    }

    ///< population structure

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
    fitgp_t FitnessGroup(const phenotype_t & score);

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
     * @param alpha Shape of sharing function.
     * @param sigma Similarity threshold.
     *
     * @return Vector with parent id's that are selected.
     */
    phenotype_t FitnessSharing(const fmatrix_t & neigh, const phenotype_t & score, const double alpha, const double sigma);

    /**
     * Fitness Sharing: Sharing Function
     *
     * This function will return a score of 0 or 1 - (score/sigma)^alpha.
     * Depending on score < sigma.
     *
     * \param dist single score we are evaluating
     * \param sigma similarity threshold we are using
     * \param alpha shape of penalty function
     *
     * \return a tranformed fitness score, with fitness sharing integrated within the score
     */
    double SharingFunction(const double dist, const double sigma, const double alpha);

    /**
     * Novelty Fitness Transformation:
     *
     * This function will transform original fitness values as novelty scores.
     * Each score has its associated neighbors (2nd argument).
     *
     *
     * @param neigh Vector containing neighbor score sets by population positions.
     * @param K Size of each neighborhood.
     * @param exp Exponent we are using for the p norm calculations.
     *
     * @return Vector with transformed scores.
     */
    phenotype_t NoveltySOS(const neigh_t & neigh, const size_t K, const double exp = 2.0);

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
    phenotype_t ParetoFitness(const pareto_g_t & groups, const fmatrix_t & phenos, const double alpha, const double sigma, const double low, const double high);

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
    size_t Tournament(const size_t t, const phenotype_t & score);

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
     * Title: A Novel Selection Approach for Genetic Algorithms for GlobalOptimization of Multimodal Continuous Functions
     * Authors: Ehtasham-ulHaq, IshfaqAhmad, AbidHussain, and Ibrahim M.Almanjahie
     *
     * We start by checking if there exist any solutions with a score greater than 0.
     * If there isn't we assign all solutions a dummy score of 1.0, and allow seleciton to continue.
     * We then shuffle a vector of id's that the solutions will be processed in.
     *
     * After we go through each solution in the set, and divde all fitness by the population fitness mean.
     * Next, we iterate through each solutinon and add them as parrents according the interger portion
     * of thier fitness score. The decimal portion of their adjusted fitness is the probability of them
     * being selected again as parent. We loop through the population until enough parents are selected.
     * Shuffling the order in which solutions are seen each iteration.
     *
     * @param mscore vector of solution fitnesses.
     *
     * @return A vector with parent ids.
     */
    ids_t StochasticRemainder(const phenotype_t & scores);

  private:

    // random pointer from world.h
    emp::Ptr<emp::Random> random;
};

///< population structure

Selection::fitgp_t Selection::FitnessGroup(const phenotype_t & score)
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


///< fitness transformation >///

Selection::phenotype_t Selection::FitnessSharing(const fmatrix_t & neigh, const phenotype_t & score, const double alpha, const double sigma)
{
  // quick checks
  emp_assert(neigh.size() == score.size()); emp_assert(0 <= alpha); emp_assert(0 <= sigma);
  emp_assert(0 < neigh.size()); emp_assert(0 < score.size());

  // edge case where K == 0
  if(sigma == 0.0)
  {
    phenotype_t tscore = score;
    return tscore;
  }

  phenotype_t tscore(score.size());

  for(size_t i = 0; i < neigh.size(); ++i)
  {
    // mi value that holds niche count (or scaling factor)
    // we can start at 1 because sh(d_ij) (where i == j), will equal 1.0
    double mi = 1.0;

    // sum all
    for(size_t j = 0; j < neigh[i].size(); ++j)
    {
      const double euclid = std::pow(neigh[i][j], (1.0/2.0));
      mi += SharingFunction(euclid, sigma, alpha);
    }

    tscore[i] = score[i] / mi;
  }

  return tscore;
}

double Selection::SharingFunction(const double dist, const double sigma, const double alpha)
{
  // quick checks
  emp_assert(0.0 <= dist); emp_assert(0.0 <= sigma); emp_assert(0.0 <= alpha);

  const double rse = std::pow((dist/sigma), alpha);
  return 1.0 - rse;
}

Selection::phenotype_t Selection::NoveltySOS(const neigh_t & neigh, const size_t K, const double exp)
{
  // quick checks
  emp_assert(0 < neigh.size()); emp_assert(0 < K); emp_assert(0.0 <= exp);

  phenotype_t fitness(neigh.size(), 0.0);

  for(size_t i = 0; i < neigh.size(); ++i)
  {
    // quick checks
    emp_assert(neigh[i].size() == K);
    // add
    for(size_t k = 0; k < K; ++k)
    {
      fitness[i] += std::pow(neigh[i][k], (1.0/exp)) / static_cast<double>(K);
    }
  }

  return fitness;
}

Selection::phenotype_t Selection::ParetoFitness(const pareto_g_t & groups, const fmatrix_t & phenos, const double alpha, const double sigma, const double low, const double high)
{
  // quick checks
  emp_assert(0 < groups.size()); emp_assert(0 < phenos.size());
  emp_assert(0 < alpha); emp_assert(0 < sigma);
  emp_assert(0 < high); emp_assert(0 < low);

  // initialize starting fitnessess
  phenotype_t fitness(phenos.size(), 0.0);
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
      fitness[g[0]] = group_min;
      group_min *= low;
      continue;
    }

    // collect what we need
    phenotype_t group_scores;
    fmatrix_t group_phenos;

    // go through each id and record current min_score and phenotype
    for(const size_t & id : g)
    {
      // current groups scores
      group_scores.push_back(group_min);
      //current groups phenotype
      group_phenos.push_back(phenos[id]);
    }

    // get similar neighbors
    const fmatrix_t neighbors = SimilarNeighbors(group_phenos, sigma * sigma);
    // transform scores
    const phenotype_t gscores = FitnessSharing(neighbors, group_scores, alpha, sigma);

    // update fitness vector
    for(size_t i = 0; i < g.size(); ++i)
    {
      fitness[g[i]] = gscores[i];
      if(gscores[i] < group_min) {group_min = gscores[i];}
    }

    // lower group_min by low
    group_min *= low;
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

size_t Selection::Tournament(const size_t t, const phenotype_t & score)
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
    phenotype_t scores(filter.size());
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

Selection::ids_t Selection::StochasticRemainder(const phenotype_t & scores)
{
  // quick checks
  emp_assert(0 < scores.size());

  // average population fitness
  double normalizer = std::accumulate(scores.begin(), scores.end(), 0.0);
  normalizer /= static_cast<double>(scores.size());

  // save all solutions with positions and fitness in set
  // add transformed fitness to the set
  emp::vector<id_f_t> id_fit;

  for(size_t i = 0; i < scores.size(); ++i)
  {
    if(0.0 < scores[i] / normalizer)
    {
      id_fit.push_back(std::make_pair(i, scores[i] / normalizer));
    }
  }

  // make sure we have solutions with scores to populate
  if(id_fit.size() == 0)
  {
    // parents
    ids_t parents(scores.size());
    std::iota(parents.begin(), parents.end(), 0);
    return parents;
  }

  // parents
  ids_t parents;
  emp_assert(0 == parents.size());

  // random order we go through
  ids_t order(id_fit.size());
  std::iota(order.begin(), order.end(), 0);

  // continue to push parents back until we are done
  while (parents.size() != scores.size())
  {
    //shuffle order in which solutions are being processed
    emp::Shuffle(*random, order);

    // go through and do expected insertions
    for(const auto & o : order)
    {
      // sol_id
      const size_t id = std::get<0>(id_fit[o]);
      emp_assert(0 <= id); emp_assert(id < scores.size());
      emp_assert(0 <= o); emp_assert(o < id_fit.size());

      // sol_fitness
      double fi = std::get<1>(id_fit[o]), intPart, fractPart;
      fractPart = modf(fi, &intPart);
      emp_assert(0.0 <= fi);

      // add parent integer many times, if possible
      for(size_t i = 0; i < static_cast<size_t>(intPart); ++i)
      {
        parents.push_back(id);
        if(parents.size() == scores.size()) {break;}
      }
      // fraction part is probability of being added again
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

Selection::fmatrix_t Selection::SimilarNeighbors(const gmatrix_t & mat, const double & threshold, const double exp)
{
  // quick checks
  emp_assert(0 < mat.size());

  // will hold all neighbors that fall within threshold of similarity
  fmatrix_t fit_mat(mat.size());

  // go through genomes pairings and find neighbors
  for(size_t i = 0; i < mat.size(); ++i)
  {
    // will hold all neighbors
    emp::vector<double> neigh;

    for(size_t j = 0; j < mat.size(); ++j)
    {
      // skip same pairs
      if(i == j) {continue;}

      const double smart = SmartSOS(mat[i], mat[j], exp, threshold);

      // stopped early
      if(smart < 0.0) {continue;}

      // add to the neighbor set
      neigh.push_back(smart);
    }

    fit_mat[i] = neigh;
  }

  return fit_mat;
}

bool Selection::PartiallyGreater(const phenotype_t & A, const phenotype_t & B)
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

Selection::fmatrix_t Selection::NoveltySearchNearSum(const fmatrix_t & mat, const size_t K, const size_t N, const size_t exp)
{
  // quick checks
  emp_assert(0 < K); emp_assert(0 < N); emp_assert(K < N);
  emp_assert(mat.size() == N);

  // will hold the final set of sums
  fmatrix_t fit_mat(N);

  // find nearest k sums
  for(size_t i = 0; i < N; ++i)
  {
    // container to hold distances
    std::multiset<double> neighbors;

    // compare against each other position
    for(size_t j = 0; j < N; ++j)
    {
      // skip same pairs
      if(i == j){continue;}

      // add scores until we have enough to test
      if(neighbors.size() < K) { neighbors.insert(SumOfSquares(mat[i],mat[j],exp)); }
      // check if we need to remove any socres
      else
      {
        const double smart = SmartSOS(mat[i],mat[j],exp,*(neighbors.rbegin()));

        // negative means we stopped early
        if(smart < 0.0){ continue; }
        // positive means we can add a score and pop the end off
        else
        {
          neighbors.insert(smart);
          auto it = --neighbors.end();
          neighbors.erase(it);
        }
      }
    }

    // make sure that neighbors has k elements in neighbors
    emp_assert(neighbors.size() == K);

    // add all neighbors to vector and save in fit_mat
    phenotype_t score;
    for(auto it = neighbors.begin(); it != neighbors.end(); ++it)
    {
      score.push_back(*it);
    }
    fit_mat[i] = score;
  }

  return fit_mat;
}

#endif