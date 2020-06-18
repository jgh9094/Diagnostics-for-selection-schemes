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
#include <bits/stdc++.h>
#include <set>

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
  // typename for vector of bools
  using opti_t = emp::vector<bool>;

public:
  DiaWorld(DiaConfig & _config) : config(_config)
  {
    // initialize ids, targets,  & rng
    for(size_t i = 0; i < config.POP_SIZE(); i++) { solution_id.push_back(i); }
    for(size_t i = 0; i < config.TRAIT_CNT(); i++) { trait_id.push_back(i); }
    random_ptr = emp::NewPtr<emp::Random>(config.SEED());
    target.resize(config.TRAIT_CNT(), config.TARGET());
    unique_opti.resize(config.TRAIT_CNT(), false);

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
  // set data nodes and file for data recording
  void SetDataTracking();
  // initialize the world
  void InitializeWorld();

  /* functions that represent the steps for an evolutionary algorithm */

  // evaluate all solutions with diagnostic
  void Evaluate();
  // select parents to reproduce
  ids_t Selection();
  // mutate all offspring
  size_t Mutate();
  // do births for the parent ids we have
  void Births(const ids_t & parents);

  /* helper functions for evolutionary algorithm */

  // generate all data
  void GenerateDate(const ids_t & parents);
  // generate data we need per org
  void GenerateSoloData();
  // generate data from population
  void GeneratePopData(const ids_t & parents);

  /* helper functions for evaluation step */

  // what evaluate are we using?
  void Exploitation();
  void Structured();
  void Contradiction();
  void Exploration();

  /* setters and getters */

  void SetParent(const ids_t & parents)
  {
    emp_assert(parent_id.size() == 0, parent_id.size());
    emp_assert(parents.size() == config.POP_SIZE(), parents.size());

    parent_id.resize(config.POP_SIZE());
    for(size_t i = 0; i < config.POP_SIZE(); ++i) {parent_id[i] = parents[i];}
  }


  const ids_t & GetParent() const {emp_assert(parent_id.size() != 0, parent_id.size()); return parent_id;}


private:

  // configuration of the experiment we are working with
  DiaConfig & config;
  // short cut to correctly-typed systematics manager, base class will be responsible for memory management
  // emp::Ptr<systematics_t> sys_ptr;
  // vector to hold solution ids from the world
  ids_t solution_id;
  // vector to hold parent ids for selection
  ids_t parent_id;
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
  // vector that holds unique optimized traits
  opti_t unique_opti;

  ///< diagnostic related variables
  enum class DIAGNOSTIC {EXPLOITATION, STRUCTURED, CONTRADICTION, EXPLORATION};

  ///< data file & node related variables

  // name of data file
  std::string FILE_NAME = "data.csv";
  // name of node for current population fitness data
  std::string NODE_FITS = "fitness";
  // name of node for current population traits data
  std::string NODE_OPTS = "optimize";
  // name of node for current parents fitness data
  std::string NODE_PNTS = "parents";
};

// call all other functions that initialize
void DiaWorld::InitSetup()
{
  // reset the world
  Reset();
  // remove orgs when doing births
  SetPopStruct_Mixed(true);

  // stuff we need to initialize for the experiment
  SetMutation();
  SetEvaluation();
  SetOnUpdate();
  SetDataTracking();
}

// set the OnUpdate function from base class.
void DiaWorld::SetOnUpdate()
{
  OnUpdate([this](size_t gen) {
    // 1) evaluate all organisms
    Evaluate();
    // 2) select for parents
    const auto parents = Selection();
    // 3) generate all data
    GenerateDate(parents);
    // Give birth to the next gen & mutate
    Births(parents);
  });
}

// set the mutation operator
void DiaWorld::SetMutation()
{
  // set up the worlds mutation function!
  SetMutFun([this](DiaOrg & org, emp::Random & random) {

    // prints
    std::cerr << "Setting mutations function... " << std::endl;

    // Get org genome!
    auto & genome = org.GetGenome();
    size_t cnt = 0;

    // check that genome is correct length
    emp_assert(genome.size() == config.TRAIT_CNT(), genome.size());

    // loop through and apply mutations if applicable
    for(size_t i = 0; i < genome.size(); i++)
    {
      if(random_ptr->P(config.MUTATE_PER()))
      {
        // get mutation and see if we need to wrap around
        double evo = random_ptr->GetRandNormal(config.MEAN(), config.STD());

        // check if over or under allowed range
        if(genome[i] + evo > config.TARGET())
        {
          genome[i] = config.TARGET() - (genome[i] + evo - config.TARGET());
        }
        else if (genome[i] + evo < 0.0)
        {
          genome[i] = 0.0;
        }
        else
        {
          genome[i] += evo;
        }
        cnt += 1;
      }
    }

    return cnt;
  });

  std::cerr << "Mutations set!" << std::endl;
}

