#!/usr/bin/env bash


# Enable extended globs
shopt -s extglob


runGroupDirectory="./csv_records/groupRecord/"
csvRunGroupRecord="groupRecord.csv"

upperIterLimit="10"



# For if the environment needs to be set up before running
#source ~/Documents/root_development/rel/bin/thisroot.sh


mkdir "${runGroupDirectory}"

# Make csv record file and enter header row for run group csv record files
touch "${runGroupDirectory}""${csvRunGroupRecord}"
echo "runNum,numFields,numEntries" > "${runGroupDirectory}""${csvRunGroupRecord}"


runNum="0"

for (( i = 1; i <= "$upperIterLimit"; i++ )); do
   runNum=$(("${runNum}" + 1))

   numFields=$(("${i}" ** 2))
   numEntries=$(("${i}" ** 2))

   root -q read_write_record.C+("${runNum}", "${numFields}", "${numEntries}", \"w\")
   root -q read_write_record.C+("${runNum}", "${numFields}", "${numEntries}", \"r\")
done

root -q analyze.C+(\"./csv_records/groupRecord/\", \"w\")
root -q analyze.C+(\"./csv_records/groupRecord/\", \"r\")
