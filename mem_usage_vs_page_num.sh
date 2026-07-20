#!/usr/bin/env bash


# Enable extended globs
shopt -s extglob


upperIterLimit="10"

# For if the environment needs to be set up before running
#source ~/Documents/root_development/rel/bin/thisroot.sh

mkdir ./csv_records/groupRecord

# Make csv record files for run group
touch ./csv_records/groupRecord/writeGroupRecord.csv
touch ./csv_records/groupRecord/readGroupRecord.csv

# Enter header row for run group csv record files
echo "runNum,numFields,numEntries" > ./csv_records/groupRecord/writeGroupRecord.csv
echo "runNum,numFields,numEntries" > ./csv_records/groupRecord/readGroupRecord.csv


runNum="0"

for (( i = 1; i <= "$upperIterLimit"; i++ )); do
   runNum=$(("${runNum}" + 1))

   numFields=$(("${i}" ** 2))
   numEntries=$(("${i}" ** 2))

   root -q read_write_record.C+("${runNum}", "${numFields}", "${numEntries}", \"w\")
   root -q read_write_record.C+("${runNum}", "${numFields}", "${numEntries}", \"r\")
done
