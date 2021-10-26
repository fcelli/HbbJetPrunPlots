#include <iostream>
#include <sstream>
#include <vector>

#include "HbbJetPrunPlots/Helpers.h"

// ROOT
#include "TFile.h"

// RooFit
#include "RooWorkspace.h"
#include "RooFitResult.h"
#include "RooRealVar.h"

void usage() {
    std::cout << "From the build/ directory:\n\n./fit_comp list,of,pruning,levels list,of,roofit,workspaces\n\n";
}

struct Parameter {
    Parameter() : name(""), prun_level(0.), value(0.), error_up(0.), error_down(0.) {}
    Parameter(const TString &n, float p, float v, float eu, float ed) : name(n), prun_level(p), value(v), error_up(eu), error_down(ed) {}
    TString name;
    float prun_level;
    float value;
    float error_up;
    float error_down;
};

int main(int argc, char **argv) {

    std::string prun_str, filepath_str;
    const TString workspace_name = "combWS";
    const TString snapshot_name  = "quickfit";
    const TString fitresult_name = "fitResult";

    // Parse arguments
    if (argc != 3) {
        usage();
        exit(1);
    }
    else {
        prun_str     = argv[1];
        filepath_str = argv[2];
    }

    // Split input strings
    std::vector<std::string> prun_levels_str = split_string(prun_str);
    std::vector<std::string> filepath_names  = split_string(filepath_str);

    // Turn pruning levels to float
    std::vector<float> prun_levels(prun_levels_str.size());
    std::transform(prun_levels_str.begin(), prun_levels_str.end(), prun_levels.begin(), [](std::string str) -> float {return std::stof(str);});

    // Check that vectors have the same size
    if(prun_levels.size() != filepath_names.size()) {
        std::cerr << "Error: arguments have different number of entries." << std::endl;
        exit(1);
    }

    // Bundle filenames and pruning level
    std::vector<std::pair<float, TString>> file_pairs;
    for (std::size_t i = 0; i < prun_levels.size(); ++i) {
        file_pairs.push_back(std::pair<float, TString>(prun_levels[i], (TString)filepath_names[i]));
    }

    // Sort filename bundle in ascending pruning level order
    std::sort(file_pairs.begin(), file_pairs.end(), sort_prun_ascending);

    // Loop over files
    std::vector<Parameter> pars;
    for (const std::pair<float, TString> &file_pair : file_pairs) {
        const float prun_level  = file_pair.first;
        const TString file_name = file_pair.second;
        // Open input file
        TFile *f = TFile::Open(file_name.Data(), "READ");
        // Open workspace
        RooWorkspace *ws = (RooWorkspace*)f->Get(workspace_name.Data());
        if (ws == NULL) {
            std::cerr << "ERROR: no workspace with name " << workspace_name << " found in input file." << std::endl;
            exit(1);
        }
        // Load post-fit snapshot
        ws->loadSnapshot(snapshot_name.Data());
        // Fit result
        RooFitResult *fit_result = (RooFitResult*)f->Get(fitresult_name.Data());
        if (fit_result == NULL) {
            std::cerr << "ERROR: no fit result with name " << fitresult_name << " found in input file." << std::endl;
            exit(1);
        }
    
        // Loop over post-fit floating parameters
        RooArgSet all_pars  = (RooArgSet)fit_result->floatParsFinal();
        TIterator *par_iter = all_pars.createIterator();
        RooRealVar *pari;
        while (pari=(RooRealVar*)par_iter->Next()) {
            TString par_name = (TString)pari->GetName();
            if(!par_name.BeginsWith("mu_")) continue;
            Parameter p(par_name, prun_level, pari->getValV(), pari->getErrorHi(), pari->getErrorLo());
            pars.push_back(p);
        }
        delete par_iter;
    }

    // Determine reference pruning level
    const float ref_prun = file_pairs[0].first;

    // Determine unique parameter names
    std::vector<TString> par_names;
    for (const Parameter &par : pars) {
        if (par.prun_level != ref_prun) continue;
        if (std::find(par_names.begin(), par_names.end(), par.name) != par_names.end()) continue;
        else par_names.push_back(par.name);
    }

    // Table format control
    const std::string sep = "|";
    const int prun_width  = 18;
    const int val_width   = 10;
    const int errd_width  = 13;
    const int erru_width  = 11;
    const int sig_width   = 13;
    const int reld_width  = 15;
    const int relu_width  = 13;
    const int num_fields  = 7;
    const int total_width = prun_width + val_width + errd_width + erru_width + sig_width + reld_width + relu_width + num_fields * sep.size();
    const std::string line = sep + std::string( total_width-1, '-' ) + sep;
    for (const TString &par_name : par_names) {
        // Write heading
        std::cout << "\n\n- " << par_name << "\n" << line << '\n' << sep 
        << std::setw(prun_width) << "Pruning Level (%)" << sep
        << std::setw(val_width) << "mu Value" << sep
        << std::setw(errd_width) << "Error Down" << sep
        << std::setw(erru_width) << "Error Up" << sep
        << std::setw(sig_width) << "mu/<sigma>" << sep
        << std::setw(reld_width) << "Rel. Down (%)" << sep
        << std::setw(relu_width) << "Rel. Up (%)" << sep
        << '\n' << line << '\n';
        for (const std::pair<float, TString> &file_pair : file_pairs) {
            const float prun_level = file_pair.first;
            // Find parameter and reference parameter
            const Parameter *par, *ref_par;
            for (const Parameter &p : pars) {
                if (p.name != par_name) continue;
                if (p.prun_level == prun_level) par = &p;
                if (p.prun_level == ref_prun) ref_par = &p;
            }
            // Compute significance and relative error difference
            const float sigma_mean = (fabs(par->error_down) + fabs(par->error_up)) / 2;
            const float significance = par->value / sigma_mean;
            const float rel_diff_down = (par->error_down - ref_par->error_down) * 100 / ref_par->error_down;
            const float rel_diff_up   = (par->error_up - ref_par->error_up) * 100 / ref_par->error_up;
            // Write table rows
            std::cout << sep 
            << std::setw(prun_width) << std::fixed << std::setprecision(1) << par->prun_level << sep
            << std::setw(val_width)  << std::setprecision(4) << par->value << sep
            << std::setw(errd_width) << par->error_down << sep
            << std::setw(erru_width) << par->error_up << sep
            << std::setw(sig_width)  << std::setprecision(2) << significance << sep
            << std::setw(reld_width) << rel_diff_down << sep
            << std::setw(relu_width) << rel_diff_up << sep
            << '\n';
        }
        std::cout << line << '\n';
    }
}