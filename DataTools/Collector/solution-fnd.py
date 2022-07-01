#####################################################################################################
#
# Creates a csv with generation a satisfactory solution is found
# If found, will add the earliest generation a satisfactory solution is found
# If not found, will leave an NA
#
# Command Line Inputs:
#
# data_directory: directory where data is located
# dump_directory: directory where data is located
#      selection: selection scheme used
#     diagnostic: diagnostic used
#    seed_offset: seed offset (if any)
#     objectives: dimensionality
#       accuracy: satisfactory trait accuracy %
#    generations: generations ran
#      param_two: genotypic (0) or phenotypic (1) similarity for fitness sharing
#
# Output: csv with generations a satisfactory solution is found for a specific selection scheme and
#          diagnostic
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
    df = df[df[data_params.POP_OPT_MAX] == int(cnt)]
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
        for i in range(9):
            sol.append([])
        return sol

    elif s == 2 or s == 3 or s == 4 or s == 5:
        for i in range(7):
            sol.append([])
        return sol

    else:
        sys.exit('SOL LIST SELECTION UKNOWN')

# create csv time
def ExportCSV(sol_list, var_list,s,d,dump, obj, acc, gens, pt):
    if s == 0 or s == 1:
        df = pd.DataFrame({var_list[0]: pd.Series(sol_list[0]),
                           var_list[1]: pd.Series(sol_list[1]),
                           var_list[2]: pd.Series(sol_list[2]),
                           var_list[3]: pd.Series(sol_list[3]),
                           var_list[4]: pd.Series(sol_list[4]),
                           var_list[5]: pd.Series(sol_list[5]),
                           var_list[6]: pd.Series(sol_list[6]),
                           var_list[7]: pd.Series(sol_list[7]),
                           var_list[8]: pd.Series(sol_list[8])})

        df.to_csv(path_or_buf= dump + 'ssf-' + data_params.SetDiagnostic(d).lower() + + data_params.SetSelection(s,pt).lower() + '.csv', index=False)

    elif s == 2  or s == 3 or s == 4 or s == 5:
        df = pd.DataFrame({var_list[0]: pd.Series(sol_list[0]),
                           var_list[1]: pd.Series(sol_list[1]),
                           var_list[2]: pd.Series(sol_list[2]),
                           var_list[3]: pd.Series(sol_list[3]),
                           var_list[4]: pd.Series(sol_list[4]),
                           var_list[5]: pd.Series(sol_list[5]),
                           var_list[6]: pd.Series(sol_list[6])})

        df.to_csv(path_or_buf= dump + 'ssf-' + data_params.SetDiagnostic(d).lower() + + data_params.SetSelection(s,pt).lower() + '.csv', index=False)

    else:
        sol_list.exit('SOL LIST SELECTION UKNOWN')

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
    SEL_DIR = data + data_params.SetSelection(sel,pt) + '/TRT_' + obj + '__ACC_' + acc + '__GEN_' + gens + '/'
    if os.path.isdir(SEL_DIR) == False:
        print('SEL_DIR=', SEL_DIR)
        sys.exit('EXIT -1')

    # loop through sub data directories
    print('Full data Dir=', SEL_DIR + 'DIA_' + data_params.SetDiagnostic(dia) + '__' + data_params.SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/')
    print('Now checking data replicates sub directories')
    VLIST = data_params.SetVarList(sel)
    SEEDS = data_params.SetSeeds(sel)

    # create list of list solution counts
    SOL_LIST = SetSolList(sel)

    for s in SEEDS:
        seed = str(s + offs)
        it = int((s-1)/data_params.REPLICATES)
        var_val = str(VLIST[it])
        DATA_DIR =  SEL_DIR + 'DIA_' + data_params.SetDiagnostic(dia) + '__' + data_params.SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/'
        print('Sub data directory:', DATA_DIR+'data.csv')

        # get data from file and check if can store it
        sol = FindSolGen(DATA_DIR+'data.csv', obj)
        if 0 <= sol:
            SOL_LIST[it].append(sol)

    # Time to export the csv file
    ExportCSV(SOL_LIST, VLIST, sel, dia, dump, obj, acc, gens, pt)


def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_dir",     type=str, help="Target experiment directory.")
    parser.add_argument("dump_dir",     type=str, help="Data dumping directory")
    parser.add_argument("selection",      type=int, help="Selection scheme we are looking for? \n0: (μ,λ)\n1: Tournament\n2: Fitness Sharing\n3: Aggregate Novelty\n4: Espilon Lexicase\n5: Euclidean Novelty")
    parser.add_argument("diagnostic",     type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Strong Ecology\n3: Exploration\n4: Weak Ecology")
    parser.add_argument("seed_offset",  type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("objectives",   type=str, help="Number of objectives being optimized")
    parser.add_argument("accuracy",     type=str, help="Accuracy for experiment")
    parser.add_argument("generations",  type=str, help="Number of generations experiments ran for")
    parser.add_argument("param_two",    type=str, help="Second paramater for any selection scheme")

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_dir.strip()
    print('Data directory=',data_dir)
    dump_dir = args.dump_dir.strip()
    print('Dump directory=', dump_dir)
    selection = args.selection
    print('Selection scheme=', data_params.SetSelection(selection,args.param_two))
    diagnostic = args.diagnostic
    print('Diagnostic=', data_params.SetDiagnostic(diagnostic))
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