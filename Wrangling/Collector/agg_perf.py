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
# 40001 gens=
GENERATIONS = 40001
# name of column we need to extract
POP_FIT_AVG = 'pop_fit_avg'
# optimal count expecting (depending on experiment config)
OPTI_CNT = 100
# list of total generations
GEN_LIST = [x for x in range(GENERATIONS)]

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
def DirExplore(data, dump, sel, dia, offs, res):
    # check if data dir exists
    if os.path.isdir(data) == False:
        print('DATA=', data)
        sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check if data dir exists
    if os.path.isdir(dump) == False:
        print('DATA=', data)
        sys.exit('DATA DIRECTORY DOES NOT EXIST')

    # check that selection data folder exists
    SEL_DIR = data + SetSelection(sel) + '/'
    if os.path.isdir(SEL_DIR) == False:
        print('SEL_DIR=', SEL_DIR)
        sys.exit('EXIT -1')

    # Set vars that we need to loop through
    VLIST = SetVarList(sel)
    SEEDS = SetSeeds(sel)

    # create list for each thing we need to track
    GENERATION = []
    AGGREGATE = []
    DEVIATION = []
    TREATEMENT = []

    # iterate through the sets of seeds
    for i in range(len(SEEDS)):
        seeds = SEEDS[i]
        var_val = str(VLIST[i])
        print('i=',i)

        # iterate through seeds to get the mean treatment
        mean = [0] * int(GENERATIONS/res)
        for s in seeds:
            seed = str(s + offs)
            DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/data.csv'

            # create pandas data frame of entire csv and grab the row
            df = pd.read_csv(DATA_DIR)
            df = df.iloc[::res, :]
            agg = df[POP_FIT_AVG].tolist()

            # add lists and continue
            mean = np.add(mean, agg)

        # final touches to mean
        mean = [x/REP_NUM for x in mean]

        # iterate through seeds to std var per treatment
        std = [0] * int(GENERATIONS/res)
        for s in seeds:
            seed = str(s + offs)
            DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/data.csv'

            # create pandas data frame of entire csv and grab the row
            df = pd.read_csv(DATA_DIR)
            df = df.iloc[::res, :]
            agg = df[POP_FIT_AVG].tolist()

            # substract agg from mean and square it
            summ = np.subtract(agg, mean)
            summ = [x**2 for x in summ]
            std = np.add(std, summ)

        # finishing touches on standard deviation: divide by repliation number and square root
        std = [mth.sqrt(x/REP_NUM) for x in std]

        GENERATION = GENERATION + GEN_LIST[::res]
        AGGREGATE = AGGREGATE + mean
        DEVIATION = DEVIATION + std
        TREATEMENT = TREATEMENT + [var_val] * int(GENERATIONS/res)

    # time to export the data
    df = pd.DataFrame({'gen': pd.Series(GENERATION),
                       'agg': pd.Series(AGGREGATE),
                       'dev': pd.Series(DEVIATION),
                       'trt': pd.Series(TREATEMENT)})

    df.to_csv(path_or_buf= dump + SetDiagnostic(dia) + 'PERF_AGG.csv', index=False)


def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_dir",    type=str, help="Target experiment directory.")
    parser.add_argument("dump_dir",    type=str, help="Data dumping directory")
    parser.add_argument("selection",   type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Novelty Search\n4: Espilon Lexicase")
    parser.add_argument("diagnostic",  type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Ecology Contradictory Traits\n3: Exploration")
    parser.add_argument("seed_offset", type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("resolution",  type=int, help="The resolution desired for the data extraction")

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

    # Get to work!
    print("\nChecking all related data directories now!")
    DirExplore(data_dir, dump_dir, selection, diagnostic, offset, resolution)

if __name__ == "__main__":
    main()


# DO NOT USE PANDAS
# TOO SLOW
# USE https://stackoverflow.com/questions/27307385/best-way-to-access-the-nth-line-of-csv-file/27307452