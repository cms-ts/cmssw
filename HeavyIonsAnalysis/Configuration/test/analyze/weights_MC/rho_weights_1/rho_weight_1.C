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
#include "../../helpers.h"           // for getLumiFromSummary, cen tables, etc.
//#include "../MC_samples.h" // Include the header file
#include "../tdrstyle.C"

void rho_weight_1(const char * collision_type = "PbPb23", int after_flag  = 0) {

        //histogram parameters
        std::string histo_name = "h_avg_rho";
        std::string x_title = "<#rho>";
        std::string y_title = "";
        int n_bin = 100;
        double x_min = 0.;
        double x_max = 300.;

        setTDRStyle();

        double number_A = 208; // Lead
        // Collision name
        TString collision_name = collision_type; // Collision name
        bool is2023 = true;
        if (collision_name.Contains("PbPb24")) is2023 = false;
        // --- Read Lumi Automatically ---
        double Lumi = getLumiFromSummary("../../brilcalc_Collisions2023HI.csv"); // nb-1
        if (!is2023) Lumi = getLumiFromSummary("../../brilcalc_Collisions2024_HI.csv"); // nb-1
        std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;

        // Create legend
        TLegend* legend = new TLegend(0.66, 0.7, 0.88, 0.8);
        legend->SetBorderSize(0);

        // Open MC file
        TString name_output = "HI";
        if (collision_name.Contains("PbPb24")) name_output = "HI24";
        TString MC_file_name = (after_flag == 0) ? "./output_"+name_output+"_mu_MC_rho_weights.root"
                                                 : "../vz_weights_2/output_"+name_output+"_mu_MC_rho_weights_after.root";
        TFile* file_ = TFile::Open(MC_file_name.Data(), "READ");
        TDirectoryFile* dir = (TDirectoryFile*)file_->Get(name_output+"/Muons");
        TH1D* h = (TH1D*)dir->Get(histo_name.c_str());
        // Get data histogram
        TFile* file_data = TFile::Open("../../plot/output_"+name_output+"_mu_data.root", "READ");
        TDirectoryFile* dir_data = (TDirectoryFile*)file_data->Get(name_output+"/Muons");
        TH1D* h_data = (TH1D*)dir_data->Get(histo_name.c_str());

        // Calculate normalization
        cout << "before norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " data: " << h_data->Integral(0, h_data->GetNbinsX()+1) << endl; 
        double norm_MC = h->Integral(0, h->GetNbinsX()+1);
        double norm_data = h_data->Integral(0, h_data->GetNbinsX()+1);
        h->Scale(1./norm_MC);
        h_data->Scale(1./norm_data);
        cout << "after norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " data: " << h_data->Integral(0, h_data->GetNbinsX()+1) << endl;

        h->SetFillColor(TColor::GetColor("#e42536")); // Simple color assignment
        h->SetLineColor(h->GetFillColor());

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

        // gstyle to remove horizontal error bars, put in rootlogon
        gStyle->SetErrorX(0);

        TRatioPlot *h_ratio = new TRatioPlot(h_data, h, "pois");
        h_ratio->SetH1DrawOpt("EX0");
        h_ratio->SetH2DrawOpt("HIST");
        h_ratio->Draw();
        h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
        h_ratio->GetLowerRefGraph()->SetMinimum(0.4);
        h_ratio->GetLowerRefGraph()->SetMaximum(2.0);
        // Draw MC and data
        double y_max=0;
        //y_max=h_data->GetBinContent(h_data->GetMaximumBin());
        h_data->SetMaximum(0.1);
        h_data->SetMinimum(0.0003);
        h_data->GetXaxis()->SetTitle(x_title.c_str());
        h_data->GetYaxis()->SetTitle(y_title.c_str());
        h_data->SetMarkerStyle(20);
        h_data->SetMarkerSize(1.);
        h_data->SetLineColor(1);
        h_data->SetMarkerColor(1);

        // Legend
        legend->AddEntry(h_data, "Data", "PE");
        legend->AddEntry(h, "Drell-Yan", "f");

        TPad *pad = h_ratio->GetUpperPad();
        pad->cd();
        h->Draw("HISTsame");
        h_data->Draw("EsameX0");
        legend->SetTextFont(42);
        legend->SetTextColor(kBlack);
        legend->SetTextSize(0.036);
        legend->Draw();

        TLatex* latex = new TLatex();
        latex->SetTextSize(0.06); // Set text size (adjust as needed)
        latex->SetTextColor(kBlack); // Set text color (optional)
        latex->SetTextFont(61);
        latex->DrawLatexNDC(0.1,0.92,"CMS");


        TLatex* latex1 = new TLatex();
        latex1->SetTextSize(0.045); // Set text size (adjust as needed)
        latex1->SetTextColor(kBlack); // Set text color (optional)
        latex1->SetTextFont(52);
        latex1->DrawLatexNDC(0.19,0.92,"Preliminary");

        TLatex* latex2 = new TLatex();
        latex2->SetTextSize(0.05); // Set text size (adjust as needed)
        latex2->SetTextColor(kBlack); // Set text color (optional)
        latex2->SetTextFont(42);

        //latex->DrawLatexNDC(0.24,0.86,TString::Format("#int data = %.0f", h_data->Integral(0, h_data->GetNbinsX()+1)));
        latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));

        // Set titles and labels and lines
        h_ratio->GetLowerRefYaxis()->SetTitle("Data/MC");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
        h_ratio->SetGridlines(onlyOneGridline);

        c->Update();

        // Create ratio histogram
        TH1D* h_weight_rho = (TH1D*)h_data->Clone("h_weight_rho");
        h_weight_rho->Divide(h);
        h_weight_rho->SetMinimum(-1.2);
        h_weight_rho->SetMaximum(5.2);

        if (after_flag == 0) {
          TString out_name = "weight_"+name_output+"_rho.root";
          TFile* file_weight_rho = new TFile(out_name, "RECREATE");
          h_weight_rho->Write("h_weight_rho");
          file_weight_rho->Close();
          std::cout << "Weight file created: " << out_name << std::endl;
        }

        // Print the canvas
        std::string is_bef_or_aft = "_before.pdf";
        if (after_flag == 1) is_bef_or_aft = "_after.pdf";
        c->Print((histo_name + "_" + name_output.Data() + is_bef_or_aft).c_str());
}

