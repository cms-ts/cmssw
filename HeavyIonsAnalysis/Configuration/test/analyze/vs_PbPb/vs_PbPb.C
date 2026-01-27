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
#include "tdrstyle.C"

void vs_PbPb(bool subtracted = false) {

        std::string string_subtracted = "";
        if (subtracted) string_subtracted = "_subtracted";
        //histogram parameters
//        std::string histo_name = "h_deltaPhi_Zj";
        std::string histo_name = "h_xZj";
        std::string histo_norm_name = "h_jet_pt_lj";
        std::string x_title = "";
        if (histo_name == "h_deltaPhi_Zj") x_title = "#Delta#phi_{Zj}";
        if (histo_name == "h_xZj") x_title = "x_{Zj}";
        std::string y_title = "";
        int n_bin = 20;
        double x_min = 0.;
        double x_max = TMath::Pi();

        setTDRStyle();

        double Lumi_PbPb = 1.64; // nb-1
        double Lumi_pp = 481; // pb-1
        double number_A = 208; // Lead

        // Create legend
        TLegend* legend = new TLegend(0.46, 0.7, 0.68, 0.8);
        legend->SetBorderSize(0);

        // Open PbPb file
        TFile* file_ = TFile::Open("./output_HI_mu_data.root", "READ");
        TDirectoryFile* dir = (TDirectoryFile*)file_->Get("HI/Muons");
        TH1D* h = (TH1D*)dir->Get((histo_name+string_subtracted).c_str());
        TH1D* h_norm = (TH1D*)dir->Get((histo_norm_name+string_subtracted).c_str());
        // Get ppref histograms
        TFile* file_pp = TFile::Open("../output_ppref_mu_data.root", "READ");
        TDirectoryFile* dir_pp = (TDirectoryFile*)file_pp->Get("pp/Muons");
        TH1D* h_pp = (TH1D*)dir_pp->Get(histo_name.c_str());
        TH1D* h_pp_norm = (TH1D*)dir_pp->Get(histo_norm_name.c_str());

        Double_t intgr_error;
        Double_t intgr_value = h_norm->IntegralAndError(0,h_norm->GetNbinsX()+1, intgr_error);
        cout << "Number of events in PbPb: " << intgr_value << " +- " << intgr_error << endl;
        if (histo_name == "h_deltaPhi_Zj") {
          Double_t intgr_error_pi3;
          Double_t intgr_value_pi3 = h->IntegralAndError(0,h->FindBin(TMath::Pi()/3), intgr_error_pi3, "");
          cout << "dphi<pi/3: " << intgr_value_pi3 << " +- " << intgr_error_pi3
               << ", fract: " << intgr_value_pi3/intgr_value << endl;
        }
        Double_t intgr_error_pp;
        Double_t intgr_value_pp = h_pp_norm->IntegralAndError(0,h_pp_norm->GetNbinsX()+1, intgr_error_pp);
        cout << "Number of events in pp: " << intgr_value_pp << " +- " << intgr_error_pp << endl;
        if (histo_name == "h_deltaPhi_Zj") {
          Double_t intgr_error_pi3_pp;
          Double_t intgr_value_pi3_pp = h_pp->IntegralAndError(0,h_pp->FindBin(TMath::Pi()/3), intgr_error_pi3_pp, "");
          cout << "dphi<pi/3: " << intgr_value_pi3_pp << " +- " << intgr_error_pi3_pp
               << ", fract: " << intgr_value_pi3_pp/intgr_value_pp << endl;
        }
        // Calculate normalization
        cout << "--Normalization--" << endl;
        cout << "before norm: " << h->Integral(0, h->GetNbinsX()+1) << " ppref: " << h_pp->Integral(0, h_pp->GetNbinsX()+1) << endl; 
        double norm_ = h_norm->Integral(0, h_norm->GetNbinsX()+1);
        double norm_pp = h_pp_norm->Integral(0, h_pp_norm->GetNbinsX()+1);
        h->Scale(1./norm_);
        h_pp->Scale(1./norm_pp);
        cout << "after norm: " << h->Integral(0, h->GetNbinsX()+1) << " ppref: " << h_pp->Integral(0, h_pp->GetNbinsX()+1) << endl;
        Double_t intgr_error_pi3_afternorm;
        Double_t intgr_value_pi3_afternorm = h->IntegralAndError(0, h->FindBin(TMath::Pi()/3), intgr_error_pi3_afternorm, "");
        Double_t intgr_error_pi3_pp_afternorm;
        Double_t intgr_value_pi3_pp_afternorm = h_pp->IntegralAndError(0, h_pp->FindBin(TMath::Pi()/3), intgr_error_pi3_pp_afternorm, "");
        if (histo_name == "h_deltaPhi_Zj") {
          cout << "dphi<pi/3 after norm: " << intgr_value_pi3_afternorm << " +-" << intgr_error_pi3_afternorm
               << " ppref: " << intgr_value_pi3_pp_afternorm << " +-" << intgr_error_pi3_pp_afternorm << endl;
        }
        h->SetMarkerStyle(20);
        h->SetMarkerSize(1.);
        h->SetLineColor(1);
        h->SetMarkerColor(1);

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
//        c->SetLogy();

        // gstyle to remove horizontal error bars, put in rootlogon
        gStyle->SetErrorX(0);

        TRatioPlot *h_ratio = new TRatioPlot(h, h_pp, "pois");
        h_ratio->SetH1DrawOpt("EX0");
        h_ratio->SetH2DrawOpt("EX0");
        h_ratio->Draw();
        h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
        h_ratio->GetLowerRefGraph()->SetMinimum(-0.3);
        h_ratio->GetLowerRefGraph()->SetMaximum(3.3);
        // Draw PbPb and pp
        double y_max=0;
        if (histo_name == "h_deltaPhi_Zj") h->SetMaximum(0.7);
        if (histo_name == "h_xZj") h->SetMaximum(0.4);
        h->SetMinimum(0.);
        h->GetXaxis()->SetTitle(x_title.c_str());
        h->GetYaxis()->SetTitle(y_title.c_str());
        h_pp->SetMarkerStyle(20);
        h_pp->SetMarkerSize(1.);
        h_pp->SetLineColor(2);
        h_pp->SetMarkerColor(2);

        // Legend
        if (!subtracted) legend->AddEntry(h, "PbPb Raw (0-30%)", "PE");
        if (subtracted) legend->AddEntry(h, "PbPb Raw-Bkg (0-30%)", "PE");
        legend->AddEntry(h_pp, "ppRef", "PE");

        TPad *pad = h_ratio->GetUpperPad();
        pad->cd();
        h->Draw("EsameX0");
        h_pp->Draw("EsameX0");
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

        latex2->DrawLatexNDC(0.375,0.92,TString::Format("PbPb %.2f nb^{-1}, pp %.0f pb^{-1} (5.36 TeV)", Lumi_PbPb, Lumi_pp));

        // Set titles and labels and lines
        h_ratio->GetLowerRefYaxis()->SetTitle("PbPb/ppRef");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
        h_ratio->SetGridlines(onlyOneGridline);

        c->Update();

        // Print the canvas
        c->Print(("compare_" + histo_name + string_subtracted + ".pdf").c_str());
}

