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
#include "emp/Evolve/World.hpp"
#include "emp/math/random_utils.hpp"

///< experiment headers
#include "config.h"
#include "org.h"
#include "diagnostic.h"
#include "selection.h"

class DiagWorld : public emp::World<Org>
{
  // object types for consistency between working class
  public:
    ///< Org related

    // solution genome + diagnotic problem types
    using genome_t = emp::vector<double>;
    // phenotype vector for a solution
    using phenotype_t = emp::vector<double>;
    // boolean optimal vector per objective
    using optimal_t = emp::vector<bool>;

    ///< selection related types

    // vector of position ids
    using ids_t = emp::vector<size_t>;
    // matrix of population phenotype vectors
    using fmatrix_t = emp::vector<phenotype_t>;
    // matrix of population genomes
    using gmatrix_t = emp::vector<genome_t>;
    // map holding population id groupings by fitness (keys in decending order)
    using fitgp_t = std::map<double, ids_t, std::greater<double>>;
    // vector of double vectors for K neighborhoods
    using neigh_t = emp::vector<phenotype_t>;
    // vector of vector size_t for Pareto grouping
    using pareto_t = emp::vector<emp::vector<size_t>>;

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

    DiagWorld(DiaConfig & _config) : emp::World<Org>("",false), config(_config), data_file(_config.OUTPUT_DIR() + "data.csv")
    {
      // set random pointer seed
      random_ptr = emp::NewPtr<emp::Random>(config.SEED());

      // initialize the world
      Initialize();
    }

