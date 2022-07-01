#####################################################################################################
#####################################################################################################
# Will hold all of the data that we share across python scripts
#
# python3
#####################################################################################################
#####################################################################################################

import sys
import os

# variables we are testing for each replicate range
TR_LIST = ['1','2','4','8','16','32','64','128','256']
TS_LIST = ['1','2','4','8','16','32','64','128','256']
LX_LIST = ['0.0']
FS_LIST = ['0.0','0.1','0.3','0.6','1.2','2.5','5.0']
ND_LIST = ['0.0','0.1','0.3','0.6','1.2','2.5','5.0']
NS_LIST = ['0','1','2','4','8','15','30']

# columns we are interested in grabbing
GENERATION = 'gen'
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
# trait coverage
UNI_STR_POS = 'uni_str_pos'
# # pareto data
PARETO_CNT = 'pareto_cnt'
# # novelty data
ARCHIVE_CNT = 'archive_cnt'
PMIN = 'pmin'
ARC_ACTI_GENE = 'arc_acti_gene'
OVERLAP = 'overlap'

# seed experiements replicates range
REPLICATES = 50

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
        return 'EXPLOITATION_RATE'
    elif s == 1:
        return 'ORDERED_EXPLOITATION'
    elif s == 2:
        return 'CONTRADICTORY_OBJECTIVES'
    elif s == 3:
        return 'MULTIPATH_EXPLORATION'
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
    # TRUNCATIONN
    if s == 0:
        return [x for x in range(1,451)]
    # TOURNAMENT
    elif s == 1:
        return [x for x in range(1,451)]
    # FITNESS SHARING
    elif s == 2:
        return [x for x in range(1,351)]
    # LEXICASE
    elif s == 3:
        return [x for x in range(1,51)]
    # NONDOMINATED SORTING
    elif s == 4:
        return [x for x in range(1,351)]
    # NOVELTY
    elif s == 5:
        return [x for x in range(1,351)]
    # ERROR
    else:
        sys.exit('SEEDS SELECTION UNKNOWN')

# return the correct amount of seed ran by experiment treatment
def SetSeedSets(s):
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
        return seed

    elif s == 2 or s == 4 or s == 5:
        seed = []
        seed.append([x for x in range(1,51)])
        seed.append([x for x in range(51,101)])
        seed.append([x for x in range(101,151)])
        seed.append([x for x in range(151,201)])
        seed.append([x for x in range(201,251)])
        seed.append([x for x in range(251,301)])
        seed.append([x for x in range(301,351)])
        return seed

    elif s == 3:
        seed = []
        seed.append([x for x in range(1,51)])
        return seed

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