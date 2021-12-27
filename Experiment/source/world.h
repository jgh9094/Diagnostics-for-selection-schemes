/// World that will manage solutions during the evolutionary run

#ifndef DIA_WORLD_H
#define DIA_WORLD_H

///< standard headers
#include <functional>
#include <map>
#include <set>
#include <fstream>
#include <string.h>
#include <set>
#include <string>
#include <algorithm>
#include <cmath>

///< empirical headers
#include "Evolve/World.h"

///< experiment headers
#include "config.h"
#include "org.h"
#include "problem.h"
#include "selection.h"

class DiagWorld : public emp::World<Org>
{
  // object types for consistency between working class
  public:
    ///< Org related

    // solution genome + diagnotic problem types
    using genome_t = emp::vector<double>;
    // score vector for a solution
    using score_t = emp::vector<double>;
    // boolean optimal vector per objective
    using optimal_t = emp::vector<bool>;
    // target vector type
    using target_t = emp::vector<double>;

    ///< selection related types

    // vector of position ids
    using ids_t = emp::vector<size_t>;
    // matrix of population score vectors
    using fmatrix_t = emp::vector<score_t>;
    // matrix of population genomes
    using gmatrix_t = emp::vector<genome_t>;
    // map holding population id groupings by fitness (keys in decending order)
    using fitgp_t = std::map<double, ids_t, std::greater<double>>;
    // vector of double vectors for K neighborhoods
    using neigh_t = emp::vector<score_t>;

    ///< world related types

    // evaluation function type
    using eval_t = std::function<double(Org &)>;
    // selection function type
    using sele_t = std::function<ids_t()>;

    ///< data tracking stuff (ask about)
    using nodef_t = emp::Ptr<emp::DataMonitor<double>>;
    using nodeo_t = emp::Ptr<emp::DataMonitor<size_t>>;
    using como_t = std::map<size_t, ids_t>;

  public:

    DiagWorld(DiaConfig & _config) : config(_config), data_file(_config.OUTPUT_DIR() + "data.csv")
    {
      // set random pointer seed
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());

      // initialize the world
      Initialize();
    }

    ~DiagWorld()
    {
      selection.Delete();
      diagnostic.Delete();
      pop_fit.Delete();
      pop_opti.Delete();
      pnt_fit.Delete();
      pnt_opti.Delete();
    }

    ///< functions called to setup the world

    // call all functions to initiallize the world
    void Initialize();

    // set OnUpdate function from World.h
    void SetOnUpdate();

    // set mutation operator from World.h
    void SetMutation();

    // set selction scheme
    void SetSelection();

    // set what to do when offspring is ready to go
    void SetOnOffspringReady();

    // set evaluation function
    void SetEvaluation();

    // set data tracking with data nodes
    void SetDataTracking();

    // populate the world with initial solutions
    void PopulateWorld();


    ///< principle steps during an evolutionary run

    // reset all data step
    void ResetData();

    // evaluation step
    void EvaluationStep();

    // selction step
    void SelectionStep();

    // reprodutive step
    void ReproductionStep();

    // record data step
    void RecordData();


    ///< selection scheme implementations

    void MuLambda();

    void Tournament();

    void FitnessSharing();

    void NoveltyAggregate();

    void NoveltyEuclidean();

    void EpsilonLexicase();


    ///< evaluation function implementations

    void Exploitation();

    void StructuredExploitation();

    void StrongEcology();

    void Exploration();

    void WeakEcology();


    ///< data tracking

    size_t UniqueObjective();

    size_t FindElite();

    size_t FindCommon();

    size_t FindOptimized();

    size_t FindUniqueStart();

    size_t FindStreak();

    // void SnapshotPhylogony();


    ///< helper functions

    // create a matrix of popultion score vectors
    fmatrix_t PopFitMat();

    // create matrix of population genomes
    gmatrix_t PopGenomes();


  private:
    // experiment configurations
    DiaConfig & config;

    // target vector
    target_t target;
    // vector holding population aggregate scores (by position id)
    score_t fit_vec;
    // vector holding parent solutions selected by selection scheme
    ids_t parent_vec;


    // evaluation lambda we set
    eval_t evaluate;
    // selection lambda we set
    sele_t select;


    // select.h var
    emp::Ptr<Selection> selection;
    // problem.h var
    emp::Ptr<Diagnostic> diagnostic;

    ///< data file & node related variables

    // file we are working with
    emp::DataFile data_file;
    // systematics tracking
    // emp::Ptr<systematics_t> sys_ptr;
    // node to track population fitnesses
    nodef_t pop_fit;
    // node to track population opitmized count
    nodeo_t pop_opti;
    // node to track parent fitnesses
    nodef_t pnt_fit;
    // node to track parent optimized count
    nodeo_t pnt_opti;
    // node to track streak counts
    nodeo_t pop_str;
    // csv file to track best performing solutions
    std::ofstream elite_csv;

    ///< data we are tracking during an evolutionary run

    // elite solution position
    size_t elite_pos;
    // common solution position
    size_t comm_pos;
    // optimal solution position
    size_t opti_pos;
    // streak solution position
    size_t strk_pos;
    // common solution dictionary
    como_t common;
};

