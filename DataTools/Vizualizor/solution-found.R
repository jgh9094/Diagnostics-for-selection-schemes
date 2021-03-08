cat("\014")
setwd("C:/Users/josex/Desktop/Research/Projects/SelectionDiagnostics/Selection-Scheme-Diagnotics-Part-I/Data/VISUALIZOR")
rm(list = ls())

library(ggplot2)
library(dplyr)
library(reshape2)
library(tidyverse)
source("https://gist.githubusercontent.com/benmarwick/2a1bb0133ff568cbe28d/raw/fb53bd97121f7f9ce947837ef1a4c65a73bffb3f/geom_flat_violin.R")

raincloud_theme <- theme(
  text = element_text(size = 10),
  axis.title.x = element_text(size = 12),
  axis.title.y = element_text(size = 12),
  axis.text = element_text(size = 10),
  axis.text.x = element_text(angle = 0, vjust = 0.5),
  legend.title = element_text(size = 16),
  legend.text = element_text(size = 16),
  legend.position = "right",
  plot.title = element_text(lineheight = .8, face = "bold", size = 16),
  panel.border = element_blank(),
  panel.grid.minor = element_blank(),
  axis.line.x = element_line(colour = "black", size = 0.5, linetype = "solid"),
  axis.line.y = element_line(colour = "black", size = 0.5, linetype = "solid"),
  plot.margin = margin(10, 20, 10, 10))



# Get tournament data and plot + save it
df_tour = read.csv(file='../TOURNAMENT/EXPLOITATION_SOL_FND.csv', header = TRUE)
colnames(df_tour) = c(1,2,4,8,16,32,64,128,256,512)
df_tour = as.data.frame(sapply(df_tour, as.numeric))
df_tour = melt(df_tour, id.vars=c(0))
colnames(df_tour) = c("T", "Gen")

ggplot(df_tour, aes(x = T, y = Gen, fill = T)) +
  geom_flat_violin(position = position_nudge(x = .2, y = 0), alpha = .8) +
  geom_point(aes(y = Gen, color = T), position = position_jitter(width = .1), size = 1.5, alpha = 10.8) +
  geom_boxplot(width = .1, outlier.shape = NA, alpha = 0.5) +
  xlab("Tournament Size") + ylab('Generation Optimal Solution Found') + ggtitle("Tournament Generation Optimal Solution Found") +
  guides(fill = FALSE) +
  guides(color = FALSE) +
  scale_color_brewer(palette = "Spectral") +
  scale_fill_brewer(palette = "Spectral") +
  coord_flip() +
  theme_bw() +
  scale_y_continuous(breaks=seq(0,12000,1000))+
  raincloud_theme
ggsave("tour_sol_fnd.pdf", device = 'pdf')

# Get mu lambda data and plot + save it
df_mu = read.csv(file='../MULAMBDA//EXPLOITATION_SOL_FND.csv', header = TRUE)
colnames(df_mu) = c(1,2,4,8,16,32,64,128,256,512)
df_mu = as.data.frame(sapply(df_mu, as.numeric))
df_mu = melt(df_mu, id.vars=c(0))
colnames(df_mu) = c("Mu", "Gen")

ggplot(df_mu, aes(x = Mu, y = Gen, fill = Mu)) +
  geom_flat_violin(position = position_nudge(x = .2, y = 0), alpha = .8) +
  geom_point(aes(y = Gen, color = Mu), position = position_jitter(width = .1), size = 1.5, alpha = 10.8) +
  geom_boxplot(width = .1, outlier.shape = NA, alpha = 0.5) +
  xlab("Mu") + ylab('Generation Optimal Solution Found') + ggtitle("Mu Lambda Generation Optimal Solution Found") +
  guides(fill = FALSE) +
  guides(color = FALSE) +
  scale_color_brewer(palette = "Spectral") +
  scale_fill_brewer(palette = "Spectral") +
  coord_flip() +
  theme_bw() +
  scale_y_continuous(breaks=seq(0,12000,1000))+
  raincloud_theme
ggsave("mulambda_sol_fnd.pdf", device = 'pdf')


# Get Lexicase data and plot + save it
df_lex = read.csv(file='../LEXICASE/EXPLOITATION_SOL_FND.csv', header = TRUE)
colnames(df_lex) = c(0.0, 0.1, 0.3, 0.6, 1.2, 2.5, 5.0, 10.0)
df_lex = as.data.frame(sapply(df_lex, as.numeric))
df_lex = melt(df_lex, id.vars=c(0))
colnames(df_lex) = c("Eps", "Gen")

ggplot(df_lex, aes(x = Eps, y = Gen, fill = Eps)) +
  geom_flat_violin(position = position_nudge(x = .2, y = 0), alpha = .8) +
  geom_point(aes(y = Gen, color = Eps), position = position_jitter(width = .1), size = 1.0, alpha = 10.8) +
  geom_boxplot(width = .1, outlier.shape = NA, alpha = 0.5) +
  xlab("Epsilon") + ylab('Generation Optimal Solution Found') + ggtitle("Lexicase Generation Optimal Solution Found") +
  scale_y_continuous(breaks=seq(14500,17000,250))+
  guides(fill = FALSE) +
  guides(color = FALSE) +
  scale_color_brewer(palette = "Spectral") +
  scale_fill_brewer(palette = "Spectral") +
  coord_flip() +
  theme_bw() +
  raincloud_theme
ggsave("lexicase_sol_fnd.pdf", device = 'pdf')










