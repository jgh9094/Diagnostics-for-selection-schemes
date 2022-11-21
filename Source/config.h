#ifndef DIA_CONFIG_H
#define DIA_CONFIG_H

// empirical  headers
#include "emp/config/config.hpp"

EMP_BUILD_CONFIG(DiaConfig,
  GROUP(WORLD, "World be setup details."),
  VALUE(POP_SIZE,     size_t,      512,    "Population size."),
  VALUE(MAX_GENS,     size_t,    50000,    "Maximum number of generations."),
  VALUE(SEED,            int,        1,    "Random number seed."),
  VALUE(START,          bool,     true,    "Do we start randomly (true) or from the lowest point (false)."),

  GROUP(DIAGNOSTICS, "How are the diagnostics setup?"),
  VALUE(LOWER_BND,           double,       0.0,      "Lower bound for random starts."),
  VALUE(UPPER_BND,           double,       1.0,      "Upper bound for random starts."),
  VALUE(TARGET,              double,     100.0,      "Target values that genes must reach."),
  VALUE(ACCURACY,            double,      0.99,      "Accuracy percentage needed to be considered an optimal trait."),
  VALUE(CREDIT,              double,      0.00,      "Maximum credit a solution can get on an trait if applicable."),
  VALUE(DIMENSIONALITY,      size_t,       100,      "Diagnositc dimensionality."),
  VALUE(SELECTION,           size_t,         0,      "Which selection are we doing? \n0: Truncation\n1: Tournament\n2: Fitness Sharing\n"
                                                     "3: Espilon Lexicase\n4: Nondominated Sorting\n5: Novelty Search."),
  VALUE(DIAGNOSTIC,          size_t,         0,      "Which diagnostic are we doing? \n0: Exploitation Rate\n1: Ordered Exploitation\n"
                                                     "2: Contradictory Objectives \n3: Multi-path Exploration"),
  VALUE(VALLEY_CROSSING,       bool,     false,      "Do we add multi-valley crossing layer to the diagnostics?"),

  GROUP(MUTATIONS, "Mutation details."),
  VALUE(MUTATE_PER,       double,     0.007,        "Probability of genes recieving a mutation."),
  VALUE(MEAN,             double,     0.0,          "Mean of nurmal distribution for point mutations on genes."),
  VALUE(STD,              double,     1.0,          "Standard deviation of normal distribution for point mutations on genes."),

  GROUP(TRUNCATION, "Parameters for truncation selection."),
  VALUE(TRUNC_SIZE,       size_t,     8,            "Truncation size."),

  GROUP(TOURNAMENT, "Parameters for tournament selection."),
  VALUE(TOUR_SIZE,        size_t,     8,            "Tournament size."),

  GROUP(FITSHARING, "Parameters for fitness sharing."),
  VALUE(FIT_SIGMA,        double,        0.3,       "Similarity threshold to determine penalty (if applicable)."),
  VALUE(FIT_ALPHA,        double,        1.0,       "Penalty function shape."),
  VALUE(FIT_APPLI,          bool,      false,       "Fitness sharing applied: 0->Genotypic or 1->Phenotypic."),

  GROUP(NOVELTY, "Parameters for novelty search."),
  VALUE(NOVEL_K,          size_t,             15,       "K-nearest neighbors."),
  VALUE(NOVEL_PMIN,       double,           10.0,       "Starting novelty score needed for organism to enter archive."),
  VALUE(NOVEL_UP,         double,           0.25,       "Increase pmin by value if more than 4 organisms enter archive."),
  VALUE(NOVEL_RI,         double,        0.00001,       "Probability of solution entering in archive."),
  VALUE(NOVEL_GEN,        size_t,            500,       "Number of generations with no new archive entry."),
  VALUE(NOVEL_DOWN,       double,           0.05,       "Decrease pmin by value if NOVEL_GEN generations pass with no new archive entry."),

  GROUP(LEXICASE, "Parameters for lexicase selection."),
  VALUE(LEX_EPS,          double,            0.0,       "Epsillon value for differences between best solution when filtering."),

  GROUP(NONDOMINATED, "Parameters for nondominated sorting."),
  VALUE(NDS_MAX,            double,   9000000000000000.0,       "Starting fitness value for first pareto front group (pre fitness sharing)."),
  VALUE(NDS_REDUC,          double,                 0.99,       "Fitness reduction from max value in the previous pareto front."),
  VALUE(NDS_ALPHA,          double,                  2.0,       "Alpha value for pareto fitness sharing (within front)."),
  VALUE(NDS_SIGMA,          double,                  0.1,       "Sigma value for pareto fitness sharing (within front)."),

  GROUP(SYSTEMATICS, "Output rates for OpenWorld"),
  VALUE(OUTPUT_DIR,     std::string,              "./",          "Directory where we put all data in")
)

#endif