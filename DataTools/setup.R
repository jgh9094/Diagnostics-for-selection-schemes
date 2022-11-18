library(ggplot2)
library(dplyr)

# variables used throughout

TRAITS = 100
TSIZE = 26
ORDER = c('Truncation (tru)','Tournament (tor)','Lexicase (lex)', 'Genotypic Fitness Sharing (gfs)','Phenotypic Fitness Sharing (pfs)','Nondominated Sorting (nds)','Novelty Search (nov)','Random (ran)')
ACRON = tolower(c('TRU','TOR','LEX','GFS','PFS','NDS','NOV','RAN'))
SHAPE = c(5,3,1,2,6,0,4,20,1)
PARAM = c('8', '8', '0.0', '0.3', '0.3', '0.3', '15', '1')
cb_palette <- c('#332288','#88CCEE','#EE7733','#EE3377','#117733','#882255','#44AA99','#CCBB44', '#000000')
GENERATIONS = 50000

# selection scheme parameters

TR_LIST = c(1, 2, 4, 8, 16, 32, 64, 128, 256)
TS_LIST = c(2, 4, 8, 16, 32, 64, 128, 256)
FS_LIST = c(0.0, 0.1, 0.3, 0.6, 1.2, 2.5, 5.0)
ND_LIST = c(0.0, 0.1, 0.3, 0.6, 1.2, 2.5, 5.0)
NS_LIST = c(1, 2, 4, 8, 15, 30)

# theme that graphs will follow

p_theme <- theme(
  text = element_text(size = 28),
  plot.title = element_text( face = "bold", size = 22, hjust=0.5),
  panel.border = element_blank(),
  panel.grid.minor = element_blank(),
  legend.title=element_text(size=22),
  legend.text=element_text(size=23),
  axis.title = element_text(size=23),
  axis.text = element_text(size=22),
  legend.position="bottom",
  panel.background = element_rect(fill = "#f1f2f5",
                                  colour = "white",
                                  size = 0.5, linetype = "solid")
)

# cross comparison data frames
cc_over_time <- read.csv('./DATA-FINAL/POLISHED/cross-comp-over-time.csv', header = TRUE, stringsAsFactors = FALSE)
colnames(cc_over_time)[colnames(cc_over_time) == "Selection.Scheme"] = 'Selection\nScheme'
cc_over_time$`Selection\nScheme` <- factor(cc_over_time$`Selection\nScheme`, levels = ORDER)
cc_over_time$uni_str_pos = cc_over_time$uni_str_pos + cc_over_time$arc_acti_gene - cc_over_time$overlap

cc_best = read.csv('./DATA-FINAL/POLISHED/cross-comp-best.csv', header = TRUE, stringsAsFactors = FALSE)
cc_best$acron <- factor(cc_best$acron, levels = ACRON)
colnames(cc_best)[colnames(cc_best) == "Selection.Scheme"] = 'Selection\nScheme'

cc_ssf = read.csv('./DATA-FINAL/POLISHED/selection-scheme-ssf.csv', header = TRUE, stringsAsFactors = FALSE)
cc_ssf$acron <- factor(cc_ssf$acron, levels = ACRON)
colnames(cc_over_time)[colnames(cc_over_time) == "Selection.Scheme"] = 'Selection\nScheme'
cc_ssf[is.na(cc_ssf)] <- 59999

cc_end <- filter(cc_over_time, gen == 50000)
cc_end$acron <- factor(cc_end$acron, levels = ACRON)

# selection scheme data frames
ss_over_time <- read.csv('./DATA-FINAL/POLISHED/selection-scheme-over-time.csv', header = TRUE, stringsAsFactors = FALSE)
colnames(ss_over_time)[colnames(ss_over_time) == "Selection.Scheme"] = 'Selection\nScheme'
ss_over_time$uni_str_pos = ss_over_time$uni_str_pos + ss_over_time$arc_acti_gene - ss_over_time$overlap

ss_best <- read.csv('./DATA-FINAL/POLISHED/selection-scheme-best.csv', header = TRUE, stringsAsFactors = FALSE)
colnames(ss_best)[colnames(ss_best) == "Selection.Scheme"] = 'Selection\nScheme'

ss_ssf <- read.csv('./DATA-FINAL/POLISHED/selection-scheme-ssf.csv', header = TRUE, stringsAsFactors = FALSE)
colnames(ss_ssf)[colnames(ss_ssf) == "Selection.Scheme"] = 'Selection\nScheme'

## genotypic fitness sharing data frames
gfs_ot <- filter(ss_over_time, acron == 'gfs')
gfs_ot$Sigma <- factor(gfs_ot$trt, levels = FS_LIST)
gfs_best <- filter(ss_best, acron == 'gfs')
gfs_best$Sigma <- factor(gfs_best$trt, levels = FS_LIST)
gfs_end  <- filter(gfs_ot, gen == 50000)

## phenotypic fitness sharing data frames
pfs_ot <- filter(ss_over_time, acron == 'pfs')
pfs_ot$Sigma <- factor(pfs_ot$trt, levels = FS_LIST)
pfs_best <- filter(ss_best, acron == 'pfs')
pfs_best$Sigma <- factor(pfs_best$trt, levels = FS_LIST)
pfs_end  <- filter(pfs_ot, gen == 50000)

## nodominated sorting data frames
nds_ot <- filter(ss_over_time, acron == 'nds')
nds_ot$Sigma <- factor(nds_ot$trt, levels = ND_LIST)
nds_best <- filter(ss_best, acron == 'nds')
nds_best$Sigma <- factor(nds_best$trt, levels = ND_LIST)
nds_end  <- filter(nds_ot, gen == 50000)

## novelty search data frames
nov_ot <- filter(ss_over_time, acron == 'nov' & trt != 0)
nov_ot$K <- factor(nov_ot$trt, levels = NS_LIST)
nov_best <- filter(ss_best, acron == 'nov' & trt != 0)
nov_best$K <- factor(nov_best$trt, levels = NS_LIST)
nov_end  <- filter(nov_ot, gen == 50000)

## tournament data frames
tor_ot <- filter(ss_over_time, acron == 'tor' & trt != 1)
tor_ot$T <- factor(tor_ot$trt, levels = TS_LIST)
tor_best <- filter(ss_best, acron == 'tor' & trt != 1)
tor_best$T <- factor(tor_best$trt, levels = TS_LIST)
tor_end  <- filter(tor_ot, gen == 50000)
tor_ssf <- filter(ss_ssf, acron == 'tor' & trt != 1)
tor_ssf$T <- factor(tor_ssf$trt, levels = TS_LIST)
tor_ssf[is.na(tor_ssf)] <- 59999

## truncation data frames
tru_ot <- filter(ss_over_time, acron == 'tru')
tru_ot$T <- factor(tru_ot$trt, levels = TR_LIST)
tru_best <- filter(ss_best, acron == 'tru')
tru_best$T <- factor(tru_best$trt, levels = TR_LIST)
tru_end  <- filter(tru_ot, gen == 50000)
tru_ssf <- filter(ss_ssf, acron == 'tru')
tru_ssf$T <- factor(tru_ssf$trt, levels = TR_LIST)
tru_ssf[is.na(tru_ssf)] <- 59999