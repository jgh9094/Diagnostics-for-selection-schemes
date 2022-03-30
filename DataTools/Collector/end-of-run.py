#####################################################################################################
#####################################################################################################
# Create csv's with requested data from the last generation for all replicates
#
#
# Output : csv with for data over time
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
TR_LIST = ['1','2','4','8','16','32','64','128','256']
TS_LIST = ['1','2','4','8','16','32','64','128','256']
LX_LIST = ['0.0','0.1','0.3','0.6','1.2','2.5','5.0']
FS_LIST = ['0.0','0.1','0.3','0.6','1.2','2.5','5.0']
ND_LIST = ['0.0','0.1','0.3','0.6','1.2','2.5','5.0']
NS_LIST = ['0','1','2','4','8','15','30']

# seed experiements replicates range
REP_NUM = 50

# columns we are interested in grabbing
# pop level
POP_FIT_AVG = 'pop_fit_avg'
POP_FIT_MAX = 'pop_fit_max'
POP_OPT_AVG = 'pop_opt_avg'
POP_OPT_MAX = 'pop_opt_max'
POP_UNI_OBJ = 'pop_uni_obj'
POP_STR_AVG = 'pop_str_avg'
POP_STR_MAX = 'pop_str_max'
# elite solutions (max agg traits)
ELE_AGG_PER = 'ele_agg_per'
ELE_OPT_CNT = 'ele_opt_cnt'
# # optimal solution (max opti trait count)
# OPT_AGG_PER = 'opt_agg_per'
# OPT_OBJ_CNT = 'opt_obj_cnt'
# # streak solution (longest streak of non-zero values)
# STR_AGG_PER = 'str_agg_per'
# STR_OBJ_CNT = 'str_obj_cnt'
# trait coverage
UNI_STR_POS = 'uni_str_pos'
# # pareto data
# PARETO_CNT = 'pareto_cnt'
# # novelty data
# ARCHIVE_CNT = 'archive_cnt'
# PMIN = 'pmin'
# ARC_ELITE = 'arc_elite'
# ARC_OPTI = 'arc_opti'
# ARC_STRK = 'arc_strk'
GEN = 'gen'

# return appropiate string dir name (based off run.sb file naming system)
def SetSelection(s,p):
    # case by case
    if s == 0:
        return 'TRUNCATION'
    elif s == 1:
        return 'TOURNAMENT'
    elif s == 2:
        if p == '0':
            return 'FITSHARING_G'
        elif p == '1':
            return 'FITSHARING_P'
        else:
            sys.exit("UNKNOWN SELECTION")
    elif s == 3:
        return 'LEXICASE'
    elif s == 4:
        return 'NONDOMINATEDSORTING'
    elif s == 5:
        return 'NOVELTY'
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
        return 'TR'
    elif s == 1:
        return 'T'
    elif s == 2:
        return 'SIG'
    elif s == 3:
        return 'EPS'
    elif s == 4:
        return 'SIG'
    elif s == 5:
        return 'NOV'
    else:
        sys.exit("UNKNOWN SELECTION VAR")

# return the correct amount of seed ran by experiment treatment
def SetSeeds(s):
    # case by case
    if s == 0:
        return [x for x in range(1,451)]
    elif s == 1:
        return [x for x in range(1,451)]
    elif s == 2:
        return [x for x in range(1,351)]
    elif s == 3:
        return [x for x in range(1,351)]
    elif s == 4:
        return [x for x in range(1,351)]
    elif s == 5:
        return [x for x in range(1,351)]
    else:
        sys.exit('SEEDS SELECTION UNKNOWN')

