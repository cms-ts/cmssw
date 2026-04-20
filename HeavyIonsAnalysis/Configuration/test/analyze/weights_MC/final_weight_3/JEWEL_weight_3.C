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
#include "../../helpers.h" // For getLumiFromSummary
#include "../tdrstyle.C"

void JEWEL_weight_3(const char * collision_type = "PbPb23", int after_flag = 0, int cent_min = 0, int cent_max = 30, double ptZ_min = 40.0, double ptZ_max = 9999.0) {

        // Setup Collision Logic ---
        TString s_coll = collision_type;
        bool isPbPb = s_coll.Contains("PbPb");

        // Build the dynamic run tag
        TString run_tag;
        if (isPbPb) {
          if (ptZ_max > 9000) run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
          else run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
        } else {
          if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
          else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
        }

        //histogram parameters
        std::string histo_name = "h_xZj_for_JEWEL_w";
        std::string x_title = "x_{Zj}";
        std::string y_title = "";
        int n_bin = 30;
        double x_min = 0.;
        double x_max = 3.;

        setTDRStyle();

        double Lumi = 1.;
        double number_A = 208; // Lead

        // --- Read Lumi Automatically ---
        if (s_coll.Contains("PbPb23"))      Lumi = getLumiFromSummary("../../brilcalc_Collisions2023HI.csv");
        else if (s_coll.Contains("PbPb24")) Lumi = getLumiFromSummary("../../brilcalc_Collisions2024_HI.csv");
        else if (s_coll.Contains("ppref24")) Lumi = getLumiFromSummary("../../brilcalc_Collisions2024_ppref.csv");

        if (isPbPb) std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;
        else        std::cout << "Parsed Lumi  : " << Lumi << " pb^-1" << std::endl;

        // Create legend
        double xmin_leg = (after_flag == 0) ? 0.6 : 0.55;
        TLegend* legend = new TLegend(xmin_leg, 0.7, 0.85, 0.8);
        legend->SetBorderSize(0);

        // Determine internal names (HI, HI24, ppref)
        TString name_output = "HI";
        if (s_coll.Contains("PbPb24")) name_output = "HI24";
        else if (s_coll.Contains("ppref24")) name_output = "ppref";

        // Open MC file Dynamically with the run_tag
        TString MC_file_name;
        if (after_flag == 0) {
          MC_file_name = "../../plot/output_" + name_output + "_mu_MC_signal" + run_tag + ".root";
        } else {
          MC_file_name = "../../syst_prior_model/output_" + name_output + "_mu_MC_prior_model" + run_tag + ".root";
        }
        std::cout << "Opening MC file   : " << MC_file_name << std::endl;

        TFile* file_ = TFile::Open(MC_file_name, "READ");
        if (!file_ || file_->IsZombie()) {
          std::cerr << "Error: Cannot open MC file " << MC_file_name << std::endl;
          return;
        }

        TDirectoryFile* dir = (TDirectoryFile*)file_->Get(name_output + "/Muons");
        if (!dir) {
          std::cerr << "Error: Directory " << name_output << "/Muons not found in MC file." << std::endl;
          return;
        }
        TH1D* h = (TH1D*)dir->Get(histo_name.c_str());
        if (!h) {
          std::cerr << "Error: Histogram " << histo_name << " not found." << std::endl;
          return;
        }

        // --- Open JEWEL File Dynamically ---
        TString jewel_path;
        if (isPbPb) {
          // PbPb Case: Use the new parameterized QGP files
          TString pt_tag;
          if (ptZ_max > 9000) pt_tag = Form("_ptZ%.0f_Inf", ptZ_min);
          else pt_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
          
          jewel_path = Form("/gfsvol01/cms/users/rdelliga/work/Hi_forest/JEWEL/CMSSW_13_2_13/src/jewel-2.4.0/jewel_converted_Zj_QGP_Cen%d_%d.pu14%stest.root", cent_min, cent_max, pt_tag.Data());
        } else {
          // ppref Case: Use Alternative MC with run_tag
          jewel_path = "../../plot/output_" + name_output + "_mu_MC_alternative" + run_tag + ".root";
        }

        std::cout << "Opening Jewel file   : " << jewel_path << std::endl;

        TFile* file_JEWEL = TFile::Open(jewel_path, "READ");
        if (!file_JEWEL || file_JEWEL->IsZombie()) {
          std::cerr << "Error: Cannot open JEWEL file " << jewel_path << std::endl;
          return;
        }
        
        // Get JEWEL histogram
        TH1D* h_JEWEL = nullptr;
        if (isPbPb) h_JEWEL = (TH1D*)file_JEWEL->Get(histo_name.c_str());
        else {
          TDirectoryFile* dir_JEWEL = (TDirectoryFile*)file_JEWEL->Get(name_output + "/Muons");
          if (!dir_JEWEL) {
            std::cerr << "Error: Directory " << name_output << "/Muons not found in MC file." << std::endl;
            return;
          }
          h_JEWEL = (TH1D*)dir_JEWEL->Get(histo_name.c_str());
        }
        
        if (!h_JEWEL) {
          std::cerr << "Error: JEWEL Histogram " << histo_name << " not found." << std::endl;
          return;
        }

        // Calculate normalization
        cout << "before norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " JEWEL: " << h_JEWEL->Integral(0, h_JEWEL->GetNbinsX()+1) << endl; 
        double norm_MC = h->Integral(0, h->GetNbinsX()+1);
        double norm_JEWEL = h_JEWEL->Integral(0, h_JEWEL->GetNbinsX()+1);
        if (norm_MC > 0) h->Scale(1./norm_MC);
        if (norm_JEWEL > 0) h_JEWEL->Scale(1./norm_JEWEL);
        cout << "after norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " JEWEL: " << h_JEWEL->Integral(0, h_JEWEL->GetNbinsX()+1) << endl;

        // Create canvas
        int H_ref = 800;
        int W_ref = 800;

        // references for T, B, L, R
        float T = 0.08*H_ref;
        float B = 0.12*H_ref;
        float L = 0.12*W_ref;
        float R = 0.04*W_ref;
        TCanvas *c = new TCanvas("c", "c", W_ref, H_ref);
        c->cd();
        c->SetFillColor(0);
        c->SetBorderMode(0);
        c->SetFrameFillStyle(0);
        c->SetFrameBorderMode(0);
        c->SetLeftMargin( L/W_ref );
        c->SetRightMargin( R/W_ref );
        c->SetTopMargin( T/H_ref );
        c->SetBottomMargin( B/H_ref );
        c->SetTickx(0);
        c->SetTicky(0);
        c->SetLogy();

        gStyle->SetErrorX(0);

        TRatioPlot *h_ratio = new TRatioPlot(h_JEWEL, h, "divsym");
        h_ratio->SetH1DrawOpt("E");
        h_ratio->SetH2DrawOpt("E");
        h_ratio->Draw();
        h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
        h_ratio->GetLowerRefGraph()->SetMinimum(0.4);
        h_ratio->GetLowerRefGraph()->SetMaximum(1.6);
        
        h_JEWEL->SetMaximum(0.5);
        h_JEWEL->SetMinimum(0.00004);
        h_JEWEL->GetXaxis()->SetTitle(x_title.c_str());
        h_JEWEL->GetYaxis()->SetTitle(y_title.c_str());
        h_JEWEL->SetMarkerStyle(20);
        h_JEWEL->SetMarkerSize(0.7);
        h_JEWEL->SetLineColor(TColor::GetColor("#7a21dd"));
        h_JEWEL->SetMarkerColor(TColor::GetColor("#7a21dd"));
        if (after_flag == 0) {
          h->SetMarkerStyle(20);
          h->SetMarkerSize(0.7);
          h->SetLineColor(TColor::GetColor("#e42536"));
          h->SetMarkerColor(TColor::GetColor("#e42536"));
        }
        else {
          h->SetMarkerStyle(20);
          h->SetMarkerSize(0.7);
          h->SetLineColor(TColor::GetColor("#9c9ca1"));
          h->SetMarkerColor(TColor::GetColor("#9c9ca1"));
        }

        // Legend
        if (isPbPb) legend->AddEntry(h_JEWEL, "JEWEL (truth) med", "epl");
        else legend->AddEntry(h_JEWEL, "POWHEG+PYTHIA (truth)", "epl");
        if (after_flag == 0) legend->AddEntry(h, "MadGraph+PYTHIA", "epl");
        else legend->AddEntry(h, "MadGraph+PYTHIA (rew)", "epl");

        TPad *pad = h_ratio->GetUpperPad();
        pad->cd();
        h_JEWEL->Draw("Esame");
        h->Draw("Esame");
        legend->SetTextFont(42);
        legend->SetTextColor(kBlack);
        legend->SetTextSize(0.03);
        legend->Draw();

        TLatex* latex = new TLatex();
        latex->SetTextSize(0.06); 
        latex->SetTextColor(kBlack); 
        latex->SetTextFont(61);
        latex->DrawLatexNDC(0.1,0.92,"CMS");

        TLatex* latex1 = new TLatex();
        latex1->SetTextSize(0.045); 
        latex1->SetTextColor(kBlack); 
        latex1->SetTextFont(52);
        latex1->DrawLatexNDC(0.19,0.92,"Preliminary");

        TLatex* latex2 = new TLatex();
        latex2->SetTextSize(0.05); 
        latex2->SetTextColor(kBlack); 
        latex2->SetTextFont(42);

        if (isPbPb) {
          latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
        } else {
          latex2->DrawLatexNDC(0.59, 0.92, TString::Format("pp %.0f pb^{-1} (5.36 TeV)", Lumi));
        }

        // --- Dynamic Cut Labels (Top-Left or Center-Right) ---
        latex2->SetTextSize(0.035);
        float textX = xmin_leg+0.01;
        float textY = 0.65;

        if (isPbPb) {
            latex2->DrawLatexNDC(textX, textY, TString::Format("Centrality %d-%d%%", cent_min, cent_max));
            textY -= 0.05;
        }

        if (ptZ_max > 9000) latex2->DrawLatexNDC(textX, textY, Form("p_{T}^{Z} > %.0f GeV", ptZ_min));
        else latex2->DrawLatexNDC(textX, textY, Form("p_{T}^{Z}: %.0f-%.0f GeV", ptZ_min, ptZ_max));
        textY -= 0.05;

        latex2->DrawLatexNDC(textX, textY, "AK2 jets");
        textY -= 0.05;
        latex2->DrawLatexNDC(textX, textY, "p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.5");
        textY -= 0.05;
        latex2->DrawLatexNDC(textX, textY, "#Delta#phi_{Zj} > 7#pi/8");

        h_ratio->GetLowerRefYaxis()->SetTitle("JEW/POW");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  
        h_ratio->SetGridlines(onlyOneGridline);

        c->Update();

        // Create ratio histogram
        TH1D* h_weight_JEWEL = (TH1D*)h_JEWEL->Clone("h_weight_JEWEL");
        h_weight_JEWEL->Divide(h);

        if (after_flag == 0) {
          // --- WRITE TAGGED WEIGHT FILE ---
          TFile* file_weight_JEWEL = new TFile("weight_" + name_output + "_JEWEL" + run_tag + ".root", "RECREATE");
          h_weight_JEWEL->Write("h_weight_JEWEL");
          file_weight_JEWEL->Close();
          std::cout << "Weight file created: weight_" << name_output << "_JEWEL" << run_tag << ".root" << std::endl;
        }

        // Print the canvas with the tag
        std::string is_bef_or_aft = "_before.pdf";
        if (after_flag == 1) is_bef_or_aft = "_after.pdf";
        c->Print((histo_name + "_" + name_output.Data() + run_tag.Data() + is_bef_or_aft).c_str());
}