    ~DiagWorld()
    {
      selection.Delete();
      random_ptr.Delete();
      diagnostic.Delete();
      pop_fit.Delete();
      pop_opti.Delete();
      pnt_fit.Delete();
      pnt_opti.Delete();
      pop_str.Delete();
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

    void Truncation();

    void Tournament();

    void FitnessSharing();

    void EpsilonLexicase();

    void NonDominatedSorting();

    void NoveltySearch();


    ///< diagnostic function implementations

    void ExploitationRate();

    void OrderedExploitation();

    void MultiPathExploration();

    void ContradictoryObjectives();

    void MultiValleyCrossing();


    ///< data tracking

    size_t UniqueObjective();

    size_t FindUniqueStart();

    void FindEverything();

    size_t ActivationGeneOverlap();

    double MaxPopTrait();

    double MaxPopGene();

    ///< helper functions

    // create a matrix of popultion phenotype vectors
    fmatrix_t PopFitMat();

    // create matrix of population genomes
    gmatrix_t PopGenomes();

    // update archive
    // return true if archive changes
    bool ArchiveUpdate(const phenotype_t & nov_scores, const fmatrix_t & dmat);

    // update archive data
    void ArchiveDataUpdate(const size_t & org_id);

    // record the population & parent id
    void RecordPopulation();

    size_t FindCommon();

  private:
    // experiment configurations
    DiaConfig & config;
    enum Scheme {TRUNCATION=0,TOURNAMENT=1,FITNESS_SHARING=2,LEXICASE=3,NONDOMINATED=4,NOVELTY=5};

    // vector holding population aggregate scores (by position id)
    phenotype_t fit_vec;
    // vector holding parent solutions selected by selection scheme
    ids_t parent_vec;
    // novelty minimum
    double pmin = 0.0;
    // generations since solution added to archive
    size_t archive_gens = 0;

    // evaluation lambda we set
    eval_t evaluation;
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
    std::ofstream elite_pheno_csv;
    std::ofstream elite_geno_csv;

    // mutation data
    std::ofstream mutations_txt;

    ///< data we are tracking during an evolutionary run

    // elite solution position
    size_t elite_pos;
    // common solution position
    size_t comm_pos;
    // optimal solution position
    size_t opti_pos;
    // streak solution position
    size_t strk_pos;
    // population activation gene vector
    optimal_t pop_acti_gene;
    // common solution dictionary
    como_t common;

    // Pareto group count
    size_t pareto_cnt = 0;

    // novelty search archive
    emp::vector<phenotype_t> archive;
    // elite solution position
    double arc_elite = 0.0;
    // archive optimal trait vector
    optimal_t arc_opti_trt;
    // archive activation gene vector
    optimal_t arc_acti_gene;


    // multi-valley crossing data
    // valley peaks for each floored integer gene value
    const phenotype_t peaks = { -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,  8.0,  9.0,
                           9.0, 11.0, 11.0, 11.0, 14.0, 14.0, 14.0, 14.0, 18.0, 18.0,
                          18.0, 18.0, 18.0, 23.0, 23.0, 23.0, 23.0, 23.0, 23.0, 29.0,
                          29.0, 29.0, 29.0, 29.0, 29.0, 29.0, 36.0, 36.0, 36.0, 36.0,
                          36.0, 36.0, 36.0, 36.0, 44.0, 44.0, 44.0, 44.0, 44.0, 44.0,
                          44.0, 44.0, 44.0, 53.0, 53.0, 53.0, 53.0, 53.0, 53.0, 53.0,
                          53.0, 53.0, 53.0, 63.0, 63.0, 63.0, 63.0, 63.0, 63.0, 63.0,
                          63.0, 63.0, 63.0, 63.0, 74.0, 74.0, 74.0, 74.0, 74.0, 74.0,
                          74.0, 74.0, 74.0, 74.0, 74.0, 74.0, 86.0, 86.0, 86.0, 86.0,
                          86.0, 86.0, 86.0, 86.0, 86.0, 86.0, 86.0, 86.0, 86.0, 99.0};
    // where do the dips start?
    const double dips_start = 8.0;
    // where do dips end?
    const double dips_end = 99.9;

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

    // step 1: evaluation all solutions on diagnostic
    EvaluationStep();

    // step 2: select parent solutions for
    SelectionStep();

    // step 3: gather and record data
    RecordData();

    // step 4: reproduce and create new solutions
    ReproductionStep();

    mutations_txt << "\n";
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
    emp_assert(genome.size() == config.DIMENSIONALITY());
    emp_assert(0.0 < config.TARGET());

    for(size_t i = 0; i < genome.size(); ++i)
    {
      // if we do a mutation at this objective
      if(random_ptr->P(config.MUTATE_PER()))
      {
        const double mut = random_ptr->GetRandNormal(config.MEAN(), config.STD());

        // rebound if
        if(config.TARGET() < genome[i] + mut)
        {
          genome[i] = config.TARGET() - (genome[i] + mut - config.TARGET());
        }
        // rebound if
        else if(genome[i] + mut < config.LOWER_BND())
        {
          genome[i] = std::abs(genome[i] + mut) + config.LOWER_BND();
        }
        // else add mutation
        else
        {
          genome[i] = genome[i] + mut;
        }

        mutations_txt << mut << ",";
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
    case static_cast<size_t>(Scheme::TRUNCATION):
      Truncation();
      break;

    case static_cast<size_t>(Scheme::TOURNAMENT):
      Tournament();
      break;

    case static_cast<size_t>(Scheme::FITNESS_SHARING):
      FitnessSharing();
      break;

    case static_cast<size_t>(Scheme::LEXICASE):
      EpsilonLexicase();
      break;

    case static_cast<size_t>(Scheme::NONDOMINATED):
      NonDominatedSorting();
      break;

    case static_cast<size_t>(Scheme::NOVELTY):
      NoveltySearch();
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
    emp_assert(org.GetGenome().size() == config.DIMENSIONALITY());
    emp_assert(org.GetM() == config.DIMENSIONALITY());

    // do mutations on offspring
    size_t mcnt = fun_do_mutations(org, *random_ptr);

    // no mutations were applied to offspring
    if(mcnt == 0)
    {
      Org & parent = *pop[parent_pos];

      // quick checks
      emp_assert(parent.GetGenome().size() == config.DIMENSIONALITY());
      emp_assert(parent.GetM() == config.DIMENSIONALITY());

      // give everything to offspring from parent
      org.MeClone();
      org.Inherit(parent.GetPhenotype(), parent.GetOptimal(), parent.GetCount(), parent.GetAggregate(), parent.GetStart(), parent.GetStreak());
    }
    else
    {
      org.Reset();
    }
  });

  std::cout << "Finished setting OnOffspringReady function!\n" << std::endl;
}

void DiagWorld::SetEvaluation()
{
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "Setting Evaluation function..." << std::endl;

  diagnostic = emp::NewPtr<Diagnostic>(config.TARGET(), config.CREDIT());
  std::cout << "Created diagnostic emp::Ptr" << std::endl;

  switch (config.DIAGNOSTIC())
  {
    case 0: // exploitation
      ExploitationRate();
      break;

    case 1: // structured exploitation
      OrderedExploitation();
      break;

    case 2: // contradictory objectives
      ContradictoryObjectives();
      break;

    case 3: // exploration
      MultiPathExploration();
      break;

    case 4: // valley crossing
      MultiValleyCrossing();
      break;

    default: // error, unknown diganotic
      std::cout << "ERROR: UNKNOWN DIAGNOSTIC" << std::endl;
      emp_assert(false);
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
  pop_fit.New();
  pop_opti.New();
  pnt_fit.New();
  pnt_opti.New();
  pop_str.New();
  std::cout << "Nodes initialized!" << std::endl;

  // track population aggregate phenotype stats: average, variance, min, max
  data_file.AddMean(*pop_fit, "pop_fit_avg", "Population average aggregate performance.");
  data_file.AddVariance(*pop_fit, "pop_fit_var", "Population variance aggregate performance.");
  data_file.AddMax(*pop_fit, "pop_fit_max", "Population maximum aggregate performance.");
  data_file.AddMin(*pop_fit, "pop_fit_min", "Population minimum aggregate performance.");

  // track population optimized objective count stats: average, variance, min, max
  data_file.AddMean(*pop_opti, "pop_opt_avg", "Population average objective optimization count.");
  data_file.AddVariance(*pop_opti, "pop_opt_var", "Population variance objective optimization count.");
  data_file.AddMax(*pop_opti, "pop_opt_max", "Population maximum objective optimization count.");
  data_file.AddMin(*pop_opti, "pop_opt_min", "Population minimum objective optimization count.");

  // track parent aggregate phenotype stats: average, variance, min, max
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
    emp_assert(pop_acti_gene.size()  == config.DIMENSIONALITY());
    return FindUniqueStart();
  }, "uni_str_pos", "Number of unique starting positions in the population!");

