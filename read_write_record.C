/// \date June 2026
/// \author Anna Helena Harms


#include <ROOT/RNTupleModel.hxx>
#include <ROOT/RNTupleReader.hxx>
#include <ROOT/RNTupleWriter.hxx>

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


// For resident set size tracking
#include <sys/time.h>
#include <sys/resource.h>


// for resident set size tracking
long get_mem_usage() {
    struct rusage usage;
    int ret;
    ret = getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // in KB
}


// returns the virtual set size in kB
long get_vss() {
   long vss = 0;
   std::string line;

   ifstream statusFile("/proc/self/status");

   while (statusFile.good()) {
      if (statusFile.is_open()) {
         // Iterate through the different lines of the /proc/self/status file
         getline(statusFile, line);

         // Get the label for the listed value
         std::string delimiter = ":";
         size_t position = line.find(delimiter);
         std::string label = line.substr(0, position);

         // Check to see if the line contains one of the sought out values and extract them if so
         if ((label.compare("VmSize")) == 0) {
            line.erase(0, position + delimiter.length());

            // remove the trailing " kB" characters from the line
            line.erase((line.length() - 3), 3);

            // trim leading space before digits 0-9
            line.erase(0, line.find_first_of("123456790"));

            vss = stoul(line);
         }

      } else {
         std::cout << "Error opening status file" << std::endl;
      }
   }
   return vss;
}


void runInformationRecord(std::string csvFileName, int runNum, int numFields, int numEntries) {
   // Open csv file for recording memory usage statistics for the run group
   std::ofstream csvRunGroupRecord;
   csvRunGroupRecord.open(csvFileName, std::ios_base::app);

   // Record information for this specific run to run group csv file
   csvRunGroupRecord << runNum << "," << numFields << "," << numEntries << std::endl;
}



void CreateFile(int runNum, int numEntries, int numFields) {
   // Create and open csv file for recording memory usage statistics for given run
   std::ofstream csvRunRecord;
   std::string csvFileName = "./csv_records/groupRecord/write_" + std::to_string(runNum);
   csvRunRecord.open(csvFileName);

   // Write header row to run specific csv record file
   csvRunRecord << "runNum,EntryNum,vss,rss" << std::endl;
   
   // Creating a unique pointer to an empty data model
   auto model = ROOT::RNTupleModel::Create();

   // Defining the data model
   for (int i = 1; i <= numFields; i++) {
      auto fldPtr = model->MakeField<int>("Category" + std::to_string(i));
   }

   // creates a root file and a page sink which the writer connects the model to
   std::string fileName =  "./test_files/numFields" + std::to_string(numFields) + "_numEntries" + std::to_string(numEntries);
   auto writer = ROOT::RNTupleWriter::Recreate(std::move(model), "blank", fileName);


   // Do an initial save of memory usage statistics and run information to csv file
   csvRunRecord << runNum << "," << 0 << "," << get_vss() << "," << get_mem_usage() << std::endl;

   // Write entries to data model
   for (int i = 1; i <= numEntries; i++) {
      auto entryPtr = writer->CreateEntry();

      for (int j = 1; j <= numFields; j++) {
         auto fldPtr = entryPtr->GetPtr<int>("Category" + std::to_string(j));

         *fldPtr = 0;
      }
      // Save memory usage statistics and run information to csv file
      csvRunRecord << runNum << "," << i << "," << get_vss() << "," << get_mem_usage() << std::endl;

      writer->Fill();
   }
}



void ReadFile(int runNum, int numEntries, int numFields) {
   // Create an empty RNTuple model and get a unique pointer to it
   auto model = ROOT::RNTupleModel::Create();

   // Needs to still be written
}



void read_write_record(int runNum, int numFields, int numEntries, std::string rw) {

   if (std::strcmp(rw.c_str(), "w")==0) {
      runInformationRecord("./csv_records/groupRecord/writeGroupRecord.csv", runNum, numFields, numEntries);
      CreateFile(runNum, numEntries, numFields);
   } else if (std::strcmp(rw.c_str(), "r")==0) {
      runInformationRecord("./csv_records/groupRecord/readGroupRecord.csv", runNum, numFields, numEntries);
      ReadFile(runNum, numEntries, numFields);
   } else {
      std::cout << "Incorrect arguments provided. Please review the required command line options and arguments." << std::endl;
   }
}
