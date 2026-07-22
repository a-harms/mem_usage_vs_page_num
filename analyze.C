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




void AnalyzeResults(std::string csvRunGroupRecordName) {
   // Create a dataframe from the RunGroupRecord record csv file
   auto df = ROOT::RDF::FromCSV(csvRunGroupRecordName);

   //std::list<int> entryNums;


   // Get the numEvents column values as an RVec collection
   auto entryNumColumnVec = df.Take<int>("numEvents");

   // Get a set of the numEvents column entries
   auto entryNumSet = std::set(entryNumColumnVec.begin(), entryNumColumnVec.end());

   // Iterate over the different numEvents entries
   for (auto entryNum : entryNumSet) {
      std::cout << entryNum << std::endl;
      
   }

}



void analyze(std::string csvRunGroupRecordName, std::string rw) {
   AnalyzeResults()
}