  // Pareto group count
  data_file.AddFun<size_t>([this]()
  {
    if(config.SELECTION() == static_cast<size_t>(Scheme::NONDOMINATED))
    {
      emp_assert(pareto_cnt != 0);
      return pareto_cnt;
    }
    else
    {
      emp_assert(pareto_cnt == 0);
      return pareto_cnt;
    }
  }, "pareto_cnt", "Number of Pareto groups generated!");

  // archive group count
  data_file.AddFun<size_t>([this]()
  {
    return archive.size();
  }, "archive_cnt", "Number phenotypes in the archive!");

  // archive group count
  data_file.AddFun<double>([this]()
  {
    return pmin;
  }, "pmin", "pmin used for archive approval!");

  // archive group count
  data_file.AddFun<double>([this]()
  {
    return arc_elite;
  }, "arc_elite", "archive best fitness found so far!");

  // archive unique optimal traits
  data_file.AddFun<size_t>([this]()
  {
    return std::accumulate(arc_opti_trt.begin(), arc_opti_trt.end(), 0);
  }, "arc_opti_trt", "Unique optimal traits found in the archive!");

  // archive unique activation genes
  data_file.AddFun<size_t>([this]()
  {
    return std::accumulate(arc_acti_gene.begin(), arc_acti_gene.end(), 0);
  }, "arc_acti_gene", "Unique activation genes found in the archive!");

  // archive unique activation genes
  data_file.AddFun<size_t>([this]()
  {
    return ActivationGeneOverlap();
  }, "overlap", "Unique activation genes found in the archive!");

  // max trait in the population
  data_file.AddFun<double>([this]()
  {
    return MaxPopTrait();
  }, "pop_max_trt", "Maximum trait value found in the population!");

  // max gene in the population
  data_file.AddFun<double>([this]()
  {
    return MaxPopGene();
  }, "pop_max_gene", "Maximum gene value found in the population!");

  data_file.PrintHeaderKeys();

