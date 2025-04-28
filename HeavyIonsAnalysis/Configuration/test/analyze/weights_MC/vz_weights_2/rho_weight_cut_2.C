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
//#include "../MC_samples.h" // Include the header file
#include "../tdrstyle.C"
#include "../mycolor.h"

void rho_weight_cut_2() {

        //histogram parameters
        std::string histo_name = "h_avg_rho";
        std::string x_title = "<#rho>";
        std::string y_title = "";
        int n_bin = 80;
        double x_min = 0.;
        double x_max = 400.;

        setTDRStyle();

        double Lumi = 1.64; // nb-1
        double number_A = 208; // Lead

        // Create legend
        TLegend* legend = new TLegend(0.66, 0.7, 0.88, 0.8);
        legend->SetBorderSize(0);

        // Open MC file
        TFile* file_ = TFile::Open("./output_HI_mu_MC_vz_weights.root", "READ");
        TDirectoryFile* dir = (TDirectoryFile*)file_->Get("HI/Muons");
        TH1D* h = (TH1D*)dir->Get(histo_name.c_str());
        // Get data histogram
        TFile* file_data = TFile::Open("../../plot/output_HI_mu_data.root", "READ");
        TDirectoryFile* dir_data = (TDirectoryFile*)file_data->Get("HI/Muons");
        TH1D* h_data = (TH1D*)dir_data->Get(histo_name.c_str());

        // Calculate normalization
        cout << "before norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " data: " << h_data->Integral(0, h_data->GetNbinsX()+1) << endl; 
        double norm_MC = h->Integral(0, h->GetNbinsX()+1);
        double norm_data = h_data->Integral(0, h_data->GetNbinsX()+1);
        h->Scale(1./norm_MC);
        h_data->Scale(1./norm_data);
        cout << "after norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " data: " << h_data->Integral(0, h_data->GetNbinsX()+1) << endl;

        h->SetFillColor(my_color_six(3)); // Simple color assignment
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
        h_ratio->GetLowerRefGraph()->SetMinimum(-1.2);
        h_ratio->GetLowerRefGraph()->SetMaximum(5.2);
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
        latex2->DrawLatexNDC(0.52,0.92,TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));

        // Set titles and labels and lines
        h_ratio->GetLowerRefYaxis()->SetTitle("Data/MC");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
        h_ratio->SetGridlines(onlyOneGridline);

        c->Update();

        // Print the canvas
        c->Print((histo_name + "_after_cut.pdf").c_str());
}