///< functions called to setup the world

void DiagWorld::Initialize()
{
  std::cout << "==========================================" << std::endl;
  std::cout << "BEGINNING INITIAL SETUP" << std::endl;
  std::cout << "==========================================" << std::endl;

  // reset the world upon start
  Reset();
  // set world to well mixed so we don't over populate
  SetPopStruct_Mixed(true);


  // stuff we need to initialize for the experiment
  SetEvaluation();
  SetMutation();
  SetOnUpdate();
  SetDataTracking();
  SetSelection();
  SetOnOffspringReady();
  PopulateWorld();

  std::cout << "==========================================" << std::endl;
  std::cout << "FINISHED INITIAL SETUP" << std::endl;
  std::cout << "==========================================" << std::endl;
}

void DiagWorld::SetOnUpdate()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting OnUpdate function..." << std::endl;

  // set up the evolutionary algorithm
  OnUpdate([this](size_t gen)
  {
    // step 0: reset all data collection variables
    ResetData();

    // step 1: evaluate all solutions on diagnostic
    EvaluationStep();

    // take a snapshot if nessecaryn (ask if appropiate place to take snapshot)
    // if(GetUpdate() == config.MAX_GENS() - 1){SnapshotPhylogony();}

    // step 2: select parent solutions for
    SelectionStep();

    // step 3: gather and record data
    RecordData();

    // step 4: reproduce and create new solutions
    ReproductionStep();
  });

  std::cout << "Finished setting the OnUpdate function! \n" << std::endl;
}

void DiagWorld::SetMutation()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting mutation function..." << std::endl;

  // set the mutation function
  SetMutFun([this](Org & org, emp::Random & random)
  {
    // number of mutations and solution genome
    size_t mcnt = 0;
    genome_t & genome = org.GetGenome();

    // quick checks
    emp_assert(genome.size() == config.OBJECTIVE_CNT());
    emp_assert(target.size() == config.OBJECTIVE_CNT());

    for(size_t i = 0; i < genome.size(); ++i)
    {
      // if we do a mutation at this objective
      if(random_ptr->P(config.MUTATE_PER()))
      {
        const double mut = random_ptr->GetRandNormal(config.MEAN(), config.STD());

        // mutation puts objective above target
        if(config.UPPER_BND() < genome[i] + mut)
        {
          // we wrap it back around target value
          genome[i] = target[i] - (genome[i] + mut - target[i]);
        }
        // mutation puts objective in the negatives 
        else if(genome[i] + mut < config.LOWER_BND())
        {
          genome[i] = std::abs(genome[i] + mut) + config.LOWER_BND();
        }
        else
        {
          // else we can simply add mutation
          genome[i] = genome[i] + mut;
        }

        ++mcnt;
      }
    }

    return mcnt;
  });

  std::cout << "Mutation function set!\n" << std::endl;
}

void DiagWorld::SetSelection()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting Selection function..." << std::endl;

  selection = emp::NewPtr<Selection>(random_ptr);
  std::cout << "Created selection" << std::endl;

  switch (config.SELECTION())
  {
    case 0: // mu lambda
      MuLambda();
      break;

    case 1: // tournament
      Tournament();
      break;

    case 2: // fitness sharing
      FitnessSharing();
      break;

    case 3: // novelty search
      NoveltyAggregate();
      break;

    case 4: // epsilon lexicase
      EpsilonLexicase();
      break;

    case 5: // novelty euclidean
      NoveltyEuclidean();
      break;

    default:
      std::cout << "ERROR UNKNOWN SELECTION CALL" << std::endl;
      emp_assert(true);
      break;
  }

  std::cout << "Finished setting the Selection function! \n" << std::endl;
}

