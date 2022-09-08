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
import pandas as pd
import csv
import sys
import os

# file location for data-params.py file
sys.path.insert(1, '../')
import data_params

# return the number of rows in a csv file
def CountRows(file_name):
    # create pandas data frame of entire csv
    try:
        df = pd.read_csv(file_name)
    except pd.errors.EmptyDataError:
        df = pd.DataFrame()

    if(df.shape[0] == 0):
        return 0

    gens = df['gen'].to_list()

    return gens[-1]

# responsible for looking through the data directories for success
def CheckDir(dir, sel, dia, offs, obj, acc, gens, pt):

    # check if data dir exists
    if os.path.isdir(dir):
        print('Data dirctory exists=', dir)
    else:
        print('DOES NOT EXIST=', dir)
        # sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check if selection scheme data folder exists
    SEL_DIR = dir + data_params.SetSelection(sel,pt) + '/TRT_' + obj + '__ACC_' + acc + '__GEN_' + gens + '/'
    if os.path.isdir(SEL_DIR):
        print('Selection scheme data folder exists', SEL_DIR)
    else:
        print('SELECTION DIRECTORY DOES NOT EXIST=', SEL_DIR)
        # sys.exit('SELECTION DATA DIRECTORY DOES NOT EXIST')

    # create seed data directories and check if exist
    VLIST = data_params.SetVarList(sel)
    DIR_DNE = []
    DAT_DNE = []
    DAT_DNF = []

    print('Full data Dir=', SEL_DIR + 'DIA_' + data_params.SetDiagnostic(dia) + '__' + data_params.SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/')
    print('Now checking data replicates sub directories')

    SEEDS = data_params.SetSeeds(sel)

    for s in SEEDS:
        seed = str(s + offs)
        var_val = str(VLIST[int((s-1)/data_params.REPLICATES)])
        DATA_DIR =  SEL_DIR + 'DIA_' + data_params.SetDiagnostic(dia) + '__' + data_params.SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/'
        print('Sub directory:', DATA_DIR)

        # add full directory to missing list if not there
        if os.path.isdir(DATA_DIR) == False:
            DIR_DNE.append(int(seed))
            continue

        # now check if the data file exists in full data director
        if os.path.isfile(DATA_DIR + 'data.csv') == False:
            DAT_DNE.append(int(seed))
            continue

        # make sure that the data.csv file did in fact finish all generations
        if CountRows(DATA_DIR + 'data.csv') != int(gens):
            DAT_DNF.append(int(seed))
            continue

    # print out the list of incomplete seeds
    print('Directories that were not created:', DIR_DNE)
    print('Data files that do not exist:', DAT_DNE)
    print('Data files that did not finish:', DAT_DNF)
    print('')
    print('Total list of unfinished runs:')

    fin = DIR_DNE + DAT_DNF + DAT_DNE
    fin.sort()
    fins = ''
    for x in fin:
        fins = fins + str(x) + ','
    # print out the sorted list
    print(fins[:len(fins)-1])
    print('-'*(len(fins)-1))

# runner
def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("selection",      type=int, help="Selection scheme we are looking for? \n0: Truncation\n1: Tournament\n2: Fitness Sharing\n3: Espilon Lexicase\n4: Nondominated Sorting\n5: Novelty Search")
    parser.add_argument("diagnostic",     type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Ordered Exploitation\n2: Contradictory Objectives\n3: Multi-path Exploration\n4: Multi-valley Crossing")
    parser.add_argument("seed_offset",    type=int, help="Experiment seed offset.")
    parser.add_argument("objectives",     type=str, help="Number of objectives being optimized")
    parser.add_argument("accuracy",       type=str, help="Accuracy for experiment")
    parser.add_argument("generations",    type=str, help="Number of generations experiments ran for")
    parser.add_argument("param_two",      type=str, help="Second paramater for any selection scheme")

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_directory.strip()
    print('Data directory=',data_dir)
    selection = args.selection
    print('Selection scheme=', data_params.SetSelection(selection,args.param_two))
    diagnostic = args.diagnostic
    print('Diagnostic=', data_params.SetDiagnostic(diagnostic))
    offset = int(args.seed_offset)
    print('Offset=', offset)
    objectives = args.objectives
    print('Objectives=', objectives)
    accuracy = args.accuracy
    print('Accuracy=', accuracy)
    generations = args.generations
    print('Generations=', generations)
    param_two = args.param_two
    print('2nd param=', param_two)


    # Get to work!
    print("\nChecking all related data directories now!")
    CheckDir(data_dir, selection, diagnostic, offset, objectives, accuracy, generations, param_two)

if __name__ == "__main__":
    main()