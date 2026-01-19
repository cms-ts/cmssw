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

void JEWEL_weight_3(int after_flag  = 0) {

        //histogram parameters
        std::string histo_name = "h_xZj_for_JEWEL_w";
//        std::string histo_name = "h_xZj_true";
        std::string x_title = "x_{Zj}";
        std::string y_title = "";
        int n_bin = 30;
        double x_min = 0.;
        double x_max = 3.;

        setTDRStyle();

        double Lumi = 1.64; // nb-1
        double number_A = 208; // Lead

        // Create legend
        double xmin_leg = (after_flag == 0) ? 0.6 : 0.5;
        TLegend* legend = new TLegend(xmin_leg, 0.7, 0.8, 0.8);
        legend->SetBorderSize(0);

        // Open MC file
        std::string MC_file_name = "../../plot/output_HI_mu_MC_signal.root";
        if (after_flag == 1) MC_file_name = "../../syst_prior_model/output_HI_mu_MC_prior_model.root";
        TFile* file_ = TFile::Open(MC_file_name.c_str(), "READ");
        TDirectoryFile* dir = (TDirectoryFile*)file_->Get("HI/Muons");
        TH1D* h = (TH1D*)dir->Get(histo_name.c_str());
        // Get JEWEL histogram
        TFile* file_JEWEL = TFile::Open("/gfsvol01/cms/users/rdelliga/work/Hi_forest/JEWEL/CMSSW_13_2_13/src/jewel-2.4.0/jewel_converted_Zj_QGP.pu14test.root", "READ");
        file_JEWEL->cd();
        TH1D* h_JEWEL = (TH1D*)gDirectory->Get(histo_name.c_str());

        // Calculate normalization
        cout << "before norm MC: " << h->Integral(0, h->GetNbinsX()+1) << " JEWEL: " << h_JEWEL->Integral(0, h_JEWEL->GetNbinsX()+1) << endl; 
        double norm_MC = h->Integral(0, h->GetNbinsX()+1);
        double norm_JEWEL = h_JEWEL->Integral(0, h_JEWEL->GetNbinsX()+1);
        h->Scale(1./norm_MC);
        h_JEWEL->Scale(1./norm_JEWEL);
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

        // gstyle to remove horizontal error bars, put in rootlogon
        gStyle->SetErrorX(0);

        TRatioPlot *h_ratio = new TRatioPlot(h_JEWEL, h, "pois");
        h_ratio->SetH1DrawOpt("E");
        h_ratio->SetH2DrawOpt("E");
        h_ratio->Draw();
        h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
        h_ratio->GetLowerRefGraph()->SetMinimum(0.4);
        h_ratio->GetLowerRefGraph()->SetMaximum(1.6);
        // Draw MC and JEWEL
        double y_max=0;
        //y_max=h_JEWEL->GetBinContent(h_JEWEL->GetMaximumBin());
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
        legend->AddEntry(h_JEWEL, "JEWEL (truth) med", "epl");
        if (after_flag == 0) legend->AddEntry(h, "POWHEG+PYTHIA", "epl");
        else legend->AddEntry(h, "POWHEG+PYTHIA (JEWEL rew)", "epl");

        TPad *pad = h_ratio->GetUpperPad();
        pad->cd();
        h_JEWEL->Draw("Esame");
        h->Draw("Esame");
        legend->SetTextFont(42);
        legend->SetTextColor(kBlack);
        legend->SetTextSize(0.03);
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

        //latex->DrawLatexNDC(0.24,0.86,TString::Format("#int JEWEL = %.0f", h_JEWEL->Integral(0, h_JEWEL->GetNbinsX()+1)));
        latex2->DrawLatexNDC(0.52,0.92,TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));

        // Set titles and labels and lines
        h_ratio->GetLowerRefYaxis()->SetTitle("JEW/POW");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
        h_ratio->SetGridlines(onlyOneGridline);

        c->Update();

        // Create ratio histogram
        TH1D* h_weight_JEWEL = (TH1D*)h_JEWEL->Clone("h_weight_JEWEL");
        h_weight_JEWEL->Divide(h);

        if (after_flag == 0) {
          TFile* file_weight_JEWEL = new TFile("weight_JEWEL.root", "RECREATE");
          h_weight_JEWEL->Write("h_weight_JEWEL");
          file_weight_JEWEL->Close();
        }

        // Print the canvas
        std::string is_bef_or_aft = "_before.pdf";
        if (after_flag == 1) is_bef_or_aft = "_after.pdf";
        c->Print((histo_name + is_bef_or_aft).c_str());
}

