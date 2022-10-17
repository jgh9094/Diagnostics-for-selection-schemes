#####################################################################################################
#
# Will list all of the incomplete run id's that need to finish running, per selection/diagnotic treatment
# Script will go through each replicate for a specific selection/diagnostic treatment
#
# Command Line Inputs:
#
# data_directory: directory where data is located
#      selection: selection scheme used
#     diagnostic: diagnostic used
#    seed_offset: seed offset (if any)
#     objectives: dimensionality
#       accuracy: satisfactory trait accuracy %
#    generations: generations ran
#      param_two: genotypic (0) or phenotypic (1) similarity for fitness sharing
#
# Output: list of seeds that need to be reran in terminal display
#
# python3
#####################################################################################################

######################## IMPORTS ########################
import argparse
import csv
from csv import reader
import sys
import os
import matplotlib.pyplot as plt
import pandas as pd

# runner
def main():
    muts = []
    gen = 0
    with open("mutations-0.txt", "r") as filestream:
        for line in filestream:
            cur = line.split(",")
            cur.pop(-1)
            muts += cur
            print(gen)
            gen += 1


    muts = [float(x) for x in muts]
    print('floated')
    muts.sort()
    plt.hist(muts, bins = 40, density=True)
    plt.show()

    l3sd = (1.0 - .9973) / 2.0
    h3sd = 1.0 - l3sd

    l2sd = (1.0 - .9545) / 2.0
    h2sd = 1.0 - l2sd

    l1sd = (1.0 - .6827) / 2.0
    h1sd = 1.0 - l1sd

    print('cnt :', len(muts))
    print('min :',muts[0])
    print('-3sd:',muts[int(len(muts) * l3sd)])
    print('-2sd:',muts[int(len(muts) * l2sd)])
    print('-1sd:',muts[int(len(muts) * l1sd)])
    print('medi:',muts[int(len(muts) * .5)])
    print('mean:', float(sum(muts)) / float(len(muts)))
    print('1sd :',muts[int(len(muts) * h1sd)])
    print('2sd :',muts[int(len(muts) * h2sd)])
    print('3sd :',muts[int(len(muts) * h3sd)])
    print('max :',muts[-1])


if __name__ == "__main__":
    main()