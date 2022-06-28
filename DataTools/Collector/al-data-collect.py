#####################################################################################################
#####################################################################################################
# Create csv's with requested data from all replicates every X generations
#
#
# Output : csv with for data over time
#
# python3
#####################################################################################################
#####################################################################################################

######################## IMPORTS ########################
import pandas as pd
import argparse
import sys
import os

# file location for data-params.py file
sys.path.insert(1, '../')
import data_params

# loop through differnt files that exist
def DirExplore(data, dump, sel, dia, offs, res, obj, acc, gens, pt):
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
        sys.exit('SELECTION DIRECTORY NOT FOUND')

    # Set vars that we need to loop through
    VLIST = data_params.SetVarList(sel)
    SEEDS = data_params.SetSeedSets(sel)

    # gens we are expecting
    GEN_LIST = [x for x in range(int(gens)+1) if x%res == 0]
    # data frame list for concatanation
    DF_LIST = []

    # iterate through the sets of seeds
    for i in range(len(SEEDS)):
        seeds = SEEDS[i]
        print('i=',i)

        TRT = [VLIST[i]] * len(GEN_LIST)

        # iterate through seeds to collect data
        for s in seeds:
            it = int((s-1)/data_params.REPLICATES)
            var_val = str(VLIST[it])
            seed = str(s + offs)
            DATA_DIR =  SEL_DIR + 'DIA_' + data_params.SetDiagnostic(dia) + '__' + data_params.SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/' + 'data.csv'

            # check if data file even exists
            if os.path.isfile(DATA_DIR) == False:
                print('DNE: ' + DATA_DIR)
                continue

            # create pandas data frame of entire csv and grab the row
            df = pd.read_csv(DATA_DIR)
            df = df.iloc[::res, :]

            # time to export the data
            cdf = pd.DataFrame(
                {   'gen': pd.Series(GEN_LIST),
                    'trt': pd.Series(TRT),
                    data_params.POP_FIT_AVG: pd.Series(df[data_params.POP_FIT_AVG].tolist()),
                    data_params.POP_FIT_MAX: pd.Series(df[data_params.POP_FIT_MAX].tolist()),
                    data_params.POP_OPT_AVG: pd.Series(df[data_params.POP_OPT_AVG].tolist()),
                    data_params.POP_OPT_MAX: pd.Series(df[data_params.POP_OPT_MAX].tolist()),
                    data_params.POP_UNI_OBJ: pd.Series(df[data_params.POP_UNI_OBJ].tolist()),
                    data_params.POP_STR_AVG: pd.Series(df[data_params.POP_STR_AVG].tolist()),
                    data_params.POP_STR_MAX: pd.Series(df[data_params.POP_STR_MAX].tolist()),
                    data_params.ELE_AGG_PER: pd.Series(df[data_params.ELE_AGG_PER].tolist()),
                    data_params.ELE_OPT_CNT: pd.Series(df[data_params.ELE_OPT_CNT].tolist()),
                    data_params.ARC_ACTI_GENE: pd.Series(df[data_params.ARC_ACTI_GENE].tolist()),
                    data_params.OVERLAP: pd.Series(df[data_params.OVERLAP].tolist()),
                    data_params.ARCHIVE_CNT: pd.Series(df[data_params.ARCHIVE_CNT].tolist()),
                    data_params.UNI_STR_POS:  pd.Series(df[data_params.UNI_STR_POS].tolist()),
                    data_params.PMIN:        pd.Series(df[data_params.PMIN].tolist()),
                    data_params.PARETO_CNT :  pd.Series(df[data_params.PARETO_CNT].tolist())
                })
            DF_LIST.append(cdf)

    fin_df = pd.concat(DF_LIST)

    fin_df.to_csv(path_or_buf= dump + 'over-time-' + data_params.SetDiagnostic(dia).lower() + '-' + gens + '-' + obj + '-' + acc + '.csv', index=False)

def main():
    # Generate and get the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_dir",    type=str, help="Target experiment directory.")
    parser.add_argument("dump_dir",    type=str, help="Data dumping directory")
    parser.add_argument("selection",      type=int, help="Selection scheme we are looking for? \n0: Truncation\n1: Tournament\n2: Fitness Sharing\n4: Espilon Lexicase\n6: Nondominated Sorting\n7: Novelty Search")
    parser.add_argument("diagnostic",     type=int, help="Diagnostic we are looking for?\n0: Exploitation\n1: Structured Exploitation\n2: Strong Ecology\n3: Exploration\n4: Weak Ecology")
    parser.add_argument("seed_offset", type=int, help="Experiment seed offset. (REPLICATION_OFFSET + PROBLEM_SEED_OFFSET")
    parser.add_argument("resolution",  type=int, help="The resolution desired for the data extraction")
    parser.add_argument("objectives", type=str, help="Number of objectives being optimized")
    parser.add_argument("accuracy", type=str, help="Accuracy for experiment")
    parser.add_argument("generations", type=str, help="Number of generations experiments ran for")
    parser.add_argument("param_two",      type=str, help="Second paramater for any selection scheme")

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_dir.strip()
    print('Data directory=',data_dir)
    dump_dir = args.dump_dir.strip()
    print('Dump directory=', dump_dir)
    selection = args.selection
    print('Selection scheme=', data_params.SetSelection(selection, args.param_two))
    diagnostic = args.diagnostic
    print('Diagnostic=', data_params.SetDiagnostic(diagnostic))
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
    param_two = args.param_two
    print('2nd param=', param_two)

    # Get to work!
    print("\nChecking all related data directories now!")
    DirExplore(data_dir, dump_dir, selection, diagnostic, offset, resolution, objectives, accuracy, generations, param_two)

if __name__ == "__main__":
    main()