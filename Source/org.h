/// organims class that holds its genotype and phenotype, both numerical vectors of a predetermined dimensionality

#ifndef ORG_H
#define ORG_H

///< standard headers
#include <algorithm>

///< empirical headers
#include "emp/base/vector.hpp"

///< coordiante we start from
constexpr double START_DB = 0.0;
constexpr size_t START_ST = 0;

class Org
{
  public:
    // genotype vector type
    using genotype_t = emp::vector<double>;
    // phenotype vector type
    using phenotype_t = emp::vector<double>;
    // optimal gene vector type
    using optimal_t = emp::vector<bool>;

  public:
    // for initial population
    Org(size_t _m)
    {
      // quick checks
      emp_assert(genotype.size() == 0); emp_assert(M == 0);

      // set all requried variables
      M = _m;
      start_pos = _m;
      streak = _m;
      genotype.resize(_m, START_DB);
    }

    // every org after starting generation
    Org(genotype_t _g)
    {
      // quick checks
      emp_assert(genotype.size() == 0); emp_assert(M == 0);

      // set all requried variables
      M = _g.size();
      start_pos = _g.size();
      streak = _g.size();
      genotype = _g;
    }

    // ask Charles
    Org(const Org &) = default;
    Org(Org &&) = default;
    ~Org() { ; }
    Org &operator=(const Org &) = default;
    Org &operator=(Org &&) = default;

    ///< getters

    // const + reference to
    const genotype_t & GetGenotype() const {emp_assert(0 < genotype.size()); return genotype;}
    const phenotype_t & GetPhenotype() const {emp_assert(evaluated); return phenotype;}
    const optimal_t & GetOptimal() const {emp_assert(opti); return optimal;}
    // reference to
    genotype_t & GetGenotype() {emp_assert(0 < genotype.size()); return genotype;}
    phenotype_t & GetPhenotype() {emp_assert(evaluated); return phenotype;}
    optimal_t & GetOptimal() {emp_assert(opti); return optimal;}
    // get const aggregate fitness
    double GetAggregate() {emp_assert(aggregated); return aggregate;}
    // get const aggregate fitness
    double GetAggregate()const {emp_assert(aggregated); return aggregate;}
    // get clone bool
    bool GetClone() const {emp_assert(0 < genotype.size()); return clone;}
    // get optimal
    size_t GetCount() const {emp_assert(counted); return count;}
    // get gene count
    size_t GetM() {emp_assert(0 < M); return M;}
    // get start position
    size_t GetStart() {emp_assert(start_pos != M); return start_pos;}
    size_t GetStart() const {emp_assert(start_pos != M); return start_pos;}
    // get streak count
    size_t GetStreak() {emp_assert(streak != M); return streak;}
    size_t GetStreak() const {emp_assert(streak != M); return streak;}
    // Are we optimized at this objective?
    bool OptimizedAt(const size_t obj);
    // get evaluated bool
    bool GetEvaluated() {return evaluated;}
    // get optimal bool
    bool GetOpti() {return opti;}
    // get aggregated bool
    bool GetAggregated() {return aggregated;}
    // get counted bool
    bool GetCounted() {return counted;}
    // get streak bool
    bool GetStreaked() {return streaked;}
    // get max trait
    double GetMaxTrait()
    {
      // quick checks
      emp_assert(start); emp_assert(0 < M);
      emp_assert(phenotype.size() == M);

      return phenotype[start_pos];
    }


    ///< setters

    // set phenotype vector (recieved from problem.h in world.h or inherited from parent)
    void SetScore(const phenotype_t & s_)
    {
      // make sure that phenotype vector hasn't been set before.
      emp_assert(!evaluated); emp_assert(s_.size() == M); emp_assert(phenotype.size() == 0); emp_assert(0 < M);
      evaluated = true;
      phenotype = s_;
    }

    // set the optimal gene vector (recieved from problem.h in world.h or inherited from parent)
    void SetOptimal(const optimal_t & o_)
    {
      // make sure that optimal gene vector hasn't been set before.
      emp_assert(!opti); emp_assert(o_.size() == M); emp_assert(optimal.size() == 0); emp_assert(0 < M);
      opti = true;
      optimal = o_;
    }

    // set the optimal gene count (called from world.h or inherited from parent)
    void SetCount(size_t c_)
    {
      emp_assert(!counted); emp_assert(0 < M);
      counted = true;
      count = c_;
    }

    // set the aggregated phenotype (called from world.h or inherited from parent)
    void SetAggregate(double a_)
    {
      emp_assert(!aggregated); emp_assert(0 < M);
      aggregated = true;
      aggregate = a_;
    }

    // set the starting position
    void SetStart(size_t s_)
    {
      emp_assert(!start); emp_assert(0 < M);
      start = true;
      start_pos = s_;
    }

    // set the starting position
    void SetStreak(size_t s_)
    {
      emp_assert(!streaked); emp_assert(0 < M);
      streaked = true;
      streak = s_;
    }


    ///< functions to calculate scores and related data

