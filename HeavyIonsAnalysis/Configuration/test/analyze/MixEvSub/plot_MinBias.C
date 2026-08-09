#include "TFile.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>
#include <iomanip>
#include "TLegend.h"
#include "TMath.h"
#include "TRatioPlot.h"
#include "TLatex.h"
#include "TGraph.h"
#include "../helpers.h" 
#include "tdrstyle.C"

std::map<std::string, std::tuple<const char*, const char*, int, double, double>> histo_params = {
    {"h_jet_pt_lj", {"leading jet p_{T} [GeV]", "Events", 30, 0, 300}},
    {"h_deltaPhi_Zj", {"#Delta#phi_{Zj}", "Events" , 20, 0, TMath::Pi()}},
    {"h_xZj", {"x_{Zj}", "Events", 5, 0, 2}},
};

void plot_MinBias(const char* collision_type = "PbPb23", const char* h_n = "h_deltaPhi_Zj", bool isData = true, int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {
    
    if (histo_params.find(h_n) == histo_params.end()) {
        std::cerr << "Error: Histogram '" << h_n << "' not found." << std::endl;
        return;
    }
    const char* x_title; const char* y_title; int bin; double min; double max;
    std::tie(x_title, y_title, bin, min, max) = histo_params[h_n];

    setTDRStyle();
    
    // --- Graphical Setup ---
    gStyle->SetErrorX(0.5);      // Enables horizontal error bars
    gStyle->SetEndErrorSize(0);  // Removes transversal lines (caps)

    TString run_tag;
    if (ptZ_max > 9000) run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
    else run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);

    TString canvName = TString(h_n) + "_MinBias" + run_tag + (isData ? "_data" : "_MC");
    TString name_prefix = TString(collision_type).Contains("PbPb24") ? "HI24" : "HI";
    
    TString fname;
    if (isData) fname.Form("../plot/output_%s_mu_data%s.root", name_prefix.Data(), run_tag.Data());
    else        fname.Form("../plot/output_%s_mu_MC_signal%s.root", name_prefix.Data(), run_tag.Data());

    TFile* file = TFile::Open(fname, "READ");
    TDirectoryFile* dir_Muons = (TDirectoryFile*)file->Get(Form("%s/Muons", name_prefix.Data()));

    TH1D* h_raw = (TH1D*)dir_Muons->Get(h_n);
    TH1D* h_MinBias = (TH1D*)dir_Muons->Get((std::string(h_n) + "_MinBias").c_str());
    TH1D* h_subtracted = (TH1D*)dir_Muons->Get((std::string(h_n) + "_subtracted").c_str());
    TH1D* h_matched = (!isData) ? (TH1D*)dir_Muons->Get((std::string(h_n) + "_matched").c_str()) : nullptr;

    // --- Styling ---
    int bkgColor = TColor::GetColor("#7a21dd");
    int sigColor = TColor::GetColor("#e42536"); 

    // Both Data and MC get markers for Raw and Bkg now
    h_raw->SetLineColor(kBlack); h_raw->SetMarkerStyle(20); h_raw->SetMarkerColor(kBlack); h_raw->SetMarkerSize(0.8);
    h_MinBias->SetLineColor(bkgColor); h_MinBias->SetMarkerStyle(20); h_MinBias->SetMarkerColor(bkgColor); h_MinBias->SetMarkerSize(0.8);
    
    h_subtracted->SetLineColor(sigColor); h_subtracted->SetMarkerStyle(20); h_subtracted->SetMarkerColor(sigColor); h_subtracted->SetMarkerSize(0.8);

    if (!isData) {
        h_matched->SetLineColor(sigColor);
        h_matched->SetMarkerSize(0); // Simple histogram (line) for True
    }

    TCanvas *c = new TCanvas("c", "c", 800, 800);
    c->SetLeftMargin(0.12); c->SetRightMargin(0.04);
    c->SetTopMargin(0.08); c->SetBottomMargin(0.12);

    // TRatioPlot Configuration: Sub / True (MC) or Bkg / Raw (Data)
    TRatioPlot *h_ratio;
    if (!isData) {
        h_ratio = new TRatioPlot(h_subtracted, h_matched, "divsym");
        h_ratio->SetH1DrawOpt("E P"); 
        h_ratio->SetH2DrawOpt("HIST");
    } else {
        h_ratio = new TRatioPlot(h_MinBias, h_raw, "divsym");
        h_ratio->SetH1DrawOpt("E P"); 
        h_ratio->SetH2DrawOpt("E P");
    }
    h_ratio->Draw();

    // Range and Axis
    double y_max_val = h_raw->GetBinContent(h_raw->GetMaximumBin());
    h_ratio->GetUpperRefYaxis()->SetRangeUser(0, 1.4 * y_max_val);
    h_ratio->GetUpperRefYaxis()->SetTitle(y_title);
    
    if (!isData) {
        h_ratio->GetLowerRefYaxis()->SetTitle("Subtracted / True");
        h_ratio->GetLowerRefGraph()->SetMinimum(0.5);
        h_ratio->GetLowerRefGraph()->SetMaximum(1.5);
        h_ratio->GetLowerRefGraph()->SetMarkerColor(sigColor);
        h_ratio->GetLowerRefGraph()->SetLineColor(sigColor);
        std::vector<double> gridlines = {1.0}; 
        h_ratio->SetGridlines(gridlines);
    } else {
        h_ratio->GetLowerRefYaxis()->SetTitle("Bkg Fraction");
        h_ratio->GetLowerRefGraph()->SetMinimum(0.0);
        if (canvName.Contains("delta")) h_ratio->GetLowerRefGraph()->SetMaximum(1.);
        else h_ratio->GetLowerRefGraph()->SetMaximum(0.2);
        h_ratio->GetLowerRefGraph()->SetMarkerColor(bkgColor);
        h_ratio->GetLowerRefGraph()->SetLineColor(bkgColor);
        std::vector<double> gridlines = {}; 
        h_ratio->SetGridlines(gridlines);
    }

    // Style the ratio points (matches numerator styling)
    h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);

    // --- Draw Content (Upper Pad) ---
    h_ratio->GetUpperPad()->cd();
    // Raw, Bkg, and Subtracted all drawn with E P for horizontal and vertical bars
    h_raw->Draw("E P same");      
    h_MinBias->Draw("E P same");
    h_subtracted->Draw("E P same"); 
    if (!isData) h_matched->Draw("HIST same"); // True is simple line

    // --- LaTeX logic ---
    double x_min_lat = canvName.Contains("delta") ? 0.49 : 0.61;

    TLatex* latex1 = new TLatex();
    latex1->SetTextFont(42); latex1->SetTextSize(0.035);
    latex1->DrawLatexNDC(x_min_lat, 0.6, Form("Centrality: %d-%d%%", cent_min, cent_max));
    latex1->DrawLatexNDC(x_min_lat, 0.54, (ptZ_max > 9000) ? Form("p_{T}^{Z} > %.0f GeV, p_{T}^{#mu} > 20 GeV", ptZ_min) : Form("p_{T}^{Z}: %.0f-%.0f GeV, p_{T}^{#mu} > 20 GeV", ptZ_min, ptZ_max));
    latex1->DrawLatexNDC(x_min_lat, 0.48, "p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.1");
    
    if (!canvName.Contains("delta")) latex1->DrawLatexNDC(x_min_lat, 0.44, "#Delta#phi_{Zj} > 7#pi/8");

    // Legend
    TLegend* legend = new TLegend(x_min_lat-0.01, 0.7, x_min_lat+0.21, 0.86);
    legend->SetBorderSize(0); legend->SetTextSize(0.035);
    legend->SetTextFont(42); // 42 is standard; 62 is bold
    legend->AddEntry(h_raw, "Raw", "epl");
    legend->AddEntry(h_MinBias, "Bkg Estimate", "epl");
    legend->AddEntry(h_subtracted, "Raw - Bkg", "epl");
    if (!isData) legend->AddEntry(h_matched, "True (Signal)", "l");
    legend->Draw();

    // CMS Header
    TLatex* l_header = new TLatex();
    l_header->SetTextFont(61); l_header->SetTextSize(0.06);
    l_header->DrawLatexNDC(0.1, 0.92, "CMS");
    l_header->SetTextFont(52); l_header->SetTextSize(0.045);
    l_header->DrawLatexNDC(0.19, 0.92, "Preliminary");

    double Lumi = getLumiFromSummary(TString(collision_type).Contains("24") ? "../brilcalc_Collisions2024_HI.csv" : "../brilcalc_Collisions2023HI.csv");
    l_header->SetTextFont(42); l_header->SetTextSize(0.05);
    l_header->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));

    c->Update();
    c->Print(canvName + ".pdf");
}
