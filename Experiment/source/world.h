/// World that will manage solutions during the evolutionary run

/* TODO SECTION
 * [X]: Finished coding
 * [P]: Process of coding
 * []: Nothing done yet
 *
 * SELECTION SCHEME:
 * [X]: (μ,λ)
 * [X]: Tournament
 * [X]: Fitness Sharing
 * [X]: Novelty Search
 * [X]: Epsilon-Lexicae
 *
 * DIAGNOSTICS:
 * [P]: Exploitation
 * [P]: Structured Exploitation
 * [P]: Contradictory Traits Ecologoy
 * [P]: Exploitation
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
    using fitgp_t = std::map<double, ids_t, std::greater<int>>;
    // vector of double vectors for K neighborhoods
    using neigh_t = emp::vector<score_t>;

    ///< world related types

    // evaluation function type
    using eval_t = std::function<double(Org &)>;
    // selection function type
    using sele_t = std::function<ids_t()>;

    ///< data tracking stuff
    using node_t = emp::Ptr<emp::DataMonitor<int>>;


  public:

    DiagWorld(DiaConfig & _config) : config(_config), data_file("data.csv"), elite_pos(config.POP_SIZE()),
    comm_pos(config.POP_SIZE()), opti_pos(config.POP_SIZE())
    {
      // set random pointer seed
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());
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

    // initialize the world
    void InitializeWorld();


    ///< principle steps during an evolutionary run

    // evaluation step
    void EvaluationStep();

    // selction step
    ids_t SelectionStep();

    // reprodutive step
    void ReproductionStep();


    ///< selection scheme implementations

    void MuLambda();

    void Tournament();

    void FitnessSharing();

    void NoveltySearch();

    void EpsilonLexicase();


    ///< evaluation function implementations

    void Exploitation();

    void StructuredExploitation();

    void ContraEcology();

    void Exploration();


    ///< data tracking

    size_t UniqueObjective();

    size_t FindElite();

    size_t FindCommon();

    size_t FindOptimized();


    ///< helper functions

    // create a vector of population aggregate scores
    score_t PopAggFit();

    // create a matrix of popultion score vectors
    fmatrix_t PopFitMat();

    // create matrix of population genomes
    gmatrix_t PopGenomes();


  private:
    // experiment configurations
    DiaConfig & config;
    // target vector
    target_t target; // need to initialize
    // evaluation lambda we set
    eval_t evaluate;
    // selection lambda we set
    sele_t select;
    // vector holding population aggregate scores (by position id)
    score_t fit_vec;
    // vector holding parent solutions selected by selection scheme
    ids_t parent_set;

    // select.h var
    emp::Ptr<Selection> selection;
    // problem.h var
    emp::Ptr<Diagnostic> diagnostic;

    ///< data file & node related variables

    // file we are working with
    emp::DataFile data_file;
    // node to track pop fitnesses
    node_t pop_fit;
    // node to track pop opitmized count
    node_t pop_opti;
    // node to track parent fitnesses
    node_t pnt_fit;
    // node to track parent optimized count
    node_t pnt_opti;

    ///< data we are tracking during an evolutionary run

    // elite solution position
    size_t elite_pos;
    // common solution position
    size_t comm_pos;
    // optimal solution position
    size_t opti_pos;
};

///< functions called to setup the world

void DiagWorld::Initialize()
{
  // reset the world upon start
  Reset();

  // set world to well mixed so we don't over populate
  SetPopStruct_Mixed(true);

  std::cerr << "==========================================" << std::endl;
  std::cerr << "BEGINNING INITIAL SETUP" << std::endl;
  std::cerr << "==========================================" << std::endl;

  // stuff we need to initialize for the experiment
  SetMutation();
  SetEvaluation();
  SetOnUpdate();
  SetDataTracking();
  SetSelection();
  SetOnOffspringReady();
  InitializeWorld();

  std::cerr << "==========================================" << std::endl;
  std::cerr << "FINISHED INITIAL SETUP" << std::endl;
  std::cerr << "==========================================" << std::endl;
}

void DiagWorld::SetOnUpdate()
{
  std::cerr << "------------------------------------------------" << std::endl;
  std::cerr << "Setting OnUpdate function..." << std::endl;

  // set up the evolutionary algorithm
  OnUpdate([this](size_t gen)
  {
    // step 1: evaluate all solutions on diagnostic
    EvaluationStep();

    // step 2: select parent solutions for
    SelectionStep();

    // step 3
    ReproductionStep();

  });

  std::cerr << "Finished setting the OnUpdate function! \n" << std::endl;
}

void DiagWorld::SetMutation()
{
  std::cerr << "------------------------------------------------" << std::endl;
  std::cerr << "Setting mutation function..." << std::endl;

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
        double mut = random_ptr->GetRandNormal(config.MEAN(), config.STD());

        // mutation puts objective above target
        if(config.TARGET() < genome[i] + mut)
        {
          // we wrap it back around target value
          genome[i] = target[i] - (genome[i] + mut - target[i]);
        }
        // mutation puts objective in the negatives
        else if(genome[i] + mut < 0)
        {
          genome[i] = 0.0;
        }
        // else we can simply add mutation
        else
        {
          genome[i] = genome[i] + mut;
        }

        ++mcnt;
      }
    }

    return mcnt;
  });

  std::cerr << "Mutation function set!\n" << std::endl;
}

void DiagWorld::SetSelection()
{
  std::cerr << "------------------------------------------------" << std::endl;
  std::cerr << "Setting Selection function..." << std::endl;

  selection = emp::NewPtr<Selection>(random_ptr);
  std::cerr << "Created selection emp::Ptr" << std::endl;

  switch (config.SELECTION())
  {
    // mu lambda
    case 0:
      MuLambda();
      break;

    // tournament
    case 1:
      Tournament();
      break;

    // fitness sharing
    case 2:
      FitnessSharing();
      break;

    // novelty search
    case 3:
      NoveltySearch();
      break;

    // epsilon lexicase
    case 4:
      EpsilonLexicase();
      break;

    default:
      std::cerr << "ERROR UNKNOWN SELECTION CALL" << std::endl;
      exit(-1);
      break;
  }

  std::cerr << "Finished setting the Selection function! \n" << std::endl;
}

void DiagWorld::SetOnOffspringReady()
{
  std::cerr << "------------------------------------------------" << std::endl;
  std::cerr << "Setting OnOffspringReady function..." << std::endl;

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
      org.Inherit(parent.GetScore(), parent.GetOptimal(), parent.GetCount(), parent.GetAggregate());
    }
    else
    {
      org.Reset();
    }
  });

  std::cerr << "Finished setting OnOffspringReady function!\n" << std::endl;
}

void DiagWorld::SetEvaluation()
{
  std::cerr << "------------------------------------------------" << std::endl;
  std::cerr << "Setting Selection function..." << std::endl;

  target_t target(config.POP_SIZE(), config.TARGET());
  diagnostic = emp::NewPtr<Diagnostic>(target, config.CREDIT());
  std::cerr << "Created diagnostic emp::Ptr" << std::endl;

  switch (config.DIAGNOSTIC())
  {
  case 0: // exploitation
    Exploitation();
    break;

  case 1: // structured exploitation
    StructuredExploitation();
    break;

  case 2: // contradictory ecology
    ContraEcology();
    break;

  case 3: // exploration
    Exploration();
    break;

  default:
    break;
  }

  std::cerr << "Evaluation function set!\n" <<std::endl;
}

void DiagWorld::SetDataTracking()
{
  std::cerr << "------------------------------------------------" << std::endl;
  std::cerr << "Setting up data tracking..." << std::endl;


  // initialize all nodes
  std::cerr << "Initializing nodes..." << std::endl;
  pop_fit.New(); pop_opti.New(); pnt_fit.New(); pnt_opti.New();
  std::cerr << "Nodes initialized!" << std::endl;

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

  std::cerr << "Added all data nodes to data file!" << std::endl;

  // unique optimized objectives count
  data_file.AddFun<size_t>([this]()
  {
    return UniqueObjective();
  }, "pop_uni_obj", "Number of unique optimized traits per generation!");

  // number of genetically distinct solutions

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
  data_file.AddFun<double>([this]()
  {
    // quick checks
    emp_assert(elite_pos != config.POP_SIZE());
    emp_assert(pop.size() == config.POP_SIZE());

    Org & org = *pop[elite_pos];

    return org.GetCount();
  }, "ele_opt_cnt", "Elite solution optimized objective count!");

  // common solution aggregate performance

  // common solution optimized objectives count

  // optimized solution aggregate performance
  data_file.AddFun<size_t>([this]()
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


  std::cerr << "Finished setting data tracking!\n" << std::endl;
}





void DiagWorld::InitializeWorld()
{

}






///< principle steps during an evolutionary run

void DiagWorld::EvaluationStep()
{
  // quick checks
  emp_assert(fit_vec.size() == 0); emp_assert(0 < pop.size());
  emp_assert(pop.size() == config.POP_SIZE());

  // iterate through the world and populate fitness vector
  fit_vec.clear();
  fit_vec.resize(config.POP_SIZE());
  for(size_t i = 0; i < pop.size(); ++i)
  {
    auto & org = *pop[i];

    // no evaluate needed if offspring is a clone
    fit_vec[i] = (org.GetClone()) ? org.GetAggregate() : evaluate(org);
  }
}

DiagWorld::ids_t DiagWorld::SelectionStep()
{

  return 0;
}

void DiagWorld::ReproductionStep()
{

}


///< selection scheme implementations

void DiagWorld::MuLambda()
{
  std::cerr << "Setting selection scheme: MuLambda" << std::endl;

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

  std::cerr << "MuLambda selection scheme set!" << std::endl;
}

void DiagWorld::Tournament()
{
  std::cerr << "Setting selection scheme: Tournament" << std::endl;

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

  std::cerr << "Tournament selection scheme set!" << std::endl;
}

void DiagWorld::FitnessSharing()
{
  std::cerr << "Setting selection scheme: FitnessSharing" << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    gmatrix_t genomes = PopGenomes();

    // generate distance matrix + fitness transformation
    fmatrix_t dist_mat = selection->SimilarityMatrix(genomes, config.PNORM_EXP());
    score_t tscore = selection->FitnessSharing(dist_mat, fit_vec, config.FIT_ALPHA(), config.FIT_SIGMA());

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->Tournament(config.TOUR_SIZE(), tscore);
    }

    return parent;
  };

  std::cerr << "Fitness sharing selection scheme set!" << std::endl;
}

void DiagWorld::NoveltySearch()
{
  std::cerr << "Setting selection scheme: NoveltySearch" << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

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

  std::cerr << "Novelty search selection scheme set!" << std::endl;
}

void DiagWorld::EpsilonLexicase()
{
  std::cerr << "Setting selection scheme: EpsilonLexicase" << std::endl;

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

  std::cerr << "Epsilon Lexicase selection scheme set!" << std::endl;
}


///< evaluation function implementations

void DiagWorld::Exploitation()
{
  std::cerr << "Setting exploitation diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->Exploitation(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    return org.GetAggregate();
  };

  std::cerr << "Exploitation diagnotic set!" << std::endl;
}

void DiagWorld::StructuredExploitation()
{
  std::cerr << "Setting structured exploitation diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->StructExploitation(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    return org.GetAggregate();
  };

  std::cerr << "Structured exploitation diagnotic set!" << std::endl;
}

void DiagWorld::ContraEcology()
{
  std::cerr << "Setting contradictory ecology diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->ContraEcology(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    return org.GetAggregate();
  };

  std::cerr << "Contradictory ecology diagnotic set!" << std::endl;
}

void DiagWorld::Exploration()
{
  std::cerr << "Setting exploration diagnostic..." << std::endl;

  evaluate = [this](Org & org)
  {
    // set score & aggregate
    score_t score = diagnostic->Exploration(org.GetGenome());
    org.SetScore(score);
    org.AggregateScore();

    // set optimal vector and count
    optimal_t opti = diagnostic->OptimizedVector(org.GetGenome(), config.ACCURACY());
    org.SetOptimal(opti);
    org.CountOptimized();

    return org.GetAggregate();
  };

  std::cerr << "Exploration diagnotic set!" << std::endl;
}


///< data tracking

size_t DiagWorld::UniqueObjective()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());

  // iterate through objectives
  size_t cnt = 0;
  for(size_t o = 0; 0 < config.OBJECTIVE_CNT(); ++o)
  {
    // iterate pop to check is a solution has the objective optimized
    for(size_t p = 0; p < pop.size(); ++p)
    {
      Org & org = *pop[p];

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


  // find max value position
  auto elite_it = std::max_element(fit_vec.begin(), fit_vec.end());
  elite_pos = std::distance(fit_vec.begin(), elite_it);

  return elite_pos;
}

size_t DiagWorld::FindCommon()
{

}

size_t DiagWorld::FindOptimized()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());

  // iterate through pop and find optimal solution
  size_t max = 0;
  for(size_t i = 0; i < pop.size(); ++i)
  {
    const Org & org = *pop[i];
    if(max < org.GetCount())
    {
      max = org.GetCount();
      opti_pos = i;
    }
  }

  return opti_pos;
}


///< helper functions

DiagWorld::score_t DiagWorld::PopAggFit()
{
  //quick checks
  emp_assert(pop.size() == config.POP_SIZE());

  // create vector of population aggregate scores
  score_t pscore(config.POP_SIZE());

  for(size_t i = 0; i < pop.size(); ++i)
  {
    Org & org = *pop[i];
    pscore[i] = org.GetAggregate();
  }

  return pscore;
}

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