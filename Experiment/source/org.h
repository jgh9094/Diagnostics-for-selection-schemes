/// digital organisms (real number vectors) for the diagnostic experiments

#ifndef ORG_H
#define ORG_H

///< standard headers
#include <algorithm>

///< empirical headers
#include "base/vector.h"

///< coordiante we start from
constexpr double START_DB = 0.0;
constexpr size_t START_ST = 0;


class Org
{
  public:
    // genome vector type
    using genome_t = emp::vector<double>;
    // score vector type
    using score_t = emp::vector<double>;
    // optimal gene vector type
    using optimal_t = emp::vector<bool>;

  public:
    // for initial population
    Org(size_t _m)
    {
      // make sure nun weird is happening
      emp_assert(genome.size() == 0); emp_assert(M == 0);
      M = _m;
      genome.resize(_m, START_DB);
    }

    // every org after starting generation
    Org(genome_t _g)
    {
      // make sure we aren't seeing anything weird
      emp_assert(genome.size() == 0); emp_assert(M == 0);
      M = _g.size();
      genome.resize(M);
      std::copy(_g.begin(), _g.end(), genome.begin());
    }

    // ask Charles
    Org(const Org &) = default;
    Org(Org &&) = default;
    ~Org() { ; }
    Org &operator=(const Org &) = default;
    Org &operator=(Org &&) = default;

    ///< getters

    // const + reference to
    const genome_t & GetGenome() const {emp_assert(0 < genome.size()); return genome;}
    const score_t & GetScore() const {emp_assert(scored); return score;}
    const optimal_t & GetOptimal() const {emp_assert(opti); return optimal;}
    // reference to
    genome_t & GetGenome() {emp_assert(0 < genome.size()); return genome;}
    score_t & GetScore() {emp_assert(scored); return score;}
    optimal_t & GetOptimal() {emp_assert(opti); return optimal;}
    // get const aggregate fitness
    double GetAggregate() {emp_assert(aggregated); return agg_score;}
    // get clone bool
    bool GetClone() const {emp_assert(0 < genome.size()); return clone;}
    // get optimal
    size_t GetCount() const {emp_assert(counted); return count;}
    // get gene count
    size_t GetM() {emp_assert(0 < M); return M;}
    // Are we optimized at this objective?
    bool OptimizedAt(const size_t obj);

    ///< setters

    // set score vector (recieved from problem.h in world.h or inherited from parent)
    void SetScore(const score_t & s_)
    {
      // make sure that score vector hasn't been set before.
      emp_assert(!scored); emp_assert(s_.size() == M); emp_assert(score.size() == 0); emp_assert(0 < M);
      scored = true;
      score.resize(s_.size());
      std::copy(s_.begin(), s_.end(), score.begin());
    }

    // set the optimal gene vector (recieved from problem.h in world.h or inherited from parent)
    void SetOptimal(const optimal_t & o_)
    {
      // make sure that optimal gene vector hasn't been set before.
      emp_assert(!opti); emp_assert(o_.size() == M); emp_assert(optimal.size() == 0); emp_assert(0 < M);
      opti = true;
      optimal.resize(o_.size());
      std::copy(o_.begin(), o_.end(), optimal.begin());
    }

    // set the optimal gene count (called from world.h or inherited from parent)
    void SetCount(size_t c_)
    {
      emp_assert(!counted); emp_assert(0 < M);
      counted = true;
      count = c_;
    }

    // set the aggregated score (called from world.h or inherited from parent)
    void SetAggregate(double a_)
    {
      emp_assert(!aggregated); emp_assert(0 < M);
      aggregated = true;
      agg_score = a_;
    }


    ///< functions to calculate scores and related data

    /**
     * Aggregate Score function:
     *
     * First we check to see if the score has been calculated.
     * If yes, aggregated=True and throw expection.
     * Else, aggregated=False and we calculate it and return agg_score.
     *
     * @return agg_score
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
     * @param s score vector recived
     * @param o optimal gene vector recieved
     * @param c optimal gene count recieved
     * @param a aggregate score recieved
     *
    */
    void Inherit(const score_t & s, const optimal_t & o, size_t c, double a);

    /**
     * Me Clone function:
     *
     * Will set the clone variable to true.
    */
    void MeClone() {emp_assert(0 < M); emp_assert(!clone); clone = true;}

  private:
    // organism genome vector
    genome_t genome;

    // organism score vector
    score_t score;
    // score vector set?
    bool scored = false;

    // organims gene optimal vector
    optimal_t optimal;
    // gene optimal vector calculated?
    bool opti = false;

    // optimal gene count
    size_t count = 0;
    // gene optimal vector counted?
    bool counted = false;

    // aggregate score
    double agg_score = 0.0;
    // aggregate calculate?
    bool aggregated = false;

    // Number of genes in genome
    size_t M = 0;

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
  emp_assert(score.size() == M, score.size());

  // calculate the aggregate score and set it
  SetAggregate(std::accumulate(score.begin(), score.end(), START_DB));

  return agg_score;
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

///< functions related to the birth of an organism

void Org::Reset()
{
  // quick checks
  emp_assert(0 < M); emp_assert(0 < genome.size());

  // reset score vector stuff
  score.clear();
  scored = false;

  // reset optimal gene vector stuff
  optimal.clear();
  opti = false;

  // reset optimal gene count stuff
  count = 0;
  counted = false;

  // reset aggregate score stuff
  agg_score = 0.0;
  aggregated = false;

  // reset clone var
  clone = false;
}

void Org::Inherit(const score_t & s, const optimal_t & o, size_t c, double a)
{
  // quick checks
  emp_assert(0 < M); emp_assert(0 < genome.size()); emp_assert(!clone);

  // copy everything into offspring solution
  SetScore(s);
  SetOptimal(o);
  SetCount(c);
  SetAggregate(a);
}

#endif