  // create elite csv plus headers
  elite_pheno_csv.open(config.OUTPUT_DIR() + "elite-pheno.csv");
  elite_geno_csv.open(config.OUTPUT_DIR() + "elite-geno.csv");
  mutations_txt.open(config.OUTPUT_DIR() + "mutations.txt");


  std::string header_pheno = "Gen";
  std::string header_geno = "Gen";
  for(size_t i = 0; i < config.DIMENSIONALITY(); ++i)
  {
    header_pheno += ",t";
    header_geno += ",g";

    header_pheno += std::to_string(i);
    header_geno += std::to_string(i);
  }

  elite_pheno_csv << header_pheno << "\n";
  elite_geno_csv << header_geno << "\n";

  std::cout << "Finished setting data tracking!\n" << std::endl;
}

void DiagWorld::PopulateWorld()
{
  std::cout << "------------------------------------------" << std::endl;
  std::cout << "Populating world with initial solutions..." << std::endl;

  // random starting organisms
  if(config.START())
  {
    for(int i = 0; i < config.POP_SIZE(); ++i)
    {
      genome_t g = emp::RandomDoubleVector(*random_ptr, config.DIMENSIONALITY(), config.LOWER_BND(), config.UPPER_BND());
      Inject(g,1);
    }
  }
  // same starting organisms
  else
  {
    Org org(config.DIMENSIONALITY());
    Inject(org.GetGenome(), config.POP_SIZE());
  }

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
  // unique_starts = config.POP_SIZE();
  pareto_cnt = 0;

  // reset all vectors/maps holding current gen data
  fit_vec.clear();
  parent_vec.clear();
  pop_acti_gene.clear();
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

    // no evaluation needed if offspring is a clone
    fit_vec[i] = (org.GetClone()) ? org.GetAggregate() : evaluation(org);
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

  parent_vec = parents;
}

void DiagWorld::RecordData()
{
  /// Add data to all nodes

  // get pop data
  emp_assert(pop.size() == config.POP_SIZE());
  for(size_t i = 0; i < pop.size(); ++i)
  {
    const Org & org = *pop[i];
    pop_fit->Add(org.GetAggregate());
    pop_opti->Add(org.GetCount());
    pop_str->Add(org.GetStreak());

    const Org & par = *pop[parent_vec[i]];
    pnt_fit->Add(par.GetAggregate());
    pnt_opti->Add(par.GetCount());
  }
  emp_assert(pop_fit->GetCount() == config.POP_SIZE());
  emp_assert(pop_opti->GetCount() == config.POP_SIZE());
  emp_assert(pop_str->GetCount() == config.POP_SIZE());

  // get parent data
  emp_assert(parent_vec.size() == config.POP_SIZE());
  emp_assert(pnt_fit->GetCount() == config.POP_SIZE());
  emp_assert(pnt_opti->GetCount() == config.POP_SIZE());

  /// get all position ids

  FindEverything();

  /// fill vectors & map
  emp_assert(fit_vec.size() == config.POP_SIZE()); // should be set already
  emp_assert(parent_vec.size() == config.POP_SIZE()); // should be set already

  /// update the file
  data_file.Update();

  // record elite solution traits
  Org & ele = *pop[elite_pos];

  std::string traits = std::to_string(update);
  const auto & p = ele.GetPhenotype();
  for(size_t i = 0; i < p.size(); ++i)
  {
    traits += ",";
    traits += std::to_string(p[i]);
  }
  elite_pheno_csv << traits << "\n";

  std::string genes = std::to_string(update);
  const auto & g = ele.GetGenome();
  for(size_t i = 0; i < g.size(); ++i)
  {
    genes += ",";
    genes += std::to_string(g[i]);
  }
  elite_geno_csv << genes << "\n";


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
  for(auto & id : parent_vec)
  {
    DoBirth(GetGenomeAt(id), id);
  }
}


///< selection scheme implementations

