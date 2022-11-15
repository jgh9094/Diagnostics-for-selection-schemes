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

echo ""
echo "======================"
echo "FINISHED RUNNING FILES"
echo "======================"