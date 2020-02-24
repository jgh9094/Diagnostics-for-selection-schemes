# Selection Scheme Diagnostics Part I: The Intro

## Overview

This repository is associated with our **TBD** submission, Selection Scheme Diagnostics Part I. This is currently a work in progress and will be kept up to date!

**Repository Guide:**
- Still a work in progress

## Authors

- Jose Guadalupe Hernandez
- Alexander Lalejini (maybe?)
- [Charles Ofria](http://ofria.com) (PhD advisor extraordinaire)

## Abstract

> Evolutionary algorithms have demonstrated to potential to solve complex problems in an unconventional way. A major component of these evolutionary algorithms is the selection of genetic material for the generation of candidate solutions, or selection schemes. Over time, the complexity of these selection schemes has grown, and in turn become more robust and efficient towards finding optimal solutions. With every new selection scheme that emerges, a new set of problems the scheme must contend with to prove its worthiness arises. This is a problem when comparing the efficiency and robustness across different selection schemes. In this work we present four diagnostic problems that are isolated components across different problems evolutionary algorithms must contend with. This set of diagnostic problems gives us a better understanding of the kinds of problems that selection schemes perform well on and what they struggle with. We analyze how the five selection schemes chosen perform in regards to diversity and performance on a given diagnostic.

## Selection Schemes

This section discusses the kinds selection schemes we are analyzing in this project. These selection schemes were selected because of we believe they

- ($\mu$,$\lambda$)
- Fitness sharing
- Novelty search
- $\epsilon$ Lexicase
- Age-Layered Population Structure (ALPS)

## Diagnostic Problems
This section discusses the isolated component that different problems share.

- Exploration
- Exploitation
- Structured Exploitation
- Ecological Niche Filling
-     h<sub>&theta;</sub>(x) = &theta;<sub>o</sub> x + &theta;<sub>1</sub>x