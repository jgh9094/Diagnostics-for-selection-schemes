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
#include "dia-config.h"
#include "DiaOrg.h"
#include "dia-problems.h"
#include "dia-selection.h"

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
  DiaWorld(DiaConfig & _config) : config(_config), elite(_config.POP_SIZE()), trait(_config.POP_SIZE()),
                                  common(_config.POP_SIZE()), com_prop(-1.0), data_file("data.csv")
  {
    solution_id.resize(config.POP_SIZE());
    trait_id.resize(config.TRAIT_CNT());
    // initialize ids, targets,  & rng
    for(size_t i = 0; i < config.POP_SIZE(); i++) { solution_id[i] = i; }
    for(size_t i = 0; i < config.TRAIT_CNT(); i++) { trait_id[i] = i; }
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
  ids_t SelectionScheme();
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
  // set the id for the most common solution
  void FindCommonOrg();
  // clear and reset everything each generation
  void DataReset();

  /* helper functions for evaluation step */

  // what evaluatation are we using?
  void Exploitation();
  void Structured();
  void Contradiction();
  void Exploration();

  /* setters and getters */

  // set the vector of parents that we track data on
  void SetParent(const ids_t & parents)
  {
    emp_assert(parent_id.size() == 0, parent_id.size());
    emp_assert(parents.size() == config.POP_SIZE(), parents.size());

    parent_id.resize(config.POP_SIZE());
    for(size_t i = 0; i < config.POP_SIZE(); ++i) {parent_id[i] = parents[i];}
  }

  // get the vector of parents
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
  // position of the best performing org in the world
  size_t elite;
  // position of the best trait optimized org in the world
  size_t trait;
  // position of the most common solution in the world
  size_t common;
  // proportion of the population that the common org consumes
  double com_prop;

  // dummy
  Diagnostic dia;
  Selection select;



  ///< diagnostic related variables
  enum class DIAGNOSTIC {EXPLOITATION, STRUCTURED, CONTRADICTION, EXPLORATION};

  ///< data file & node related variables

  // file we are working with
  emp::DataFile data_file;
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


  std::cerr << "==========================================" << std::endl;
  std::cerr << "BEGINNING INITIAL SETUP" << std::endl;
  std::cerr << "==========================================" << std::endl;

  // stuff we need to initialize for the experiment
  SetMutation(); // DONE
  SetEvaluation(); // DONE
  SetOnUpdate(); // DONE
  SetDataTracking(); // Done
  SetSelection(); // TODO: make seperate class
  SetOnOffspringReady(); // DONE
  InitializeWorld(); // TODO: add starting orgs

  std::cerr << "==========================================" << std::endl;
  std::cerr << "FINISHED INITIAL SETUP" << std::endl;
  std::cerr << "==========================================" << std::endl;
}

// set the OnUpdate function from base class.
void DiaWorld::SetOnUpdate()
{
  OnUpdate([this](size_t gen)
  {
    // 0) reset everything in the data nodes
    DataReset();
    // 1) evaluate all organisms (DONE)
    Evaluate();
    // 2) select for parents (TODO NEW CLASS)
    const auto parents = SelectionScheme();
    // 3) generate + write all data
    std::cout << "gen data" << std::endl;
    GenerateDate(parents);
    std::cout << "gen data done" << std::endl;
    // Give birth to the next gen & mutate
    Births(parents);
  });
}