    /**
     * Aggregate Score function:
     *
     * First we check to see if the phenotype has been calculated.
     * If yes, aggregated=True and throw expection.
     * Else, aggregated=False and we calculate it and return aggregate.
     *
     * @return aggregate
    */
    double AggregateScore();

    /**
     * Count Optimized function:
     *
     * First we check to see if the count has been calculated.
     * If yes, counted=True and throw expection.
     * Else, countedd=False and we calculate it and return count.
     *
     * @return count
    */
    size_t CountOptimized();

    /**
     * Find Starting Position
     *
     * Find the staring position in the phenotype vector.
     */
    size_t StartPosition();


    /**
     * Find Maximum Streak
     *
     * Find the biggest streak.
     */
    size_t CalcStreak();


    ///< functions related to the birth of an organism

    /**
     * Reset function:
     *
     * Will reset all variables in an organims when birth occurs.
     * Function executes if offspring is not a clone
    */
    void Reset();

    /**
     * Inherit function:
     *
     * Will pass all info from parent to offspring solution.
     * Function executes if offpsring is an clone
     *
     * @param s phenotype vector recived
     * @param o optimal gene vector recieved
     * @param c optimal gene count recieved
     * @param a aggregate phenotype recieved
     *
    */
    void Inherit(const phenotype_t & s, const optimal_t & o, const size_t c, const double a, const size_t st, const size_t sr);

    /**
     * Me Clone function:
     *
     * Will set the clone variable to true.
    */
    void MeClone() {emp_assert(0 < M); emp_assert(!clone); clone = true;}

  private:
    // organism genotype vector
    genotype_t genotype;

    // organism phenotype vector
    phenotype_t phenotype;
    // phenotype vector set?
    bool evaluated = false;

    // organims gene optimal vector
    optimal_t optimal;
    // gene optimal vector calculated?
    bool opti = false;

    // optimal gene count
    size_t count = 0;
    // gene optimal vector counted?
    bool counted = false;

    // aggregate phenotype
    double aggregate = 0.0;
    // aggregate calculate?
    bool aggregated = false;

    // streak count
    size_t streak = 0;
    // streak calculated?
    bool streaked = false;

    // Number of genes in genotype
    size_t M = 0;

    // starting position
    size_t start_pos;
    // starting position located?
    bool start = false;

    // Are we a clone?
    bool clone = false;
};

///< getters with extra
bool Org::OptimizedAt(const size_t obj)
{
  // quick checks
  emp_assert(0 <= obj); emp_assert(obj < M);
  emp_assert(0 < optimal.size()); emp_assert(M == optimal.size());

  return optimal[obj];
}

///< functions to calculate scores and related data

double Org::AggregateScore()
{
  //quick checks
  emp_assert(!aggregated); emp_assert(0 < M);
  emp_assert(phenotype.size() == M, phenotype.size());

  // calculate the aggregate phenotype and set it
  SetAggregate(std::accumulate(phenotype.begin(), phenotype.end(), START_DB));

  return aggregate;
}

size_t Org::CountOptimized()
{
  //quick checks
  emp_assert(!counted); emp_assert(0 < M); emp_assert(opti);
  emp_assert(optimal.size() == M, optimal.size());

  // calculate total optimal genes and set it
  SetCount(std::accumulate(optimal.begin(), optimal.end(), START_ST));

  return count;
}

size_t Org::StartPosition()
{
  // quick checks
  emp_assert(!start); emp_assert(0 < M);
  emp_assert(phenotype.size() == M);

  // find max value position
  auto opti_it = std::max_element(phenotype.begin(), phenotype.end());
  SetStart(std::distance(phenotype.begin(), opti_it));

  return start_pos;
}

size_t Org::CalcStreak()
{
  // quick checks
  emp_assert(!streaked); emp_assert(0 < M);
  emp_assert(phenotype.size() == M);

  // get longest streak
  size_t count = 0;
  size_t max_cnt = 0;
  for(auto & s : phenotype)
  {
    if(s > 0.0)
    {
      count++;
    }
    else
    {
      if(count > max_cnt)
      {
        max_cnt = count;
      }
      count = 0;
    }
  }

  streak = max_cnt;

  return streak;
}

///< functions related to the birth of an organism

void Org::Reset()
{
  // quick checks
  emp_assert(0 < M); emp_assert(0 < genotype.size());

  // reset phenotype vector stuff
  phenotype.clear();
  evaluated = false;

  // reset optimal gene vector stuff
  optimal.clear();
  opti = false;

  // reset optimal gene count stuff
  count = 0;
  counted = false;

  // reset aggregate phenotype stuff
  aggregate = 0.0;
  aggregated = false;

  // reset starting position info
  start_pos = genotype.size();
  start = false;

  // reset clone var
  clone = false;
}

void Org::Inherit(const phenotype_t & s, const optimal_t & o, const size_t c, const double a, const size_t st, const size_t sr)
{
  // quick checks
  emp_assert(0 < M); emp_assert(0 < genotype.size()); emp_assert(clone);

  // copy everything into offspring solution
  SetScore(s);
  SetOptimal(o);
  SetCount(c);
  SetAggregate(a);
  SetStart(st);
  SetStreak(sr);
}

#endif