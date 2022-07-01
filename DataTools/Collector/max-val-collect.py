#####################################################################################################
#
# Creates a csv with data that consists of the best data found throughout an evolutionary run per
# replicate (e.g. the best performance througout an entire evolutionary run)
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
# Output: csv with best data for a specific selection scheme and diagnostic
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

    # Will hold the vars
    TRT = []
    VAL = []
    COL = []
    GEN = []

    # data traversing
    data = {data_params.POP_FIT_AVG, data_params.POP_FIT_MAX, data_params.POP_OPT_AVG,
            data_params.POP_OPT_MAX, data_params.POP_UNI_OBJ, data_params.POP_STR_AVG,
            data_params.POP_STR_MAX, data_params.ELE_AGG_PER, data_params.ELE_OPT_CNT,
            data_params.ARC_ACTI_GENE, data_params.OVERLAP, data_params.ARCHIVE_CNT,
            data_params.UNI_STR_POS, data_params.PMIN, data_params.PARETO_CNT }

    for s in SEEDS:
        seed = str(s + offs)
        it = int((s-1)/data_params.REPLICATES)
        var_val = str(VLIST[it])
        DATA_DIR =  SEL_DIR + 'DIA_' + data_params.SetDiagnostic(dia) + '__' + data_params.SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/'
        print('Sub data directory:', DATA_DIR+'data.csv')

        # get data from file and check if can store it
        df = pd.read_csv(DATA_DIR+'data.csv')

        for key in data:
            # Population fit average
            # Population fit max
            max_val = df[key].max()
            max_gen = df[df[key] == max_val][data_params.GENERATION].values.tolist()[0]
            TRT.append(VLIST[it])
            VAL.append(max_val)
            GEN.append(max_gen)
            COL.append(key)

    # Time to export the csv file
    # time to export the data
    fdf = pd.DataFrame({'trt': pd.Series(TRT),
                    'val': pd.Series(VAL),
                    'gen': pd.Series(GEN),
                    'col': pd.Series(COL)})

    fdf.to_csv(path_or_buf= dump + 'best-' + data_params.SetDiagnostic(dia).lower() + '-' + data_params.SetSelection(sel,pt).lower() + '.csv', index=False)

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