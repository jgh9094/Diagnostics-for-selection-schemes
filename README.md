# Selection Scheme Diagnostics Part I: The Intro

## Overview

This repository is associated with our **TBD** submission, Selection Scheme Diagnostics Part I. This is currently a work in progress and will be kept up to date!

**Repository Guide:**
- Still a work in progress

## Authors

- [Jose Guadalupe Hernandez](https://jgh9094.github.io/)
- [Charles Ofria](http://ofria.com) (PhD advisor extraordinaire)

## Abstract

> Evolutionary algorithms have demonstrated to ability to solve complex real-world optimization problems, sometimes presenting solutions that are unintuitive to a human designer.
> The difference between solving complex optimization problem and not can be heavily influenced by the kind of algorithm and its configuration.
> Thus, being able to piece together the best algorithm for a give optimization problem may be the key to solving it.
> One of the major phases of an evolutionary algorithm, that many in fact share, is the selection of genetic material for the construction of a new population of solutions for the following generation -- otherwise known as a selection scheme.
> Over time, the complexity of selection schemes has grown in an effort to increase robustness and effectiveness when traversing the fitness landscape to find high quality solutions.
> The effectiveness of new selection schemes is usually illustrated with different sets of real-world problems, which can make it difficult to compare schemes.
> While this approach demonstrates a selection scheme's ability to solve a complex optimization problem, understanding or visualizing the fitness landscape being traversed is difficult and may be impossible.
> In this work we present four diagnostic optimization problems that focus on isolating problem characteristics that algorithms will encounter in a real-world optimization problem.
This will allow for a better understanding of the strengths and weaknesses of different selection schemes.

## Selection Schemes

This section discusses the kinds selection schemes we are analyzing in this project. These selection schemes were selected because of we believe they

- Mu-Lambda Selection
- Fitness Sharing
- Novelty Search
- Epsilon Lexicase

## Diagnostic Problems
This section discusses the isolated component that different problems share.

- Exploitation
- Structured Exploitation
- Exploration
- Ecological Niche Filling

## Experimental Setup

A more detailed description of the experiment set up can be found [here](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/experiment_setup.html).

## Preliminary Graphs

Below you will find the preliminary graphs that help us understand how the selection schemes perform on the diagnostics.

- [Exploitation](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/exploitation-graphs.html)
- [Structured Exploitation](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/structured-exploitation-graphs.html)
- [Exploration](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/exploration-graphs.html)
- [Ecological Niche Filling](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/niching-graphs.html)

## Preliminary Stats

Below you will find the preliminary results that drive the final selection scheme parameters that we will be testing.
In each file there are statistics that provide evidence as to why each final selection scheme parameter is selected.

**Need to update**

- [Exploitation](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/stats-prelim-exploitation.html)
- [Structured Exploitation](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/stats-prelim-struct-exploitation.html)
- [Exploration](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/stats-prelim-exploration.html)
- [Ecological Niche Filling](https://jgh9094.github.io/Selection-Scheme-Diagnotics-Part-I/Descriptions/stats-prelim-niching.html)