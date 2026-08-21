# RNTuple Memory Usage Benchmarking Code

## Description

This project aims to benchmark the memory usage of the reading and writing functionality for the RNTuple data format in ROOT.

For more details, see =link report=


## Requirements

An operational install of ROOT to be benchmarked.


## Running Instructions

1. Make sure that the ROOT version to be benchmarked is properly sourced:

    '''
    source ~/path\_to\_build\_ROOT\_directory/bin/thisroot.sh
    '''

2. (optional) Double check that parameters are set as desired in the code. Generally, all the iteration parameters are preset, but they can be manually customized within the code and this is discussed further in the next section (Setting Relevant Iteration Parameters).

3. Execute the mem\_usage\_vs\_page\_num.sh script from the project directory:

    '''
    ./mem\_usage\_vs\_page\_num.sh
    '''

4. The analysis is automatically run from the mem\_usage\_vs\_page\_num.sh script but can also be manually run using the command:

    '''
    root -q analyze.C+("${groupNum}", \"w\")
    '''

    Here the first parameter represents the run group number. The second parameter can either be given as a "r" or "w" for specifying whether the read or write csv record files should be used as input for analysis. Output location is described in the next section.


## Project Directory Structure

The main directory houses both the code and output directories. There are three output directories:

### output\_plots:

Houses the run group subdirectories containing the output plots produced by the analyze.C macro.

### test\_files

Contains the run group subdirectories with the root files produced by the read\_write\_record.C macro.

### output\_csv

Contains the run group subdirectories with the csv files produced by read\_write\_record.C and 

-explain output directories
