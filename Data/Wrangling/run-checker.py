# Will list all of the incomplete id's that need to finish running
# SEED_ORIGINAL & TREATMENTS  will need to be adjusted based on the problems, treatments, and seeds that the project requires.
# Will also need to handle RANGE if different from the expected results!
#
# Input 1: file directory where all the folders are located
# Input 2: Directory where the new csv files will be placed
# Input 3: Print Interval!
#
# Output : create a csv for each population size
#
# python3

######################## IMPORTS ########################
import datetime
import argparse
import os
import pandas as pd







def main():
    # Generate the arguments
    parser = argparse.ArgumentParser(description="Data aggregation script.")
    parser.add_argument("data_directory", type=str, help="Target experiment directory.")
    parser.add_argument("dump_directory", type=str, help="Target dump directory")
    parser.add_argument("selection", type=int, help="Snapshot at each generation")
    parser.add_argument("snapshot", type=int, help="Snapshot at each generation")

    # Get the arguments
    args = parser.parse_args()
    data_directory = args.data_directory.strip()
    write_directory = args.dump_directory.strip()
    sel = args.selection
    snapshot = args.snapshot



if __name__ == "__main__":
    main()