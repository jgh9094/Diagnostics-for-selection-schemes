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
LX_LIST = [0.0,0.1,0.3,0.6,1.2,2.5,5.0,10.0]
FS_LIST = [0.0,10.0,30.0,60.0,12.0,250.0,500.0,1000.0]
NS_LIST = [0,1,2,4,8,15,30,60]
# seed experiements replicates range
SMAX = 50

# return appropiate string dir name (based off run.sb file naming system)
def SetSelection(s):
    # case by case
    if s == 0:
        return 'MULAMBDA'
    elif s == 1:
        return 'TOURNAMENT'
    elif s == 2:
        return 'FITSHARING'
    elif s == 3:
        return 'NOVELTY'
    elif s == 4:
        return 'LEXICASE'
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
        return 'STRONGECOLOGY'
    elif s == 3:
        return 'EXPLORATION'
    elif s == 4:
        return 'WEAKECOLOGY'
    else:
        sys.exit('UNKNOWN DIAGNOSTIC')

# return appropiate string dir name (based off run.sb file naming system)
def SetSelectionVar(s):
    # case by case
    if s == 0:
        return 'MU'
    elif s == 1:
        return 'T'
    elif s == 2:
        return 'SIG'
    elif s == 3:
        return 'NOV'
    elif s == 4:
        return 'EPS'
    else:
        sys.exit("UNKNOWN SELECTION VAR")

# return the correct amount of seed ran by experiment treatment
def SetSeeds(s):
    # case by case
    if s == 0:
        return [x for x in range(1,501)]
    elif s == 1:
        return [x for x in range(1,501)]
    elif s == 2:
        return [x for x in range(1,401)]
    elif s == 3:
        return [x for x in range(1,401)]
    elif s == 4:
        return [x for x in range(1,351)]
    else:
        sys.exit('SEEDS SELECTION UNKNOWN')

# set the appropiate list of variables we are checking for
def SetVarList(s):
    # case by case
    if s == 0:
        return MU_LIST
    elif s == 1:
        return TR_LIST
    elif s == 2:
        return FS_LIST
    elif s == 3:
        return NS_LIST
    elif s == 4:
        return LX_LIST
    else:
        sys.exit("UNKNOWN VARIABLE LIST")

# return extra parameter directory if needed
def SetSecondParam(s, pt):
    # case by case
    if s == 0:
        return ''
    elif s == 1:
        return ''
    elif s == 2:
        return 'TOUR_' + pt + '/'
    elif s == 3:
        return 'TOUR_' + pt + '/'
    elif s == 4:
        return ''
    else:
        sys.exit("UNKNOWN SELECTION")

# return the number of rows in a csv file
def CountRows(file_name):
    # create pandas data frame of entire csv
    df = pd.read_csv(file_name)
    gens = df['gen'].to_list()

    return gens[-1]

# responsible for looking through the data directories for success
def CheckDir(dir, sel, dia, offs, obj, acc, gens, pt):

    # check if data dir exists
    if os.path.isdir(dir):
        print('Data dirctory exists=', dir)
    else:
        print('DOES NOT EXIST=', dir)
        sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check if selection scheme data folder exists
    SEL_DIR = dir + SetSelection(sel) + '/TRT_' + obj + '__ACC_' + acc + '__GEN_' + gens + '/'
    if os.path.isdir(SEL_DIR):
        print('Selection scheme data folder exists', SEL_DIR)
    else:
        print('SELECTION DIRECTORY DOES NOT EXIST=', SEL_DIR)
        sys.exit('SELECTION DATA DIRECTORY DOES NOT EXIST')

    # create seed data directories and check if exist
    VLIST = SetVarList(sel)
    DIR_DNE = []
    DAT_DNE = []
    DAT_DNF = []

    SECOND_PARAM = SetSecondParam(sel, pt)

    print('Full data Dir=', SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/' + SECOND_PARAM)
    print('Now checking data replicates sub directories')

    SEEDS = SetSeeds(sel)

    for s in SEEDS:
        seed = str(s + offs)
        var_val = str(VLIST[int((s-1)/SMAX)])
        DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/' + SECOND_PARAM
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


def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("selection",      type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Novelty Search\n4: Espilon Lexicase")
    parser.add_argument("diagnostic",     type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Ecology Contradictory Traits\n3: Exploration")
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
    print('Selection scheme=', SetSelection(selection))
    diagnostic = args.diagnostic
    print('Diagnostic=',SetDiagnostic(diagnostic))
    offset = args.seed_offset
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