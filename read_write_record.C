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


#include <list>

// For memory tracking
#include <sys/time.h>
#include <sys/resource.h>



// for memory tracking
long get_mem_usage() {
    struct rusage usage;
    int ret;
    ret = getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // in KB
}




void CreateFile(int fileNum, int numEntries, int numFields) {
   // get initial memory usage value
   long memInit = get_mem_usage();
   
   // Creating a unique pointer to an empty data model
   auto model = ROOT::RNTupleModel::Create();

   // Defining the data model
   for (int i = 0; i < numFields; i++) {
      auto fldPtr = model->MakeField<int>("Category" + std::to_string(i));
   }

   // creates a root file and a page sink which the writer connects the model to
   std::string fileName =  "./test_files/fileNum_" + std::to_string(fileNum);
   auto writer = ROOT::RNTupleWriter::Recreate(std::move(model), "blank", fileName);


   // Write entries to data model
   for (int i = 0; i < numEntries; i++) {
      auto entryPtr = writer->CreateEntry();

      for (int j = 0; j < numFields; j++) {
         auto fldPtr = entryPtr->GetPtr<int>("Category" + std::to_string(j));

         *fldPtr = 0;
      }
   }


   //// test memory consumption output
   //int *p = (int *)malloc(1024 * 100);
   //memset(p, 1, 1024*100);


   // calculate memory usage by subtracting initial measurement from the current measurement
   long memFinal = get_mem_usage();
   long totalMemUsage = memFinal - memInit;

   // output results
   std::cout << "numFields: " << numFields  << ", numEntries: " << numEntries << ", memUsage: " << std::to_string(totalMemUsage) << std::endl;
}


void ReadFile(int fileNum, int numEntries, int numFields) {
   // Create an empty RNTuple model and get a unique pointer to it
   auto model = ROOT::RNTupleModel::Create();

   // Needs to still be written
}



void mem_usage_vs_page_num() {
   int maxEntryNum = 4;
   int maxFieldNum = 5;

   int fileNum = 15;
   //int numPages = 

   for (int numFields = 1; numFields <= maxFieldNum; numFields++) {
      for (int numEntries = 1; numEntries <= maxEntryNum; numEntries++) {

         CreateFile(fileNum, numEntries, numFields);   
         ReadFile(fileNum, numEntries, numFields);
      }
   }
   
   //AnalyzeReport("./reports/writing_report.root");
   //AnalyzeReport("./reports/reading_report.root");
}