# set the appropiate list of variables we are checking for
def SetVarList(s):
    # case by case
    if s == 0:
        return TR_LIST
    elif s == 1:
        return TS_LIST
    elif s == 2:
        return FS_LIST
    elif s == 3:
        return LX_LIST
    elif s == 4:
        return FS_LIST
    elif s == 5:
        return NS_LIST
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
        return ''
    elif s == 3:
        return ''
    elif s == 4:
        return ''
    elif s == 5:
        return ''
    else:
        sys.exit("UNKNOWN SELECTION")

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
    SEL_DIR = data + SetSelection(sel,pt) + '/TRT_' + obj + '__ACC_' + acc + '__GEN_' + gens + '/'
    if os.path.isdir(SEL_DIR) == False:
        print('SEL_DIR=', SEL_DIR)
        sys.exit('EXIT -1')

    # loop through sub data directories
    print('Full data Dir=', SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_XXX' + '__SEED_XXX' + '/')
    print('Now checking data replicates sub directories')
    VLIST = SetVarList(sel)
    SEEDS = SetSeeds(sel)

    # lists that will hold all the date
    PFA = []
    PFM = []
    POA = []
    POM = []
    PUO = []
    PSA = []
    PSM = []
    EAP = []
    EOC = []
    OAP = []
    OOC = []
    SAP = []
    SOC = []
    USP = []
    PC = []
    AC = []
    P = []
    AE = []
    AO = []
    AS = []
    TRT = []

    # second parameter dir
    SECOND_PARAM = SetSecondParam(sel, pt)

    for s in SEEDS:
        seed = str(s + offs)
        it = int((s-1)/REP_NUM)
        var_val = str(VLIST[it])
        DATA_DIR =  SEL_DIR + 'DIA_' + SetDiagnostic(dia) + '__' + SetSelectionVar(sel) + '_' + var_val + '__SEED_' + seed + '/' + SECOND_PARAM
        print('Sub data directory:', DATA_DIR+'data.csv')

        df = pd.read_csv(DATA_DIR + 'data.csv')
        dfl = df.iloc[[-1]]

        if dfl[GEN].tolist()[0] != int(gens):
            print('DNF:', DATA_DIR)
            continue
            # sys.exit('FILE DID NOT REACH GENS')

        TRT.append(VLIST[it])
        PFA.append(dfl[POP_FIT_AVG].tolist()[0])
        PFM.append(dfl[POP_FIT_MAX].tolist()[0])
        POA.append(dfl[POP_OPT_AVG].tolist()[0])
        POM.append(dfl[POP_OPT_MAX].tolist()[0])
        PUO.append(dfl[POP_UNI_OBJ].tolist()[0])
        PSA.append(dfl[POP_STR_AVG].tolist()[0])
        PSM.append(dfl[POP_STR_MAX].tolist()[0])
        EAP.append(dfl[ELE_AGG_PER].tolist()[0])
        EOC.append(dfl[ELE_OPT_CNT].tolist()[0])
        # OAP.append(dfl[OPT_AGG_PER].tolist()[0])
        # OOC.append(dfl[OPT_OBJ_CNT].tolist()[0])
        # SAP.append(dfl[STR_AGG_PER].tolist()[0])
        # SOC.append(dfl[STR_OBJ_CNT].tolist()[0])
        USP.append(dfl[UNI_STR_POS].tolist()[0])
        # PC.append(dfl[PARETO_CNT].tolist()[0])
        # AC.append(dfl[ARCHIVE_CNT].tolist()[0])
        # P.append(dfl[PMIN].tolist()[0])
        # AE.append(dfl[ARC_ELITE].tolist()[0])
        # AO.append(dfl[ARC_OPTI].tolist()[0])
        # AS.append(dfl[ARC_STRK].tolist()[0])

    # time to export the data
    fdf = pd.DataFrame({'trt': pd.Series(TRT),
                    POP_FIT_AVG: pd.Series(PFA),
                    POP_FIT_MAX: pd.Series(PFM),
                    POP_OPT_AVG: pd.Series(POA),
                    POP_OPT_MAX: pd.Series(POM),
                    POP_UNI_OBJ: pd.Series(PUO),
                    POP_STR_AVG: pd.Series(PSA),
                    POP_STR_MAX: pd.Series(PSM),
                    ELE_AGG_PER: pd.Series(EAP),
                    ELE_OPT_CNT: pd.Series(EOC),
                    # OPT_AGG_PER: pd.Series(OAP),
                    # OPT_OBJ_CNT: pd.Series(OOC),
                    # STR_AGG_PER: pd.Series(SAP),
                    # STR_OBJ_CNT: pd.Series(SOC),
                    UNI_STR_POS: pd.Series(USP)})
                    # PARETO_CNT:  pd.Series(PC),
                    # ARCHIVE_CNT: pd.Series(AC),
                    # PMIN:        pd.Series(P),
                    # ARC_ELITE:   pd.Series(AE),
                    # ARC_OPTI:    pd.Series(AO),
                    # ARC_STRK:    pd.Series(AS)})

    fdf.to_csv(path_or_buf= dump + 'eor-' + SetDiagnostic(dia).lower() + '-' + gens + '-' + obj + '-' + acc + '.csv', index=False)


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
    parser.add_argument("param_two",      type=str, help="Second paramater for any selection scheme")

    # Parse all the arguments
    args = parser.parse_args()
    data_dir = args.data_dir.strip()
    print('Data directory=',data_dir)
    dump_dir = args.dump_dir.strip()
    print('Dump directory=', dump_dir)
    selection = args.selection
    print('Selection scheme=', SetSelection(selection, args.param_two))
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