void DiagWorld::SetOnOffspringReady()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting OnOffspringReady function..." << std::endl;

  OnOffspringReady([this](Org & org, size_t parent_pos)
  {
    // quick checks
    emp_assert(fun_do_mutations); emp_assert(random_ptr);
    emp_assert(org.GetGenome().size() == config.OBJECTIVE_CNT());
    emp_assert(org.GetM() == config.OBJECTIVE_CNT());

    // do mutations on offspring
    size_t mcnt = fun_do_mutations(org, *random_ptr);

    // no mutations were applied to offspring
    if(mcnt == 0)
    {
      Org & parent = *pop[parent_pos];

      // quick checks
      emp_assert(parent.GetGenome().size() == config.OBJECTIVE_CNT());
      emp_assert(parent.GetM() == config.OBJECTIVE_CNT());

      // give everything to offspring from parent
      org.MeClone();
      org.Inherit(parent.GetScore(), parent.GetOptimal(), parent.GetCount(), parent.GetAggregate(), parent.GetStart(), parent.GetStreak());
    }
    else{org.Reset();}
  });

  std::cout << "Finished setting OnOffspringReady function!\n" << std::endl;
}

void DiagWorld::SetEvaluation()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting Evaluation function..." << std::endl;

  target_t tar(config.OBJECTIVE_CNT(), config.TARGET());
  target.clear(); target.resize(config.OBJECTIVE_CNT());
  std::copy(tar.begin(), tar.end(), target.begin());

  diagnostic = emp::NewPtr<Diagnostic>(target, config.CREDIT());
  std::cout << "Created diagnostic emp::Ptr" << std::endl;

  switch (config.DIAGNOSTIC())
  {
    case 0: // exploitation
      Exploitation();
      break;

    case 1: // structured exploitation
      StructuredExploitation();
      break;

    case 2: // strong ecology
      StrongEcology();
      break;

    case 3: // exploration
      Exploration();
      break;

    case 4: // weak ecology
      WeakEcology();
      break;

    default: // error, unknown diganotic
      std::cout << "ERROR: UNKNOWN DIAGNOSTIC" << std::endl;
      emp_assert(true);
      break;
  }

  std::cout << "Evaluation function set!\n" <<std::endl;
}

