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
#include "../MC_samples.h" // Include the header file
#include "tdrstyle.C"
#include "mycolor.h"

//to store histogram parameters
struct histoPar {
    std::string  histo_name;
    std::string  x_title;
    std::string  y_title;
    int n_bin;
    double x_min;
    double x_max;
};

void h_stack() {
    // Create a vector of histoPar structs
    std::vector<histoPar> histo_par = {
    {"h_mumu", "m_{#mu#mu} [GeV]", "Events", 20, 60, 120},
    {"h_Z_pt", "p_{T}^{Z} [GeV]", "Events", 30, 0, 300},
    {"h_njet", "n_{jet}", "Events", 10, 0, 10},
    {"h_cen", "cen", "Events", 20, 0, 100},
    {"h_mumu_j", "m_{#mu#mu} [GeV]", "Events", 20, 60, 120},
    {"h_Z_pt_j", "p_{T}^{Z} [GeV]", "Events", 30, 0, 300},
    {"h_jet_pt_lj", "leading jet p_{T} [GeV]", "Events", 30, 0, 300},
    {"h_cen_j", "cen_j", "Events", 20, 0, 100},
    {"h_deltaPhi_Zj", "#Delta#phi_{Zj}", "Events" , 20, 0, TMath::Pi()},
    {"h_xZj", "x_{Zj}", "Events", 20, 0, 3},
    {"h_jetgirth", "girth", "Events", 10, 0, 0.2},
    {"h_jet_deltaR", "R_{g}", "Events", 10, 0, 0.2},
    {"h_mu_pt", "leading p_{t}^{#mu} [GeV]", "Events", 30, 0, 300},
    {"h_antimu_pt", "subleading p_{t}^{#mu} [GeV]", "Events", 30, 0, 300},
    {"h_mu_eta", "leading #eta^{#mu}", "Events", 20, -2.5, 2.5},
    {"h_antimu_eta", "subleading #eta^{#mu}", "Events", 20, -2.5, 2.5},
    {"h_mu_phi", "leading #phi^{#mu}", "Events", 20, -TMath::Pi(), TMath::Pi()},
    {"h_antimu_phi", "subleading #phi^{#mu}", "Events", 20, -TMath::Pi(), TMath::Pi()},
    {"h_jet_pt_onej", "jet p_{T} [GeV]", "Events", 30, 0, 300},
    {"h_deltaR_muj", "#Delta R_{#mu^{-}j}", "Events", 20, 0, 2.},
    {"h_deltaR_antimuj", "#Delta R_{#mu^{+}j}", "Events", 20, 0, 2.}
    };
    setTDRStyle();

    double Lumi = 1.64; // nb-1
    double number_A = 208; // Lead

    // Get MC all histogram
    TFile* file_MC_all = TFile::Open("./output_HI_mu_MC_all.root", "READ");
    TDirectoryFile* dir_Muons_MC_all = (TDirectoryFile*)file_MC_all->Get("HI/Muons");
    TH1D* h_norm = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights");
    TH1D* h_norm_cen = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights_cen");
    TH1D* h_nev = (TH1D*)dir_Muons_MC_all->Get("h_n_events");
    TH1D* h_cen_after = (TH1D*)dir_Muons_MC_all->Get("h_cen_after");

    double n_ev = h_nev->Integral(0, h_nev->GetNbinsX()+1);
    double sum_w = h_norm->Integral(0, h_norm->GetNbinsX()+1);
    double sum_ncoll = h_norm_cen->Integral(0, h_norm_cen->GetNbinsX()+1);
    double sum_w_and_ncoll = h_cen_after->Integral(0, h_cen_after->GetNbinsX()+1);
    std::cout << "n_ev = " << n_ev << " sum_w = " << sum_w << " sum_ncoll = " << sum_ncoll << std::endl;

    TCanvas *c[20];

    int ih = 0;
    // Loop over histograms
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
        TLegend* legend = new TLegend(0.7, 0.65, 0.88, 0.85);
        legend->SetBorderSize(0);
        // Loop over files
        for (const auto& file : files) {
            const std::string& file_name = file.out_filename;
            const std::string& label = file.label;
            TFile* file_ = TFile::Open(file_name.c_str(), "READ");
            TDirectoryFile* dir = (TDirectoryFile*)file_->Get("HI/Muons");
            TH1D* h = (TH1D*)dir->Get(histo_name.c_str());

            // Calculate normalization
            double Xsec = file.xsec;
            double Ngen = file.ngen;
            double norm_signal = number_A*number_A*Lumi*Xsec*(n_ev/sum_ncoll)/sum_w;
            double norm_others = number_A*number_A*Lumi*Xsec*n_ev/sum_ncoll/Ngen;
            //double norm_others = number_A * number_A * Lumi * Xsec / Ngen;
            if (label=="signal") h->Scale(norm_signal);
            else h->Scale(norm_others);
            std::cout << file_name << " " << std::fixed << std::setprecision(2) << h->Integral(0, h->GetNbinsX()+1) << std::endl;

            if (label=="signal") {
              h_DYMM->SetFillColor(my_color_six(3)); // Simple color assignment
              h_DYMM->SetLineColor(h_DYMM->GetFillColor());
              h_DYMM->Add(h);
            }
            else if (label=="TT") {
              h_TT->SetFillColor(my_color_six(1)); // Simple color assignment
              h_TT->SetLineColor(h_TT->GetFillColor());
              h_TT->Add(h);
            }
            else if (label=="WWto2L2Nu" || label=="WZto2L2Q" || label=="WZto3LNu" || label=="ZZto2L2Q" || label=="ZZto4L") {
              h_diboson->SetFillColor(my_color_six(5)); // Simple color assignment
              h_diboson->SetLineColor(h_diboson->GetFillColor());
              h_diboson->Add(h);
            }
            else {
              h_others->SetFillColor(my_color_six(2)); // Simple color assignment
              h_others->SetLineColor(h_others->GetFillColor());
              h_others->Add(h);
            }
        }
        hs->Add(h_others);
        hs->Add(h_TT);
        hs->Add(h_diboson);
        hs->Add(h_DYMM);

        h_MC_tot->Add(h_others);
        h_MC_tot->Add(h_TT);
        h_MC_tot->Add(h_diboson);
        h_MC_tot->Add(h_DYMM);
        h_MC_tot->SetFillColor(my_color_six(3)); // Simple color assignment
        h_MC_tot->SetLineColor(h_DYMM->GetFillColor());

        // Get data histogram
        TFile* file_data = TFile::Open("./output_HI_mu_data.root", "READ");
        TDirectoryFile* dir_data = (TDirectoryFile*)file_data->Get("HI/Muons");
        TH1D* h_data = (TH1D*)dir_data->Get(histo_name.c_str());

        std::cout << "data: " << std::fixed << std::setprecision(2) << h_data->Integral(0, h_data->GetNbinsX()+1) << std::endl
             << "DYMM: " << std::fixed << std::setprecision(2) << h_DYMM->Integral(0, h_DYMM->GetNbinsX()+1) << std::endl
             << "diboson: " << std::fixed << std::setprecision(2) << h_diboson->Integral(0, h_diboson->GetNbinsX()+1) << std::endl
             << "TT: " << std::fixed << std::setprecision(2) << h_TT->Integral(0, h_TT->GetNbinsX()+1) << std::endl
             << "others: " << std::fixed << std::setprecision(2) << h_others->Integral(0, h_others->GetNbinsX()+1) << std::endl;


        // Create canvas
        int H_ref = 800; 
        int W_ref = 800; 

        // references for T, B, L, R
        float T = 0.08*H_ref;
        float B = 0.12*H_ref; 
        float L = 0.12*W_ref;
        float R = 0.04*W_ref;
        TString c_name = Form("c_%d", ih);
        c[ih] = new TCanvas(c_name,histo_name.c_str(),W_ref,H_ref);
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

        TRatioPlot *h_ratio = new TRatioPlot(h_data, h_MC_tot, "pois");
        h_ratio->SetH1DrawOpt("EX0");
        h_ratio->SetH2DrawOpt("HIST");
        h_ratio->Draw();
        h_ratio->GetLowerRefGraph()->SetMarkerStyle(20);
        h_ratio->GetLowerRefGraph()->SetMinimum(0.3);
        h_ratio->GetLowerRefGraph()->SetMaximum(1.7);
        // Draw stack and data
        double y_max=0;
        y_max=h_data->GetBinContent(h_data->GetMaximumBin());
        if (histo_name == "h_deltaPhi_Zj" || histo_name == "h_jet_deltaR" || histo_name == "h_antimu_eta" ||
            histo_name == "h_antimu_phi" || histo_name == "h_deltaR_antimuj" || histo_name == "h_deltaR_muj" ||
            histo_name == "h_mu_eta" || histo_name == "h_mu_phi") h_data->SetMaximum(60*y_max);
        else  h_data->SetMaximum(5.*y_max);
        h_data->SetMinimum(0.03);
        h_data->GetXaxis()->SetTitle(x_title.c_str());
        h_data->GetYaxis()->SetTitle(y_title.c_str());
        h_data->SetMarkerStyle(20);
        h_data->SetMarkerSize(1.);
        h_data->SetLineColor(1);
        h_data->SetMarkerColor(1);

        // Legend
        legend->AddEntry(h_data, "Data", "PE");
        legend->AddEntry(h_DYMM, "Drell-Yan", "f");
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
        latex2->DrawLatexNDC(0.52,0.92,TString::Format("PbPb %.3f nb^{-1} (5.36 TeV)", Lumi));


        // Set titles and labels and lines
        h_ratio->GetLowerRefYaxis()->SetTitle("Data/MC");
        h_ratio->GetUpperRefYaxis()->SetTitle(y_title.c_str());
        std::vector<double> onlyOneGridline = {1.0};  // Keep only the line at 1.0
        h_ratio->SetGridlines(onlyOneGridline);

        c[ih]->Update();

        // Print the canvas
        c[ih]->Print((histo_name + "_stack.pdf").c_str());
    }
}

