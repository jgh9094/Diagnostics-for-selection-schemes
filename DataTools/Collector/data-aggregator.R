###########################################################################
###                                                                     ###
###            DATA SCRIPT TO COLLECT & CLEAN DATA FILES                ###
###                                                                     ###
###########################################################################

# clearing previous r data

cat("\014")
rm(list = ls())
setwd("C:/Users/jgh9094/Desktop/Research/Projects/SelectionDiagnostics/Selection-Scheme-Diagnotics-Part-I/")

# includes

library(dplyr)
library(plyr)
library(tidyverse)
library(reshape2)

# default variables

REPLICATES = 50
final = data.frame()

# selection scheme related stuff
ACRON = tolower(c('TRU','TOR','GFS','PFS','LEX','NDS','NOV','RAN'))
NAMES = c('Truncation (tru)','Tournament (tor)', 'Genotypic Fitness Sharing (gfs)','Phenotypic Fitness Sharing (pfs)','Lexicase (lex)','Nondominated Sorting (nds)','Novelty Search (nov)','Random (ran)')
SCHEME = c('TRUNCATION','TOURNAMENT','FITSHARING_G','FITSHARING_P','LEXICASE','NONDOMINATEDSORTING','NOVELTY','TOURNAMENT')

# selection scheme parameter we are looking for
PARAM = c('8', '8', '0.3', '0.3', '0.0', '0.3', '15', '1')

# for diagnostic loops
DIAGNOSTIC = tolower(c('EXPLOITATION_RATE', 'ORDERED_EXPLOITATION', 'CONTRADICTORY_OBJECTIVES', 'MULTIPATH_EXPLORATION', 'MULTIVALLEY_CROSSING'))

# data related
DATA_DIR = './DATA-10-21/'
DUMP_DIR = './DATA-10-21/POLISHED/'
dir.create(DUMP_DIR, showWarnings = TRUE)

# go through each diagnostic and collect over time data for cross comparison (cc)
for(diagnostic in DIAGNOSTIC)
{
  print(diagnostic)
  for(i in 1:8)
  {
    dir = paste(DATA_DIR,SCHEME[i],'/over-time-',diagnostic,'-', tolower(SCHEME[i]), '.csv', sep = "", collapse = NULL)
    print(paste('DIRECTORY:',dir, sep = "", collapse = NULL))
    print(paste('PARAMETER:',PARAM[i], sep = "", collapse = NULL))

    # read csv
    df = read.csv(dir, header = TRUE, stringsAsFactors = FALSE)

    # add names/tags
    df$acron = ACRON[i]
    df$`Selection\nScheme` = NAMES[i]
    df$diagnostic = diagnostic

    # add to final data frame
    if(i == 5)
    {
      final = rbind(final, df)
    }
    else
    {
      df = filter(df, trt == PARAM[i])
      final = rbind(final, df)
    }
  }
}

# save final data frame
file_name = paste(DUMP_DIR,'cross-comp-over-time.csv', sep = "", collapse = NULL)
print(paste('FILE NAME:',file_name, sep = "", collapse = NULL))
write.csv(final,file_name, row.names = FALSE)
final = data.frame()
print('$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$')
print('')


# go through each diagnostic and collect best over time for cross comparison (cc)
for(diagnostic in DIAGNOSTIC)
{
  print(diagnostic)
  for(i in 1:8)
  {
    dir = paste(DATA_DIR,SCHEME[i],'/best-',diagnostic,'-', tolower(SCHEME[i]), '.csv', sep = "", collapse = NULL)
    print(paste('DIRECTORY:',dir, sep = "", collapse = NULL))
    print(paste('PARAMETER:',PARAM[i], sep = "", collapse = NULL))

    # read csv
    df = read.csv(dir, header = TRUE, stringsAsFactors = FALSE)

    # add names/tags
    df$acron = ACRON[i]
    df$`Selection\nScheme` = NAMES[i]
    df$diagnostic = diagnostic
    
    if(SCHEME[i] == SCHEME[5])
    {
      final = rbind(final, df)
    }
    else
    {
      df = filter(df, trt == PARAM[i])
      final = rbind(final, df)
    }
  }
}

# save final data frame
file_name = paste(DUMP_DIR,'cross-comp-best.csv', sep = "", collapse = NULL)
print(paste('FILE NAME:',file_name, sep = "", collapse = NULL))
write.csv(final,file_name, row.names = FALSE)
final = data.frame()
print('$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$')
print('')

