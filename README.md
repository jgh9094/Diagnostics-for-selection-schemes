# A Suite Of Diagnostic Metrics For Characterizing Selection Schemes

## Overview

This repository is associated with the 6th chapter of my dissertation: A suite of diagnostic metrics for characterizing selection schemes.

**Repository Guide:**
- [DataTools/](https://github.com/jgh9094/ECJ-2022-suite-of-diagnostics-for-selection-schemes/tree/master/DataTools)
    - This directory contains all tools used in this work to check, collect, and aggregate data (a combination of Python and R scripts).
- [Experiments/](https://github.com/jgh9094/ECJ-2022-suite-of-diagnostics-for-selection-schemes/tree/master/Experiments)
    - This directory contains slurm script configurations for for all experiments (Experiments/SELECTION_SCHEME/Hpcc/), data analysis found in our submission (Experiments/Analysis/), and preliminary work (Experiments/Preliminary/).
- [Source/](https://github.com/jgh9094/ECJ-2022-suite-of-diagnostics-for-selection-schemes/tree/master/Source)
    - This directory contains the source code (C++) for all experiments associated with this work.

## Authors

- [Jose Guadalupe Hernandez](https://jgh9094.github.io/)
- [Charles Ofria](http://ofria.com) (PhD advisor extraordinaire)
- [Alexander Lalejini](https://lalejini.com) (PhD advisor extraordinaire)

## Abstract

> Benchmark suites provide useful assessments  of an evolutionary algorithm's problem-solving capacity, but the constituent problems are often too complex to cleanly identify an algorithm's strengths and weaknesses.
> Here, we introduce the benchmark suite DOSSIER (``Diagnostic Overview of Selection Schemes In Evolutionary Runs'') for empirically analyzing selection schemes on important aspects of exploitation and exploration.
> Exploitation is fundamentally hill climbing, and we consider two scenarios: pure exploitation where each position in the representation can be optimized independently, and constrained exploitation where upward progress is more limited due to interactions between positions.
> Exploration is necessary when the optimization path is less clear; we consider the ability to simultaneously follow multiple independent hill climbing pathways and the ability to cross fitness valleys.
> Each combination of these scenarios produces distinct fitness landscapes that help characterize the evolutionary dynamics associated with a given selection scheme.
> We handcraft five unique fitness landscapes, and label each one as a diagnostic: the exploitation rate diagnostic, the ordered exploitation rate diagnostic, the contradictory objectives diagnostic, and the multi-path exploration diagnostic.
> Here, we evaluate six popular selection schemes on each diagnostic.
> Tournament selection and truncation selection both excelled on exploitation diagnostics, but performed poorly when exploration was required; conversely, novelty search excelled at exploration but failed to exploit gradients.
> Fitness sharing performed well when crossing fitness valleys, but poorly across all other diagnostics.
> Nondominated sorting was best for maintaining diverse populations comprised of individuals inhabiting multiple optima, but struggled to effectively exploit gradients.
Lexicase selection balanced search space exploration without sacrificing exploitation, generally performing well across diagnostics.
> Our work demonstrates the value of diagnostics for quickly building an intuitive understanding of selection scheme characteristics, which can then be used to improve or develop new selection methods.

### TL;DR (_i.e._, the abstract is _sooo_ long and technical, what does it say in three sentences?)

- [Evolutionary algorithms](https://en.wikipedia.org/wiki/Evolutionary_algorithm) provide an effective general purpose technique for optimization problems and typically follow three steps: evaluation, selection, and variation.
- Each of these three steps play a crucial role in guiding an evolutionary search and typically interact with one another, making it difficult to isolate each individual components impact.
- We isolate the [selection scheme](https://en.wikipedia.org/wiki/Selection_(genetic_algorithm)), and characterize its abilities on exploitation, exploration, and valley crossing with our set of diagnostic metrics.

## Diagnostic Problems

Our diagnostics use a handcrafted search space, each search space possessing unique calculated features, topology, and fitness distribution; these diagnostics help us to disentangle how a selection scheme trade-offs between exploitation and exploration.
Some diagnostics can be minor alterations of other ones; if a selection scheme performs differently on such diagnostics, that difference can be attributed to the specific alteration, isolating the effect.
Each diagnostic specifies a transformation from one numerical vector of a designated cardinality to another numerical vector of the same cardinality.
Specifically, our two exploitation focused diagnostics (exploitaiton rate and ordered exploitation) focus on measuring two different flavors of exploitation: pure exploitation and ordered exploitation.
Conversely, our two exploration focused diagnostics (contradictory objectives and multi-path exploration) focus on measuring two flavors of exploration: exploration of optima with a smooth gradient of similar fitness and exploration of optima, each with a sigle narrow gradient that leads to a differing fitness peak.

- Exploitation Rate Diagnostic:
    - Measures a selection scheme's ability to exploit a smooth fitness gradient
- Ordered Exploitation Diagnostic:
    - Measures a selection scheme's ability to pursue a single, narrow gradient that leads toward a single global optimum
- Contradictory Objectives Diagnostic:
    - Measures a selection scheme's ability to locate and optimize conflicting objectives
- Multi-path Exploration Diagnostic:
    - Measures a selection scheme's ability to maintain and simultaneously exploit multiple gradients of differing fitness peaks

## Selection Schemes

Below are the selection schemes analyzed in this work:

- Truncation Selection
- Tournament Selection
- Genotypic and Phenotypic Fitness Sharing
- Novelty Search
- Lexicase Selection
- Nondominated Sorting

## Experimental Setup

We use a simple evolutionary algorithm to diagnose a selection scheme's abilities.
We repeat the evolutionary cycle below for 50,000 generations with a population of 512 solutions.
Once a selection scheme determines the set of parents, each parent reproduces asexually and point mutations are applied to offspring.
Note that the completion of each cycle is one generation.

1. **Evaluate** all candidate solutions
2. **Select** parents from population of candidate solutions
3. **Generate** offspring from set of parents

## Experiment Source Code

The source code and configs for this work be found here: [Source/](https://github.com/jgh9094/ECJ-2022-suite-of-diagnostics-for-selection-schemes/tree/master/Source).

This work is implemented in C++ using the [Empirical library](https://github.com/devosoft/Empirical), which is required to compile and re-run the experiments.

**WARNING:** the Empirical library is under development, and as a result, it can often change in ways that may break the code used for the experiments used in this work.
I make no promises that I will keep these problems up to date with the latest changes to the Empirical library.
However, **I am more than happy to update the code upon request**.
Just submit an issue/email me (jgh9094@gmail.com).

## Results

The results for this work can be found in our [supplemental material page](https://jgh9094.github.io/Diagnostics-for-selection-schemes/).

## Reference

Hernandez, J. G., Lalejini, A., & Ofria, C. (2022). A suite of diagnostic metrics for characterizing selection schemes. arXiv preprint arXiv:2204.13839.
