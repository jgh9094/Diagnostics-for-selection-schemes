cat("\014") 
setwd("C:/Users/josex/Desktop/Research/Projects/SelectionDiagnostics/Selection-Scheme-Diagnotics-Part-I/DataTools/Vizualizor/")
rm(list = ls())

library(ggplot2)
library(dplyr)
library(reshape2)
library(tidyverse)

# Grab data for tournament
df_tour = read.csv(file='../../Data/TOURNAMENT/EXPLOITATION_PERF_AGG.csv', header = TRUE)
df_tour$trt <- factor(df_tour$trt, labels = c(1,2,4,8,16,32,64,128,256,512))
ggplot(df_tour, aes(x=gen, y=agg, group=trt, color=trt)) + 
  geom_line() +
  geom_point()+
  geom_errorbar(aes(ymin=agg-dev, ymax=agg+dev), width=.2, position=position_dodge(0.05)) +
  ggtitle("Tournament Selection Population Aggregate Average") + xlab("Generations") + ylab("Average Aggregate Performance")
ggsave("tour_perf_agg_full.pdf", device = 'pdf')

# Trim tournament data
df_tour = filter(df_tour, gen <= 10000)
ggplot(df_tour, aes(x=gen, y=agg, group=trt, color=trt)) + 
  geom_line() +
  geom_point()+
  geom_errorbar(aes(ymin=agg-dev, ymax=agg+dev), width=.2,position=position_dodge(0.05)) +
  ggtitle("Tournament Selection Population Aggregate Average") + xlab("Generations") + ylab("Population Average Performance")
ggsave("tour_perf_agg_10000.pdf", device = 'pdf')


# Grad data for Mu Lambda
df_mu = read.csv(file='../../Data/MULAMBDA/EXPLOITATION_PERF_AGG.csv', header = TRUE)
df_mu$trt <- factor(df_mu$trt, labels =c(1,2,4,8,16,32,64,128,256,512))
ggplot(df_mu, aes(x=gen, y=agg, group=trt, color=trt)) +
  geom_line() +
  geom_point()+
  geom_errorbar(aes(ymin=agg-dev, ymax=agg+dev), width=.2,position=position_dodge(0.05)) +
  ggtitle("Mu Lambda Selection Population Aggregate Average") + xlab("Generations") + ylab("Population Average Performance")
ggsave("mu_perf_agg_full.pdf", device = 'pdf')

# Trim Mu Lambda data
df_mu = filter(df_mu, gen <= 10000)
ggplot(df_mu, aes(x=gen, y=agg, group=trt, color=trt)) +
  geom_line() +
  geom_point()+
  geom_errorbar(aes(ymin=agg-dev, ymax=agg+dev), width=.2,position=position_dodge(0.05))+
  ggtitle("Mu Lambda Selection Population Aggregate Average") + xlab("Generations") + ylab("Population Average Performance")
ggsave("mu_perf_agg_10000.pdf", device = 'pdf')


# Grab data for Lexicase
df_lex = read.csv(file='../../Data/LEXICASE/EXPLOITATION_PERF_AGG.csv', header = TRUE)
df_lex$trt <- factor(df_lex$trt, labels =c(0.0, 0.1, 0.3, 0.6, 1.2, 2.5, 5.0, 10.0))
ggplot(df_lex, aes(x=gen, y=agg, group=trt, color=trt)) +
  geom_line() +
  geom_point()+
  geom_errorbar(aes(ymin=agg-dev, ymax=agg+dev), width=.2,position=position_dodge(0.05)) +
  ggtitle("Lexicase Selection Population Aggregate Average") + xlab("Generations") + ylab("Population Average Performance")
ggsave("lex_perf_agg_full.pdf", device = 'pdf')

# Trim data for Lexicase
df_lex = filter(df_lex, gen <= 25000)
ggplot(df_lex, aes(x=gen, y=agg, group=trt, color=trt)) +
  geom_line() +
  geom_point()+
  geom_errorbar(aes(ymin=agg-dev, ymax=agg+dev), width=.2,position=position_dodge(0.05)) +
  ggtitle("Lexicase Selection Population Aggregate Average") + xlab("Generations") + ylab("Population Average Performance")
ggsave("lex_perf_agg_25000.pdf", device = 'pdf')