void DiagWorld::SetDataTracking()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting up data tracking..." << std::endl;

  // initialize all nodes
  std::cout << "Initializing nodes..." << std::endl;
  pop_fit.New(); pop_opti.New(); pnt_fit.New(); pnt_opti.New(); pop_str.New();
  std::cout << "Nodes initialized!" << std::endl;

  // track population aggregate score stats: average, variance, min, max
  data_file.AddMean(*pop_fit, "pop_fit_avg", "Population average aggregate performance.");
  data_file.AddVariance(*pop_fit, "pop_fit_var", "Population variance aggregate performance.");
  data_file.AddMax(*pop_fit, "pop_fit_max", "Population maximum aggregate performance.");
  data_file.AddMin(*pop_fit, "pop_fit_min", "Population minimum aggregate performance.");

  // track population optimized objective count stats: average, variance, min, max
  data_file.AddMean(*pop_opti, "pop_opt_avg", "Population average objective optimization count.");
  data_file.AddVariance(*pop_opti, "pop_opt_var", "Population variance objective optimization count.");
  data_file.AddMax(*pop_opti, "pop_opt_max", "Population maximum objective optimization count.");
  data_file.AddMin(*pop_opti, "pop_opt_min", "Population minimum objective optimization count.");

  // track parent aggregate score stats: average, variance, min, max
  data_file.AddMean(*pnt_fit, "pnt_fit_avg", "Parent average aggregate performance.");
  data_file.AddVariance(*pnt_fit, "pnt_fit_var", "Parent variance aggregate performance.");
  data_file.AddMax(*pnt_fit, "pnt_fit_max", "Parent maximum aggregate performance.");
  data_file.AddMin(*pnt_fit, "pnt_fit_min", "Parent minimum aggregate performance.");

  // track parent optimized objective count stats: average, variance, min, max
  data_file.AddMean(*pnt_opti, "pnt_opt_avg", "Parent average objective optimization count.");
  data_file.AddVariance(*pnt_opti, "pnt_opt_var", "Parent variance objective optimization count.");
  data_file.AddMax(*pnt_opti, "pnt_opt_max", "Parent maximum objective optimization count.");
  data_file.AddMin(*pnt_opti, "pnt_opt_min", "Parent minimum objective optimization count.");

  // track parent optimized objective count stats: average, variance, min, max
  data_file.AddMean(*pop_str, "pop_str_avg", "Population average streak count.");
  data_file.AddVariance(*pop_str, "pop_str_var", "Population variance streak count.");
  data_file.AddMax(*pop_str, "pop_str_max", "Population maximum streak count.");
  data_file.AddMin(*pop_str, "pop_str_min", "Population minimum streak count.");

  std::cout << "Added all data nodes to data file!" << std::endl;

  // update we are at
  data_file.AddFun<size_t>([this]()
  {
    return update;
  }, "gen", "Current generation at!");

  // unique optimized objectives count
  data_file.AddFun<size_t>([this]()
  {
    return UniqueObjective();
  }, "pop_uni_obj", "Number of unique optimized traits per generation!");

  // count of common solution in the population
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(0 < common.size());
    emp_assert(comm_pos != config.POP_SIZE());

    // find iterator to common org
    const auto it = common.find(comm_pos);
    emp_assert(it != common.end());
    emp_assert(0 < it->second.size());

    return it->second.size();
  }, "com_sol_cnt", "Count of genetically common solution!");

  // elite solution aggregate performance
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(elite_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[elite_pos];

    return org.GetAggregate();
  }, "ele_agg_per", "Elite solution aggregate performance!");

  // elite solution optimized objectives count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(elite_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[elite_pos];

    return org.GetCount();
  }, "ele_opt_cnt", "Elite solution optimized objective count!");

  // common solution aggregate performance
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(comm_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[comm_pos];

    return org.GetAggregate();
  }, "com_agg_per", "Common solution aggregate performance!");

  // common solution optimized objectives count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(comm_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[comm_pos];

    return org.GetCount();
  }, "com_opt_cnt", "Common solution optimized objective count!");

  // optimized solution aggregate performance
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(opti_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[opti_pos];

    return org.GetAggregate();
  }, "opt_agg_per", "Otpimal solution aggregate performance");

  // optimized solution optimized objectives count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(opti_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[opti_pos];

    return org.GetCount();
  }, "opt_obj_cnt", "Otpimal solution aggregate performance");

  // streak solution aggregate performance
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(strk_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[strk_pos];

    return org.GetAggregate();
  }, "str_agg_per", "Otpimal solution aggregate performance");

  // streak solution optimized objectives count
  data_file.AddFun<size_t>([this]()
  {
    // quick checks
    emp_assert(strk_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[strk_pos];

    return org.GetCount();
  }, "str_obj_cnt", "Otpimal solution aggregate performance");

  // loss of diversity
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(parent_vec.size() == config.POP_SIZE());

    std::set<size_t> unique;
    for(auto & id : parent_vec) {unique.insert(id);}

    // ask Charles
    const double num = static_cast<double>(unique.size());
    const double dem = static_cast<double>(config.POP_SIZE());

    return num / dem;
  }, "los_div", "Loss in diversity generated by the selection scheme!");

  // selection pressure
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(pop_fit->GetCount() == config.POP_SIZE());
    emp_assert(pnt_fit->GetCount() == config.POP_SIZE());

    const double pop = pop_fit->GetMean();
    const double pnt = pnt_fit->GetMean();
    const double var = pop_fit->GetVariance();

    if(var == 0.0) {return 0.0;}

    return (pop - pnt) / var;
  }, "sel_pre", "Selection pressure applied by selection scheme!");

  // selection variance
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(pop_fit->GetCount() == config.POP_SIZE());
    emp_assert(pnt_fit->GetCount() == config.POP_SIZE());

    const double pop = pop_fit->GetVariance();
    const double pnt = pnt_fit->GetVariance();

    if(pnt == 0.0) {return 0.0;}

    return pop / pnt;
  }, "sel_var", "Selection pressure applied by selection scheme!");

  // unique starting positions
  data_file.AddFun<size_t>([this]()
  {
    return FindUniqueStart();
  }, "uni_str_pos", "Number of unique optimized positions in the population!");

  data_file.PrintHeaderKeys();

  // create elite csv plus headers
  elite_csv.open(config.OUTPUT_DIR() + "elite.csv");

  std::string header = "Gen";
  for(size_t i = 0; i < config.OBJECTIVE_CNT(); ++i)
  {
    header += ",t";
    header += std::to_string(i);
  }
  elite_csv << header << "\n";


  std::cout << "Finished setting data tracking!\n" << std::endl;
}

