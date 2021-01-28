#ifndef DIA_CONFIG_H
#define DIA_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(DiaConfig,
  GROUP(WORLD_STRUCTURE, "How should the world be setup?"),
  VALUE(POP_SIZE,     size_t,      512,    "Population size."),
  VALUE(MAX_GENS,     size_t,        0,    "Maximum number of generations."),
  VALUE(SEED,           int,         0,    "Random number seed."),

  GROUP(DIAGNOSTICS, "How are the diagnostics setup?"),
  VALUE(TARGET,              double,     100.0,      "Target that traits are trying to optimize towards."),
  VALUE(ACCURACY,            double,      0.99,      "Accuracy percentage needed to be considered an optimal trait"),
  VALUE(TRAIT_CNT,           size_t,       100,      "Number of traits an organism has"),
  VALUE(SELECTION,           size_t,         0,      "Which selection are we doing? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Novelty Search\n4: Espilon Lexicase"),
  VALUE(DIAGNOSTIC,          size_t,         0,      "Which diagnostic are we doing? \n0: Exploitation\n1: Structured Exploitation\n2: Ecology Contradictory Traits\n3: Exploration"),

  GROUP(MUTATIONS, "Mutation rates for organisms."),
  VALUE(MUTATE_PER,       double,     0.007,        "Probability of instructions being mutated"),
  VALUE(MEAN,             double,     0.0,          "Mean of Gaussian Distribution for mutations"),
  VALUE(STD,              double,     1.0,          "Standard Deviation of Gaussian Distribution for mutations"),

  GROUP(PARAMETER_ESTIMATES, "Parameter estimations all selection schemes."),
  VALUE(MU,               size_t,         256,       "Parameter estiamte for μ."),
  VALUE(TOUR_SIZE,        size_t,         256,       "Parameter estiamte for tournament size."),
  VALUE(FIT_SIGMA,        double,         256,       "Parameter estiamte for similarity threshold."),
  VALUE(NOVEL_K,          size_t,         256,       "Parameter estiamte k-nearest neighbors."),
  VALUE(LEX_EPS,          double,        10.0,       "Parameter estimate for lexicase epsilon."),

  GROUP(OUTPUT, "Output rates for OpenWorld"),
  VALUE(SNAP_INTERVAL,             size_t,             10000,          "How many updates between prints?"),
  VALUE(PRINT_INTERVAL,            size_t,              1000,          "How many updates between prints?"),
  VALUE(OUTPUT_DIR,           std::string,        "../Data/",          "What directory are we dumping all this data")
)

#endif