# get generation a satisfactory solution is found for cross comparison (cc)
for(diagnostic in DIAGNOSTIC)
{
  print(diagnostic)
  for(i in 1:8)
  {
    dir = paste(DATA_DIR,SCHEME[i],'/ssf-',diagnostic,'-', tolower(SCHEME[i]), '.csv', sep = "", collapse = NULL)
    print(paste('DIRECTORY:',dir, sep = "", collapse = NULL))
    print(paste('PARAMETER:',PARAM[i], sep = "", collapse = NULL))

    # read and fill csv
    df = read.csv(dir, header = TRUE, stringsAsFactors = FALSE, check.names = FALSE)

    diff = REPLICATES - nrow(df)
    print(paste('DIFF:',diff, sep = "", collapse = NULL))
    if(diff > 0)
    {
      df[nrow(df)+diff, ] <- NA
    }

    df = melt(df, id.vars=c(0))
    colnames(df) = c("trt", "generation")
    df$trt = factor(df$trt)

    # add names/tags
    df$acron = ACRON[i]
    df$`Selection\nScheme` = NAMES[i]
    df$diagnostic = diagnostic

    # add to final data frame
    if(SCHEME[i] == SCHEME[5])
    {
      final = rbind(final, df)
    }
    else
    {
      df = filter(df, trt == PARAM[i])
      final = rbind(final, df)
    }
  }
}

# save final data frame
file_name = paste(DUMP_DIR,'cross-comp-ssf.csv', sep = "", collapse = NULL)
print(paste('FILE NAME:',file_name, sep = "", collapse = NULL))
write.csv(final,file_name, row.names = FALSE)
final = data.frame()
print('$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$')



# go through each scheme and collect over time data
for(i in 1:8)
{
  scheme = SCHEME[i]
  print(scheme)
  for(diagnostic in DIAGNOSTIC)
  {
    dir = paste(DATA_DIR,scheme,'/over-time-',diagnostic,'-', tolower(scheme), '.csv', sep = "", collapse = NULL)
    print(paste('DIRECTORY:',dir, sep = "", collapse = NULL))
    print(paste('DIAGNOSTIC:',diagnostic, sep = "", collapse = NULL))

    # read csv
    df = read.csv(dir, header = TRUE, stringsAsFactors = FALSE)

    # add names/tags
    df$acron = ACRON[i]
    df$`Selection\nScheme` = NAMES[i]
    df$diagnostic = diagnostic

    # add to final data frame
    final = rbind(final, df)
  }
}

# save final data frame
file_name = paste(DUMP_DIR,'selection-scheme-over-time.csv', sep = "", collapse = NULL)
print(paste('FILE NAME:',file_name, sep = "", collapse = NULL))
write.csv(final,file_name, row.names = FALSE)
final = data.frame()
print('$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$')
print('')

# go through each scheme and collect best data
for(i in 1:8)
{
  scheme = SCHEME[i]
  print(scheme)
  for(diagnostic in DIAGNOSTIC)
  {
    dir = paste(DATA_DIR, scheme, '/best-',diagnostic,'-', tolower(scheme), '.csv', sep = "", collapse = NULL)
    print(paste('DIRECTORY:',dir, sep = "", collapse = NULL))
    print(paste('DIAGNOSTIC:',diagnostic, sep = "", collapse = NULL))

    # read csv
    df = read.csv(dir, header = TRUE, stringsAsFactors = FALSE)

    # add names/tags
    df$acron = ACRON[i]
    df$`Selection\nScheme` = NAMES[i]
    df$diagnostic = diagnostic

    # add to final data frame
    final = rbind(final, df)
  }
}

# save final data frame
file_name = paste(DUMP_DIR,'selection-scheme-best.csv', sep = "", collapse = NULL)
print(paste('FILE NAME:',file_name, sep = "", collapse = NULL))
write.csv(final,file_name, row.names = FALSE)
final = data.frame()
print('$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$')
print('')

# go through each scheme and collect satisfactory solution found
for(i in 1:8)
{
  scheme = SCHEME[i]
  print(scheme)
  for(diagnostic in DIAGNOSTIC)
  {
    dir = paste(DATA_DIR, scheme, '/ssf-',diagnostic,'-', tolower(scheme), '.csv', sep = "", collapse = NULL)
    print(paste('DIRECTORY:',dir, sep = "", collapse = NULL))
    print(paste('DIAGNOSTIC:',diagnostic, sep = "", collapse = NULL))

    # read csv
    df = read.csv(dir, header = TRUE, stringsAsFactors = FALSE, check.names = FALSE)

    diff = REPLICATES - nrow(df)
    print(paste('DIFF:',diff, sep = "", collapse = NULL))
    if(diff > 0)
    {
      df[nrow(df)+diff, ] <- NA
    }

    df = melt(df, id.vars=c(0))
    colnames(df) = c("trt", "generation")
    df$trt = factor(df$trt)

    # add names/tags
    df$acron = ACRON[i]
    df$`Selection\nScheme` = NAMES[i]
    df$diagnostic = diagnostic

    # add to final data frame
    final = rbind(final, df)
  }
}

# save final data frame
file_name = paste(DUMP_DIR,'selection-scheme-ssf.csv', sep = "", collapse = NULL)
print(paste('FILE NAME:',file_name, sep = "", collapse = NULL))
write.csv(final,file_name, row.names = FALSE)
final = data.frame()
print('$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$')
print('')