void DiagWorld::PopulateWorld()
{
  std::cout << "------------------------------------------" << std::endl;
  std::cout << "Populating world with initial solutions..." << std::endl;

  // Fill the workd with requested population size!
  Org org(config.OBJECTIVE_CNT());
  Inject(org.GetGenome(), config.POP_SIZE());

  std::cout << "Initialing world complete!" << std::endl;
}


///< principle steps during an evolutionary run

void DiagWorld::ResetData()
{
  // reset all data nodes
  pop_fit->Reset();
  pop_opti->Reset();
  pnt_fit->Reset();
  pnt_opti->Reset();
  pop_str->Reset();

  // reset all positon ids
  elite_pos = config.POP_SIZE();
  comm_pos = config.POP_SIZE();
  opti_pos = config.POP_SIZE();
  strk_pos = config.POP_SIZE();

  // reset all vectors/maps holding current gen data
  fit_vec.clear();
  parent_vec.clear();
  common.clear();
}

void DiagWorld::EvaluationStep()
{
  // quick checks
  emp_assert(fit_vec.size() == 0); emp_assert(0 < pop.size());
  emp_assert(pop.size() == config.POP_SIZE());

  // iterate through the world and populate fitness vector
  fit_vec.resize(config.POP_SIZE());
  for(size_t i = 0; i < pop.size(); ++i)
  {
    Org & org = *pop[i];

    // no evaluate needed if offspring is a clone
    fit_vec[i] = (org.GetClone()) ? org.GetAggregate() : evaluate(org);
  }
}

void DiagWorld::SelectionStep()
{
  // quick checks
  emp_assert(parent_vec.size() == 0); emp_assert(0 < pop.size());
  emp_assert(pop.size() == config.POP_SIZE());

  // store parents
  auto parents = select();
  emp_assert(parents.size() == config.POP_SIZE());

  parent_vec.resize(config.POP_SIZE());
  std::copy(parents.begin(), parents.end(), parent_vec.begin());
}

void DiagWorld::RecordData()
{
  /// Add data to all nodes

  // get pop data
  emp_assert(pop.size() == config.POP_SIZE());
  for(size_t i = 0; i < pop.size(); ++i)
  {
    Org & org = *pop[i];
    pop_fit->Add(org.GetAggregate());
    pop_opti->Add(org.GetCount());
    pop_str->Add(org.GetStreak());
  }
  emp_assert(pop_fit->GetCount() == config.POP_SIZE());
  emp_assert(pop_opti->GetCount() == config.POP_SIZE());
  emp_assert(pop_str->GetCount() == config.POP_SIZE());

  // get parent data
  emp_assert(parent_vec.size() == config.POP_SIZE());
  for(size_t i = 0; i < parent_vec.size(); ++i)
  {
    Org & org = *pop[parent_vec[i]];
    pnt_fit->Add(org.GetAggregate());
    pnt_opti->Add(org.GetCount());
  }
  emp_assert(pnt_fit->GetCount() == config.POP_SIZE());
  emp_assert(pnt_opti->GetCount() == config.POP_SIZE());

  /// get all position ids

  elite_pos = FindElite();
  emp_assert(elite_pos != config.POP_SIZE());

  comm_pos = FindCommon();
  emp_assert(comm_pos != config.POP_SIZE());

  opti_pos = FindOptimized();
  emp_assert(opti_pos != config.POP_SIZE());

  strk_pos = FindStreak();
  emp_assert(strk_pos != config.POP_SIZE());

  /// fill vectors & map
  emp_assert(fit_vec.size() == config.POP_SIZE()); // should be set already
  emp_assert(parent_vec.size() == config.POP_SIZE()); // should be set already
  emp_assert(0 < common.size());  // should already be set in FindCommon

  /// update the file
  data_file.Update();

  // record elite solution traits
  std::string traits = std::to_string(update);
  Org & ele = *pop[elite_pos];
  const auto & g = ele.GetScore();
  for(size_t i = 0; i < g.size(); ++i)
  {
    traits += ",";
    traits += std::to_string(g[i]);
  }
  elite_csv << traits << "\n";


  // output this so we know where we are in terms of generations and fitness
  Org & org = *pop[elite_pos];
  Org & opt = *pop[opti_pos];
  std::cout << "gen=" << GetUpdate() << ", max_fit=" << org.GetAggregate()  << ", max_opt=" << opt.GetCount() << std::endl;
}

