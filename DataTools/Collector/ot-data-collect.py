#####################################################################################################
#
# Creates a csv with data over time with some for some given resolutions (e.g. number of generations)
# per selection/diagnotic treatment
#
# Script will go through each replicate for a specific selection/diagnostic treatment
#
# Command Line Inputs:
#
# data_directory: directory where data is located
# dump_directory: directory where data is located
#      selection: selection scheme used
#     diagnostic: diagnostic used
#    seed_offset: seed offset (if any)
#     resolution: resolution of the over time data to gather (e.g. collect data every 100 generations)
#     objectives: dimensionality
#       accuracy: satisfactory trait accuracy %
#    generations: generations ran
#      param_two: genotypic (0) or phenotypic (1) similarity for fitness sharing
#
# Output: csv with over time data for a specific selection scheme and diagnostic
#
# python3
#####################################################################################################

######################## IMPORTS ########################
import pandas as pd
import argparse
import sys
import os

# file location for data-params.py file
sys.path.insert(1, '../')
import data_params as dp

    # loop through differnt files that exist
def DirExplore(data, dump, sel, dia, offs, pt, val):

    # check that selection data folder exists
    SEL_DIR = dp.GetDataDirectory(data,sel,pt)

    if val:
        print('Full data Dir=', SEL_DIR + 'DIA_' + dp.SetDiagnostic(dia) + '__' + dp.SetSelectionVar(sel) + '_XXX' + '__SEED_XXXXXX__MVC/')
    else:
        print('Full data Dir=', SEL_DIR + 'DIA_' + dp.SetDiagnostic(dia) + '__' + dp.SetSelectionVar(sel) + '_XXX' + '__SEED_XXXXXX/')

    # loop through sub data directories
    print('Full data Dir=', SEL_DIR + 'DIA_' + dp.SetDiagnostic(dia) + '__' + dp.SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/')
    print('Now checking data replicates sub directories')

    SEEDS = dp.SetSeedSets(sel)
    # gens we are expecting
    GEN_LIST = [x for x in range(int(dp.GENERATIONS)+1) if x % dp.RESOLUTION == 0]
    # collect all data
    DF_LIST = []

    # iterate through the sets of seeds
    for i in range(len(SEEDS)):
        for s in SEEDS[i]:
            seed = str(s + offs)
            DATA_DIR =  SEL_DIR + 'DIA_' + dp.SetDiagnostic(dia) + '__' + dp.SetSelectionVar(sel) + '_' + dp.SetVarList(sel)[i] + '__SEED_' + seed
            if val:
                DATA_DIR += '__MVC/data.csv'
            else:
                DATA_DIR += '/data.csv'

            print('Sub directory:', DATA_DIR)

            # get data from file and check if can store it
            df = pd.read_csv(DATA_DIR)
            df = df.iloc[::dp.RESOLUTION, :]

            # time to export the data
            cdf = pd.DataFrame(
                {   'gen': pd.Series(GEN_LIST),
                    'trt': pd.Series([dp.SetVarList(sel)[i]] * len(GEN_LIST)),
                    dp.POP_FIT_AVG:   pd.Series(df[dp.POP_FIT_AVG].tolist()),
                    dp.POP_FIT_MAX:   pd.Series(df[dp.POP_FIT_MAX].tolist()),
                    dp.POP_OPT_MAX:   pd.Series(df[dp.POP_OPT_MAX].tolist()),
                    dp.POP_UNI_OBJ:   pd.Series(df[dp.POP_UNI_OBJ].tolist()),
                    dp.POP_STR_MAX:   pd.Series(df[dp.POP_STR_MAX].tolist()),
                    dp.ARC_ACTI_GENE: pd.Series(df[dp.ARC_ACTI_GENE].tolist()),
                    dp.OVERLAP:       pd.Series(df[dp.OVERLAP].tolist()),
                    dp.ARCHIVE_CNT:   pd.Series(df[dp.ARCHIVE_CNT].tolist()),
                    dp.UNI_STR_POS:   pd.Series(df[dp.UNI_STR_POS].tolist()),
                    dp.PMIN:          pd.Series(df[dp.PMIN].tolist()),
                    dp.POP_MAX_TRT:   pd.Series(df[dp.POP_MAX_TRT].tolist()),
                    dp.POP_MAX_GENE:  pd.Series(df[dp.POP_MAX_GENE].tolist()),
                    dp.PARETO_CNT :   pd.Series(df[dp.PARETO_CNT].tolist())
                })
            DF_LIST.append(cdf)

    fin_df = pd.concat(DF_LIST)

    fin_df.to_csv(path_or_buf= dump + 'over-time-' + dp.SetDiagnostic(dia).lower() + '-' + dp.SetSelection(sel,pt).lower() + '.csv', index=False)

def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_dir",      type=str, help="Target experiment directory.")
    parser.add_argument("dump_dir",      type=str, help="Data dumping directory")
    parser.add_argument("selection",     type=int, help="Selection scheme we are looking for? \n0: Truncation\n1: Tournament\n2: Fitness Sharing\n4: Espilon Lexicase\n6: Nondominated Sorting\n7: Novelty Search")
    parser.add_argument("diagnostic",    type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Strong Ecology\n3: Exploration\n4: Weak Ecology")
    parser.add_argument("seed_offset",   type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("param_two",     type=str, help="Second paramater for any selection scheme")
    parser.add_argument("--valleys",     type=int, help="True (1) or False (0) on whether or not valleys are applied", action='store', required=False)

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_dir.strip()
    print('Data directory=',data_dir)
    dump_dir = args.dump_dir.strip()
    print('Dump directory=', dump_dir)
    selection = args.selection
    print('Selection scheme=', dp.SetSelection(selection,args.param_two))
    diagnostic = args.diagnostic
    print('Diagnostic=', dp.SetDiagnostic(diagnostic))
    offset = args.seed_offset
    print('Offset=', offset)
    param_two = args.param_two
    print('2nd param=', param_two)
    valleys = bool(args.valleys)
    print('valleys=', valleys)

    # Get to work!
    print("\nChecking all related data directories now!")
    DirExplore(data_dir, dump_dir, selection, diagnostic, offset, param_two, valleys)

if __name__ == "__main__":
    main()