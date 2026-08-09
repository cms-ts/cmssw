#include "TSystem.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TLatex.h"
#include <iostream>
#include <vector>
#include "TLatex.h"

// Include CMS JEC/JER headers (these work inside your CMSSW environment)
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"

void plot_JES_JER(double test_pt = 40.0, double test_rho = 80.0) {
    // 1. Load the CMSSW libraries needed for JEC/JER
    gSystem->Load("libCondFormatsJetMETObjects");
    gSystem->Load("libJetMETCorrectionsModules");

    // 2. Define filenames and their paths separately
    //std::string jec_file = "Prompt23HIPbPb_V1_MC_L2Relative_AK2PF.txt"; 
    //std::string jer_file = "My_PbPb23_MC_PtResolution_AK2PF.txt";

    // ppref
    std::string jec_file = "Prompt24HIpp_V1_MC_L2Relative_AK2PF.txt";
    std::string jer_file = "My_ppref24_MC_PtResolution_AK2PF.txt";
    // Create path strings that point to the parent directory
    std::string jec_path = "../" + jec_file;
    std::string jer_path = "../derive_JER_AK2/" + jer_file;

    // 3. Setup JEC (Use the PATH here)
    std::vector<JetCorrectorParameters> vPar;
    JetCorrectorParameters L2JetPar(jec_path); // Loads from ../
    vPar.push_back(L2JetPar);
    FactorizedJetCorrector *JetCorrector = new FactorizedJetCorrector(vPar);

    // 4. Setup JER (Use the PATH here)
    JME::JetResolution resolution = JME::JetResolution(jer_path); // Loads from ../

    // 5. Create TGraphs to hold the output
    int n_points = 50;
    TGraph* g_JES = new TGraph(n_points);
    TGraph* g_JER = new TGraph(n_points);

    // 6. Loop over eta from -3. to 3
    double eta_range = 3.;
    for (int i = 0; i < n_points; ++i) {
        double eta = -eta_range + i * (2*eta_range / (n_points - 1));

        // --- Calculate JES (1.0 / JEC) ---
        JetCorrector->setJetEta(eta);
        JetCorrector->setJetPt(test_pt);
        JetCorrector->setJetA(0.1); // Jet Area (approx for AK2)
        JetCorrector->setRho(test_rho);
        double jec = JetCorrector->getCorrection();
        double jes = 1.0 / jec; // Detector Scale is the inverse of the Correction
        g_JES->SetPoint(i, eta, jes);

        // --- Calculate JER ---
        JME::JetParameters jer_params;
        jer_params.setJetPt(test_pt);
        jer_params.setJetEta(eta);
        jer_params.setRho(test_rho);
        double jer = resolution.getResolution(jer_params);
        g_JER->SetPoint(i, eta, jer);
    }

// 7. Drawing the Plots
    TCanvas* c = new TCanvas("c", "JES and JER", 1000, 500);
    c->Divide(2, 1);

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.02);
    latex->SetTextColor(kBlack);
    latex->SetTextFont(42);

    // Style and Draw JES
    c->cd(1);
    gPad->SetLeftMargin(0.15);  // Widen the left margin
    gPad->SetRightMargin(0.05);
    gPad->SetBottomMargin(0.12);

    g_JES->SetTitle(Form("Jet Energy Scale (p_{T} = %.0f GeV, #rho = %.0f);Jet #eta;JES (p_{T}^{reco} / p_{T}^{true})", test_pt, test_rho));
    g_JES->SetMarkerStyle(20); g_JES->SetMarkerColor(kBlack); g_JES->SetLineColor(kBlack);
    g_JES->GetYaxis()->SetRangeUser(0.65, 0.95);
    g_JES->GetYaxis()->SetTitleOffset(1.5); // Push title away from axis numbers
    g_JES->GetXaxis()->SetTitleOffset(1.2);
    g_JES->Draw("APL");
    latex->DrawLatexNDC(0.45, 0.87, jec_file.c_str());

    // Style and Draw JER
    c->cd(2);
    gPad->SetLeftMargin(0.15);  // Widen the left margin
    gPad->SetRightMargin(0.05);
    gPad->SetBottomMargin(0.12);

    g_JER->SetTitle(Form("Jet Energy Resolution (p_{T} = %.0f GeV, #rho = %.0f);Jet #eta;JER (#sigma_{p_{T}} / p_{T})", test_pt, test_rho));
    g_JER->SetMarkerStyle(20); g_JER->SetMarkerColor(kRed+1); g_JER->SetLineColor(kRed+1);
    g_JER->GetYaxis()->SetRangeUser(0.0, 0.3); 
    g_JER->GetYaxis()->SetTitleOffset(1.5); // Push title away from axis numbers
    g_JER->GetXaxis()->SetTitleOffset(1.2);
    g_JER->Draw("APL");
    latex->DrawLatexNDC(0.5, 0.16, jer_file.c_str());

    c->SaveAs("JES_JER_Performance.pdf");
}