void DiagWorld::ReproductionStep()
{
  // quick checks
  emp_assert(parent_vec.size() == config.POP_SIZE());
  emp_assert(pop.size() == config.POP_SIZE());

  // go through parent ids and do births
  for(auto & id : parent_vec){DoBirth(GetGenomeAt(id), id);}
}


///< selection scheme implementations

void DiagWorld::MuLambda()
{
  std::cout << "Setting selection scheme: MuLambda" << std::endl;

  // set select lambda to mu lambda selection
  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // group population by fitness
    fitgp_t group = selection->FitnessGroup(fit_vec);

    return selection->MLSelect(config.MU(), config.POP_SIZE(), group);
  };

  std::cout << "MuLambda selection scheme set!" << std::endl;
}

void DiagWorld::Tournament()
{
  std::cout << "Setting selection scheme: Tournament" << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // will hold parent ids + get pop agg score values
    ids_t parent(pop.size());

    // get pop size amount of parents
    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->Tournament(config.TOUR_SIZE(), fit_vec);
    }

    return parent;
  };

  std::cout << "Tournament selection scheme set!" << std::endl;
}

void DiagWorld::FitnessSharing()
{
  std::cout << "Setting selection scheme: FitnessSharing" << std::endl;
  std::cout << "Fitness Sharing applied on: ";
  if(!config.FIT_APPLI()){
    std::cout << "Genome" << std::endl;
  }
  else
  {
    std::cout << "Phenotype" << std::endl;
  }

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // If we get asked to do standard tournament selection with fitness sharing enabled
    if(config.FIT_SIGMA() == 0.0)
    {
      // select parent ids
      ids_t parent(pop.size());

      for(size_t i = 0; i < parent.size(); ++i)
      {
        parent[i] = selection->Tournament(config.TOUR_SIZE(), fit_vec);
      }

      return parent;
    }

    // are we using genomes or phenotypes for similarity comparison?
    gmatrix_t comps = (config.FIT_APPLI()) ? PopFitMat() : PopGenomes();

    // generate distance matrix + fitness transformation
    fmatrix_t dist_mat = selection->SimilarityMatrix(comps, config.PNORM_EXP());
    score_t tscore = selection->FitnessSharing(dist_mat, fit_vec, config.FIT_ALPHA(), config.FIT_SIGMA());

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->Tournament(config.TOUR_SIZE(), tscore);
    }

    return parent;
  };

  std::cout << "Fitness sharing selection scheme set!" << std::endl;
}

void DiagWorld::NoveltyAggregate()
{
  std::cout << "Setting selection scheme: NoveltyAggregate" << std::endl;
  std::cout << "Tournament size for novelty: " << config.TOUR_SIZE() << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // If we get asked to do standard tournament selection with novelty euclidean selected
    if(config.NOVEL_K() == 0)
    {
      // select parent ids
      ids_t parent(pop.size());

      for(size_t i = 0; i < parent.size(); ++i)
      {
        parent[i] = selection->Tournament(config.TOUR_SIZE(), fit_vec);
      }

      return parent;
    }

    // generate nearest neighbor pop structure
    neigh_t neighborhood = selection->FitNearestN(fit_vec, config.NOVEL_K());

    // transform original fitness into novelty fitness
    score_t tscore = selection->Novelty(fit_vec, neighborhood, config.NOVEL_K());

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->Tournament(config.TOUR_SIZE(), tscore);
    }

    return parent;
  };

  std::cout << "Novelty search selection scheme set!" << std::endl;
}

