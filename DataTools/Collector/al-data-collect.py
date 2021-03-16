#####################################################################################################
#####################################################################################################
# Will create a csv how alex's r code expects data
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
import pandas as pd
import numpy as np
import argparse
import math as mth
import sys
import os

# variables we are testing for each replicate range
MU_LIST = [1,2,4,8,16,32,64,128,256,512]
TR_LIST = [1,2,4,8,16,32,64,128,256,512]
LX_LIST = [0.0,0.1,0.3,0.6,1.2,2.5,5.0,10.0]
FS_LIST = [0.0,10.0,30.0,60.0,12.0,250.0,500.0,1000.0]
NS_LIST = [0,1,2,4,8,15,30,60]
# seed experiements replicates range
REP_NUM = 50
# columns we are interested in grabbing
POP_FIT_AVG = 'pop_fit_avg'
POP_FIT_MAX = 'pop_fit_max'
POP_OPT_AVG = 'pop_opt_avg'
POP_UNI_OBJ = 'pop_uni_obj'
COM_SOL_CNT = 'com_sol_cnt'
ELE_AGG_PER = 'ele_agg_per'
ELE_OPT_CNT = 'ele_opt_cnt'
COM_AGG_PER = 'com_agg_per'
COM_OPT_CNT = 'com_opt_cnt'
OPT_AGG_PER = 'opt_agg_per'
OPT_OBJ_CNT = 'opt_obj_cnt'

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
    if s == 0 or s == 1:
        seed = []
        seed.append([x for x in range(1,51)])
        seed.append([x for x in range(51,101)])
        seed.append([x for x in range(101,151)])
        seed.append([x for x in range(151,201)])
        seed.append([x for x in range(201,251)])
        seed.append([x for x in range(251,301)])
        seed.append([x for x in range(301,351)])
        seed.append([x for x in range(351,401)])
        seed.append([x for x in range(401,451)])
        seed.append([x for x in range(451,501)])
        return seed

    elif s == 2 or s == 3 or s == 4:
        seed = []
        seed.append([x for x in range(1,51)])
        seed.append([x for x in range(51,101)])
        seed.append([x for x in range(101,151)])
        seed.append([x for x in range(151,201)])
        seed.append([x for x in range(201,251)])
        seed.append([x for x in range(251,301)])
        seed.append([x for x in range(301,351)])
        seed.append([x for x in range(351,401)])
        return seed

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

# loop through differnt files that exist
def DirExplore(data, dump, sel, dia, offs, res, obj, acc, gens):
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
        sys.exit('SELECTION DIRECTORY NOT FOUND')

    # Set vars that we need to loop through
    VLIST = SetVarList(sel)
    SEEDS = SetSeeds(sel)

    # gens we are expecting
    GEN_LIST = [x for x in range(int(gens)+1) if x%res == 0]
    # data frame list for concatanation
    DF_LIST = []

    # iterate through the sets of seeds
    for i in range(len(SEEDS)):
        seeds = SEEDS[i]
        var_val = str(VLIST[i])
        print('i=',i)

        TRT = [VLIST[i]] * len(GEN_LIST)

        # iterate through seeds to collect data
        for s in seeds:
            seed = str(s + offs)
            DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/data.csv'

            # create pandas data frame of entire csv and grab the row
            df = pd.read_csv(DATA_DIR)
            df = df.iloc[::res, :]

            # time to export the data
            cdf = pd.DataFrame({'gen': pd.Series(GEN_LIST),
                            'treatnt': pd.Series(TRT),
                            'fit_avg': pd.Series(df[POP_FIT_AVG].tolist()),
                            'fit_max': pd.Series(df[POP_FIT_MAX].tolist()),
                            'opt_avg': pd.Series(df[POP_OPT_AVG].tolist()),
                            'uni_avg': pd.Series(df[POP_UNI_OBJ].tolist()),
                            'com_cnt': pd.Series(df[COM_SOL_CNT].tolist())})

            DF_LIST.append(cdf)

    fin_df = pd.concat(DF_LIST)

    fin_df.to_csv(path_or_buf= dump + SetDiagnostic(dia).lower() + '-' + gens + '-' + obj + '-' + acc + '.csv', index=False)

def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_dir",    type=str, help="Target experiment directory.")
    parser.add_argument("dump_dir",    type=str, help="Data dumping directory")
    parser.add_argument("selection",   type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Novelty Search\n4: Espilon Lexicase")
    parser.add_argument("diagnostic",  type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Ecology Contradictory Traits\n3: Exploration")
    parser.add_argument("seed_offset", type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("resolution",  type=int, help="The resolution desired for the data extraction")
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
    resolution = args.resolution
    print('Resolution=', resolution)
    objectives = args.objectives
    print('Objectives=', objectives)
    accuracy = args.accuracy
    print('Accuracy=', accuracy)
    generations = args.generations
    print('Generations=', generations)

    # Get to work!
    print("\nChecking all related data directories now!")
    DirExplore(data_dir, dump_dir, selection, diagnostic, offset, resolution, objectives, accuracy, generations)

if __name__ == "__main__":
    main()