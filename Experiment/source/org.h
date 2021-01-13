/// digital organisms (real number vectors) for the diagnostic experiments

#ifndef DIA_ORG_H
#define DIA_ORG_H

///< standard headers
#include <algorithm>

///< empirical headers
#include "base/vector.h"

///< score we start from
constexpr double START = 0.0;


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
    Org(size_t _m) : M(_m)
    {
      genome.resize(_m, START);
    }

    // every org after starting generation
    Org(genome_t _g) : M(_g.size())
    {
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
    const genome_t & GetGenome() const {return genome;}
    const score_t & GetScore() const {return score;}
    const optimal_t & GetOptimal() const {return optimal;}
    // reference to
    genome_t & GetGenome() {return genome;}
    score_t & GetScore() {return score;}
    optimal_t & GetOptimal() {return optimal;}
    // get const aggregate fitness
    double GetAggregate() const {emp_assert(aggregated); return agg_score;}
    // get clone bool
    const bool & GetClone() const {return clone;}
    // get optimal
    const size_t & GetCount() const {emp_assert(counted); return count;}


    ///< setters

    // set score vector (recieved from problem.h in world.h)
    void SetScore(const score_t & s_) {score.resize(s_.size()); std::copy(s_.begin(), s_.end(), score.begin());}


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
    */
    void Inherit();

    /**
     * Me Clone function:
     *
     * Will set the clone variable to true.
    */
    void MeClone() {clone = true;}

  private:
    // organism genome vector
    genome_t genome;
    // organism score vector
    score_t score;
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
    size_t M;
    // Are we a clone?
    bool clone = false;

};

double Org::AggregateScore()
{
  //quick checks
  emp_assert(!aggregated);
  emp_assert(score.size() == M, score.size());

  // calculate the aggregate score
  agg_score = std::accumulate(score.begin(), score.end(), START);
  aggregated = true;

  return agg_score;
}

size_t Org::CountOptimized()
{
  //quick checks
  emp_assert(!counted);
  emp_assert(optimal.size() == M, optimal.size());

  // calculate total optimal genes
  count = std::accumulate(optimal.begin(), optimal.end(), START);
  counted = true;

  return count;
}

#endif