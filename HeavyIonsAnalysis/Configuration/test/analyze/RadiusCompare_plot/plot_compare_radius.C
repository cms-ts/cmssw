/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//   ANALYSIS MACRO: Compare Gen-Level x_Zj distributions for different Jet Radii (R=0.2, 0.3, 0.4)                           //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#include <iostream>
#include <vector>
#include <cmath>

#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TString.h"
#include "TSystem.h"

using namespace std;

// Helper function to normalize the xZj histogram exactly like in RooUnfoldZjet
void NormalizeHistogram(TH1D* h_xZj, TH1D* h_mumu) {
    if (!h_xZj || !h_mumu) return;

    // 1. Scale by 1 / N_Z (Total generated Z bosons)
    double n_Z = h_mumu->Integral(0, h_mumu->GetNbinsX() + 1);
    if (n_Z > 0) {
        h_xZj->Scale(1.0 / n_Z);
    }

    // 2. Divide by bin width to get dN/dx
    for (int i = 1; i <= h_xZj->GetNbinsX(); ++i) {
        double content = h_xZj->GetBinContent(i);
        double error = h_xZj->GetBinError(i);
        double width = h_xZj->GetBinWidth(i);
        h_xZj->SetBinContent(i, content / width);
        h_xZj->SetBinError(i, error / width);
    }
}

