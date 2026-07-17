#!/usr/bin/env bash



# Enable extended globs
shopt -s extglob

#source ~/Documents/root_development/rel/bin/thisroot.sh


for i in {0..10}; do
   numFields="${i}"
   numEntries=$((10-"${i}"))

   root -q read_write_record.C+("${numFields}", "${numEntries}", \"w\")
   root -q read_write_record.C+("${numFields}", "${numEntries}", \"r\")
done
