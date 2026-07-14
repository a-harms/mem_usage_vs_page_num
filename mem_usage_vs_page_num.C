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
   //ROOT::RDataFrame df("report", fileName);   // this does not seem to work ):

   // Need to take from RDataFrame into TGraph(?)

   //auto g = new TGraph(numPages, 
   //g->SetTitle("Memory Consumption;number of pages;memory usage (KB)");
   //g->Draw("AC*");

   // Then need to fit plot here
}

void results_analysis() {

   CreateReport("./reports/writing_report.root");
   CreateReport("./reports/reading_report.root");

   gROOT->ProcessLine(".x read_write.C+");

   
}
