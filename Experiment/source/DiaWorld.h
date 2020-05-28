/// This is the world for DiaOrgs

// Hello World

/* TODO SECTION
 * [X]: Finished coding
 * [P]: Process of coding
 * []: Nothing done yet
 *
 * SELECTION SCHEME:
 * []: (μ,λ)
 * []: Tournament
 * []: Fitness Sharing
 * []: Novelty Search
 * []: Epsilon-Lexicae
 * []: Age Layered Population Structure (ALPS)
 *
 * DIAGNOSTICS:
 * []: Exploitation
 * []: Structured Exploitation
 * []: Contradictory Traits Ecologoy
 * []: Exploitation
 *
 * PERFORMANCE METRICS:
 *
 * Individual Level:
 * []: Optimized Trait Count
 * []: Fitness Aggregate
 * []: Average Trait Fitness
 *
 * Population Level:
 * []: Average Optimized Trait Count
 * []: Maximum Optimized Trait Count
 * []: Unique Optimized Trait Count
 * []: Fitness Aggregate Average
 * []: Optimial Solution Count
 *
 * Solutions Of Interest
 *
 * Solution With Maximum Fitness
 * []: Record Everything From Individual Level
 *
 * Solution With Maximum Optimized Trait Count
 * []: Record Everything From Individual Level
 *
 * Most Common Solution
 * []: Record Everything From Individual Level
 *
 * DIVERSITY METRICS:
 * []: Emperical Systematics Tracker
 * []: Loss in Diversity
 * []: Selection Presure
 * []: Selection Variance
*/

#ifndef DIA_WORLD_H
#define DIA_WORLD_H

///< standard headers
#include <functional>
#include <utility>
#include <cmath>
#include <fstream>
#include <string>

///< empirical headers
#include "Evolve/World.h"
#include "base/vector.h"

///< experiment headers
#include "config.h"
#include "DiaOrg.h"

class DiaWorld : public emp::World<DiaOrg> {

public:

  // typename that will hold vector ids
  using ids_t = emp::vector<size_t>;
  // typname for a function that will return the aggreagte fitness
  using fit_agg_t = std::function<double(DiaOrg &)>;
  // typname for a function that will return individual trait fitness
  using fit_lex_t = std::function<double(DiaOrg &, size_t)>;
  // typname for a function that calculates solution perforance on given diagnostic
  using eva_fun_t = std::function<void(DiaOrg &)>;
  // typname for a function that sets the selection scheme
  using sel_fun_t = std::function<ids_t()>;
  // typname for a vector of target objectives
  using tar_t = emp::vector<double>;

public:
  DiaWorld(DiaConfig & _config) : config(_config)
  {
    // initialize ids, targets,  & rng
    for(size_t i = 0; i < config.POP_SIZE(); i++) { solution_id.push_back(i); }
    for(size_t i = 0; i < config.TRAIT_CNT(); i++) { trait_id.push_back(i); }
    random_ptr = emp::NewPtr<emp::Random>(config.SEED());
    target.resize(config.TRAIT_CNT(), config.TARGET());

    InitSetup();

  }

  /* functions for initial set up */

  // call all other functions that initialize
  void InitSetup();
  // set the OnUpdate function from base class.
  void SetOnUpdate();
  // set the mutation operator
  void SetMutation();
  // set the selection scheme
  void SetSelection();
  // set what to do when offspring is ready to go
  void SetOnOffspringReady();
  // set how we are evaluating the orgs
  void SetEvaluation();
  // initialize the world
  void InitializeWorld();

  /* functions that represent the steps for an evolutionary algorithm */

  // evaluate all solutions with diagnostic
  void Evaluate();
  // select parents to reproduce
  ids_t Selection();
  // mutate all offspring
  size_t Mutate();
  //
  void Births(const ids_t & parents);

private:

  // configuration of the experiment we are working with
  DiaConfig & config;
  // short cut to correctly-typed systematics manager, base class will be responsible for memory management
  // emp::Ptr<systematics_t> sys_ptr;
  // vector to hold solution ids from the world
  ids_t solution_id;
  // vector to hold trait ids
  ids_t trait_id;
  // lambda that returns aggregate fitness
  fit_agg_t fitness_agg;
  // lambda that returns individual trait fitness
  fit_lex_t fitness_lex;
  // lambda that returns vector of parent winnder ids
  sel_fun_t selection_fun;
  // lambda that calculates solution performance on diagnostic
  eva_fun_t evaluate_fun;
  // vector that holds targets
  tar_t target;
};

  // call all other functions that initialize
  void DiaWorld::InitSetup()
  {
    // reset the world
    Reset();
    // remove orgs when doing births
    SetPopStruct_Mixed(true);
    SetMutation();


  }

  // set the OnUpdate function from base class.
  void DiaWorld::SetOnUpdate()
  {

  }

  // set the mutation operator
  void DiaWorld::SetMutation()
  {
    // Set up the worlds mutation function!
  SetMutFun([this](DiaOrg & org, emp::Random & random) {
    // // Get org genome!
    // auto & genome = org.GetGenome();
    // size_t cnt = 0;
    // // Check that genome is correct length
    // emp_assert(genome.size() == config.K_TRAITS(), genome.size());

    // // Loop through and apply mutations if applicable
    // for(size_t i = 0; i < genome.size(); i++) {
    //   // Can we do a point offset mutation?
    //   if(random_ptr->P(config.MUTATE_VAL())) {
    //     // Get mutation and see if we need to cap :P
    //     double evo = random_ptr->GetRandNormal(config.MEAN(), config.STD());

    //     // Check if we are over
    //     if(genome[i] + evo > config.TARGET()) {
    //       genome[i] = config.TARGET();
    //     }
    //     // Check if we are under
    //     else if (genome[i] + evo < 0.0) {
    //       genome[i] = 0.0;
    //     }
    //     else {
    //       genome[i] += evo;
    //     }
    //     cnt += 1;
    //   }
    // }

    // return cnt;
  });

  }

  // set the selection scheme
  void DiaWorld::SetSelection()
  {

  }

  // set what to do when offspring is ready to go
  void DiaWorld::SetOnOffspringReady()
  {

  }

  // set how we are evaluating the orgs
  void DiaWorld::SetEvaluation()
  {

  }

  // initialize the world
  void DiaWorld::InitializeWorld()
  {

  }


#endif
