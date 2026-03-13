#include <iostream>
#include <string>
#include <algorithm>
#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TString.h"
#include "TLegend.h"
#include "TStyle.h"
#include "../helpers.h"           // for getLumiFromSummary, cen tables, etc.

void plot_iso_check(const char * collision_type = "PbPb23", int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

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
        if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
        else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
    }

    // File names dynamically built
    TString file_data_name = "./output_" + name_output + "_mu_data" + run_tag + ".root";
    TString file_mc_name   = "./output_" + name_output + "_mu_MC_" + TString(mc_sample) + run_tag + ".root";

    std::cout << "[INFO] Opening Data file: " << file_data_name << std::endl;
    std::cout << "[INFO] Opening MC file  : " << file_mc_name << std::endl;
    std::cout << "[INFO] Collision: " << collision_type << std::endl;

    double Lumi = 1.;

    // --- Read Lumi Automatically ---
    if (collision_name.Contains("PbPb23")) Lumi = getLumiFromSummary("../brilcalc_Collisions2023HI.csv"); // nb-1
    else if (collision_name.Contains("PbPb24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_HI.csv"); // nb-1
    else if (collision_name.Contains("ppref24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_ppref.csv"); // pb-1

    if (isPbPb) std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;
    else std::cout << "Parsed Lumi  : " << Lumi << " pb^-1" << std::endl;

    // 1. Open the files
    TFile* f_data = TFile::Open(file_data_name);
    TFile* f_mc   = TFile::Open(file_mc_name);
    if (!f_data || f_data->IsZombie() || !f_mc || f_mc->IsZombie()) {
        std::cerr << "Error: Cannot open Data or MC file!" << std::endl;
        return;
    }

    // 2. Retrieve the histograms
    TString dirPath = name_output + "/Muons/";
    TH1D* h_data = (TH1D*)f_data->Get(dirPath + "h_muon_iso_nocut");
    TH1D* h_mc   = (TH1D*)f_mc->Get(dirPath + "h_muon_iso_nocut");

    if (!h_data || !h_mc) {
        std::cerr << "Error: Could not find 'h_muon_iso_nocut' in " << name_output << "/Muons/." << std::endl;
        return;
    }

    // 3. Format Histograms
    h_data->SetTitle("");
    if (isPbPb) {
        h_data->GetXaxis()->SetTitle("Muon MVA Isolation Score");
    } else {
        h_data->GetXaxis()->SetTitle("Muon Relative PF Isolation");
    }
    h_data->SetStats(0);
    h_data->SetMarkerStyle(20);
    h_data->SetMarkerSize(1.2);
    h_data->SetMarkerColor(kBlack);
    h_data->SetLineColor(kBlack);

    h_mc->SetTitle("");
    h_mc->SetStats(0);
    h_mc->SetLineColor(TColor::GetColor("#e42536"));
    h_mc->SetLineWidth(2);
    //h_mc->SetFillColorAlpha(TColor::GetColor("#e42536"), 0.3); // Red

    // Normalize MC to Data area to compare shapes
    if (h_mc->Integral() > 0) {
        h_mc->Scale(h_data->Integral() / h_mc->Integral());
    }

    // Determine Y-axis max so both fit on the plot
    double max_val = std::max(h_data->GetMaximum(), h_mc->GetMaximum());
    h_data->SetMaximum(max_val * 1.4); // Add 40% headroom for legend

    // 4. Setup Latex for CMS labels
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.055); 
    latex->SetTextColor(kBlack); 
    latex->SetTextFont(61);

    TLatex* latex1 = new TLatex();
    latex1->SetTextSize(0.04); 
    latex1->SetTextColor(kBlack); 
    latex1->SetTextFont(52);

    TLatex* latex2 = new TLatex();
    latex2->SetTextFont(42);
    latex2->SetTextSize(0.04);

    // 5. Create Legend
    TLegend* leg = new TLegend(0.65, 0.75, 0.88, 0.88);
    leg->SetBorderSize(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h_data, "Data", "pe");
    leg->AddEntry(h_mc, "Drell-Yan", "l");

    // 6. Create Canvas and draw
    TCanvas* c1 = new TCanvas("c1", "Muon Isolation Check", 800, 600);
    c1->cd();
    
    // Draw Data first for axes, then MC, then redraw Data to keep points on top
    h_data->Draw("E");
    h_mc->Draw("HIST SAME");
    h_data->Draw("E SAME");
    
    leg->Draw("SAME");

    // LATEX LINES ***
    latex->DrawLatexNDC(0.1,0.92,"CMS");
    latex1->DrawLatexNDC(0.22,0.92,"Preliminary");
    
    // Adaptive Label for PbPb vs pp
    if (isPbPb) {
        latex2->DrawLatexNDC(0.6, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
    } else {
        latex2->DrawLatexNDC(0.65, 0.92, TString::Format("pp %.0f pb^{-1} (5.36 TeV)", Lumi));
    }

    // 7. Save dynamically
    c1->SaveAs("muon_iso_DataVsMC_" + name_output + run_tag + ".pdf");
    std::cout << "[INFO] Plot saved as muon_iso_DataVsMC_" << name_output << run_tag << ".pdf" << std::endl;
}
