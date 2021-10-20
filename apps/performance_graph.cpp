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

int main(int argc, char **argv) {

  // Parse arguments
  std::string ifileName = argv[1];
  std::string ofileName = argv[2];

  std::vector<Record> records;

  // Open input file
  std::ifstream input;
  input.open(ifileName.c_str(), std::ifstream::in);

  // Read data from input file
  if (input.is_open()) {
    std::string line;
    while (std::getline(input, line)) {
      std::stringstream ss(line);
      std::string prun_level, np_num, cpu_time_m0;
      std::getline(ss, prun_level, ',');
      std::getline(ss, np_num, ',');
      std::getline(ss, cpu_time_m0, ',');
      Record r(std::stof(prun_level), std::stoi(np_num), std::stof(cpu_time_m0));
      records.push_back(r);
    }
  }
  else {
    std::cerr << "ERROR: cannot open input file with name " << ifileName << "." << std::endl;
    exit(1);
  }

  // Sort in ascending order of NP number
  std::sort(records.begin(), records.end(), sort_NP_ascending);

  // Create graph
  int n = records.size();
  std::vector<float> x(n), y(n);
  std::transform(records.begin(), records.end(), x.begin(), [](Record r) -> float {return r.getNpNum();});
  std::transform(records.begin(), records.end(), y.begin(), [](Record r) -> float {return r.getCpuTimeM0h();});
  TGraph *g = new TGraph(n, x.data(), y.data());

  // Add labels to graph
  for (std::size_t i = 0; i < n; ++i) {
    std::string label = " " + to_string_with_precision(records[i].getPrunLev(), 1) + "%";
    TLatex *latex = new TLatex(g->GetX()[i], g->GetY()[i], label.c_str());
    latex->SetTextSize(0.035);
    latex->SetTextAlign(13);
    g->GetListOfFunctions()->Add(latex);
  }

  // Create legend
  TLegend *leg = new TLegend(0.20, 0.80, 0.35, 0.90);
  leg->AddEntry(g, "minos 0", "lp");
  
  // Load ATLAS ROOT style
  gROOT->SetStyle("ATLAS");

  // Draw graph
  TCanvas *canv = new TCanvas("canv", "canv", 800, 600);
  g->SetLineColor(kBlue);
  g->SetMarkerStyle(20);
  g->SetMarkerColor(kBlue);
  g->SetTitle("");
  g->GetXaxis()->SetTitle("Number of NP");
  g->GetYaxis()->SetTitle("CPU Time (hours)");
  g->Draw("alp");
  leg->Draw();
  canv->SaveAs(ofileName.c_str());

}
