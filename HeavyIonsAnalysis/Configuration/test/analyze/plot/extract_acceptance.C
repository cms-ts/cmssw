#include "TFile.h"
#include "TH1D.h"
#include <iostream>
#include <fstream>

void extract_acceptance(const char* input_file = "output_HI_mu_MC_signal_Cen0_30_ptZ40_Inf.root") {
    // 1. Open your analyzer's output file
    TFile* f_in = TFile::Open(input_file, "READ");
    if (!f_in || f_in->IsZombie()) {
        std::cerr << "Error: Could not open input file: " << input_file << std::endl;
        return;
    }

    // 2. Get the two histograms 
    TH1D* h_tot = (TH1D*)f_in->Get("HI/Muons/h_Z_eta_gen_total");
    TH1D* h_acc = (TH1D*)f_in->Get("HI/Muons/h_Z_eta_gen_accepted");

    if (!h_tot || !h_acc) {
        std::cerr << "Error: Histograms not found! Check the directory path." << std::endl;
        f_in->Close();
        return;
    }

    // 3. Create the Probability Histogram
    TH1D* h_prob = (TH1D*)h_acc->Clone("h_Z_Acceptance_Probability");
    h_prob->SetDirectory(0); // Protects it from ROOT's automatic memory deletion
    h_prob->SetTitle("Z Boson Acceptance Probability vs #eta;Gen Z #eta;Probability (Muons in Acc)");
    
    // Divide: h_acc / h_tot. The "B" option calculates proper binomial efficiency errors.
    h_prob->Divide(h_acc, h_tot, 1.0, 1.0, "B");

    // 4. Save to a plain text .dat file (DO THIS BEFORE CLOSING FILES)
    std::ofstream out_file("Z_Acceptance.dat");
    out_file << "# Eta_Min\tEta_Max\tProbability\tStat_Error" << std::endl;
    
    for (int i = 1; i <= h_prob->GetNbinsX(); ++i) {
        double eta_min = h_prob->GetXaxis()->GetBinLowEdge(i);
        double eta_max = h_prob->GetXaxis()->GetBinUpEdge(i);
        double prob    = h_prob->GetBinContent(i);
        double err     = h_prob->GetBinError(i);
        
        out_file << eta_min << "\t" << eta_max << "\t" << prob << "\t" << err << std::endl;
    }
    out_file.close();

    // 5. Save to a clean, new ROOT file for the theorist
    TFile* f_out = new TFile("Z_Acceptance.root", "RECREATE");
    h_prob->Write();
    f_out->Close();
    
    f_in->Close();

    std::cout << "Done! Generated both 'Z_Acceptance.root' and 'Z_Acceptance.dat'." << std::endl;
}