void DiagWorld::NoveltyEuclidean()
{
  std::cout << "Setting selection scheme: NoveltyEuclidean" << std::endl;
  std::cout << "Tournament size for novelty: " << config.TOUR_SIZE() << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // If we get asked to do standard tournament selection with novelty euclidean selected
    if(config.NOVEL_K() == 0)
    {
      // select parent ids
      ids_t parent(pop.size());

      for(size_t i = 0; i < parent.size(); ++i)
      {
        parent[i] = selection->Tournament(config.TOUR_SIZE(), fit_vec);
      }

      return parent;
    }

    // generate distance matrix
    fmatrix_t fit_mat = PopFitMat();
    fmatrix_t dist_mat = selection->SimilarityMatrix(fit_mat, config.PNORM_EXP());
    // generate neighbors for distances
    neigh_t neighborhood = selection->EuclideanNearestN(dist_mat, config.NOVEL_K());
    // generate 0 vector for novelty scoring
    score_t zeros(pop.size(), 0.0);
    // transform original fitness into novelty fitness
    score_t tscore = selection->Novelty(zeros, neighborhood, config.NOVEL_K());

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->Tournament(config.TOUR_SIZE(), tscore);
    }

    return parent;
  };

  std::cout << "Novelty search selection scheme set!" << std::endl;
}

void DiagWorld::EpsilonLexicase()
{
  std::cout << "Setting selection scheme: EpsilonLexicase" << std::endl;
  std::cout << "Epsilon: " << config.LEX_EPS() << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size());

    fmatrix_t matrix = PopFitMat();

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->EpsiLexicase(matrix, config.LEX_EPS(), config.OBJECTIVE_CNT());
    }

    return parent;
  };

  std::cout << "Epsilon Lexicase selection scheme set!" << std::endl;
}


///< evaluation function implementations

void DiagWorld::Exploitation()
{
  std::cout << "Setting exploitation diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->Exploitation(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set the starting position
    org.StartPosition();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    // set streak
    org.CalcStreak();

    return org.GetAggregate();
  };

  std::cout << "Exploitation diagnotic set!" << std::endl;
}

void DiagWorld::StructuredExploitation()
{
  std::cout << "Setting structured exploitation diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->StructExploitation(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set the starting position
    org.StartPosition();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    // set streak
    org.CalcStreak();

    return org.GetAggregate();
  };

  std::cout << "Structured exploitation diagnotic set!" << std::endl;
}

void DiagWorld::StrongEcology()
{
  std::cout << "Setting strong ecology diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->StrongEcology(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set the starting position
    org.StartPosition();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    // set streak
    org.CalcStreak();

    return org.GetAggregate();
  };

  std::cout << "Strong ecology diagnotic set!" << std::endl;
}

void DiagWorld::Exploration()
{
  std::cout << "Setting exploration diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->Exploration(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set the starting position
    org.StartPosition();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    // set streak
    org.CalcStreak();

    return org.GetAggregate();
  };

  std::cout << "Exploration diagnotic set!" << std::endl;
}

void DiagWorld::WeakEcology()
{
  std::cout << "Setting weak ecology diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->WeakEcology(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set the starting position
    org.StartPosition();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    // set streak
    org.CalcStreak();

    return org.GetAggregate();
  };

  std::cout << "Weak ecology diagnotic set!" << std::endl;
}

///< data tracking

size_t DiagWorld::UniqueObjective()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());

  // iterate through objectives
  size_t cnt = 0;
  for(size_t o = 0; o < config.OBJECTIVE_CNT(); ++o)
  {
    // iterate pop to check is a solution has the objective optimized
    for(size_t p = 0; p < pop.size(); ++p)
    {
      Org & org = *pop[p];

      // quick checks
      emp_assert(org.GetOptimal().size() == config.OBJECTIVE_CNT());

      if(org.OptimizedAt(o))
      {
        ++cnt;
        break;
      }
    }
  }

  return cnt;
}

size_t DiagWorld::FindElite()
{
  // quick checks
  emp_assert(0 < fit_vec.size()); emp_assert(fit_vec.size() == pop.size());
  emp_assert(fit_vec.size() == config.POP_SIZE());
  emp_assert(elite_pos == config.POP_SIZE());

  // find max value position
  auto elite_it = std::max_element(fit_vec.begin(), fit_vec.end());

  return std::distance(fit_vec.begin(), elite_it);
}

