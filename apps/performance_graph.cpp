#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "HbbJetPrunPlots/Record.h"
#include "HbbJetPrunPlots/Helpers.h"

// ROOT
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TMultiGraph.h"

// Script usage function
void usage() {
  std::cout << "From the build/ directory:\n\n./apps/performance_graph path-to-input-file path-to-output-file\n\n";
}

int main(int argc, char **argv) {

  std::string input_name;
  std::string output_name;
  std::vector<Record> records;

  // Parse arguments
  if (argc != 3) {
    usage();
    exit(1);
  }
  else {
    input_name  = argv[1];
    output_name = argv[2];
  }

  // Open input file
  std::ifstream input;
  input.open(input_name.c_str(), std::ifstream::in);

  // Read data from input file
  if (input.is_open()) {
    std::string line;
    while (std::getline(input, line)) {
      std::stringstream ss(line);
      std::string prun_level, np_num, cpu_time_m0, cpu_time_m1, cpu_time_m3;
      std::getline(ss, prun_level, ',');
      std::getline(ss, np_num, ',');
      std::getline(ss, cpu_time_m0, ',');
      std::getline(ss, cpu_time_m1, ',');
      std::getline(ss, cpu_time_m3, ',');
      Record r(std::stof(prun_level), std::stoi(np_num), std::stof(cpu_time_m0), std::stof(cpu_time_m1), std::stof(cpu_time_m3));
      records.push_back(r);
    }
  }
  else {
    std::cerr << "ERROR: cannot open input file with name " << input_name << "." << std::endl;
    exit(1);
  }

  // Sort in ascending order of NP number
  std::sort(records.begin(), records.end(), sort_NP_ascending);

  // Create graphs
  int n = records.size();
  std::vector<float> x(n), y(n);
  std::transform(records.begin(), records.end(), x.begin(), [](Record r) -> float {return r.getNpNum();});
  std::transform(records.begin(), records.end(), y.begin(), [](Record r) -> float {return r.getCpuTimeM0h();});
  TGraph *g0 = new TGraph(n, x.data(), y.data());
  std::transform(records.begin(), records.end(), y.begin(), [](Record r) -> float {return r.getCpuTimeM1h();});
  TGraph *g1 = new TGraph(n, x.data(), y.data());
  std::transform(records.begin(), records.end(), y.begin(), [](Record r) -> float {return r.getCpuTimeM3h();});
  TGraph *g3 = new TGraph(n, x.data(), y.data());
  std::vector<TGraph*> graphs = {g0, g1, g3};
  std::vector<int> colors = {9, 30, 46};
  std::vector<TString> legend_labels = {"Minos 0", "Minos 1", "Minos 3"};
  

  // Add labels to graphs
  for (TGraph *g : graphs) {
    for (std::size_t i = 0; i < n; ++i) {
      std::string label = " " + to_string_with_precision(records[i].getPrunLev(), 1) + "%";
      TLatex *latex = new TLatex(g->GetX()[i], g->GetY()[i], label.c_str());
      latex->SetTextSize(0.03);
      latex->SetTextAlign(13);
      g->GetListOfFunctions()->Add(latex);
    }
  }
  
  // Load ATLAS ROOT style
  gROOT->SetStyle("ATLAS");
  gStyle->SetOptTitle(0);

  // Create legend
  TLegend *leg = new TLegend(0.20, 0.75, 0.35, 0.90);

  // Create TMultiGraph
  TMultiGraph *mg = new TMultiGraph();
  mg->SetTitle("title;Number of NPs;CPU Time (hours)");

  for (std::size_t i = 0; i < graphs.size(); ++i) {
    // Legend entry
    leg->AddEntry(graphs[i], legend_labels[i], "lp");
    // Graphical options
    graphs[i]->SetLineColor(colors[i]);
    graphs[i]->SetMarkerColor(colors[i]);
    graphs[i]->SetMarkerStyle(20);
    // Add to TMultiGraph
    mg->Add(graphs[i]);
  }

  // Draw graphs
  TCanvas *canv = new TCanvas("canv", "canv", 800, 600);
  mg->Draw("alp");
  leg->Draw();
  canv->SaveAs(output_name.c_str());
}