// set the mutation operator
void DiaWorld::SetMutation()
{
    // prints
  std::cerr << "Setting mutations function... " << std::endl;

  // set up the worlds mutation function!
  SetMutFun([this](DiaOrg & org, emp::Random & random) {
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
  std::cerr << "------------------------------------------" << std::endl;
  std::cerr << "Setting selection scheme... " << std::endl;


  std::cerr << "TODO" << std::endl;

  std::cerr << "Selection scheme complete!" << std::endl;
}

// set what to do when offspring is ready to go
void DiaWorld::SetOnOffspringReady()
{
  // prints
  std::cerr << "------------------------------------------" << std::endl;
  std::cerr << "Setting OnOffspringReady function... " << std::endl;

  OnOffspringReady([this](DiaOrg & org, size_t parent_pos)
  {
    // quick checks
    emp_assert(fun_do_mutations);  emp_assert(random_ptr);

    size_t mut_cnt = fun_do_mutations(org, *random_ptr);

    // if no mutations we can pass everything from parent to child
    if(mut_cnt == 0)
    {
      // get everything from parent and give to child
      auto & p = *pop[parent_pos];
      auto agg = p.AggregateFitness();
      auto & per = p.GetPheno();
      auto m = config.TRAIT_CNT();
      auto trt = p.OptimizedCount();

      org.Inherit(agg, per, m, trt);
    }
    // else we do a hard reset so that everything can be recomputed
    else {org.Reset(config.TRAIT_CNT());}
  });

  std::cerr << "OnOffspringReady function complete!" << std::endl;
}

// set how we are evaluating the orgs
void DiaWorld::SetEvaluation()
{
  // prints
  std::cerr << "------------------------------------------" << std::endl;
  std::cerr << "Setting the evaluation criteria..." << std::endl;
  std::cerr << "Diagnostic Selected=";

  switch (config.DIAGNOSTIC())
  {
  case int(DIAGNOSTIC::EXPLOITATION):
    Exploitation(); // done
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
  std::cerr << "------------------------------------------" << std::endl;
  std::cerr << "Setting data tracking..." << std::endl;

  // set the default fitness fucntion for file
  SetFitFun([this](DiaOrg & org)
  {
    return org.AggregateFitness();
  });

  // create data.csv file
  // auto data_file = SetupFile(FILE_NAME);

  // add extra stuff for file to track in regard to population data
  auto node_fits = AddDataNode(NODE_FITS);
  data_file.AddVariance(*node_fits, "pop_fit_var", "Variance for fitnesses in a population");
  data_file.AddStandardDeviation(*node_fits, "pop_fit_std", "Standard deviation in parent fitness.");

  // adding optimized trait data to file: Mean, Max, Min
  auto node_opts = AddDataNode(NODE_OPTS);
  data_file.AddMean(*node_opts, "trt_opt_avg", "Average number of optimized traits in population.");
  data_file.AddMax(*node_opts, "trt_opt_max", "Maximum number of optimized traits in population.");
  data_file.AddMin(*node_opts, "trt_opt_min", "Minimum number of optimized traits in population.");

  // adding parent fitness data to file: Mean, Max, Min
  auto node_pnts = AddDataNode(NODE_PNTS);
  data_file.AddMean(*node_pnts, "pnt_fit_avg", "Average fitness for parents.");
  data_file.AddMax(*node_pnts, "pnt_fit_max", "Maximum fitness for parents.");
  data_file.AddMin(*node_pnts, "pnt_fit_min", "Minimum fitness for parents.");
  data_file.AddVariance(*node_pnts, "pnt_fit_var", "Variance in parent fitness.");
  data_file.AddStandardDeviation(*node_pnts, "pnt_fit_std", "Standard deviation in parent fitness.");

  // number of unique optmized traits
  data_file.AddFun<size_t>([this]()
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
  data_file.AddFun<size_t>([this]()
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
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(parent_id.size() == config.POP_SIZE(), parent_id.size());

    // get data nodes for data
    const auto pop = GetDataNode(NODE_FITS);
    const auto pnt = GetDataNode(NODE_PNTS);

    // checks for the data node
    emp_assert(pop->GetCount() == config.POP_SIZE(), pop->GetCount());
    emp_assert(pnt->GetCount() == config.POP_SIZE(), pnt->GetCount());

    const double num = pnt->GetMean() - pop->GetMean();
    const double dem = pop->GetStandardDeviation();

    if(num == 0.0 || dem == 0.0) {return 0.0;}

    return num / dem;

  }, "sel_pre", "Selection pressure meteric!");

  // selection variance
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(parent_id.size() == config.POP_SIZE(), parent_id.size());

    // get data nodes for data
    const auto pop = GetDataNode(NODE_FITS);
    const auto pnt = GetDataNode(NODE_PNTS);

    // checks for the data node
    emp_assert(pop->GetCount() == config.POP_SIZE(), pop->GetCount());
    emp_assert(pnt->GetCount() == config.POP_SIZE(), pnt->GetCount());

    const double num = pnt->GetVariance();
    const double dem = pop->GetVariance();

    if(num == 0.0 || dem == 0.0) {return 0.0;}

    return num / dem;

  }, "sel_var", "Selection variance meteric!");

  // loss in diversity
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(parent_id.size() == config.POP_SIZE(), parent_id.size());

    // find all unique ids that exist in the parent set
    std::set<size_t> unique;
    for(size_t i = 0; i < parent_id.size(); ++i) {unique.insert(parent_id[i]); std::cout << parent_id[i] << ", " << std::endl;}

    std::cout << "unique.size=" << unique.size() << std::endl;

    return (double) unique.size() / (double) config.POP_SIZE();

  }, "loss_div", "Selection pressure meteric!");

  // max fit org fit agg
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0 <= elite, elite);
    emp_assert(elite < config.POP_SIZE());

    auto & org = *pop[elite];

    return org.AggregateFitness();

  }, "sol_fit_agg", "Maximum fit solution -> aggregate fitness!");

  // max fit org fit avg
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0 <= elite, elite);
    emp_assert(elite < config.POP_SIZE());

    auto & org = *pop[elite];

    return org.AggregateFitness() / (double) config.TRAIT_CNT();

  }, "sol_fit_avg", "Maximum fit solution -> aggregate fitness avg!");

  // max fit org optimized trait count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(0 <= elite, elite);
    emp_assert(elite < config.POP_SIZE());

    auto & org = *pop[elite];

    return org.OptimizedCount();

  }, "sol_fit_trt", "Maximum fit solution -> optimized trait count!");

  // max optimized triat org fit agg
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0 <= trait, trait);
    emp_assert(trait < config.POP_SIZE());

    auto & org = *pop[trait];

    return org.AggregateFitness();

  }, "sol_trt_agg", "Maximum fit solution -> aggregate fitness!");

  // max optimized triat org fit avg
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0 <= trait, trait);
    emp_assert(trait < config.POP_SIZE());

    auto & org = *pop[trait];

    return org.AggregateFitness() / (double) config.TRAIT_CNT();

  }, "sol_trt_avg", "Maximum fit solution -> aggregate fitness avg!");

  // max optimized triat org optimized trait count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(0 <= trait, trait);
    emp_assert(trait < config.POP_SIZE());

    auto & org = *pop[trait];

    return org.OptimizedCount();

  }, "sol_trt_cnt", "Maximum fit solution -> optimized trait count!");

  // common org fit agg
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0 <= common, common);
    emp_assert(common <= config.POP_SIZE());

    auto & org = *pop[common];

    return org.AggregateFitness();

  }, "sol_com_agg", "Common fit solution -> aggregate fitness!");

  // common org fit avg
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0 <= common, common);
    emp_assert(common <= config.POP_SIZE());

    auto & org = *pop[common];

    return org.AggregateFitness() / (double) config.TRAIT_CNT();

  }, "sol_com_avg", "Common fit solution -> average fitness!");

  // common org optimized trait count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(0 <= common, common);
    emp_assert(common <= config.POP_SIZE());

    auto & org = *pop[common];

    return org.OptimizedCount();

  }, "sol_com_trt", "Common fit solution -> optimized trait count!");

  // common org optimized population proportion
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(0.0 <= com_prop, com_prop);
    emp_assert(com_prop <= 1.0, com_prop);

    return com_prop;

  }, "sol_com_prp", "Common fit solution -> population proportion!");


  data_file.PrintHeaderKeys();
  std::cerr << "Data tracking complete!" << std::endl;
}

