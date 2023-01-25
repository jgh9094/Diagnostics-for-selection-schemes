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
POP_OPT_MAX = 'pop_opt_max'
POP_UNI_OBJ = 'pop_uni_obj'
POP_STR_MAX = 'pop_str_max'
# trait coverage
UNI_STR_POS = 'uni_str_pos'
# # pareto data
PARETO_CNT = 'pareto_cnt'
# # novelty data
ARCHIVE_CNT = 'archive_cnt'
PMIN = 'pmin'
ARC_ACTI_GENE = 'arc_acti_gene'
OVERLAP = 'overlap'
POP_MAX_TRT = 'pop_max_trt'
POP_MAX_GENE = 'pop_max_gene'

# collection of data in list
DATA_LIST = [GENERATION,POP_FIT_AVG,POP_FIT_MAX,POP_OPT_MAX,POP_UNI_OBJ,POP_STR_MAX,UNI_STR_POS,PARETO_CNT,ARCHIVE_CNT,PMIN,ARC_ACTI_GENE,OVERLAP,POP_MAX_TRT,POP_MAX_GENE]

# seed experiements replicates range
REPLICATES = 50
GENERATIONS = 50000
RESOLUTION = 100
DIMENTIONALITY = 100
ACCURACY = 99

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

# get data main directory
def GetDataDirectory(dir,sel,pt):
    # check if data dir exists
    if os.path.isdir(dir):
        print('Data dirctory exists=', dir)
    else:
        sys.exit('DATA DIRECTORY DOES NOT EXIST: ' + dir)

    # check that selection data folder exists
    SEL_DIR = dir + SetSelection(sel,pt) + '/TRT_' + str(DIMENTIONALITY) + '__ACC_' + str(ACCURACY) + '__GEN_' + str(GENERATIONS) + '/'
    if os.path.isdir(SEL_DIR):
        print('Selection scheme data folder exists', SEL_DIR)
    else:
        sys.exit('SELECTION DATA DIRECTORY DOES NOT EXIST: ' + SEL_DIR)

    return SEL_DIR