size_t DiagWorld::FindCommon()
{
  // quick checks
  emp_assert(pop.size() == config.POP_SIZE());
  emp_assert(common.size() == 0);
  emp_assert(comm_pos == config.POP_SIZE());

  // iterate through pop and place in appropiate bin
  for(size_t i = 0; i < pop.size(); ++i)
  {
    bool in_comm = false;
    const Org & org = *pop[i];

    // check if current org matches any of the existing keys
    for(const auto & p : common)
    {
      // get key orgs data
      const Org & korg = *pop[p.first];

      // get euclidean distance between both genomes
      const double dif = selection->Pnorm(org.GetGenome(), korg.GetGenome(), 2.0);

      // if they are a match
      if(dif == 0.0)
      {
        common[p.first].push_back(i);
        in_comm = true;
        break;
      }
    }

    if(!in_comm)
    {
      ids_t first{i};
      common[i] = first;
    }
  }

  // iterate through common dictionary and find common org id
  size_t max = 0; size_t max_pos = 0;
  for(const auto & p : common)
  {
    if(max < p.second.size())
    {
      max_pos = p.first;
      max = p.second.size();
    }
  }

  return max_pos;
}

size_t DiagWorld::FindOptimized()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());
  emp_assert(opti_pos == config.POP_SIZE());

  // iterate through pop and find optimal solution
  size_t max = 0; size_t max_pos = 0;
  for(size_t i = 0; i < pop.size(); ++i)
  {
    const Org & org = *pop[i];

    if(max < org.GetCount())
    {
      max = org.GetCount();
      max_pos = i;
    }
  }

  return max_pos;
}

size_t DiagWorld::FindUniqueStart()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());

  // collect number of unique starting positions
  std::set<size_t> position;

  // iterate pop to check is a solution has the objective optimized
  for(size_t p = 0; p < pop.size(); ++p)
  {
    Org & org = *pop[p];

    // check that the position has be set
    emp_assert(org.GetStart() != config.OBJECTIVE_CNT());

    // insert position into set
    position.insert(org.GetStart());
  }

  return position.size();
}

size_t DiagWorld::FindStreak()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());
  emp_assert(strk_pos == config.POP_SIZE());

  // get all solution streaks & find max
  size_t maxx = 0;
  for(size_t i = 0; i < pop.size(); ++i)
  {
    const Org & org = *pop[i];

    if(maxx < org.GetStreak())
    {
      maxx = org.GetStreak();
    }
  }

  // collect all solutions with max streak
  ids_t candidate;
  for(size_t i = 0; i < pop.size(); ++i)
  {
    const Org & org = *pop[i];

    if(maxx == org.GetStreak())
    {
      candidate.push_back(i);
    }
  }

  // find highest performer from existing candiates
  double best_score = 0.0;
  size_t best_org = 0;
  for(size_t i = 0; i < candidate.size(); i++)
  {
    const Org & org = *pop[candidate[i]];

    if(best_score < org.GetAggregate())
    {
      best_score = org.GetAggregate();
      best_org = candidate[i];
    }
  }

  return best_org;
}


///< helper functions

DiagWorld::fmatrix_t DiagWorld::PopFitMat()
{
  // quick checks
  emp_assert(pop.size() == config.POP_SIZE());

  // create matrix of population score vectors
  fmatrix_t matrix(pop.size());

  for(size_t i = 0; i < pop.size(); ++i)
  {
    Org & org = *pop[i];
    emp_assert(org.GetScore().size() == config.OBJECTIVE_CNT());

    // charles ask if this is the actual org score vector or a deep copy made
    matrix[i] = org.GetScore();
  }

  return matrix;
}

DiagWorld::gmatrix_t DiagWorld::PopGenomes()
{
  // quick checks
  emp_assert(pop.size() == config.POP_SIZE());

  gmatrix_t matrix(pop.size());

  for(size_t i = 0; i < pop.size(); ++i)
  {
    Org & org = *pop[i];
    emp_assert(org.GetGenome().size() == config.OBJECTIVE_CNT());

    // charles ask if this is the actual org genome or a deep copy made
    matrix[i] = org.GetGenome();
  }

  return matrix;
}

#endif