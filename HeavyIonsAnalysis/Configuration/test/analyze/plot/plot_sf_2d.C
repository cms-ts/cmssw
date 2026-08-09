#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath> // For std::abs

#include "TFile.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TString.h"
#include "TStyle.h"
#include "TColor.h"

// Include the JSON parser and your custom SF handler from the parent directory
#include <nlohmann/json.hpp>
#include "../CorrectionSF.h" 
// #include "../helpers.h" // Uncomment if you want to use getLumiFromSummary

using json = nlohmann::json;

// Helper function to format and save the 2D SF plots
void DrawSFPlot(TH2D* h2, TString output_name, TString collision_type, double Lumi, TString extra_label = "") {
    
    bool isPbPb = collision_type.Contains("PbPb");

    TCanvas* c1 = new TCanvas("c_" + output_name, "SF 2D Plot", 850, 700);
    c1->SetRightMargin(0.18); // Room for Z-axis palette
    c1->SetLeftMargin(0.12);
    c1->SetBottomMargin(0.12);
    c1->SetTopMargin(0.10);

    h2->SetTitle("");
    h2->SetStats(0);
    
    // Conditionally set X-axis title
    if (isPbPb) h2->GetXaxis()->SetTitle("|#eta|");
    else h2->GetXaxis()->SetTitle("#eta");
    
    h2->GetYaxis()->SetTitle("p_{T} (GeV)");
    h2->GetZaxis()->SetTitle("Scalefactor");

    h2->GetXaxis()->SetTitleSize(0.05);
    h2->GetYaxis()->SetTitleSize(0.05);
    h2->GetZaxis()->SetTitleSize(0.05);
    h2->GetXaxis()->SetLabelSize(0.045);
    h2->GetYaxis()->SetLabelSize(0.045);
    h2->GetZaxis()->SetLabelSize(0.04);
    
    h2->GetXaxis()->SetTitleOffset(1.1);
    h2->GetYaxis()->SetTitleOffset(1.1);
    h2->GetZaxis()->SetTitleOffset(1.2);

    // Auto-adjust Z-axis range for better color contrast
    double minZ = h2->GetBinContent(h2->GetMinimumBin());
    double maxZ = h2->GetBinContent(h2->GetMaximumBin());
    h2->GetZaxis()->SetRangeUser(minZ - 0.005, maxZ + 0.005);

    h2->SetMarkerSize(0.6); // Size of the text in the bins
    h2->Draw("COLZ TEXT");

    // --- CMS & Lumi Labels ---
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

    latex->DrawLatexNDC(0.12, 0.92, "CMS");
    latex1->DrawLatexNDC(0.24, 0.92, "Preliminary");

    if (isPbPb) {
        latex2->DrawLatexNDC(0.58, 0.92, TString::Format("%.1f nb^{-1} (5.36 TeV)", Lumi));
    } else {
        latex2->DrawLatexNDC(0.62, 0.92, TString::Format("%.0f pb^{-1} (5.36 TeV)", Lumi));
    }

    // --- Extra Label (e.g., "WP 95%") ---
    if (extra_label != "") {
        TLatex* latex3 = new TLatex();
        latex3->SetTextFont(62); // Bold
        latex3->SetTextSize(0.05);
        latex3->DrawLatexNDC(0.85, 0.50, extra_label);
    }

    c1->SaveAs(output_name + ".pdf");
    c1->SaveAs(output_name + ".png");
    
    delete c1;
}

