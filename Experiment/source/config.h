#ifndef DIA_CONFIG_H
#define DIA_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(DiaConfig,
  GROUP(WORLD, "How should the world be setup?"),
  VALUE(POP_SIZE,     size_t,      512,    "Population size."),
  VALUE(MAX_GENS,     size_t,    10000,    "Maximum number of generations."),
  VALUE(SEED,            int,        1,    "Random number seed."),
  VALUE(START,          bool,     true,    "Do we start randomly (true) or from the lowest point (false)"),

  GROUP(DIAGNOSTICS, "How are the diagnostics setup?"),
  VALUE(LOWER_BND,           double,       0.0,      "Lower bound for random starts."),
  VALUE(UPPER_BND,           double,       1.0,      "Upper bound for random starts."),
  VALUE(TARGET,              double,     100.0,      "Upper bound for random starts."),
  VALUE(ACCURACY,            double,      0.99,      "Accuracy percentage needed to be considered an optimal trait"),
  VALUE(CREDIT,              double,      0.00,      "Maximum credit a solution can get on an objective if applicable"),
  VALUE(OBJECTIVE_CNT,       size_t,       100,      "Number of traits an organism has"),
  VALUE(SELECTION,           size_t,         5,      "Which selection are we doing? \n0: Truncation\n1: Tournament\n2: Fitness Sharing\n"
                                                     "3: Espilon Lexicase\n4: Nondominated Sorting\n5: Novelty Search"),
  VALUE(DIAGNOSTIC,          size_t,         0,      "Which diagnostic are we doing? \n0: Exploitation\n1: Structured Exploitation\n"
                                                     "2: Strong Ecology \n3: Exploration \n4: Weak Ecology"),

  GROUP(MUTATIONS, "Mutation rates for organisms."),
  VALUE(MUTATE_PER,       double,     0.007,        "Probability of instructions being mutated"),
  VALUE(MEAN,             double,     0.0,          "Mean of Gaussian Distribution for mutations"),
  VALUE(STD,              double,     1.0,          "Standard Deviation of Gaussian Distribution for mutations"),

  GROUP(TRUNCATION, "Parameters for truncation."), // finished in under 60 secs
  VALUE(TRUNC_SIZE,       size_t,             8,       "Parameter estimate for truncation size t."),

  GROUP(TOURNAMENT, "Parameters for tournament."), // finished in about 120 secs
  VALUE(TOUR_SIZE,        size_t,             8,       "Parameter estimate for tournament size."),

  GROUP(FITSHARING, "Parameters for fitness sharing."), // 500 gens in 120 secs
  VALUE(FIT_SIGMA,        double,           1.0,       "Parameter estimate for proportion of similarity threshold sigma (based on maximum distance between solutions)."),
  VALUE(FIT_ALPHA,        double,           1.0,       "Parameter estimate for penalty function shape alpha."),
  VALUE(FIT_APPLI,        bool,           false,       "Fitness sharing applied: 0->Genome, 1->Phenotype"),

  GROUP(NOVELTY, "Parameters for novelty search."), // 500 gens in 120 secs
  VALUE(NOVEL_K,          size_t,             15,       "Parameter estimate k-nearest neighbors."),
  VALUE(NOVEL_PMIN,       double,            10.0,       "Minimum novelty score needed to enter archive."),
  VALUE(NOVEL_UP,         double,           0.25,       "Increase pmin by this much."),
  VALUE(NOVEL_DOWN,       double,           0.05,       "Decrease pmin by this much."),
  VALUE(NOVEL_RI,         double,        0.00001,       "Probability of random solution inserted in archive."),
  VALUE(NOVEL_GEN,        size_t,            500,       "Number of generations to lower pmin."),
  VALUE(NOVEL_CAP,        size_t,            512,       "Cap on number of solutions allowed in the archive"),
  VALUE(NOVEL_CQS,          bool,          false,       "Do we cap solutions?"),

  GROUP(LEXICASE, "Parameters for lexicase."), // 500 gens in 10 secs
  VALUE(LEX_EPS,          double,            0.0,       "Parameter estimate for lexicase epsilon."),

  GROUP(NONDOMINATED_SORTING, "Parameters for nondominated sorting algorithm."), // // 500 gens in 120 secs
  VALUE(NDS_MAX,      double,   9000000000000000.0,       "Large dummy number ."),
  VALUE(NDS_RED,      double,                 0.99,       "Default large value for different Pareto groups."),
  VALUE(NDS_ALP,      double,                  2.0,       "Alpha value for Pareto fitness sharing."),
  VALUE(NDS_SIG,      double,                  0.1,       "Sigma value for Pareto fitness sharing."),

  GROUP(OTHER, "Parameters for selection class."),
  VALUE(PNORM_EXP,        double,            2.0,       "Paramter we are using for the p-norm function."),

  GROUP(SYSTEMATICS, "Output rates for OpenWorld"),
  VALUE(OUTPUT_DIR,           std::string,              "./",          "What directory are we dumping all this data")
)

#endif