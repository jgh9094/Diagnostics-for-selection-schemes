#####################################################################################################
#####################################################################################################
# Will create a csv per treatment of generation a perfect solution is found
#
# Output : csv with maximum value for data found at a specific generaiton
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
FS_LIST = ['0.0','0.1','0.3','0.6','1.2','2.5','5.0','10.0']
NS_LIST = [0,1,2,4,8,15,30,60]
# seed experiements replicates range
SMAX = 50
# name of column we need to extract
POP_FIT_AVG = 'pop_fit_avg'
POP_FIT_MAX = 'pop_fit_max'
POP_OPT_AVG = 'pop_opt_avg'
POP_OPT_MAX = 'pop_opt_max'
POP_UNI_OBJ = 'pop_uni_obj'
GENERATION = 'gen'

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
    elif s == 5:
        return 'NOVELTY-ECULID'
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
    elif s == 5:
        return 'NOV'
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
    elif s == 5:
        return [x for x in range(1,401)]
    else:
        sys.exit('SEEDS SELECTION UNKNOWN')

# Will set the appropiate list of variables we are checking for
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
    elif s == 5:
        return NS_LIST
    else:
        sys.exit("UNKNOWN VARIABLE LIST")

# make sure our list is sorted
def sorted(v):
    for i in range(len(v)-1):
        if v[i] > v[i+1]:
            return False

    return True

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
    elif s == 5:
        return 'TOUR_' + pt + '/'
    else:
        sys.exit("UNKNOWN SELECTION")

# create a pandas dataframe of csv and find if optimal solutions exist
def FindSolGen(file, cnt):
    # check and make sure that the file exists
    if os.path.isfile(file) == False:
        sys.exit('DATA FILE DOES NOT EXIST')

    # create pandas data frame of entire csv
    df = pd.read_csv(file)

    # create subset of data frame with only winning solutions
    df = df[df[POP_OPT_MAX] == int(cnt)]
    gens = df['gen'].tolist()

    # check if there are any gens where optimal solution is found
    if(len(gens) == 0):
        return -1
    else:
        if sorted(gens) == False:
            sys.exit('GENERATION LIST NOT SORTED')
        return gens[0]

# create solution list with appropiate number of lists
def SetSolList(s):
    sol = []
    if s == 0 or s == 1:
        for i in range(10):
            sol.append([])
        return sol

    elif s == 2 or s == 3 or s == 4 or s == 5:
        for i in range(8):
            sol.append([])
        return sol

    else:
        sys.exit('SOL LIST SELECTION UKNOWN')

# loop through differnt files that exist
def DirExplore(data, dump, sel, dia, offs, obj, acc, gens, pt):
    # check if data dir exists
    if os.path.isdir(data) == False:
        print('DATA=', data)
        sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check if data dir exists
    if os.path.isdir(dump) == False:
        print('DATA=', data)
        sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check that selection data folder exists
    SEL_DIR = data + SetSelection(sel) + '/TRT_' + obj + '__ACC_' + acc + '__GEN_' + gens + '/'
    if os.path.isdir(SEL_DIR) == False:
        print('SEL_DIR=', SEL_DIR)
        sys.exit('EXIT -1')

    # loop through sub data directories
    print('Full data Dir=', SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/')
    print('Now checking data replicates sub directories')
    VLIST = SetVarList(sel)
    SEEDS = SetSeeds(sel)

    # Will hold the vars
    TRT = []
    VAL = []
    COL = []
    GEN = []
    # second parameter dir
    SECOND_PARAM = SetSecondParam(sel, pt)

    for s in SEEDS:
        seed = str(s + offs)
        it = int((s-1)/SMAX)
        var_val = str(VLIST[it])
        DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/' + SECOND_PARAM
        print('Sub data directory:', DATA_DIR+'data.csv')

        # get data from file and check if can store it
        df = pd.read_csv(DATA_DIR+'data.csv')

        # Population fit average
        # Population fit max
        max_val = df[POP_FIT_AVG].max()
        max_gen = df[df[POP_FIT_AVG] == max_val][GENERATION].values.tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('pfa')

        # Population fit max
        max_val = df[POP_FIT_MAX].max()
        max_gen = df[df[POP_FIT_MAX] == max_val][GENERATION].values.tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('pfm')

        # Population optimal avg
        max_val = df[POP_OPT_AVG].max()
        max_gen = df[df[POP_OPT_AVG] == max_val][GENERATION].values.tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('poa')

        # Population optimal max
        max_val = df[POP_OPT_MAX].max()
        max_gen = df[df[POP_OPT_MAX] == max_val][GENERATION].values.tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('pom')

        # Population unique objectives
        max_val = df[POP_UNI_OBJ].max()
        max_gen = df[df[POP_UNI_OBJ] == max_val][GENERATION].values.tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('puo')

    # Time to export the csv file
    # time to export the data
    fdf = pd.DataFrame({'trt': pd.Series(TRT),
                    'val': pd.Series(VAL),
                    'gen': pd.Series(GEN),
                    'col': pd.Series(COL)})

    fdf.to_csv(path_or_buf= dump + 'vxg-' + SetDiagnostic(dia).lower() + '-' + gens + '-' + obj + '-' + acc + '.csv', index=False)

def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_dir",    type=str, help="Target experiment directory.")
    parser.add_argument("dump_dir",    type=str, help="Data dumping directory")
    parser.add_argument("selection",      type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Aggregate Novelty\n4: Espilon Lexicase\n5: Euclidean Novelty")
    parser.add_argument("diagnostic",     type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Strong Ecology\n3: Exploration\n4: Weak Ecology")
    parser.add_argument("seed_offset", type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("objectives", type=str, help="Number of objectives being optimized")
    parser.add_argument("accuracy", type=str, help="Accuracy for experiment")
    parser.add_argument("generations", type=str, help="Number of generations experiments ran for")
    parser.add_argument("param_two",    type=str, help="Second paramater for any selection scheme")

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_dir.strip()
    print('Data directory=',data_dir)
    dump_dir = args.dump_dir.strip()
    print('Dump directory=', dump_dir)
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
    DirExplore(data_dir, dump_dir, selection, diagnostic, offset, objectives, accuracy, generations, param_two)

if __name__ == "__main__":
    main()