// initialize the world and insert starting orgs!
void DiaWorld::InitializeWorld()
{
  // prints
  std::cerr << "------------------------------------------" << std::endl;
  std::cerr << "Initializing world..." << std::endl;

  // Fill the workd with requested population size!
  DiaOrg org(config.TRAIT_CNT());
  Inject(org.GetGenome(), config.POP_SIZE());

  std::cerr << "Initialing world complete!" << std::endl;
}


/* functions that represent the steps for an evolutionary algorithm */

// evaluate all solutions with diagnostic
void DiaWorld::Evaluate()
{
  // gotta make sure these hold
  emp_assert(solution_id.size() == config.POP_SIZE());
  emp_assert(trait_id.size() == config.TRAIT_CNT());
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
DiaWorld::ids_t DiaWorld::SelectionScheme()
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
  // Generate the data of each individual org
  GenerateSoloData();
  // place apprpiate data into data nodes
  GeneratePopData(parents);
  // // record all data
  data_file.Update();
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

    // calculate org optimized trait count
    org.CalculateOptimizedTrt(target, config.SOLUTION_THRESH());
  }
}

// insert data into appropiate data nodes [TODO]
void DiaWorld::GeneratePopData(const ids_t & parents)
{
  // quick checks + stroing parent ids for data tracking
  emp_assert(parent_id.size() == 0, parent_id.size());
  SetParent(parents);

  // pop fitness data node + quick checks + tracking elite solution
  const auto pop_node = GetDataNode(NODE_FITS);
  emp_assert(pop_node->GetCount() == 0, pop_node->GetCount());
  double best = -1.0;

  for(size_t i = 0; i < config.POP_SIZE(); ++i)
  {
    auto & org = *pop[i];
    pop_node->Add(org.AggregateFitness());

    // keep track of best performer
    if(org.AggregateFitness() > best) {best = org.AggregateFitness(); elite = i;}
  }

  // parent data node + quick checks
  const auto pnt_node = GetDataNode(NODE_PNTS);
  emp_assert(pnt_node->GetCount() == 0, pnt_node->GetCount());

  for(size_t i = 0; i < config.POP_SIZE(); ++i)
  {
    auto & org = *pop[parents[i]];
    pnt_node->Add(org.AggregateFitness());
  }

  // pop optimal data node + quick checks + tracking optimal solution
  const auto opti_node = GetDataNode(NODE_OPTS);
  emp_assert(opti_node->GetCount() == 0, opti_node->GetCount());
  size_t bestt = 0; trait = 0;

  for(size_t i = 0; i < config.POP_SIZE(); ++i)
  {
    auto & org = *pop[i];
    opti_node->Add(org.OptimizedCount());

    // keep track of best optimal
    if(org.OptimizedCount() > bestt) {bestt = org.OptimizedCount(); trait = i;}
  }

  // common org tracking
  FindCommonOrg();
}