// set the selection scheme
void DiaWorld::SetSelection()
{
  // prints
  std::cerr << "Setting selection scheme... " << std::endl;




  std::cerr << "Selection scheme complete!" << std::endl;
}

// set what to do when offspring is ready to go
void DiaWorld::SetOnOffspringReady()
{
  // prints
  std::cerr << "Setting OnOffspringReady function... " << std::endl;


  std::cerr << "OnOffspringReady function complete!" << std::endl;
}

// set how we are evaluating the orgs
void DiaWorld::SetEvaluation()
{
  // prints
  std::cerr << "Setting the evaluation criteria..." << std::endl;
  std::cerr << "Diagnostic Selected=";

  switch (config.DIAGNOSTIC())
  {
  case int(DIAGNOSTIC::EXPLOITATION):
    Exploitation();
    break;

  case int(DIAGNOSTIC::STRUCTURED):
    Structured();
    break;

  case int(DIAGNOSTIC::CONTRADICTION):
    Contradiction();
    break;

  case int(DIAGNOSTIC::EXPLORATION):
    Exploration();
    break;

  default:
    std::cerr << "UNKNOWN DIAGNOSTIC" << std::endl;
    exit(-1);
    break;
  }

  std::cerr << "Evalation criteria complete!" << std::endl;
}

// set data nodes and file for data recording
void DiaWorld::SetDataTracking()
{
  // prints
  std::cerr << "Setting data tracking..." << std::endl;

  // create data.csv file
  auto& file = SetupFitnessFile(FILE_NAME);

  // add extra stuff for file to track in regard to population data
  auto node_fits = GetDataNode(NODE_FITS);
  file.AddVariance(*node_fits, "pop_fit_var", "Variance for fitnesses in a population");
  file.AddStandardDeviation(*node_fits, "pop_fit_std", "Standard deviation in parent fitness.");

  // adding optimized trait data to file: Mean, Max, Min
  auto node_opts = AddDataNode(NODE_OPTS);
  file.AddMean(*node_opts, "trt_opt_avg", "Average number of optimized traits in population.");
  file.AddMax(*node_opts, "trt_opt_max", "Maximum number of optimized traits in population.");
  file.AddMin(*node_opts, "trt_opt_min", "Minimum number of optimized traits in population.");

  // adding parent fitness data to file: Mean, Max, Min
  auto node_pnts = AddDataNode(NODE_PNTS);
  file.AddMean(*node_pnts, "pnt_fit_avg", "Average fitness for parents.");
  file.AddMax(*node_pnts, "pnt_fit_max", "Maximum fitness for parents.");
  file.AddMin(*node_pnts, "pnt_fit_min", "Minimum fitness for parents.");
  file.AddVariance(*node_pnts, "pnt_fit_var", "Variance in parent fitness.");
  file.AddStandardDeviation(*node_pnts, "pnt_fit_std", "Standard deviation in parent fitness.");

  // number of unique optmized traits
  file.AddFun<size_t>([this]()
  {
    // quick checking
    emp_assert(target.size() == config.TRAIT_CNT(), target.size());
    emp_assert(pop.size() == config.POP_SIZE(), pop.size());

    size_t cnt = 0;

    // loop through all traits and see if optimized
    for(size_t trt = 0; trt < config.TRAIT_CNT(); ++trt)
    {
      bool opti = false;

      // loop through orgs and see if they have the trait optimized
      for(size_t id = 0; id < config.POP_SIZE(); ++id)
      {
        auto & org = *pop[id];

        // check if org has the trait optimized, if yes we done here
        if(org.Optimized(trt, target[trt], target[trt]*config.SOLUTION_THRESH()))
        {
          opti = true;
          break;
        }
      }

      if(opti) {cnt++;}
    }

    return cnt;
  }, "pop_uni_trt", "Number of unique optimized traits per generation!");

  // number of opimal solutions in the population
  file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(pop.size() == config.POP_SIZE(), pop.size());

    size_t cnt = 0;

    // loop through pop
    for(size_t i = 0; i < config.POP_SIZE(); ++i)
    {
      auto & org = *pop[i];

      if(org.OptimizedCount() == config.TRAIT_CNT()) {cnt++;}
    }


    return cnt;
  }, "pop_sol_opt", "Number of optimal solutions in the population!");

  // selection pressure
  file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(parent_id.size() == config.POP_SIZE(), parent_id.size());

    // get data nodes for data
    const auto pop = GetDataNode(NODE_FITS);
    const auto pnt = GetDataNode(NODE_PNTS);

    const double num = pnt->GetMean() - pop->GetMean();
    const double dem = pop->GetStandardDeviation();

    if(num == 0.0 || dem == 0.0) {return 0.0;}

    return num / dem;

  }, "sel_pre", "Selection pressure meteric!");

  // selection variance
  file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(parent_id.size() == config.POP_SIZE(), parent_id.size());

    // get data nodes for data
    const auto pop = GetDataNode(NODE_FITS);
    const auto pnt = GetDataNode(NODE_PNTS);

    const double num = pnt->GetVariance();
    const double dem = pop->GetVariance();

    if(num == 0.0 || dem == 0.0) {return 0.0;}

    return num / dem;

  }, "sel_var", "Selection variance meteric!");

  // loss in diversity
  file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(parent_id.size() == config.POP_SIZE(), parent_id.size());

    // find all unique ids that exist in the parent set
    std::set<size_t> unique;
    for(size_t i = 0; i < parent_id.size(); ++i) {unique.insert(parent_id[i]);}

    const double num = unique.size();
    const double dem = config.POP_SIZE();

    return num / dem;

  }, "loss_div", "Selection pressure meteric!");

  std::cerr << "Data tracking complete!" << std::endl;
}

