#include "TFile.h"
#include "TDirectoryFile.h"
#include "TH1D.h"
#include "THStack.h"
#include "TCanvas.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include "TLegend.h"
#include "TMath.h"
#include "TRatioPlot.h"
#include "TLatex.h"
#include "TGraph.h"
#include "../MC_samples.h" // Include the header file containing 'files' and 'files_ppref'
#include "tdrstyle.C"
#include "../helpers.h" // For getLumiFromSummary

//to store histogram parameters
struct histoPar {
    std::string  histo_name;
    std::string  x_title;
    std::string  y_title;
    int n_bin;
    double x_min;
    double x_max;
};

void h_stack(const char * collision_type = "PbPb23", bool isAlternative = false, int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

    // --- 1. Setup Collision & Directory Logic ---
    TString s_coll = collision_type;
    bool isPbPb = s_coll.Contains("PbPb");

    // Determine internal directory name and file prefix
    TString name_output = "HI";
    if (s_coll.Contains("PbPb24")) name_output = "HI24";
    else if (s_coll.Contains("ppref24")) name_output = "ppref";

    // --- Build the dynamic run tag ---
    TString run_tag;
    if (isPbPb) {
        if (ptZ_max > 9000) run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
        else run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
    } else {
        // Drop centrality tag for pp collisions
        if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
        else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
    }

    // Select the correct MC file vector from MC_samples.h
    const std::vector<FileInfo>* targetVector = (isPbPb) ? &files : &files_ppref;

    // Consider alternative MC as signal
    std::string signal_label = isAlternative ? "alternative" : "signal";
    std::string alternative_label = isAlternative ? "signal" : "alternative";

    // --- 2. Create a vector of histoPar structs ---
    std::vector<histoPar> histo_par = {
    {"h_mumu", "m_{#mu#mu} [GeV]", "Events", 20, 60, 120},
    {"h_Z_pt", "p_{T}^{Z} [GeV]", "Events", 30, 0, 300},
    {"h_njet", "n_{jet}", "Events", 5, 0, 5},
    {"h_cen", "cen", "Events", 20, 0, 100},
    {"h_mumu_j", "m_{#mu#mu} [GeV]", "Events", 20, 60, 120},
    {"h_Z_pt_j", "p_{T}^{Z} [GeV]", "Events", 30, 0, 300},
    {"h_jet_pt_lj", "leading jet p_{T} [GeV]", "Events", 30, 0, 300},
    {"h_jet_pt_lj_nocut", "leading jet p_{T} [GeV]", "Events", 30, 0, 300},
    {"h_jet_pt_lj_2pi_3", "leading jet p_{T} [GeV]", "Events", 30, 0, 300},
    {"h_cen_j", "cen_j", "Events", 20, 0, 100},
    {"h_deltaPhi_Zj", "#Delta#phi_{Zj}", "Events" , 20, 0, std::acos(-1)},
    {"h_xZj_fixbinw", "x_{Zj}", "Events", 30, 0., 3.},
    {"h_vz", "vz", "Events", 30, -20, 20},
    {"h_avg_rho", "<#rho>", "Entries", 50, 0, 400}
    //{"h_jetgirth", "girth", "Events", 10, 0, 0.2},
    //{"h_jet_deltaR", "R_{g}", "Events", 10, 0, 0.2},
    };

    setTDRStyle();

    // --- 3. Read Lumi Automatically ---
    double Lumi = 1.0;
    if (s_coll.Contains("PbPb23"))      Lumi = getLumiFromSummary("../brilcalc_Collisions2023HI.csv");
    else if (s_coll.Contains("PbPb24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_HI.csv");
    else if (s_coll.Contains("ppref24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_ppref.csv");

    if (isPbPb) std::cout << "Running " << s_coll << " | Lumi: " << Lumi << " nb^-1" << std::endl;
    else        std::cout << "Running " << s_coll << " | Lumi: " << Lumi << " pb^-1" << std::endl;

    TCanvas *c[20];
    int ih = 0;

    // --- Check if missing samples are defined in MC_samples.h ---
    bool found_WZto3LNu = false;
    bool found_ZZto2L2Q = false;
    bool used_fallback_WZto3LNu = false;
    bool used_fallback_ZZto2L2Q = false;

    for (const auto& file : *targetVector) {
        if (file.label == "WZto3LNu") found_WZto3LNu = true;
        if (file.label == "ZZto2L2Q") found_ZZto2L2Q = true;
    }

    // --- Loop over histograms ---
    for (const auto& histo : histo_par) {
        const std::string& histo_name = histo.histo_name;
        const std::string& x_title = histo.x_title;
        const std::string& y_title = histo.y_title;
        int n_bin = histo.n_bin;
        double x_min = histo.x_min;
        double x_max = histo.x_max;
        ih++;

        // Create a THStack
        THStack* hs = new THStack("hs", histo_name.c_str());

        // Create histograms *inside* this loop iteration.  Crucially, do *not*
        // give them the same name as the histo_name.

        // Signal
        TH1D* h_DYMM = new TH1D(Form("h_DYMM_%d", ih), histo_name.c_str(), n_bin, x_min, x_max);
        // Background
        TH1D* h_TT = new TH1D(Form("h_TT_%d", ih), histo_name.c_str(), n_bin, x_min, x_max);
        TH1D* h_diboson = new TH1D(Form("h_diboson_%d", ih), histo_name.c_str(), n_bin, x_min, x_max);
        TH1D* h_others = new TH1D(Form("h_others_%d", ih), histo_name.c_str(), n_bin, x_min, x_max);
        // Total, needed for TRatioPlot
        TH1D* h_MC_tot = new TH1D(Form("h_MC_tot_%d", ih), histo_name.c_str(), n_bin, x_min, x_max);

        // Create legend
        float X_leg = 0.622;
        float Y_leg = 0.65;
        if (histo_name == "h_deltaPhi_Zj") X_leg = 0.15;
        if (histo_name.find("h_jet_pt_lj") != std::string::npos) X_leg = 0.4;
        if (histo_name.find("h_njet") != std::string::npos) X_leg = 0.4;
        if (histo_name.find("mumu") != std::string::npos) X_leg = 0.15;
        if (histo_name == "h_xZj_fixbinw") X_leg = 0.46;
        TLegend* legend = new TLegend(X_leg, Y_leg, X_leg+0.19, Y_leg+0.2);
        legend->SetBorderSize(0);

        // --- Loop over MC files (using the correct vector) ---
        for (const auto& file : *targetVector) {
            const std::string& label = file.label;

            // Construct filename dynamically based on collision type, label, and run_tag
            TString full_fname = Form("../plot/output_%s_mu_MC_%s%s.root", name_output.Data(), label.c_str(), run_tag.Data());

            TFile* file_ = TFile::Open(full_fname, "READ");
            if (!file_ || file_->IsZombie()) {
                // --- FALLBACK LOGIC ---
                std::string fallback_label = "";
                if (label == "WZto3LNu") fallback_label = "WWto2L2Nu";

                std::string string_WZto2L2Q = isPbPb ? "WZto2L2Q" : "WZTo2L2Q";
                if (label == "ZZto2L2Q") fallback_label = string_WZto2L2Q;

                if (fallback_label != "") {
                    TString fallback_fname = Form("../plot/output_%s_mu_MC_%s%s.root", name_output.Data(), fallback_label.c_str(), run_tag.Data());

                    // Use ih == 1 so it only prints on the first histogram loop, preventing spam!
                    if (ih == 1) std::cout << "Warning: Could not open " << full_fname << ". Attempting to use fallback file: " << fallback_label << " instead..." << std::endl;

                    file_ = TFile::Open(fallback_fname, "READ");
                    if (file_ && !file_->IsZombie()) {
                        if (label == "WZto3LNu") used_fallback_WZto3LNu = true;
                        if (label == "ZZto2L2Q") used_fallback_ZZto2L2Q = true;
                        if (ih == 1) std::cout << "Fallback successful for " << label << std::endl;
                    } else {
                        if (ih == 1) std::cerr << "Warning: Fallback file also failed to open!" << std::endl;
                        continue; 
                    }
                } else {
                    continue; // Not a target for fallback, move to next file
                }
            }

            // Retrieve from correct internal directory (HI/Muons, ppref/Muons, etc)
            TDirectoryFile* dir = (TDirectoryFile*)file_->Get(name_output + "/Muons");
            if (!dir) continue;

            TH1D* h = (TH1D*)dir->Get(histo_name.c_str());
            if (!h) continue;

            // Debug print
            // std::cout << full_fname << " " << h->Integral(0, h->GetNbinsX()+1) << std::endl;

            if (label==signal_label) {
              h_DYMM->SetFillColor(TColor::GetColor("#e42536"));
              h_DYMM->SetLineColor(h_DYMM->GetFillColor());
              h_DYMM->Add(h);
            }
            else if (label=="TT") {
              h_TT->SetFillColor(TColor::GetColor("#5790fc"));
              h_TT->SetLineColor(h_TT->GetFillColor());
              h_TT->Add(h);
            }
            else if (label=="WWto2L2Nu" || label=="WZTo2L2Q" || label=="WZto3LNu" || label=="ZZto2L2Q" || label=="ZZto2L2Nu" || label=="ZZto4L") {
              h_diboson->SetFillColor(TColor::GetColor("#9c9ca1"));
              h_diboson->SetLineColor(h_diboson->GetFillColor());
              h_diboson->Add(h);
            }
            else if (label != alternative_label) {
              h_others->SetFillColor(TColor::GetColor("#f89c20"));
              h_others->SetLineColor(h_others->GetFillColor());
              h_others->Add(h);
            }
        }// <-- This closes the loop over MC files

        // --- EXPLICIT FALLBACK LOGIC FOR SAMPLES MISSING IN MC_samples.h ---
        if (!found_WZto3LNu) {
            TString fallback_fname = Form("../plot/output_%s_mu_MC_WWto2L2Nu%s.root", name_output.Data(), run_tag.Data());
            TFile* f_fallback = TFile::Open(fallback_fname, "READ");
            if (f_fallback && !f_fallback->IsZombie()) {
                TDirectoryFile* dir = (TDirectoryFile*)f_fallback->Get(name_output + "/Muons");
                if (dir) {
                    TH1D* h_fb = (TH1D*)dir->Get(histo_name.c_str());
                    if (h_fb) {
                        h_diboson->Add(h_fb);
                        used_fallback_WZto3LNu = true;
                        if (ih == 1) std::cout << "Missing WZto3LNu in MC_samples.h. Fallback to WWto2L2Nu successful." << std::endl;
                    }
                }
                f_fallback->Close();
            }
        }

        if (!found_ZZto2L2Q) {
            std::string fallback_label = isPbPb ? "WZto2L2Q" : "WZTo2L2Q";
            TString fallback_fname = Form("../plot/output_%s_mu_MC_%s%s.root", name_output.Data(), fallback_label.c_str(), run_tag.Data());
            TFile* f_fallback = TFile::Open(fallback_fname, "READ");
            if (f_fallback && !f_fallback->IsZombie()) {
                TDirectoryFile* dir = (TDirectoryFile*)f_fallback->Get(name_output + "/Muons");
                if (dir) {
                    TH1D* h_fb = (TH1D*)dir->Get(histo_name.c_str());
                    if (h_fb) {
                        h_diboson->Add(h_fb);
                        used_fallback_ZZto2L2Q = true;
                        if (ih == 1) std::cout << "Missing ZZto2L2Q in MC_samples.h. Fallback to " << fallback_label << " successful." << std::endl;
                    }
                }
                f_fallback->Close();
            }
        }
        // -------------------------------------------------------------------

        hs->Add(h_others);
        hs->Add(h_TT);
        hs->Add(h_diboson);
        hs->Add(h_DYMM);

        h_MC_tot->Add(h_others);
        h_MC_tot->Add(h_TT);
        h_MC_tot->Add(h_diboson);
        h_MC_tot->Add(h_DYMM);
        h_MC_tot->SetFillColor(TColor::GetColor("#e42536"));
        h_MC_tot->SetLineColor(h_DYMM->GetFillColor());

        // --- Get DATA histogram dynamically with run_tag ---
        TString data_fname = Form("../plot/output_%s_mu_data%s.root", name_output.Data(), run_tag.Data());
        TFile* file_data = TFile::Open(data_fname, "READ");
        if (!file_data || file_data->IsZombie()) {
            std::cerr << "Error: Data file not found: " << data_fname << std::endl;
            continue; // Continue to the next histogram instead of fully aborting
        }
        TDirectoryFile* dir_data = (TDirectoryFile*)file_data->Get(name_output + "/Muons");
        TH1D* h_data = (TH1D*)dir_data->Get(histo_name.c_str());
        
        if (!h_data) {
            std::cerr << "Error: Histogram " << histo_name << " not found in data file." << std::endl;
            continue;
        }

        // --- PRINT DETAILED YIELDS ---
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Histogram: " << histo_name << std::endl;
        std::cout << "data:    " << std::fixed << std::setprecision(2) << h_data->Integral(0, h_data->GetNbinsX()+1) << std::endl
                  << "DY+2j:    " << std::fixed << std::setprecision(2) << h_DYMM->Integral(0, h_DYMM->GetNbinsX()+1) << std::endl
                  << "diboson: " << std::fixed << std::setprecision(2) << h_diboson->Integral(0, h_diboson->GetNbinsX()+1) << std::endl
                  << "TT:      " << std::fixed << std::setprecision(2) << h_TT->Integral(0, h_TT->GetNbinsX()+1) << std::endl
                  << "others:  " << std::fixed << std::setprecision(2) << h_others->Integral(0, h_others->GetNbinsX()+1) << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        // --- Create Canvas ---
        int H_ref = 800;
        int W_ref = 800;
        float T = 0.08*H_ref;
        float B = 0.12*H_ref;
        float L = 0.12*W_ref;
        float R = 0.04*W_ref;

        TString c_name = Form("c_%d", ih);
        c[ih] = new TCanvas(c_name, histo_name.c_str(), W_ref, H_ref);
        c[ih]->cd();
        c[ih]->SetFillColor(0);
        c[ih]->SetBorderMode(0);
        c[ih]->SetFrameFillStyle(0);
        c[ih]->SetFrameBorderMode(0);
        c[ih]->SetLeftMargin( L/W_ref );
        c[ih]->SetRightMargin( R/W_ref );
        c[ih]->SetTopMargin( T/H_ref );
        c[ih]->SetBottomMargin( B/H_ref );
        c[ih]->SetTickx(0);
        c[ih]->SetTicky(0);
        c[ih]->SetLogy();

        // gstyle to remove horizontal error bars, put in rootlogon
        gStyle->SetErrorX(0);

        // Explicitly fix the axis range
        double visual_min = x_min;
        double visual_max = x_max;
        if (histo_name.find("Z_pt") != std::string::npos) { visual_min = 40.0; visual_max = 250.0;}
        if (histo_name.find("jet_pt") != std::string::npos) {visual_min = 30.0; visual_max = 250.0;}
        if (histo_name.find("h_deltaPhi_Zj") != std::string::npos) visual_max = std::acos(-1);
        if (histo_name.find("h_njet") != std::string::npos) visual_max = 8;
        h_data->GetXaxis()->SetRangeUser(visual_min, visual_max);
        h_MC_tot->GetXaxis()->SetRangeUser(visual_min, visual_max); // Optional but good practice

        TRatioPlot *h_ratio = new TRatioPlot(h_data, h_MC_tot, "pois");
        h_ratio->SetH1DrawOpt("EX0");
        h_ratio->SetH2DrawOpt("HIST");
        h_ratio->Draw();
        h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
        h_ratio->GetLowerRefGraph()->SetMinimum(0.6);
        h_ratio->GetLowerRefGraph()->SetMaximum(1.4);
        if (histo_name == "h_xZj_fixbinw" || histo_name == "h_njet" || histo_name.find("h_jet_pt_lj") != std::string::npos) {
          h_ratio->GetLowerRefGraph()->SetMinimum(0.2);
          h_ratio->GetLowerRefGraph()->SetMaximum(1.8);
        }
        if (histo_name == "h_deltaPhi_Zj") {
          h_ratio->GetLowerRefGraph()->SetMinimum(0.4);
          h_ratio->GetLowerRefGraph()->SetMaximum(1.6);
        }

        // --- FORCE THE RANGE AFTER DRAWING ---
        // This ensures both the top and bottom pads snap to the correct edges
        h_ratio->GetXaxis()->SetRangeUser(visual_min, visual_max);
        h_ratio->GetLowerRefXaxis()->SetRangeUser(visual_min, visual_max);

        // Draw stack and data
        double y_max = h_data->GetBinContent(h_data->GetMaximumBin());
        if (histo_name == "h_vz") h_data->SetMaximum(180*y_max);
        if (histo_name == "h_njet") h_data->SetMaximum(30*y_max);
        else  h_data->SetMaximum(8.*y_max);
        if (!isPbPb && (histo_name == "h_mumu" || histo_name == "h_Z_pt")) h_data->SetMinimum(0.2);
        else h_data->SetMinimum(0.02);
        h_data->GetXaxis()->SetTitle(x_title.c_str());
        h_data->GetYaxis()->SetTitle(y_title.c_str());
        h_data->SetMarkerStyle(20);
        h_data->SetMarkerSize(1.);
        h_data->SetLineColor(1);
        h_data->SetMarkerColor(1);

        // Legend
        legend->AddEntry(h_data, "Data", "PE");
        if (isAlternative) legend->AddEntry(h_DYMM, "Drell-Yan", "f");
        else legend->AddEntry(h_DYMM, "DY + 2j", "f");
        legend->AddEntry(h_diboson, "Diboson", "f");
        legend->AddEntry(h_TT, "TT", "f");
        legend->AddEntry(h_others, "Others", "f");

        TPad *pad = h_ratio->GetUpperPad();
        pad->cd();
        hs->Draw("HISTsame");
        h_data->Draw("EsameX0");
        legend->SetTextFont(42);
        legend->SetTextColor(kBlack);
        legend->SetTextSize(0.036);
        legend->Draw();

        // --- Latex Labels
        // Standard CMS
        TLatex* latex = new TLatex();
        latex->SetTextSize(0.06);
        latex->SetTextColor(kBlack);
        latex->SetTextFont(61);
        latex->DrawLatexNDC(0.1, 0.92, "CMS");

        TLatex* latex1 = new TLatex();
        latex1->SetTextSize(0.045);
        latex1->SetTextColor(kBlack);
        latex1->SetTextFont(52);
//        latex1->DrawLatexNDC(0.19, 0.92, "Preliminary");

        TLatex* latex2 = new TLatex();
        latex2->SetTextSize(0.05);
        latex2->SetTextColor(kBlack);
        latex2->SetTextFont(42);

        // Adaptive Label for PbPb vs pp
        if (isPbPb) {
          latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
        } else {
          latex2->DrawLatexNDC(0.59, 0.92, TString::Format("pp %.0f pb^{-1} (5.36 TeV)", Lumi));
        }

        // DYNAMIC LABELS
        latex2->SetTextSize(0.034);
        float textX = 0.63; // Starting X position
        float textY = 0.60; // Starting Y position
        if (histo_name == "h_deltaPhi_Zj") {
          textX = 0.39;
          textY = 0.82;
        }
        if (histo_name.find("mumu") != std::string::npos) textY = 0.82;
        if (histo_name == "h_xZj_fixbinw") textY = 0.82;
        if (histo_name.find("h_jet_pt_lj") != std::string::npos) textY = 0.82;
        if (histo_name.find("h_njet") != std::string::npos) textY = 0.82;
        // 1. Centrality (Only for PbPb)
        if (isPbPb) {
            latex2->DrawLatexNDC(textX, textY, TString::Format("Centrality %d-%d%%", cent_min, cent_max));
            textY -= 0.05;
        }

        // 2. Z Kinematics (Always shown)
        if (ptZ_max > 9000) latex2->DrawLatexNDC(textX, textY, Form("p_{T}^{Z} > %.0f GeV, p_{T}^{#mu} > 20 GeV", ptZ_min));
        else latex2->DrawLatexNDC(textX, textY, Form("p_{T}^{Z}: %.0f-%.0f GeV, p_{T}^{#mu} > 20 GeV", ptZ_min, ptZ_max));
        textY -= 0.05;

        // 3. Jet/Selection Specific Labels
        // Check if the current plot is a "Jet Plot"
        bool isJetPlot = (histo_name.find("jet") != std::string::npos ||
                          histo_name.find("xZj") != std::string::npos ||
                          histo_name.find("Phi") != std::string::npos ||
                          histo_name.find("_j")  != std::string::npos);

        if (isJetPlot) {
            latex2->DrawLatexNDC(textX, textY, "AK2 jets");
            textY -= 0.05;
            latex2->DrawLatexNDC(textX, textY, "p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.1");
            textY -= 0.05;

            // Only add dPhi cut label for specific back-to-back plots
            if (histo_name != "h_deltaPhi_Zj" &&
                histo_name != "h_jet_pt_lj_2pi_3" && histo_name != "h_jet_pt_lj_nocut") {
                latex2->DrawLatexNDC(textX, textY, "#Delta#phi_{Zj} > 7#pi/8");
            }
            if (histo_name == "h_jet_pt_lj_2pi_3")
               latex2->DrawLatexNDC(textX, textY, "#Delta#phi_{Zj} > 2#pi/3");
        }

        // Set titles and labels and lines
        h_ratio->GetLowerRefYaxis()->SetTitle("Data/MC");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
        h_ratio->SetGridlines(onlyOneGridline);

        c[ih]->Update();

        // Dynamic output filename with run_tag to prevent overwriting
        if (isAlternative) c[ih]->Print((histo_name + "_" + name_output.Data() + run_tag.Data() + "_alternative_stack.pdf").c_str());
        else c[ih]->Print((histo_name + "_" + name_output.Data() + run_tag.Data() + "_stack.pdf").c_str());
    } // End of histogram loop

    // ==============================================================================
    // PRINT RED WARNING FOR FALLBACKS
    // ==============================================================================
    if (used_fallback_WZto3LNu || used_fallback_ZZto2L2Q) {
        std::string string_WZto2L2Q = isPbPb ? "WZto2L2Q" : "WZTo2L2Q";
        std::cout << "\n\033[1;31m=========================================================\033[0m" << std::endl;
        std::cout << "\033[1;31mWARNING!!! FALLBACK BACKGROUND SAMPLES WERE USED:\033[0m" << std::endl;
        if (used_fallback_WZto3LNu) {
            std::cout << "\033[1;31m -> Missing WZto3LNu was replaced with WWto2L2Nu\033[0m" << std::endl;
        }
        if (used_fallback_ZZto2L2Q) {
            std::cout << "\033[1;31m -> Missing ZZto2L2Q was replaced with " << string_WZto2L2Q << "\033[0m" << std::endl;
        }
        std::cout << "\033[1;31m=========================================================\033[0m\n" << std::endl;
    }
}
