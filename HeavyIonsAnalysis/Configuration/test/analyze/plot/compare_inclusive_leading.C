#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>
#include "TLegend.h"
#include "TMath.h"
#include "TRatioPlot.h"
#include "TLatex.h"
#include "../pp_vs_PbPb/tdrstyle.C"
#include "../helpers.h" // For getLumiFromSummary

void compare_inclusive_leading(std::string variable = "jetPt", const char* collision_type = "PbPb23", bool isData = true, 
                               int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

    setTDRStyle();

    // 0. Define Collision Logic & Lumi
    TString collision_name = collision_type;
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

    double Lumi = 1.;
    // Read Lumi Automatically
    if (collision_name.Contains("PbPb23")) Lumi = getLumiFromSummary("../brilcalc_Collisions2023HI.csv");
    else if (collision_name.Contains("PbPb24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_HI.csv");
    else if (collision_name.Contains("ppref24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_ppref.csv");

    // --- SETUP VARIABLES ---
    std::string h_name_leading, h_name_all, x_title, y_title;
    double y_max_scale = 1.5;

    if (variable == "deltaPhi") {
        h_name_leading = "deltaPhi_Zj";
        h_name_all = "deltaPhi_Zj_all";
        x_title = "#Delta#phi_{Zj}";
        y_title = "#frac{1}{N_{Z}} #frac{dN_{Zj}}{d#Delta#phi}";
        y_max_scale = 2.0;
    } else if (variable == "jetPt") {
        h_name_leading = "jet_pt_lj";
        h_name_all = "jet_pt_all";
        x_title = "jet p_{T} [GeV]";
        y_title = "#frac{1}{N_{Z}} #frac{dN_{Zj}}{dp_{T}}";
    } else if (variable == "xZj") {
        h_name_leading = "xZj"; 
        h_name_all = "xZj_all";
        x_title = "x_{Zj}";
        y_title = "#frac{1}{N_{Z}} #frac{dN_{Zj}}{dx_{Zj}}";
    } else {
        std::cerr << "Unknown variable. Use 'deltaPhi', 'jetPt', or 'xZj'." << std::endl;
        return;
    }

    // --- LOAD FILE ---
    TString suffix = isData ? "data" : "MC_signal"; // Assuming signal MC for comparison
    TString file_path = Form("../plot/output_%s_mu_%s%s.root", name_output.Data(), suffix.Data(), run_tag.Data());

    std::cout << "[INFO] Opening file: " << file_path << std::endl;

    TFile* f = TFile::Open(file_path, "READ");
    if (!f || f->IsZombie()) { std::cerr << "Cannot open " << file_path << std::endl; return; }

    // --- EXTRACT HISTOGRAMS ---
    TString dirPath = name_output + "/Muons/";
    
    // We explicitly use the RAW distributions here, as requested
    TH1D* h_lead_raw = (TH1D*)f->Get(dirPath + "h_" + h_name_leading.c_str());
    TH1D* h_all_raw  = (TH1D*)f->Get(dirPath + "h_" + h_name_all.c_str());
    TH1D* h_mumu     = (TH1D*)f->Get(dirPath + "h_mumu");

    if (!h_lead_raw || !h_all_raw || !h_mumu) {
        std::cerr << "Missing histograms in the root file! Check directory structure." << std::endl; return;
    }

    TH1D* h_lead = (TH1D*)h_lead_raw->Clone("h_lead"); h_lead->SetDirectory(0);
    TH1D* h_all  = (TH1D*)h_all_raw->Clone("h_all");   h_all->SetDirectory(0);

    // --- CALCULATE Nz AND NORMALIZE ---
    double nZ = h_mumu->Integral(0, h_mumu->GetNbinsX()+1);
    f->Close();

    std::cout << "Total N_Z in sample: " << nZ << std::endl;

    // 1/Nz scaling
    h_lead->Scale(1.0 / nZ);
    h_all->Scale(1.0 / nZ);

    // Bin width scaling
    h_lead->Scale(1.0, "width");
    h_all->Scale(1.0, "width");

    // --- DRAWING ---
    int color_all = TColor::GetColor("#c91f16"); // Red
    int color_lead = TColor::GetColor("#1845fb"); // Blue

    h_all->SetMarkerStyle(20); h_all->SetMarkerSize(1.2); h_all->SetMarkerColor(color_all); h_all->SetLineColor(color_all);
    h_lead->SetMarkerStyle(24); h_lead->SetMarkerSize(1.2); h_lead->SetMarkerColor(color_lead); h_lead->SetLineColor(color_lead);

    int H_ref = 800; int W_ref = 800;
    TCanvas *c = new TCanvas("c", "c", W_ref, H_ref);
    c->SetLeftMargin(0.15); c->SetRightMargin(0.04); c->SetTopMargin(0.08); c->SetBottomMargin(0.12);

    // Plot All Jets / Leading Jet
    TRatioPlot *h_ratio = new TRatioPlot(h_all, h_lead, "pois");
    h_ratio->SetH1DrawOpt("EX0"); h_ratio->SetH2DrawOpt("EX0");
    h_ratio->Draw();
    
    h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
    h_ratio->GetLowerRefGraph()->SetMarkerColor(kBlack);
    h_ratio->GetLowerRefGraph()->SetLineColor(kBlack);
    h_ratio->GetLowerRefGraph()->SetMinimum(0.8);
    h_ratio->GetLowerRefGraph()->SetMaximum(1.6); // Adjust depending on expected inclusive multiplicity

    h_all->GetXaxis()->SetTitle(x_title.c_str());
    h_all->GetYaxis()->SetTitle(y_title.c_str());
    h_all->SetMaximum(y_max_scale * std::max(h_all->GetMaximum(), h_lead->GetMaximum()));
    h_all->SetMinimum(0.);

    TPad *pad = h_ratio->GetUpperPad();
    pad->cd();
    pad->SetLeftMargin(0.15); // Fix Y-axis clipping
    h_lead->Draw("EsameX0");
    h_all->Draw("EsameX0");

    TLegend* legend = new TLegend(0.48, 0.7, 0.88, 0.85);
    legend->SetBorderSize(0); legend->SetTextFont(42); legend->SetTextSize(0.035);
    legend->AddEntry(h_all, "Inclusive Jets", "PE");
    legend->AddEntry(h_lead, "Leading Jet", "PE");
    legend->Draw();

    // --- Latex Labels ---
    // 1. Standard CMS & Lumi Header
    TLatex* latex = new TLatex(); 
    latex->SetNDC();
    latex->SetTextSize(0.06); 
    latex->SetTextColor(kBlack);
    latex->SetTextFont(61);
    latex->DrawLatex(0.15, 0.92, "CMS");

    TLatex* latex1 = new TLatex();
    latex1->SetNDC();
    latex1->SetTextSize(0.045); 
    latex1->SetTextColor(kBlack);
    latex1->SetTextFont(52);
    latex1->DrawLatex(0.24, 0.92, "Preliminary");

    TLatex* latex2 = new TLatex();
    latex2->SetNDC();
    latex2->SetTextSize(0.040); 
    latex2->SetTextColor(kBlack);
    latex2->SetTextFont(42);

    if (isPbPb) {
        latex2->DrawLatex(0.55, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
    } else {
        latex2->DrawLatex(0.60, 0.92, TString::Format("pp %.0f pb^{-1} (5.36 TeV)", Lumi));
    }

    // 2. DYNAMIC CUT LABELS
    TLatex* latex3 = new TLatex();
    latex3->SetNDC();
    latex3->SetTextSize(0.035);
    latex3->SetTextColor(kBlack);
    latex3->SetTextFont(42);

    float textX = 0.525; 
    float textY = 0.64; 

    // Centrality (Only draw for PbPb)
    if (isPbPb) {
        latex3->DrawLatex(textX, textY, TString::Format("PbPb Centrality %d-%d%%", cent_min, cent_max));
        textY -= 0.05;
    }

    // Z Kinematics
    if (ptZ_max > 9000) latex3->DrawLatex(textX, textY, Form("p_{T}^{Z} > %.0f GeV", ptZ_min));
    else latex3->DrawLatex(textX, textY, Form("p_{T}^{Z}: %.0f-%.0f GeV", ptZ_min, ptZ_max));
    textY -= 0.05;

    // Jet Kinematics
    latex3->DrawLatex(textX, textY, "AK2 jets");
    textY -= 0.05;
    latex3->DrawLatex(textX, textY, "p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.5");
    textY -= 0.05;

    if (variable != "deltaPhi") {
        latex3->DrawLatex(textX, textY, "#Delta#phi_{Zj} > 7#pi/8");
    }

    h_ratio->GetLowerRefYaxis()->SetTitle("All / Lead");
    h_ratio->GetUpperRefYaxis()->SetTitleOffset(1.6);
    h_ratio->SetGridlines({1.0});

    c->SaveAs(Form("compare_inclusive_vs_lead_%s_%s_%s%s.pdf", variable.c_str(), collision_type, suffix.Data(), run_tag.Data()));
}