void plot_sf_2d(const char * collision_type = "PbPb23", double Lumi = 1.6) {
    
    // Set global styles
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird); // Standard CMS heatmap
    gStyle->SetPaintTextFormat(".3f"); // Forces 3 decimal places on the bins

    TString col_str = collision_type;
    bool isPbPb = col_str.Contains("PbPb");

    TString name_output = "HI";
    if (col_str.Contains("PbPb24")) name_output = "HI24";
    else if (col_str.Contains("ppref24")) name_output = "ppref";

    std::cout << "[INFO] Plotting 2D SFs for " << collision_type << " | Lumi: " << Lumi << std::endl;

    // =========================================================================
    // 1. Load JSON Files & Parse with CorrectionSF
    // =========================================================================
    CorrectionSF tightID_SF, hlt_SF, iso_SF;
    
    // Select correct JSON for TightID & HLT based on collision type
    std::string json_filename_id_hlt = isPbPb ? "../HLT_HIL2SingleMu7_and_TightID_abseta1_pt1_cutAndCount_schemaV2.json" 
                                              : "../ppRef_2024_MuonSF_schemaV2.json";

    std::ifstream json_file_id_hlt(json_filename_id_hlt);
    if (json_file_id_hlt.is_open()) {
        std::string json_str((std::istreambuf_iterator<char>(json_file_id_hlt)), std::istreambuf_iterator<char>());
        
        // Sanitize "Infinity"
        size_t pos = 0;
        while ((pos = json_str.find("Infinity", pos)) != std::string::npos) {
            json_str.replace(pos, 8, "9999.0  "); 
            pos += 8;
        }

        json sf_data = json::parse(json_str);
        for (const auto& corr : sf_data["corrections"]) {
            std::string name = corr["name"];
            // Support both PbPb and ppRef scale factor keys
            if (name == "NUM_TightID_DEN_genTracks" || name == "NUM_TightID_DEN_generalTracks") tightID_SF.load(corr["data"]);
            if (name == "NUM_HLT_HIL2SingleMu7_v_DEN_TightID" || name == "NUM_HLT_PPRefL2SingleMu7_v_DEN_TightID") hlt_SF.load(corr["data"]);
        }
        std::cout << "[INFO] TightID & HLT SFs loaded from " << json_filename_id_hlt << std::endl;
    } else {
        std::cerr << "[ERROR] Cannot open TightID/HLT JSON file: " << json_filename_id_hlt << std::endl; return;
    }

    // Load ISO JSON (Same file for both, but requires sanitization just in case)
    std::string json_filename_iso = "../NUM_Iso_DEN_TightID_abseta_pt_schemaV2.json";
    std::ifstream json_file_iso(json_filename_iso);
    if (json_file_iso.is_open()) {
        std::string iso_json_str((std::istreambuf_iterator<char>(json_file_iso)), std::istreambuf_iterator<char>());
        size_t pos = 0;
        while ((pos = iso_json_str.find("Infinity", pos)) != std::string::npos) {
            iso_json_str.replace(pos, 8, "9999.0  ");
            pos += 8;
        }

        json iso_data = json::parse(iso_json_str);
        for (const auto& corr : iso_data["corrections"]) {
            if (corr["name"] == "NUM_IsoWP95_DEN_TightID") iso_SF.load(corr["data"]);
        }
        std::cout << "[INFO] Iso WP95 SFs loaded from " << json_filename_iso << std::endl;
    } else {
        std::cerr << "[ERROR] Cannot open ISO JSON file." << std::endl; return;
    }

    // =========================================================================
    // 2. Define Bin Edges and Create TH2Ds
    // =========================================================================
    
    // We replace the static arrays with vectors to handle dynamic sizing 
    // seamlessly for TH2D creation.
    std::vector<double> v_eta_tight, v_pt_tight;
    std::vector<double> v_eta_hlt,   v_pt_hlt;
    std::vector<double> v_eta_iso,   v_pt_iso;

    // Use full eta range for pp (-2.4 to 2.4), but abseta for PbPb (0 to 2.4)
    if (isPbPb) {
        v_eta_tight = {0.0, 0.35, 0.6, 1.2, 1.6, 2.1, 2.4};
        v_eta_hlt   = {0.0, 0.25, 0.35, 0.6, 1.6, 2.1, 2.4};
        v_eta_iso   = {0.0, 1.6, 1.8, 2.4};
    } else {
        // Mirrored bins
        v_eta_tight = {-2.4, -2.1, -1.6, -1.2, -0.6, -0.35, 0.0, 0.35, 0.6, 1.2, 1.6, 2.1, 2.4};
        v_eta_hlt   = {-2.4, -2.1, -1.6, -0.6, -0.35, -0.25, 0.0, 0.25, 0.35, 0.6, 1.6, 2.1, 2.4};
        v_eta_iso   = {-2.4, -1.8, -1.6, 0.0, 1.6, 1.8, 2.4};
    }

    // Pt bins remain identical
    v_pt_tight = {15.0, 45.0, 200.0};
    v_pt_hlt   = {15.0, 25.0, 35.0, 200.0};
    v_pt_iso   = {20.0, 30.0, 45.0, 200.0};

    TH2D* h_tight = new TH2D("h_tight", "TightID SF",  v_eta_tight.size() - 1, v_eta_tight.data(), v_pt_tight.size() - 1, v_pt_tight.data());
    TH2D* h_hlt   = new TH2D("h_hlt",   "HLT SF",      v_eta_hlt.size() - 1,   v_eta_hlt.data(),   v_pt_hlt.size() - 1,   v_pt_hlt.data());
    TH2D* h_iso   = new TH2D("h_iso",   "ISO WP95 SF", v_eta_iso.size() - 1,   v_eta_iso.data(),   v_pt_iso.size() - 1,   v_pt_iso.data());

    // =========================================================================
    // 3. Fill Histograms using CorrectionSF evaluator
    // =========================================================================
    
    // Fill TightID
    for (int ix = 1; ix <= h_tight->GetNbinsX(); ++ix) {
        for (int iy = 1; iy <= h_tight->GetNbinsY(); ++iy) {
            double eta_center = h_tight->GetXaxis()->GetBinCenter(ix);
            double pt_center  = h_tight->GetYaxis()->GetBinCenter(iy);
            
            double eval_eta = isPbPb ? std::abs(eta_center) : eta_center;
            h_tight->SetBinContent(ix, iy, tightID_SF.getValue(eval_eta, pt_center, "nominal"));
        }
    }

    // Fill HLT
    for (int ix = 1; ix <= h_hlt->GetNbinsX(); ++ix) {
        for (int iy = 1; iy <= h_hlt->GetNbinsY(); ++iy) {
            double eta_center = h_hlt->GetXaxis()->GetBinCenter(ix);
            double pt_center  = h_hlt->GetYaxis()->GetBinCenter(iy);
            
            double eval_eta = isPbPb ? std::abs(eta_center) : eta_center;
            h_hlt->SetBinContent(ix, iy, hlt_SF.getValue(eval_eta, pt_center, "nominal"));
        }
    }

    // Fill ISO
    for (int ix = 1; ix <= h_iso->GetNbinsX(); ++ix) {
        for (int iy = 1; iy <= h_iso->GetNbinsY(); ++iy) {
            double eta_center = h_iso->GetXaxis()->GetBinCenter(ix);
            double pt_center  = h_iso->GetYaxis()->GetBinCenter(iy);
            
            // ISO JSON universally expects abseta
            double eval_eta = std::abs(eta_center);
            h_iso->SetBinContent(ix, iy, iso_SF.getValue(eval_eta, pt_center, "nominal"));
        }
    }

    // =========================================================================
    // 4. Plot and Save
    // =========================================================================
    DrawSFPlot(h_tight, "SF_2D_" + name_output + "_TightID", col_str, Lumi);
    DrawSFPlot(h_hlt,   "SF_2D_" + name_output + "_HLT",     col_str, Lumi);
    DrawSFPlot(h_iso,   "SF_2D_" + name_output + "_ISO_WP95", col_str, Lumi);

    std::cout << "[INFO] Successfully saved all plots to current directory." << std::endl;
}