void DiagWorld::Truncation()
{
  std::cout << "Setting selection scheme: Truncation" << std::endl;

  // set select lambda to mu lambda selection
  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // group population by fitness
    const fitgp_t group = selection->FitnessGroup(fit_vec);

    return selection->MLSelect(config.TRUNC_SIZE(), config.POP_SIZE(), group);
  };

  std::cout << "Truncation selection scheme set!" << std::endl;
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
  if(!config.FIT_APPLI()) {std::cout << "Genotypic" << std::endl;}
  else {std::cout << "Phenotypic" << std::endl;}

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(fit_vec.size() == config.POP_SIZE());

    // If we get asked to do stochasitc remainder selection
    if(config.FIT_SIGMA() == 0.0)
    {
      // do stochastic remainder selection with unmodified fitness
      return selection->StochasticRemainder(fit_vec);
    }

    // are we using genomes or phenotypes for similarity comparison?
    const gmatrix_t comps = (config.FIT_APPLI()) ? PopFitMat() : PopGenomes();

    // find neighbors that follow d_xy < sigma^2 + fitness transformation
    const fmatrix_t neighbors = selection->SimilarNeighbors(comps, config.FIT_SIGMA() * config.FIT_SIGMA());
    const phenotype_t tscore = selection->FitnessSharing(neighbors, fit_vec, config.FIT_ALPHA(), config.FIT_SIGMA());

    return selection->StochasticRemainder(tscore);
  };

  std::cout << "Fitness sharing selection scheme set!" << std::endl;
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

    const fmatrix_t matrix = PopFitMat();

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->EpsiLexicase(matrix, config.LEX_EPS(), config.DIMENSIONALITY());
    }

    return parent;
  };

  std::cout << "Epsilon Lexicase selection scheme set!" << std::endl;
}

void DiagWorld::NonDominatedSorting()
{
  std::cout << "Setting selection scheme: NonDominatedSorting" << std::endl;

  select = [this]()
  {
    // quick checks
    emp_assert(selection); emp_assert(pop.size() == config.POP_SIZE());
    emp_assert(0 < pop.size()); emp_assert(pareto_cnt == 0);

    // select parent ids
    ids_t parent(pop.size());

    // get Pareto groups with ids
    const fmatrix_t matrix = PopFitMat();
    const pareto_t pgroups = selection->ParetoFrontGroups(matrix);

    // construct pareto fitnesses
    const phenotype_t fitness = selection->ParetoFitness(pgroups, matrix, config.NDS_ALPHA(), config.NDS_SIGMA(), config.NDS_REDUC(), config.NDS_MAX());

    // track data
    pareto_cnt = pgroups.size();

    return selection->StochasticRemainder(fitness);
  };

  std::cout << "NonDominated Sorting selection scheme set!" << std::endl;
}

void DiagWorld::NoveltySearch()
{
  std::cout << "Setting selection scheme: NoveltySearch" << std::endl;
  std::cout << "Starting PMIN: " << config.NOVEL_PMIN() << std::endl;
  // save starting pmin
  pmin = config.NOVEL_PMIN();

  // initialize vectors for achive
  arc_opti_trt = optimal_t(config.DIMENSIONALITY(), false);
  arc_acti_gene = optimal_t(config.DIMENSIONALITY(), false);

  emp_assert(arc_opti_trt.size() == config.DIMENSIONALITY());
  emp_assert(arc_acti_gene.size() == config.DIMENSIONALITY());

  std::cout << "Created vectors for tracking archive data" << std::endl;

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

    // find nearest neighbors for each solution
    // NOVEL_K ammount of summation of squares phenotype (x-y)^2
    const fmatrix_t fit_mat = PopFitMat();
    const fmatrix_t neighborhood = selection->NoveltySearchNearSum(fit_mat, config.NOVEL_K(), config.POP_SIZE());

    // calculate novelty scores
    const phenotype_t tscore = selection->NoveltySOS(neighborhood, config.NOVEL_K());

    // check if we need to reduce pmin
    if(archive_gens == config.NOVEL_GEN())
    {
      archive_gens = 0;
      pmin -= pmin * config.NOVEL_DOWN();
    }

    // archive tracking and updated gen count
    if(!ArchiveUpdate(tscore, fit_mat))
    {
      archive_gens++;
    }

    // select parent ids
    ids_t parent(pop.size());

    for(size_t i = 0; i < parent.size(); ++i)
    {
      parent[i] = selection->Tournament(config.TOUR_SIZE(), tscore);
    }

    return parent;
  };

  std::cout << "Novelty Search - Archive selection scheme set!" << std::endl;
}


///< evaluation function implementations