// set the id for the most common solution
void DiaWorld::FindCommonOrg()
{
  // quick checks
  emp_assert(pop.size() == config.POP_SIZE(), pop.size());

  // container to hold our collection
  emp::vector<emp::vector<size_t>> groups;

  // add first org
  emp::vector<size_t> first = {0};
  groups.push_back(first);

  // loop through world
  for(size_t i = 1; i < config.POP_SIZE(); ++i)
  {
    bool grouped = false;
    auto & cur_org = *pop[i];

    // loop through groups
    for(size_t j = 0; j < groups.size(); ++j)
    {
      auto & grp_org = *pop[groups[j][0]];

      // check if there is a match & if yes store id in group
      if(cur_org.GetGenome() == grp_org.GetGenome())
      {
        groups[j].push_back(i);
        grouped = true;
        break;
      }
    }

    if(!grouped)
    {
      emp::vector<size_t> neww;
      neww.push_back(i);
      groups.push_back(neww);
    }
  }

  // now all groups should be set
  size_t check = 0;
  size_t biggest = 0;
  size_t index = 0;

  for(size_t i = 0; i < groups.size(); ++i)
  {
    if(biggest < groups[i].size())
    {
      biggest = groups[i].size();
      index = i;
    }

    check += groups[i].size();
  }

  // quick checks
  emp_assert(check == config.POP_SIZE(), check);

  // set index of common org + proportion of pop it has
  common = groups[index][0];
  com_prop = (double) groups[index][0] / (double) config.POP_SIZE();
}

// clear and reset everything each generation
void DiaWorld::DataReset()
{
  // get file and reset all of its nodes
  GetDataNode(NODE_FITS)->Reset();
  GetDataNode(NODE_OPTS)->Reset();
  GetDataNode(NODE_PNTS)->Reset();

  // reset all variables for the elite org
  elite = config.POP_SIZE();
  trait = config.POP_SIZE();
  common = config.POP_SIZE();
  parent_id.clear();
  com_prop = -1.0;
}

/* helper functions for evaluation step */

// what evaluate are we using?
void DiaWorld::Exploitation()
{
  // what are we doing?
  std::cerr << "Exploration" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org) {org.Exploitation(target);};
}

void DiaWorld::Structured()
{
  // what are we doing?
  std::cerr << "Structured Exploitation" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org) {org.Structured(target);};
}

void DiaWorld::Contradiction()
{
  // what are we doing?
  std::cerr << "Contradictory Traits" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org) {org.Contradiction(target);};
}

void DiaWorld::Exploration()
{
  // what are we doing?
  std::cerr << "Exploration" << std::endl;

  // set up the right evaluation function per org
  evaluate_fun = [this](DiaOrg & org) {org.Exploration(target);};
}


#endif