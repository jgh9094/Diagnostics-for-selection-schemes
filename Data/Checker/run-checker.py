#####################################################################################################
#####################################################################################################
# Will list all of the incomplete id's that need to finish running, per selection/diagnotic treatment
#
# Command Line Inputs
#
# Input 1: file directory where all the folders are located
# Input 2: Selection scheme we are for
# Input 3: Diagnostic we are looking for
# Input 4: Experiment seed offset
#
# Output : list of seeds that need to be reran in terminal display
#
# python3
#####################################################################################################
#####################################################################################################


######################## IMPORTS ########################
import argparse
import pandas as pd
import csv
import sys
import os

# variables we are testing for each replicate range
MU_LIST = [1,2,4,8,16,32,64,128,256,512]
TR_LIST = [1,2,4,8,16,32,64,128,256,512]
# holds seeds
SEEDS = [x for x in range(1,501)]
# seed experiements replicates range
SMAX = 50
# 40001 gens + 1 header row
EXPECTED_GENS = 40000

# return appropiate string dir name (based off run.sb file naming system)
def SetSelection(s):
    # case by case
    if s == 0:
        return 'MULAMBDA'
    elif s == 1:
        return 'TOURNAMENT'
    else:
        sys.exit("UNKNOWN SELECTION")

# return appropiate string dir name (based off run.sb file naming system)
def SetDiagnostic(s):
    # case by case
    if s == 0:
        return 'EXPLOITATION'
    elif s == 1:
        return 'STRUCTEXPLOITATION'
    elif s == 2:
        return 'CONTRAECOLOGY'
    elif s == 3:
        return 'EXPLORATION'
    else:
        sys.exit('UNKNOWN DIAGNOSTIC')

# return appropiate string dir name (based off run.sb file naming system)
def SetSelectionVar(s):
    # case by case
    if s == 0:
        return 'MU'
    elif s == 1:
        return 'T'
    else:
        sys.exit("UNKNOWN SELECTION VAR")

# Will set the appropiate list of variables we are checking for
def SetVarList(s):
    # case by case
    if s == 0:
        return MU_LIST
    elif s == 1:
        return TR_LIST
    else:
        sys.exit("UNKNOWN VARIABLE LIST")

# return the number of rows in a csv file
def CountRows(file_name):
    # create pandas data frame of entire csv
    df = pd.read_csv(file_name)
    gens = df['gen'].to_list()

    return gens[-1]

# responsible for looking through the data directories for success
def CheckDir(dir, sel, dia, offs):

    # check if data dir exists
    if os.path.isdir(dir):
        print('Data dirctory exists=', dir)
    else:
        print('DOES NOT EXIST=', dir)
        sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check if selection scheme data folder exists
    SEL_DIR = dir + SetSelection(sel) + '/'
    if os.path.isdir(SEL_DIR):
        print('Selection scheme data folder exists', SEL_DIR)
    else:
        print('SELECTION DIRECTORY DOES NOT EXIST=', SEL_DIR)
        sys.exit('SELECTION DATA DIRECTORY DOES NOT EXIST')


    # SET ALL THE DATA DIRECTORY VARIABLES HERE!

    # step 2: create seed data directories and check if exist
    VLIST = SetVarList(sel)
    DIR_DNE = []
    DAT_DNE = []
    DAT_DNF = []

    print('Full data Dir=', SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/')

    for s in SEEDS:
        seed = str(s + offs)
        var_val = str(VLIST[int((s-1)/SMAX)])
        DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/'

        # add full directory to missing list if not there
        if os.path.isdir(DATA_DIR) == False:
            DIR_DNE.append(int(seed))
            continue

        # now check if the data file exists in full data director
        if os.path.isfile(DATA_DIR + 'data.csv') == False:
            DAT_DNE.append(int(seed))
            continue

        # make sure that the data.csv file did in fact finish all generations
        if CountRows(DATA_DIR + 'data.csv') != EXPECTED_GENS:
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



def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("selection", type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Novelty Search\n4: Espilon Lexicase")
    parser.add_argument("diagnostic", type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Ecology Contradictory Traits\n3: Exploration")
    parser.add_argument("seed_offset", type=int, help="Experiment seed offset.")

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_directory.strip()
    print('Data directory=',data_dir)
    selection = args.selection
    print('Selection scheme=', SetSelection(selection))
    diagnostic = args.diagnostic
    print('Diagnostic=',SetDiagnostic(diagnostic))
    offset = args.seed_offset
    print('Offset=', offset)

    # Get to work!
    print("\nChecking all related data directories now!")
    CheckDir(data_dir, selection, diagnostic, offset)


if __name__ == "__main__":
    main()