void DiagWorld::ExploitationRate()
{
  std::cout << "Setting exploitation diagnostic..." << std::endl;

  evaluation = [this](Org & org)
  {
    // set phenotype & aggregate
    phenotype_t phenotype = diagnostic->ExploitationRate(org.GetGenome());
    org.SetPhenotype(phenotype);
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

void DiagWorld::OrderedExploitation()
{
  std::cout << "Setting structured exploitation diagnostic..." << std::endl;

  evaluation = [this](Org & org)
  {
    // set phenotype & aggregate
    phenotype_t phenotype = diagnostic->OrderedExploitation(org.GetGenome());
    org.SetPhenotype(phenotype);
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

void DiagWorld::MultiPathExploration()
{
  std::cout << "Setting exploration diagnostic..." << std::endl;

  evaluation = [this](Org & org)
  {
    // set phenotype & aggregate
    phenotype_t phenotype = diagnostic->MultiPathExploration(org.GetGenome());
    org.SetPhenotype(phenotype);
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

void DiagWorld::ContradictoryObjectives()
{
  std::cout << "Setting weak ecology diagnostic..." << std::endl;

  evaluation = [this](Org & org)
  {
    // set phenotype & aggregate
    phenotype_t phenotype = diagnostic->ContradictoryObjectives(org.GetGenome());
    org.SetPhenotype(phenotype);
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

void DiagWorld::MultiValleyCrossing()
{
  evaluation = [this](Org & org)
  {
    // set phenotype & aggregate
    phenotype_t phenotype = diagnostic->MultiValleyCrossing(org.GetGenome(), peaks, dips_start, dips_end);
    org.SetPhenotype(phenotype);
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
}


///< data tracking

size_t DiagWorld::UniqueObjective()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());

  optimal_t unique;

  // novelty search unqiue objective trait count
  if(config.SELECTION() == static_cast<size_t>(Scheme::NOVELTY))
  {
    emp_assert(arc_opti_trt.size() == config.DIMENSIONALITY());
    unique = arc_opti_trt;
  }
  else{ unique = optimal_t(config.DIMENSIONALITY(), false); }


  for(size_t o = 0; o < config.DIMENSIONALITY(); ++o)
  {
    // iterate pop to check is a solution has the objective optimized
    for(size_t p = 0; p < pop.size(); ++p)
    {
      Org & org = *pop[p];

      // quick checks
      emp_assert(org.GetOptimal().size() == config.DIMENSIONALITY());

      if(org.OptimizedAt(o))
      {
        unique[o] = true;
        break;
      }
    }
  }

  return std::accumulate(unique.begin(), unique.end(), 0);
}

size_t DiagWorld::FindUniqueStart()
{
  // quick checks
  emp_assert(0 < pop.size()); emp_assert(pop.size() == config.POP_SIZE());\
  emp_assert(pop_acti_gene.size() == config.DIMENSIONALITY());
  return std::accumulate(pop_acti_gene.begin(), pop_acti_gene.end(), 0);
}

void DiagWorld::FindEverything()
{
  // quick checks
  emp_assert(fit_vec.size() == config.POP_SIZE());
  emp_assert(pop_acti_gene.size() == 0);
  emp_assert(elite_pos == config.POP_SIZE());
  emp_assert(comm_pos == config.POP_SIZE());

  // bools to make sure got everything
  bool elite_b = false,  opti_b = false, strk_b = false;

  // collect number of unique starting positions
  pop_acti_gene = optimal_t(config.DIMENSIONALITY(), false);

  // comm_pos = FindCommon();

  // loop and get data
  for(size_t i = 0; i < pop.size(); ++i)
  {
    const Org & org = *pop[i];

    // update the population unique activation genes
    pop_acti_gene[org.GetStart()] = true;

    // check if we need to do anything below
    if(elite_b && opti_b && strk_b) {continue;}

    // find max fit solution
    if(org.GetAggregate() == pop_fit->GetMax() && !elite_b) {elite_b = true; elite_pos = i;}
    // find max optimal count solution
    if(org.GetCount() == pop_opti->GetMax() && !opti_b) {opti_b = true; opti_pos = i;}
    // find max fit solution
    if(org.GetStreak() == pop_str->GetMax() && !strk_b) {strk_b = true; strk_pos = i;}
  }
}

size_t DiagWorld::ActivationGeneOverlap()
{
  // quick checks
  emp_assert(pop_acti_gene.size() == config.DIMENSIONALITY());
  size_t count = 0;

  // if novelty selection is running calculate overlap
  if(config.SELECTION() == static_cast<size_t>(Scheme::NOVELTY))
  {
    emp_assert(arc_acti_gene.size() == config.DIMENSIONALITY());
    for(size_t i = 0; i < config.DIMENSIONALITY(); ++i)
    {
      if(pop_acti_gene[i] && arc_acti_gene[i]) {count++;}
    }
    return count;
  }
  else
  {
    emp_assert(arc_acti_gene.size() == 0);
    return count;
  }
}

double DiagWorld::MaxPopTrait()
{
  // iterate pop to check is a solution has the objective optimized
  double max = -1000000.0;
  for(size_t p = 0; p < pop.size(); ++p)
  {
    Org & org = *pop[p];

    if(max < org.GetMaxTrait())
    {
      max = org.GetMaxTrait();
    }
  }

  return max;
}

double DiagWorld::MaxPopGene()
{
  // iterate pop to check is a solution has the objective optimized
  double max = 0.0;
  for(size_t p = 0; p < pop.size(); ++p)
  {
    Org & org = *pop[p];

    for(auto & g : org.GetGenome())
    {
      if(max < g) {max = g;}
    }
  }

  return max;
}

///< helper functions

DiagWorld::fmatrix_t DiagWorld::PopFitMat()
{
  // quick checks
  emp_assert(pop.size() == config.POP_SIZE());

  // create matrix of population phenotype vectors
  fmatrix_t matrix(pop.size());

  for(size_t i = 0; i < pop.size(); ++i)
  {
    Org & org = *pop[i];
    emp_assert(org.GetPhenotype().size() == config.DIMENSIONALITY());

    // charles ask if this is the actual org phenotype vector or a deep copy made
    matrix[i] = org.GetPhenotype();
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
    emp_assert(org.GetGenome().size() == config.DIMENSIONALITY());

    // charles ask if this is the actual org genome or a deep copy made
    matrix[i] = org.GetGenome();
  }

  return matrix;
}

bool DiagWorld::ArchiveUpdate(const phenotype_t & nov_scores, const fmatrix_t & dmat)
{
  //quick checks
  emp_assert(0 < nov_scores.size()); emp_assert(0 < dmat.size());
  emp_assert(nov_scores.size() == dmat.size());

  // archive insertion count
  size_t insert = 0;

  // check each solution novelty nov_scores
  for(size_t i = 0; i < nov_scores.size(); ++i)
  {
    // insert solution if lucky
    if(random_ptr->P(config.NOVEL_RI()))
    {
      // add nov_scores to archive
      archive.push_back(dmat[i]);

      // update archive stats with solution data (if possible)
      ArchiveDataUpdate(i);
    }
    else if(nov_scores[i] > pmin)
    {
      // increment insertion counts for update
      ++insert;

      // add phenotype to the archive
      archive.push_back(dmat[i]);

      // update archive stats with solution data (if possible)
      ArchiveDataUpdate(i);
    }
  }

  if(4 < insert)
  {
    pmin += pmin * config.NOVEL_UP();
  }

  return 0 < insert;
}

void DiagWorld::ArchiveDataUpdate(const size_t & org_id)
{
  // quick checks
  emp_assert(pop.size() == config.POP_SIZE());
  emp_assert(0 <= org_id); emp_assert(org_id < pop.size());
  emp_assert(config.SELECTION() == static_cast<size_t>(Scheme::NOVELTY));

  // get org from
  Org & org = *pop[org_id];

  // archive current trait aggregate maximum
  if(arc_elite < org.GetAggregate()) {arc_elite = org.GetAggregate();}
  // update the archive activation gene vector
  arc_acti_gene[org.GetStart()] = true;

  // update the archive optimal trait vector
  for(size_t o = 0; o < config.DIMENSIONALITY(); ++o)
  {
    if(org.OptimizedAt(o))
      {
        arc_opti_trt[o] = true;
      }
  }
}

#endif