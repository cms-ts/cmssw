#include <iostream>
#include <string>
#include "TFile.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TString.h"
#include "TPad.h"
#include "TStyle.h"
#include "../helpers.h"           // for getLumiFromSummary, cen tables, etc.

void plot_veto_check(const char * collision_type = "PbPb23", bool isData = true, int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

    TString mc_sample = "signal"; // Default use signal MC

    // 0. Define Collision Logic & Lumi
    TString collision_name = collision_type; // Collision name
    TString name_output = "HI"; // Default for PbPb23
    bool isPbPb = collision_name.Contains("PbPb");

    if (collision_name.Contains("PbPb24")) name_output = "HI24";
    else if (collision_name.Contains("ppref24")) name_output = "ppref";

    // --- ADD RUN TAG FOR DYNAMIC CUTS ---
    TString run_tag;
    if (isPbPb) {
        if (ptZ_max > 9000) run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
        else run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
    } else {
        // Drop centrality tag for pp collisions
        if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
        else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
    }

    TString filename = "./output_" + name_output + "_mu_";
    if (isData) {
      filename += "data" + run_tag + ".root";
    } else {
      filename += "MC_" + TString(mc_sample) + run_tag + ".root";
    }

    std::cout << "[INFO] Opening file: " << filename << std::endl;
    std::cout << "[INFO] Collision: " << collision_type << " | Type: " << (isData ? "DATA" : "MC") << std::endl;

    double Lumi = 1.;

    // --- Read Lumi Automatically ---
    // Note: Ensure path to csv is correct relative to where you run this script
    if (collision_name.Contains("PbPb23")) Lumi = getLumiFromSummary("../brilcalc_Collisions2023HI.csv"); // nb-1
    else if (collision_name.Contains("PbPb24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_HI.csv"); // nb-1
    else if (collision_name.Contains("ppref24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_ppref.csv"); // pb-1

    if (isPbPb) std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;
    else std::cout << "Parsed Lumi  : " << Lumi << " pb^-1" << std::endl;

    // 1. Open the file
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    // 2. Retrieve the histograms
    // Directory structure inside root file depends on name_output (e.g., "HI/Muons" or "ppref/Muons")
    TString dirPath = name_output + "/Muons/";
    TH2F* h_before = (TH2F*)f->Get(dirPath + "h_jet_etaphi_before");
    TH2F* h_after  = (TH2F*)f->Get(dirPath + "h_jet_etaphi_after");

    if (!h_before || !h_after) {
        std::cerr << "Error: Could not find histograms in " << name_output << "/Muons/." << std::endl;
        return;
    }

    // 3. Remove Titles
    h_before->SetTitle("");
    h_after->SetTitle("");

    // 4. Setup Latex
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.055); // Set text size (adjust as needed)
    latex->SetTextColor(kBlack); // Set text color (optional)
    latex->SetTextFont(61);

    TLatex* latex1 = new TLatex();
    latex1->SetTextSize(0.04); // Set text size (adjust as needed)
    latex1->SetTextColor(kBlack); // Set text color (optional)
    latex1->SetTextFont(52);

    TLatex* latex2 = new TLatex();
    latex2->SetTextFont(42);
    latex2->SetTextSize(0.04);

    // 5. Create Canvas and plot
    TString suffix = isData ? "data" : TString("MC_") + mc_sample;
    TCanvas* c1 = new TCanvas("c1", "Jet Veto Check before", 600, 600);
    c1->cd();
    h_before->SetStats(0);
    h_before->Draw("COLZ");
    // LATEX LINES ***
    latex->DrawLatexNDC(0.1,0.92,"CMS");
    latex1->DrawLatexNDC(0.22,0.92,"Preliminary");
    // Adaptive Label for PbPb vs pp
    if (isPbPb) {
        latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
    } else {
        latex2->DrawLatexNDC(0.59, 0.92, TString::Format("pp %.0f pb^{-1} (5.36 TeV)", Lumi));
    }
    c1->SaveAs("veto_check_before_" + name_output + "_" + suffix + run_tag + ".pdf");

    TCanvas* c2 = new TCanvas("c2", "Jet Veto Check after", 600, 600);
    c2->cd();
    h_after->SetStats(0);
    h_after->SetMinimum(1e-6); // Forces bins with exactly 0 to be drawn as white
    h_after->Draw("COLZ");
    // LATEX LINE ***
    latex->DrawLatexNDC(0.1,0.92,"CMS");
    latex1->DrawLatexNDC(0.22,0.92,"Preliminary");
    // Adaptive Label for PbPb vs pp
    if (isPbPb) {
        latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
    } else {
        latex2->DrawLatexNDC(0.59, 0.92, TString::Format("pp %.0f pb^{-1} (5.36 TeV)", Lumi));
    }
    c2->SaveAs("veto_check_after_" + name_output + "_" + suffix + run_tag + ".pdf");

}
