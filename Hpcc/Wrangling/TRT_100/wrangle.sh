#!/bin/bash

# Start the process
echo "STARTING THE WRANGLING PROCESS"
echo "CALLING THE SB FILES NOW..."
echo ""

echo "RUNNING: com_agg_per.sb"
sbatch com_agg_per.sb

echo "RUNNING: com_opt_cnt.sb"
sbatch com_opt_cnt.sb

echo "RUNNING: com_sol_cnt.sb"
sbatch com_sol_cnt.sb

echo "RUNNING: ele_agg_per.sb"
sbatch ele_agg_per.sb

echo "RUNNING: ele_opt_cnt.sb"
sbatch ele_opt_cnt.sb

echo "RUNNING: los_div.sb"
sbatch los_div.sb

echo "RUNNING: opt_agg_per.sb"
sbatch opt_agg_per.sb

echo "RUNNING: opt_obj_cnt.sb"
sbatch opt_obj_cnt.sb

echo "RUNNING: pop_fit_avg.sb"
sbatch pop_fit_avg.sb

echo "RUNNING: pop_opt_avg.sb"
sbatch pop_opt_avg.sb

echo "RUNNING: pop_uni_obj.sb"
sbatch pop_uni_obj.sb

echo "RUNNING: run-check.sb"
sbatch run-check.sb

echo "RUNNING: sel_pre.sb"
sbatch sel_pre.sb

echo "RUNNING: solution-fnd.sb"
sbatch solution-fnd.sb

echo ""
echo "======================"
echo "FINISHED RUNNING FILES"
echo "======================"
