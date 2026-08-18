/// \date August 2026
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
    return usage.ru_maxrss; // in KiB
}


// returns the virtual set size in KiB
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


void runInformationRecord(int groupNum, int runNum, int numFields, int numEntries) {
   // Open csv file for recording memory usage statistics for the run group
   std::ofstream csvRunGroupRecord;
   std::string csvFileName = "./csv_records/" + std::to_string(groupNum) + "/groupRecord.csv";
   csvRunGroupRecord.open(csvFileName, std::ios_base::app);

   // Determine the current file name for the pertinent run
   std::string fileName =  "./test_files/" + std::to_string(groupNum) + "/" + std::to_string(runNum) + ".root";

   // Retrieve the number of clusters used in the particular run
   int numClusters = ROOT::RNTupleReader::Open("blank", fileName)->GetDescriptor().GetNClusters();

   // Record information for this specific run to run group csv file
   csvRunGroupRecord << runNum << "," << numFields << "," << numEntries << "," << numClusters << std::endl;
}



void WriteFile(int groupNum, int runNum, int numEntries, int numFields) {
   int numEntriesPerCluster = 30;
   int numClustersPerGroup = 5;

   // Create and open csv file for recording memory usage statistics for given run
   std::ofstream csvRunRecord;
   std::string csvFileName = "./csv_records/" + std::to_string(groupNum) + "/write_" + std::to_string(runNum);
   csvRunRecord.open(csvFileName);

   // Write header row to run specific csv record file
   csvRunRecord << "runNum,EntryNum,vss,rss" << std::endl;
   
   // Creating a unique pointer to an empty data model
   auto model = ROOT::RNTupleModel::Create();

   // Defining the data model
   for (int i = 1; i <= numFields; i++) {
      auto fldPtr = model->MakeField<int>("Category" + std::to_string(i));
   }

   // Create options object in order to edit the write rntuple options
   auto options = ROOT::RNTupleWriteOptions();
   
   // Set the page size based on the field data types such that there is one entry per page
   std::size_t pageSize = sizeof(int);
   options.SetInitialUnzippedPageSize(pageSize);
   options.SetMaxUnzippedPageSize(pageSize);

   // Set the maximum cluster size to be large enough that clusters are not written before cluster writing is manually triggered
   std::size_t clusterSize = 64000000;
   options.SetApproxZippedClusterSize(clusterSize);
   options.SetMaxUnzippedClusterSize(clusterSize);

   // creates a root file and a page sink which the writer connects the model to
   std::string fileName =  "./test_files/" + std::to_string(groupNum) + "/" + std::to_string(runNum) + ".root";
   auto writer = ROOT::RNTupleWriter::Recreate(std::move(model), "blank", fileName, options);

   // Do an initial save of memory usage statistics and run information to csv file
   csvRunRecord << runNum << "," << 0 << "," << get_vss() << "," << get_mem_usage() << std::endl;

   // Create entry pointer to use for loading in and writing entries
   auto entryPtr = writer->CreateEntry();

   // Initialize cluster and clusterGroup counters
   int numClusters = 0;
   int numClusterGroups = 0;

   // Write entries to data model
   for (int i = 1; i <= numEntries; i++) {
      for (int j = 1; j <= numFields; j++) {
         // Create a field pointer so that the different fields of the current entry can be populated
         auto fldPtr = entryPtr->GetPtr<int>("Category" + std::to_string(j));

         *fldPtr = 0;
      }
      // Save memory usage statistics and run information to csv file
      csvRunRecord << runNum << "," << i << "," << get_vss() << "," << get_mem_usage() << std::endl;

      // Fill the entry into the ntuple
      writer->Fill(*entryPtr);

      // Manually commit clusters (and cluster groups when necessary)
      if ((i % numEntriesPerCluster == 0) && (i != 0)) {
         // Boolean condition for determining if a cluster group should be committed
         bool commitClusterGroup = (numClusters % numClustersPerGroup == 0) && (numClusters != 0);
         // Commit the clusters and subsequently commit a cluster group if the aformentioned condition is met
         writer->CommitCluster(commitClusterGroup);

         // Increment respective cluster and clusterGroup counters
         numClusters += 1;
         if (commitClusterGroup) {
            numClusterGroups += 1;
         }
      }
   }
}


void ReadFile(int groupNum, int runNum, int numEntries, int numFields) {
   // Create an empty RNTuple model and get a unique pointer to it
   auto model = ROOT::RNTupleModel::Create();

   // Needs to still be written
}


void read_write_record(int groupNum, int runNum, int numFields, int numEntries, std::string rw) {
   if (std::strcmp(rw.c_str(), "w")==0) {
      WriteFile(groupNum, runNum, numEntries, numFields);
      runInformationRecord(groupNum, runNum, numFields, numEntries);
   } else if (std::strcmp(rw.c_str(), "r")==0) {
      ReadFile(groupNum, runNum, numEntries, numFields);
   } else {
      std::cout << "Incorrect arguments provided. Please review the required command line options and arguments." << std::endl;
   }
}
