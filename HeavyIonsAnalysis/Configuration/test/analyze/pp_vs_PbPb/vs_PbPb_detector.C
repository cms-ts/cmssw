#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>
#include "TLegend.h"
#include "TMath.h"
#include "TRatioPlot.h"
#include "TLatex.h"
#include "tdrstyle.C"
#include "../helpers.h" // For getLumiFromSummary

void vs_PbPb_detector(std::string variable = "xZj", const char* pb_type = "PbPb23", const char* pp_type = "ppref24", 
                      int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

    setTDRStyle();

    TString s_pb = pb_type;
    TString s_pp = pp_type;
    TString pb_prefix = s_pb.Contains("PbPb24") ? "HI24" : "HI";
    TString pp_prefix = "ppref";

    TString run_tag_pb, run_tag_pp;
    if (ptZ_max > 9000) {
        run_tag_pb = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
        run_tag_pp = Form("_ptZ%.0f_Inf", ptZ_min);
    } else {
        run_tag_pb = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
        run_tag_pp = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
    }
    double Lumi_PbPb = s_pb.Contains("PbPb23") ? getLumiFromSummary("../brilcalc_Collisions2023HI.csv") : (s_pb.Contains("PbPb24") ? getLumiFromSummary("../brilcalc_Collisions2024_HI.csv") : 1.64);
    double Lumi_pp = s_pp.Contains("ppref24") ? getLumiFromSummary("../brilcalc_Collisions2024_ppref.csv") : 479;
    double number_A = 208; // Lead

    // --- SETUP VARIABLES ---
    std::string h_name_base, x_title, y_title;
    double y_max_scale = 1.5;

    if (variable == "deltaPhi") {
        h_name_base = "deltaPhi_Zj";
        x_title = "#Delta#phi_{Zj}";
        y_title = "#frac{1}{N_{Z}} #frac{dN_{Zj}}{d#Delta#phi}";
        y_max_scale = 2.0;
    } else if (variable == "jetPt") {
        h_name_base = "jet_pt_lj";
        x_title = "leading jet p_{T} [GeV]";
        y_title = "#frac{1}{N_{Z}} #frac{dN_{Zj}}{dp_{T}}";
    } else if (variable == "xZj") {
        h_name_base = "xZj"; // or xZj_fixbinw if you prefer
        x_title = "x_{Zj}";
        y_title = "#frac{1}{N_{Z}} #frac{dN_{Zj}}{dx_{Zj}}";
    } else {
        std::cerr << "Unknown variable. Use 'deltaPhi', 'jetPt', or 'xZj'." << std::endl;
        return;
    }

    // --- LOAD FILES ---
    TString pb_file_path = Form("../plot/output_%s_mu_data%s.root", pb_prefix.Data(), run_tag_pb.Data());
    TString pp_file_path = Form("../plot/output_ppref_mu_data%s.root", run_tag_pp.Data());

    TFile* f_pb = TFile::Open(pb_file_path, "READ");
    if (!f_pb || f_pb->IsZombie()) { std::cerr << "Cannot open " << pb_file_path << std::endl; return; }
    
    TFile* f_pp = TFile::Open(pp_file_path, "READ");
    if (!f_pp || f_pp->IsZombie()) { std::cerr << "Cannot open " << pp_file_path << std::endl; return; }

    // --- EXTRACT HISTOGRAMS ---
    // PbPb uses the subtracted histograms
    TH1D* h_pb_raw = (TH1D*)f_pb->Get(Form("%s/Muons/h_%s_subtracted", pb_prefix.Data(), h_name_base.c_str()));
    TH1D* h_mumu_pb = (TH1D*)f_pb->Get(Form("%s/Muons/h_mumu", pb_prefix.Data()));
    
    // pp uses the raw histograms (no mixed event subtraction)
    TH1D* h_pp_raw = (TH1D*)f_pp->Get(Form("ppref/Muons/h_%s", h_name_base.c_str()));
    TH1D* h_mumu_pp = (TH1D*)f_pp->Get("ppref/Muons/h_mumu");

    if (!h_pb_raw || !h_mumu_pb || !h_pp_raw || !h_mumu_pp) {
        std::cerr << "Missing histograms in the root files!" << std::endl; return;
    }

    TH1D* h_pb = (TH1D*)h_pb_raw->Clone("h_pb"); h_pb->SetDirectory(0);
    TH1D* h_pp = (TH1D*)h_pp_raw->Clone("h_pp"); h_pp->SetDirectory(0);

    // --- CALCULATE Nz AND NORMALIZE ---
    double nZ_pb = h_mumu_pb->Integral(0, h_mumu_pb->GetNbinsX()+1);
    double nZ_pp = h_mumu_pp->Integral(0, h_mumu_pp->GetNbinsX()+1);
    
    f_pb->Close(); f_pp->Close();

    std::cout << "PbPb N_Z: " << nZ_pb << " | pp N_Z: " << nZ_pp << std::endl;

    // 1/Nz scaling
    h_pb->Scale(1.0 / nZ_pb);
    h_pp->Scale(1.0 / nZ_pp);

    // Bin width scaling
    h_pb->Scale(1.0, "width");
    h_pp->Scale(1.0, "width");

    // --- DRAWING ---
    int color_pp = TColor::GetColor("#c91f16");
    int color_pb = TColor::GetColor("#1845fb");
    h_pb->SetMarkerStyle(24); h_pb->SetMarkerSize(1.2); h_pb->SetMarkerColor(color_pb); h_pb->SetLineColor(color_pb);
    h_pp->SetMarkerStyle(20); h_pp->SetMarkerSize(1.2); h_pp->SetMarkerColor(color_pp); h_pp->SetLineColor(color_pp);

    int H_ref = 800; int W_ref = 800;
    TCanvas *c = new TCanvas("c", "c", W_ref, H_ref);
    c->SetLeftMargin(0.15); c->SetRightMargin(0.04); c->SetTopMargin(0.08); c->SetBottomMargin(0.12);

    TRatioPlot *h_ratio = new TRatioPlot(h_pb, h_pp, "pois");
    h_ratio->SetH1DrawOpt("EX0"); h_ratio->SetH2DrawOpt("EX0");
    h_ratio->Draw();
    
    h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
    h_ratio->GetLowerRefGraph()->SetMarkerColor(kBlack);
    h_ratio->GetLowerRefGraph()->SetLineColor(kBlack);
    h_ratio->GetLowerRefGraph()->SetMinimum(0.0);
    h_ratio->GetLowerRefGraph()->SetMaximum(3.); // Wider ratio for quenching

    h_pb->GetXaxis()->SetTitle(x_title.c_str());
    h_pb->GetYaxis()->SetTitle(y_title.c_str());
    h_pb->SetMaximum(y_max_scale * std::max(h_pb->GetMaximum(), h_pp->GetMaximum()));
    h_pb->SetMinimum(0.);

    TPad *pad = h_ratio->GetUpperPad();
    pad->cd();
    pad->SetLeftMargin(0.15); // Fix Y-axis clipping
    h_pp->Draw("EsameX0");
    h_pb->Draw("EsameX0");

    TLegend* legend = new TLegend(0.48, 0.7, 0.88, 0.85);
    legend->SetBorderSize(0); legend->SetTextFont(42); legend->SetTextSize(0.035);
    legend->AddEntry(h_pb, Form("PbPb Data (%d-%d%%)", cent_min, cent_max), "PE");
    legend->AddEntry(h_pp, "ppRef Data", "PE");
    legend->Draw();

    // --- Latex Labels ---
    // 1. Standard CMS & Lumi Header
    TLatex* latex = new TLatex(); 
    latex->SetNDC();
    latex->SetTextSize(0.06); 
    latex->SetTextColor(kBlack);
    latex->SetTextFont(61);
    // Align with the left margin of 0.15
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
    latex2->DrawLatex(0.48, 0.92, TString::Format("PbPb %.2f nb^{-1}, pp %.0f pb^{-1} (5.36 TeV)", Lumi_PbPb, Lumi_pp));

    // 2. DYNAMIC CUT LABELS
    TLatex* latex3 = new TLatex();
    latex3->SetNDC();
    latex3->SetTextSize(0.035);
    latex3->SetTextColor(kBlack);
    latex3->SetTextFont(42);

    // Position them in the top-left (since Legend is top-right)
    float textX = 0.525; 
    float textY = 0.64; 

    // Centrality
    latex3->DrawLatex(textX, textY, TString::Format("PbPb Centrality %d-%d%%", cent_min, cent_max));
    textY -= 0.05;

    // Z Kinematics
    if (ptZ_max > 9000) latex3->DrawLatex(textX, textY, Form("p_{T}^{Z} > %.0f GeV", ptZ_min));
    else latex3->DrawLatex(textX, textY, Form("p_{T}^{Z}: %.0f-%.0f GeV", ptZ_min, ptZ_max));
    textY -= 0.05;

    // Jet Kinematics
    latex3->DrawLatex(textX, textY, "AK2 jets");
    textY -= 0.05;
    latex3->DrawLatex(textX, textY, "p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.5");
    textY -= 0.05;

    // dPhi Cut (Applied to Jet pT and xZj, but NOT to the dPhi plot itself)
    if (variable != "deltaPhi") {
        latex3->DrawLatex(textX, textY, "#Delta#phi_{Zj} > 7#pi/8");
    }

    h_ratio->GetLowerRefYaxis()->SetTitle("PbPb / pp");
    h_ratio->GetUpperRefYaxis()->SetTitleOffset(1.6);
    h_ratio->SetGridlines({1.0});

    c->SaveAs(Form("compare_%s_detector%s.pdf", variable.c_str(), run_tag_pb.Data()));
}
