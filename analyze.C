/// \date June 2026
/// \author Anna Helena Harms

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RCsvDS.hxx>

#include <TCanvas.h>
#include <TH1I.h>
#include <TROOT.h>
#include <TString.h>

#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <utility>

#include <map>

struct runInfo {
   int numFields;
   int numEntries;
};

void AnalyzeResults(std::string csvRunGroupRecordName) {
   // Creating empty map for accessing run information using run number
   map<int, runInfo> runNumToRunInfo;

   // Strings and characters for parsing csv file
   std::string line;
   std::string cell;
   char delimiter = ',';

   // variables for temporarily holding csv values
   int runNum;
   int numFields;
   int numEntries;

   // Initialize input file stream for run group record csv file
   std::ifstream csvRunGroupRecord(csvRunGroupRecordName);

   // Remove header row from group record csv file
   std::getline(csvRunGroupRecord, line);

   // Read in and parse the lines of the run group record csv and populate the runNumToRunInfo map
   while (csvRunGroupRecord.good()) {
      if ((csvRunGroupRecord.is_open())) {
         // Create a struct to be save run information for the current run
         runInfo tempInfoLoader;

         // Get the next line from the run group record csv file and use it to make a string stream
         while (std::getline(csvRunGroupRecord, line)) {
            // Initialize string stream from line in order to extract cell values
            std::stringstream lineStream(line);

            // Extract the csv cell values from the run group record file and save to the appropriate variables for later use
            std::getline(lineStream, cell, delimiter);
            runNum = std::stoi(cell);

            std::getline(lineStream, cell, delimiter);
            numFields = std::stoi(cell);

            std::getline(lineStream, cell, delimiter);
            numEntries = std::stoi(cell);

            // Load the extracted values into the temporary struct
            tempInfoLoader.numFields = numFields;
            tempInfoLoader.numEntries = numEntries;

            // Add new element with run information to runNumToRunInfo map with runNum as key
            runNumToRunInfo[runNum] = tempInfoLoader;
         }

      } else {
         std::cout << "Run Group Record CSV file not open." << std::endl;
     }
   }
}



void analyze(std::string csvRunGroupRecordName, std::string rw) {
   AnalyzeResults(csvRunGroupRecordName);
}