// initialize the world
void DiaWorld::InitializeWorld()
{

}


/* functions that represent the steps for an evolutionary algorithm */

// evaluate all solutions with diagnostic
void DiaWorld::Evaluate()
{
  // gotta make sure these hold
  emp_assert(solution_id == config.TRAIT_CNT());
  emp_assert(trait_id == config.TRAIT_CNT());
  emp_assert(pop.size() == config.POP_SIZE());

  // loop through pop and have them score their performance
  for(size_t i = 0; i < config.POP_SIZE(); ++i)
  {
    auto & org = *pop[i];

    // if this org is same as parent continue re-evaluate erything
    if(!org.GetClone())
    {
      // reset eryting and evaluate
      org.Reset(config.TRAIT_CNT());
      evaluate_fun(org);
    }
  }
}

// select parents to reproduce
DiaWorld::ids_t DiaWorld::Selection()
{


  return solution_id;
}

// mutate all offspring
size_t DiaWorld::Mutate()
{

  return 0;
}

// do births for the parent ids we have
void DiaWorld::Births(const ids_t & parents)
{

}

/* helper functions for evolutionary algorithm */

// generate all data
void DiaWorld::GenerateDate(const ids_t & parents)
{
  // generate data per organism
  GenerateSoloData();
  // reset all things [TODO]
  // place apprpiate data into data nodes
  GeneratePopData(parents);
  // systematic manager [TODO]
}

// generate data we need per org
void DiaWorld::GenerateSoloData()
{
  // loop thro all orgs, aggreagate performance and determin trait optimality
  for(size_t i = 0; i < config.POP_SIZE(); ++i)
  {
    auto & org = *pop[i];

    // if clone we already have all the info we need
    if (org.GetClone()) { continue; }

    // calculate aggregate fitness
    org.AggregateFitness();

    // go through performace vector and see if a trait is optimized
    for (size_t trt = 0; trt < config.TRAIT_CNT(); ++trt)
    {
      org.Optimized(trt, target[i], target[i]*config.SOLUTION_THRESH());
    }

    // calculate org optimized trait count
    org.OptimizedCount();
  }
}

// insert data into appropiate data nodes [TODO]
void DiaWorld::GeneratePopData(const ids_t & parents)
{
  SetParent(parents);
}


/* helper functions for evaluation step */

// what evaluate are we using?
void DiaWorld::Exploitation()
{
  // what are we doing?
  std::cerr << "Exploration" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org)
                  {
                    org.Exploitation(target);
                  };
}

void DiaWorld::Structured()
{
  // what are we doing?
  std::cerr << "Structured Exploitation" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org)
                  {
                    org.Structured(target);
                  };
}

void DiaWorld::Contradiction()
{
  // what are we doing?
  std::cerr << "Contradictory Traits" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org)
                  {
                    org.Contradiction(target);
                  };
}

void DiaWorld::Exploration()
{
  // what are we doing?
  std::cerr << "Exploration" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org)
                  {
                    org.Exploration(target);
                  };
}


#endif