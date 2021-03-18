#####################################################################################################
#####################################################################################################
# Will create a csv per treatment of generation a perfect solution is found
#
# Command Line Inputs
#
# Input 1: file directory location
# Input 2: file dump directory
#
# Output : csv with generations found in dump directory
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
# 40001 gens=
EXPECTED_GENS = 40000
# name of column we need to extract
POP_FIT_AVG = 'pop_fit_avg'
POP_FIT_MAX = 'pop_fit_max'
POP_OPT_AVG = 'pop_opt_avg'
POP_OPT_MAX = 'pop_opt_max'
POP_UNI_OBJ = 'pop_uni_obj'
COM_SOL_CNT = 'com_sol_cnt'
GEN = 'gen'

# return appropiate string dir name (based off run.sb file naming system)
def SetSelection(s):
    # case by case
    if s == 0:
        return 'MULAMBDA'
    elif s == 1:
        return 'TOURNAMENT'
    elif s == 2:
        return 'SHARING'
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
    elif s == 2:
        return 'SIG'
    elif s == 3:
        return 'K'
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
    else:
        sys.exit("UNKNOWN VARIABLE LIST")

# make sure our list is sorted
def sorted(v):
    for i in range(len(v)-1):
        if v[i] > v[i+1]:
            return False

    return True

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

    elif s == 2 or s == 3 or s == 4:
        for i in range(8):
            sol.append([])
        return sol

    else:
        sys.exit('SOL LIST SELECTION UKNOWN')

# loop through differnt files that exist
def DirExplore(data, dump, sel, dia, offs, obj, acc, gens):
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

    for s in SEEDS:
        seed = str(s + offs)
        it = int((s-1)/SMAX)
        var_val = str(VLIST[it])
        DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/'
        print('Sub data directory:', DATA_DIR+'data.csv')

        # get data from file and check if can store it
        df = pd.read_csv(DATA_DIR+'data.csv')

        # Population fit average
        max_val = df[POP_FIT_AVG].max()
        max_gen = df[df[POP_FIT_AVG] == max_val][GEN].tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('pfa')

        # Population fit max
        max_val = df[POP_FIT_MAX].max()
        max_gen = df[df[POP_FIT_MAX] == max_val][GEN].tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('pfm')

        # Population optimal avg
        max_val = df[POP_OPT_AVG].max()
        max_gen = df[df[POP_OPT_AVG] == max_val][GEN].tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('poa')

        # Population optimal max
        max_val = df[POP_OPT_MAX].max()
        max_gen = df[df[POP_OPT_MAX] == max_val][GEN].tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('pom')

        # Population unique objectives
        max_val = df[POP_UNI_OBJ].max()
        max_gen = df[df[POP_UNI_OBJ] == max_val][GEN].tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('puo')

        # Population common solution count
        max_val = df[COM_SOL_CNT].max()
        max_gen = df[df[COM_SOL_CNT] == max_val][GEN].tolist()[0]
        TRT.append(VLIST[it])
        VAL.append(max_val)
        GEN.append(max_gen)
        COL.append('csc')

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
    parser.add_argument("selection",   type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Novelty Search\n4: Espilon Lexicase")
    parser.add_argument("diagnostic",  type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Ecology Contradictory Traits\n3: Exploration")
    parser.add_argument("seed_offset", type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("objectives", type=str, help="Number of objectives being optimized")
    parser.add_argument("accuracy", type=str, help="Accuracy for experiment")
    parser.add_argument("generations", type=str, help="Number of generations experiments ran for")

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

    # Get to work!
    print("\nChecking all related data directories now!")
    # DirExplore(data_dir, dump_dir, selection, diagnostic, offset, objectives, accuracy, generations)

    # df = pd.read_csv('data.csv')
    # pop_fit_avg = df[POP_FIT_MAX].max()
    # pfa_gen = df[df[POP_FIT_MAX] == pop_fit_avg][GEN].tolist()[0]

    # print(pop_fit_avg)
    # print(pfa_gen)

if __name__ == "__main__":
    main()