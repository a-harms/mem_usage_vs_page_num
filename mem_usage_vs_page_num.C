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



void CreateReport(std::string fileName) {
   // Creating a unique pointer to an empty data model
   auto model = ROOT::RNTupleModel::Create();

   // Defining the data model
   auto fldFileNum = model->MakeField<int>("FileNum");
   auto fldNumFields = model->MakeField<int>("NumFields");
   auto fldNumEntries = model->MakeField<int>("NumEntries");
   auto fldMemUsage = model->MakeField<int>("MemUsage");

   // Creating a root file that will be accessed later for recording memory usage statistics
   ROOT::RNTupleWriter::Recreate(std::move(model), "report", fileName);
}


void AppendReport(std::string fileName, int fileNum, int numEntries, int numFields, int MemUsage) {
   // Functionality to be added yet
}


void AnalyzeReport(std::string fileName) {
   // Getting a unique pointer to an empty RNTuple model
   auto model = ROOT::RNTupleModel::Create();

   // Defining the fields needed for the analysis
   auto fldMemUsage = model->MakeField<int>("MemUsage");

   // Creating an nTuple and attaching the analysis/read data model to it
   auto reader = ROOT::RNTupleReader::Open(std::move(model), "report", fileName);

   // Printing a quick overview of the analysis data
   reader->Show(0);

   // Creating a Canvas for the output plots
   Double_t w = 600;
   Double_t h = 600;
   auto c = new TCanvas("c", "c", w, h);
   c->SetWindowSize(w + (w - c->GetWw()), h + (h - c->GetWh()));


   // Need to enter into RDataFrame
   //auto df = ROOT::RDataFrame df("report", fileName);   // this does not seem to work ):

   // Need to take from RDataFrame into TGraph(?)

   //auto g = new TGraph(numPages, 
   //g->SetTitle("Memory Consumption;number of pages;memory usage (KB)");
   //g->Draw("AC*");

   // Then need to fit plot here
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

      for (int numEntries = 0; j < numFields; j++) {
         auto fldPtr = entryPtr->GetPtr<int>("Category" + std::to_string(i));

         *fldPtr = 0;
      }
   }

   // could also commit the "dataset" by destructing the writer, decide whether this makes sense, see if it makes a difference?
   //writer.reset();
   
   // calculate memory usage by subtracting initial measurement from the current measurement
   long memFinal = get_mem_usage();
   long totalMemUsage = memFinal - memInit;

   // output results
   std::cout << "the total memory usage for the function is: " << std::to_string(totalMemUsage) << std::endl;
   
   // write memory usage along with fileNum, numEntries, and numFields to previously made root file
   AppendReport("./reports/writing_report.root", fileNum, numEntries, numFields, totalMemUsage);
}



void ReadFile(int fileNum, int numEntries, int numFields) {
   // Create an empty RNTuple model and get a unique pointer to it
   auto model = ROOT::RNTupleModel::Create();

   // Needs to still be written
}




void mem_usage_vs_page_num() {
   int maxEntryNum = 4;
   int maxFieldNum = 5;

   int numFiles = 15;
   //int numPages = 
   int fileNum = 0; // will be done through iteration, or maybe swap out for numPages?

   CreateReport("./reports/writing_report.root");
   CreateReport("./reports/reading_report.root");

   for (int numFields = 1; numFields <= maxFieldNum; numFields++) {
      for (int numEntries = 1; numEntries <= maxEntryNum; numEntries++) {

         CreateFile(fileNum, numEntries, numFields);   
         ReadFile(fileNum, numEntries, numFields);
      }
   }
   
   AnalyzeReport("./reports/writing_report.root");
   AnalyzeReport("./reports/reading_report.root");
}
