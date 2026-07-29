#!/usr/bin/env bash


# Enable extended globs
shopt -s extglob

# For if the environment needs to be set up before running
#source ~/Documents/root_development/rel/bin/thisroot.sh


####################################################################################################
######## Set the group number, figure out file path parameters, and makde csv record files #########
####################################################################################################

# Check to see if there are previous record files in the csv_records directory
if [ $(ls -1 ./csv_records/ | wc -l) -eq "0" ]; then
	# If there aren't any, use 1 as the group number
	groupNum="1"
else
	# Otherwise find the largest group number previously used and set the current group number to one higher
	largestGroupNum=$(ls ./csv_records/ | sort -n | tail -1)
	groupNum=$(("${largestGroupNum}" + 1))
fi

runGroupDirectory="./csv_records/""${groupNum}""/"
csvRunGroupRecord="groupRecord.csv"

mkdir "${runGroupDirectory}"

# Make csv record file and enter header row for run group csv record files
touch "${runGroupDirectory}""${csvRunGroupRecord}"
echo "runNum,numFields,numEntries,numClusters" > "${runGroupDirectory}""${csvRunGroupRecord}"



###################################################
######## Iterate over and call root macros ########
###################################################

upperIterLimit="5"
iterRepeatFactor="1"


runNum="0"

for (( i = 1; i <= "$upperIterLimit"; i++ )); do
   for ((j = 0; j < "$iterRepeatFactor"; j++)); do
      runNum=$(("${runNum}" + 1))

      #numFields=$((2 ** "${i}"))
      #numEntries=$((2 ** "${i}"))

      numFields="250"
      numEntries=$(( 5 * "${i}"))

      root -q read_write_record.C+("${groupNum}", "${runNum}", "${numFields}", "${numEntries}", \"w\")
      root -q read_write_record.C+("${groupNum}", "${runNum}", "${numFields}", "${numEntries}", \"r\")
   done
done

root -q analyze.C+(\""${runGroupDirectory}"\", \"w\")
#root -q analyze.C+(\""${runGroupDirectory}"\", \"r\")