void plot_compare_radius(const char * collision_type = "ppref", 
                         double ptZ_min = 40.0, double ptZ_max = 9999.0,
                         int cent_min = 0, int cent_max = 30) {

    // --- Formatting and Setup ---
    TString s_coll = collision_type;
    bool isPbPb = s_coll.Contains("PbPb");

    TString run_tag;
    if (isPbPb) {
        if (ptZ_max > 9000) run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
        else run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
    } else {
        if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
        else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
    }

    // Define the files to open
    TString file_R02 = Form("output_%s_GenLevel_Zjet_R02%s.root", collision_type, run_tag.Data());
    TString file_R03 = Form("output_%s_GenLevel_Zjet_R03%s.root", collision_type, run_tag.Data());
    TString file_R04 = Form("output_%s_GenLevel_Zjet_R04%s.root", collision_type, run_tag.Data());

    TFile *f02 = TFile::Open(file_R02);
    TFile *f03 = TFile::Open(file_R03);
    TFile *f04 = TFile::Open(file_R04);

    if (!f02 || f02->IsZombie() || !f03 || f03->IsZombie() || !f04 || f04->IsZombie()) {
        std::cerr << "Error: Could not open one or more input files. Check if they exist" << std::endl;
        return;
    }

    // Directory structure inside the files
    TString dir_path = Form("%s/Muons", collision_type);

    // Get Histograms
    TH1D *h_xZj_02 = (TH1D*)f02->Get(dir_path + "/h_xZj_gen");
    TH1D *h_mumu_02 = (TH1D*)f02->Get(dir_path + "/h_mumu_gen");

    TH1D *h_xZj_03 = (TH1D*)f03->Get(dir_path + "/h_xZj_gen");
    TH1D *h_mumu_03 = (TH1D*)f03->Get(dir_path + "/h_mumu_gen");

    TH1D *h_xZj_04 = (TH1D*)f04->Get(dir_path + "/h_xZj_gen");
    TH1D *h_mumu_04 = (TH1D*)f04->Get(dir_path + "/h_mumu_gen");

    if (!h_xZj_02 || !h_xZj_03 || !h_xZj_04) {
        std::cerr << "Error: Could not find xZj histograms in the files." << std::endl;
        return;
    }

    // Detach from files
    h_xZj_02->SetDirectory(0);
    h_xZj_03->SetDirectory(0);
    h_xZj_04->SetDirectory(0);

    // Normalize Histograms
    NormalizeHistogram(h_xZj_02, h_mumu_02);
    NormalizeHistogram(h_xZj_03, h_mumu_03);
    NormalizeHistogram(h_xZj_04, h_mumu_04);

    // --- Styling ---
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    // Setup Colors (Using CMS-friendly distinct colors)
    h_xZj_02->SetLineColor(TColor::GetColor("#e42536"));
    h_xZj_02->SetLineWidth(2);
    h_xZj_02->SetMarkerColor(TColor::GetColor("#e42536"));
    h_xZj_02->SetMarkerStyle(20);

    h_xZj_03->SetLineColor(kBlack);
    h_xZj_03->SetLineWidth(2);
    h_xZj_03->SetMarkerColor(kBlack);
    h_xZj_03->SetMarkerStyle(21);

    h_xZj_04->SetLineColor(TColor::GetColor("#5790fc"));
    h_xZj_04->SetLineWidth(2);
    h_xZj_04->SetMarkerColor(TColor::GetColor("#5790fc"));
    h_xZj_04->SetMarkerStyle(22);

    // Determine the maximum Y value across all three to set axes properly
    double max_val = std::max({h_xZj_02->GetMaximum(), h_xZj_03->GetMaximum(), h_xZj_04->GetMaximum()});
    h_xZj_02->GetYaxis()->SetRangeUser(0.0, max_val * 1.4); // 40% headroom for legend and labels
    h_xZj_02->GetYaxis()->SetTitle("#frac{1}{N_{Z}}#frac{dN_{Zj}}{dx_{Zj}}");
    h_xZj_02->GetXaxis()->SetTitle("x_{Zj}");
    h_xZj_02->GetYaxis()->SetTitleSize(0.032);

    // --- Plotting ---
    TCanvas *c1 = new TCanvas("c1", "Jet Radius Comparison", 800, 600);
    
    // Slight margin adjustments to match RooUnfoldZjet aesthetics
    c1->SetTopMargin(0.1);
    c1->SetRightMargin(0.05);
    c1->cd();

    // Draw histograms
    h_xZj_02->Draw("HIST E");
    h_xZj_03->Draw("HIST E SAME");
    h_xZj_04->Draw("HIST E SAME");

    // --- Legend ---
    TLegend *leg = new TLegend(0.59, 0.70, 0.8, 0.85);
    leg->SetBorderSize(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.034);
    leg->AddEntry(h_xZj_02, "MadGraph+PYTHIA (R = 0.2)", "lpe");
    leg->AddEntry(h_xZj_03, "MadGraph+PYTHIA (R = 0.3)", "lpe");
    leg->AddEntry(h_xZj_04, "MadGraph+PYTHIA (R = 0.4)", "lpe");
    leg->Draw();

    // --- Labels ---
    // Typography exactly matching RooUnfoldZjet
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.06);
    latex->SetTextColor(kBlack);
    latex->SetTextFont(61);
    
    TLatex* latex1 = new TLatex();
    latex1->SetTextSize(0.045);
    latex1->SetTextColor(kBlack);
    latex1->SetTextFont(52);
    
    TLatex* latex3 = new TLatex();
    latex3->SetTextSize(0.035);
    latex3->SetTextColor(kBlack);
    latex3->SetTextFont(42);

    // CMS Simulation Preliminary
    latex->DrawLatexNDC(0.12, 0.92, "CMS");
    latex1->DrawLatexNDC(0.22, 0.92, "Simulation Preliminary");
    // Explicitly NO Luminosity label

    // Dynamic Cut Labels (Placed Top-Right, slightly below the margin)
    float textX = 0.595;
    float textY = 0.6;

    if (isPbPb) {
        latex3->DrawLatexNDC(textX, textY, TString::Format("Centrality %d-%d%%", cent_min, cent_max));
        textY -= 0.05;
    }

    if (ptZ_max > 9000) latex3->DrawLatexNDC(textX, textY, Form("p_{T}^{Z} > %.0f GeV", ptZ_min));
    else latex3->DrawLatexNDC(textX, textY, Form("p_{T}^{Z}: %.0f-%.0f GeV", ptZ_min, ptZ_max));
    textY -= 0.05;

    latex3->DrawLatexNDC(textX, textY, "p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.1");
    textY -= 0.05;
    latex3->DrawLatexNDC(textX, textY, "#Delta#phi_{Zj} > 7#pi/8");

    // --- PRINT MEANS AS LATEX TABLE TO CONSOLE ---
    std::cout << "\n--- LaTeX Table of Gen-Level Means ---" << std::endl;
    std::cout << "\\begin{table}[h]" << std::endl;
    std::cout << " \\begin{center}" << std::endl;
    std::cout << "  \\topcaption{Mean $x_{Zj}$ values for different jet radii at Gen-Level.}" << std::endl;
    std::cout << "  \\label{tab:mean_xzj_radii}" << std::endl;
    std::cout << "  \\begin{tabular}{|c|c|} \\hline" << std::endl;
    std::cout << "  Jet Radius ($R$) & Mean $x_{Zj}$ $\\pm$ $\\sigma_{Stat}$ \\\\ \\hline" << std::endl;
    std::cout << Form("  0.2 & %.4f $\\pm$ %.4f \\\\", h_xZj_02->GetMean(), h_xZj_02->GetMeanError()) << std::endl;
    std::cout << Form("  0.3 & %.4f $\\pm$ %.4f \\\\", h_xZj_03->GetMean(), h_xZj_03->GetMeanError()) << std::endl;
    std::cout << Form("  0.4 & %.4f $\\pm$ %.4f \\\\", h_xZj_04->GetMean(), h_xZj_04->GetMeanError()) << std::endl;
    std::cout << "  \\hline" << std::endl;
    std::cout << "  \\end{tabular}" << std::endl;
    std::cout << " \\end{center}" << std::endl;
    std::cout << "\\end{table}\n" << std::endl;

    // --- Save ---
    //gSystem->mkdir("plot_RadiusCompare", true);
    TString save_name = Form("GenLevel_RadiusCompare_%s%s.pdf", collision_type, run_tag.Data());
    c1->SaveAs(save_name);

    std::cout << "Successfully saved radius comparison plot to: " << save_name << std::endl;
}
