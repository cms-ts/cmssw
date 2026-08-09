#include "TFile.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TString.h"
#include <iostream>
#include <vector>
#include <cmath>

// Define the Gompertz function for ROOT's TF1
// Parameters: [0] = b, [1] = c
// Note: We use exp(b * exp(c * x)) as specified in HIN-16-005. 
// For this to rise from <1 up to 1, 'b' should be negative and 'c' should be negative.
double gompertz_func(double *x, double *par) {
    double pt = x[0];
    double b = par[0];
    double c = par[1];
    return std::exp(b * std::exp(c * pt));
}

void compute_swap_efficiency_fit(const char* input_file = "syst_datadriven/output_HI_mu_data_transverse_Cen0_30_ptZ40_Inf.root", 
                                 const char* output_file = "swap_efficiency_PbPb_fitted.root",
                                 const char* dir_name = "HI/Muons") {
    
    TFile* fIn = TFile::Open(input_file, "READ");
    if (!fIn || fIn->IsZombie()) {
        std::cerr << "Error: Could not open input file: " << input_file << std::endl;
        return;
    }

    TString hist_path = TString::Format("%s/h_transverse_pt_max_vs_cen", dir_name);
    TH2D* h2_raw = (TH2D*)fIn->Get(hist_path);
    if (!h2_raw) {
        std::cerr << "Error: Could not find histogram " << hist_path << std::endl;
        fIn->Close();
        return;
    }

    TFile* fOut = TFile::Open(output_file, "RECREATE");

    // Define the custom, wider centrality bins (in percentages)
    // Note: hiBin goes from 0-200. Centrality % = hiBin / 2.
    // So 0-5% is hiBin 0-10.
    std::vector<std::pair<double, double>> cen_bins_pct = {
        {0.0, 5.0}, {5.0, 10.0}, {10.0, 20.0}, {20.0, 30.0}
    };

    // Prepare a canvas to draw the fits and raw data for visual inspection
    TCanvas* c_fits = new TCanvas("c_fits", "Gompertz Fits", 1000, 800);
    c_fits->Divide(3, 2);

    int pad_idx = 1;

    for (const auto& bin_pair : cen_bins_pct) {
        double cen_min = bin_pair.first;
        double cen_max = bin_pair.second;
        
        // Convert % to hiBin
        int hiBin_min = cen_min * 2;
        int hiBin_max = (cen_max * 2) - 1; // e.g. 0-5% is bins 0 to 9

        // 1. Project the Y-axis (pT) for this specific X-axis (hiBin) range
        // Note: X-axis bins are 1-indexed in ROOT. hiBin 0 is bin 1.
        int binX_min = hiBin_min + 1;
        int binX_max = hiBin_max + 1;
        
        // Create safe string names by replacing '.' with 'p'
        TString s_min = Form("%.1f", cen_min); s_min.ReplaceAll(".", "p");
        TString s_max = Form("%.1f", cen_max); s_max.ReplaceAll(".", "p");
        
        TString proj_name = Form("h_raw_pt_cen_%s_%s", s_min.Data(), s_max.Data());
        TH1D* h_proj = h2_raw->ProjectionY(proj_name, binX_min, binX_max);

        // 2. Count Total Events (N)
        double err_total;
        double N_total = h_proj->IntegralAndError(0, h_proj->GetNbinsX() + 1, err_total);
        
        // Calculate Effective Entries to handle scaled MC weights properly
        double N_eff = (err_total > 0) ? (N_total * N_total) / (err_total * err_total) : 0;

        std::cout << "--- Centrality " << cen_min << "-" << cen_max << "% ---" << std::endl;
        std::cout << "  Total Z events (Sum of Weights): " << N_total << " | Effective Events: " << N_eff << std::endl;

        // Use N_eff for the minimum statistics check
        if (N_eff < 10) {
            std::cout << "  WARNING: Too few effective events. Skipping fit." << std::endl;
            continue;
        }

        // 3. Create the Raw CDF Histogram
        TString cdf_name = Form("h_cdf_cen_%s_%s", s_min.Data(), s_max.Data());
        TH1D* h_cdf = (TH1D*)h_proj->Clone(cdf_name);
        h_cdf->Reset();

        for (int iy = 1; iy <= h_proj->GetNbinsX(); ++iy) {
            // Integral from current pT bin to infinity
            double N_above = h_proj->Integral(iy, h_proj->GetNbinsX() + 1);
            double eff = 1.0 - (N_above / N_total);
            
            // Set bin content. Assign a statistical error based on effective entries!
            h_cdf->SetBinContent(iy, eff);
            double err = (N_eff > 0) ? std::sqrt(eff * (1.0 - eff) / N_eff) : 0;
            h_cdf->SetBinError(iy, err);
        }

        // 4. Perform the Gompertz Fit
        TString func_name = Form("fit_cen_%s_%s", s_min.Data(), s_max.Data());
        // Define function from 0 to 500 GeV so it extrapolates natively, 
        // using the string formula which ROOT can serialize perfectly.
        TF1* fit_func = new TF1(func_name, "exp([0] * exp([1] * x))", 0, 500);
        
        // Initial parameter guesses are critical for exponentials of exponentials
        fit_func->SetParameter(0, -5.0);   // b: determines the depth of the drop
        fit_func->SetParameter(1, -0.05);  // c: determines how fast it recovers
        fit_func->SetParName(0, "b");
        fit_func->SetParName(1, "c");
        fit_func->SetLineColor(kRed);

        // Fit using 'S' (Save result), 'R' (Use function range), but REMOVE 'Q' to see the Minuit output
        TFitResultPtr r = h_cdf->Fit(fit_func, "SRQ", "", 30, 150);

        std::cout << "  Fit Results for " << cen_min << "-" << cen_max << "%:" << std::endl;
        std::cout << "    Status (0=OK): " << r->Status() << std::endl;
        std::cout << "    b = " << fit_func->GetParameter(0) << std::endl;
        std::cout << "    c = " << fit_func->GetParameter(1) << std::endl;
        
        // Evaluate the function at 30, 60, and 100 GeV to see if it makes physical sense
        std::cout << "    Sanity Check (Eff): @30GeV=" << fit_func->Eval(30) 
                  << " | @60GeV=" << fit_func->Eval(60) 
                  << " | @100GeV=" << fit_func->Eval(100) << std::endl;

        // 5. Save and Draw
        fOut->cd();
        h_cdf->Write();
        fit_func->Write();

        c_fits->cd(pad_idx++);
        h_cdf->SetTitle(Form("Centrality %.1f-%.1f%%;Jet p_{T} [GeV];Efficiency", cen_min, cen_max));
        h_cdf->GetXaxis()->SetRangeUser(30, 150);
        h_cdf->SetMarkerStyle(20);
        h_cdf->Draw("EP");
        fit_func->Draw("SAME");
    }

    c_fits->Write();
    fOut->Close();
    fIn->Close();

    std::cout << "Successfully generated fitted Data-Driven Efficiency curves in: " << output_file << std::endl;
}
