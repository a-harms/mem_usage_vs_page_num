/// \date June 2026
/// \author Anna Helena Harms

#include <ROOT/RDataFrame.hxx>
#include <ROOT/RCsvDS.hxx>

#include <TFrame.h>
#include <TGraph.h>
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
#include <numeric>
#include <algorithm>


//#include <TH1F.h>

struct runInfo {
   int numFields;
   int numEntries;
   int numClusters;

   int minVss;
   int maxVss;
   int minRss;
   int maxRss;
};


long long mean(std::vector<long long> const& vec) {
   return std::accumulate(vec.begin(), vec.end(), 0LL) / (long long) vec.size();
}


std::map<int, runInfo> LoadMap(int groupNum, std::string rw) {
   // Creating empty map to be filled and returned later
   std::map<int, runInfo> runNumToRunInfo;

   // Strings and characters for parsing csv file
   std::string line;
   std::string cell;
   char delimiter = ',';

   // variables for temporarily holding csv values
   int runNum;
   int numFields;
   int numEntries;
   int numClusters;

   // Initialize input file stream for run group record csv file
   std::string csvRunGroupRecordName = "./csv_records/" + std::to_string(groupNum) + "/groupRecord.csv";
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

            std::getline(lineStream, cell, delimiter);
            numClusters = std::stoi(cell);

            // Load the extracted values into the temporary struct
            tempInfoLoader.numFields = numFields;
            tempInfoLoader.numEntries = numEntries;
            tempInfoLoader.numClusters = numClusters;

            // Generate appropriate name for memory usage profile csv file
            std::string runRecordFile = rw;
            if (rw.compare("w") == 0) {
               runRecordFile = "./csv_records/" + std::to_string(groupNum) + "/write_" + std::to_string(runNum);
            } else if (rw.compare("r") == 0) {
               runRecordFile = "./csv_records/" + std::to_string(groupNum) + "/read_" + std::to_string(runNum);
            } else {
               std::cout << "Incorrect read/write setting parameter passed. Please double check parameter used." << std::endl;
            }

            // Load in memory usage profile as RDataFrame
            auto df = ROOT::RDF::FromCSV(runRecordFile);

            auto minVss = df.Min("vss");
            auto maxVss = df.Max("vss");
 
            auto minRss = df.Min("rss");
            auto maxRss = df.Max("rss");

            // Load the values extracted from the data frame into the temporary struct
            tempInfoLoader.minVss = *minVss;
            tempInfoLoader.maxVss = *maxVss;

            tempInfoLoader.minRss = *minRss;
            tempInfoLoader.maxRss = *maxRss;

            // Add new element with run information to runNumToRunInfo map with runNum as key
            runNumToRunInfo[runNum] = tempInfoLoader;
         }
      } else {
         std::cout << "Run Group Record CSV file not open." << std::endl;
     }
   }
   return runNumToRunInfo;
}


void GenerateGroupPlots(int groupNum, map<int, runInfo> runNumToRunInfo) {
   // Find the max numFields and numEntries values assuming that they are held by the last runNumToRunInfo map entry value
   auto lastMapPair = std::prev(runNumToRunInfo.end());
   int maxFieldNum = lastMapPair->second.numFields;
   int maxEntryNum = lastMapPair->second.numEntries;

   // TCanvas
   auto c = new TCanvas("c", "Something here", 0, 0, 1800, 600);
   c->Divide(3);

   // Create empty graphs for plotting memory usage plots
   auto gr1 = new TGraph();
   auto gr2 = new TGraph();
   auto gr3 = new TGraph();


   // Iterate over possible numFields and numEntries parameter values
   for (int i = 1; i <= maxFieldNum; i++) {
      for (int j = 1; j <= maxEntryNum; j++) {

         vector<int> parameterGroupRuns;

         // Traverse runNumToRunInfo map in order to find groups of runs with identical numFields and numEntries parameter values
         for (auto const& ri : runNumToRunInfo) {
            // Track the runNum if the parameters belong to the current parameter group iteration
            if (ri.second.numFields == i && ri.second.numEntries == j) {
               parameterGroupRuns.push_back(ri.first);
            }
         }

         // If there is a group of runs with the current parameter iteration, iterate over them and extract average values
         if (parameterGroupRuns.size() != 0) {

            vector<long long> maxRssValues;
            vector<long long> maxVssValues;
            vector<long long> numClustersValues;

            // Iterate over the parameter run group and track relevant values
            for (auto const& ri : parameterGroupRuns) {
               maxRssValues.push_back((long long)runNumToRunInfo[ri].maxRss);
               maxVssValues.push_back((long long)runNumToRunInfo[ri].maxVss);
               numClustersValues.push_back((long long)runNumToRunInfo[ri].numClusters);
            }

            // Get the average values for the current parameter iteration/group
            double avgMaxRss = mean(maxRssValues);
            double avgMaxVss = mean(maxVssValues);
            long long avgNumClusters = mean(numClustersValues);

            // Convert average rss and vss value to MiB from KiB for nicer plots
            avgMaxRss = avgMaxRss / 1024;
            avgMaxVss = avgMaxVss / 1024;

            // Calculate the number of pages given the current iteration of input parameters
            int numPages = j * i;

            // Save the average values along with the number of pages for the files as points in their appropriate graphs
            gr1->AddPoint(numPages, avgMaxRss);
            gr2->AddPoint(numPages, avgMaxVss);
            gr3->AddPoint(numPages, avgNumClusters);
         }
      }
   }


   // Plot the results

   // Plot the average maximum rss versus the number of pages per file
   c->cd(1);
   gPad->SetGrid();
   gr1->SetTitle("Average Maximum RSS Values");
   gr1->GetXaxis()->SetTitle("Number of pages per file");
   gr1->GetYaxis()->SetTitle("Average maximum RSS for parameter set (MiB)");
   gr1->GetXaxis()->CenterTitle(true);
   gr1->GetYaxis()->CenterTitle(true);
   gr1->Draw("ALP");

   // Plot the average maximum vss versus the number of pages per file
   c->cd(2);
   gPad->SetGrid();
   gr2->SetTitle("Average Maximum VSS Values");
   gr2->GetXaxis()->SetTitle("Number of pages per file");
   gr2->GetYaxis()->SetTitle("Average max VSS value for parameter set (MiB)");
   gr1->GetXaxis()->CenterTitle(true);
   gr1->GetYaxis()->CenterTitle(true);
   gr2->Draw("ALP");

   // Plot the average number of clusters per file versus the number of pages per file
   c->cd(3);
   gPad->SetGrid();
   gr3->SetTitle("Average number of Clusters");
   gr3->GetXaxis()->SetTitle("Number of pages per file");
   gr3->GetYaxis()->SetTitle("Number of clusters written to file");
   gr1->GetXaxis()->CenterTitle(true);
   gr1->GetYaxis()->CenterTitle(true);
   gr3->Draw("ALP");

   // Force the graphics to be rendered to the TCanvas and then save as a pdf
   c->Modified();
   c->Update();
   std::string plotFileName = "./output_plots/" + std::to_string(groupNum) + "/results.pdf";
   c->SaveAs(plotFileName.c_str());
}

void analyze(int groupNum, std::string rw) {
   // Parsing run group record csv file and creating map for accessing run information using run number
   std::map<int, runInfo> runNumToRunInfo = LoadMap(groupNum, rw);

   // Traverse previously created map and make plots for the run group
   GenerateGroupPlots(groupNum, runNumToRunInfo);
}
