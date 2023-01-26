#!/bin/bash

# Start the process
echo "STARTING THE WRANGLING PROCESS"
echo "CALLING THE SB FILES NOW..."
echo ""

echo "RUNNING: ot-data.sb"
sbatch ot-data.sb

echo "RUNNING: solution-fnd.sb"
sbatch solution-fnd.sb

echo "RUNNING: max-val.sb"
sbatch max-val.sb

echo "RUNNING: mvc-ot-data.sb"
sbatch mvc-ot-data.sb

echo "RUNNING: mvc-solution-fnd.sb"
sbatch mvc-solution-fnd.sb

echo "RUNNING: mvc-max-val.sb"
sbatch mvc-max-val.sb

echo ""
echo "======================"
echo "FINISHED RUNNING FILES"
echo "======================"