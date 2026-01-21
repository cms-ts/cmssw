#include <iostream>
#include <string>
#include "TFile.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TString.h"
#include "TPad.h"
#include "TStyle.h"

void plot_veto_check(const char* filename = "output_HI_mu_data.root") {

    TString fName = filename;
    TString sample;

    if (fName.Contains("data")) {
        sample = "data";
    } else {
        sample = "MC";
    }

    // 0. Define Lumi
    double Lumi = 1.64; // nb-1

    // 1. Open the file
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    // 2. Retrieve the histograms
    TH2F* h_before = (TH2F*)f->Get("HI/Muons/h_jet_etaphi_before");
    TH2F* h_after  = (TH2F*)f->Get("HI/Muons/h_jet_etaphi_after");

    if (!h_before || !h_after) {
        std::cerr << "Error: Could not find histograms in HI/Muons/." << std::endl;
        return;
    }

    // 3. Remove Titles
    h_before->SetTitle("");
    h_after->SetTitle("");

    // 4. Setup Latex
    TLatex* latex = new TLatex();
    latex->SetTextSize(0.055); // Set text size (adjust as needed)
    latex->SetTextColor(kBlack); // Set text color (optional)
    latex->SetTextFont(61);

    TLatex* latex1 = new TLatex();
    latex1->SetTextSize(0.04); // Set text size (adjust as needed)
    latex1->SetTextColor(kBlack); // Set text color (optional)
    latex1->SetTextFont(52);

    TLatex* latex2 = new TLatex();
    latex2->SetTextFont(42);
    latex2->SetTextSize(0.04);

    // 5. Create Canvas and plot
    TCanvas* c1 = new TCanvas("c1", "Jet Veto Check before", 600, 600);
    c1->cd();
    h_before->SetStats(0);
    h_before->Draw("COLZ");
    // LATEX LINES ***
    latex->DrawLatexNDC(0.1,0.92,"CMS");
    latex1->DrawLatexNDC(0.22,0.92,"Preliminary");
    latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
    c1->SaveAs(TString::Format("veto_check_before_%s.pdf", sample.Data()));

    TCanvas* c2 = new TCanvas("c2", "Jet Veto Check after", 600, 600);
    c2->cd();
    h_after->SetStats(0);
    h_after->Draw("COLZ");
    // LATEX LINE ***
    latex->DrawLatexNDC(0.1,0.92,"CMS");
    latex1->DrawLatexNDC(0.22,0.92,"Preliminary");
    latex2->DrawLatexNDC(0.54, 0.92, TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", Lumi));
    c2->SaveAs(TString::Format("veto_check_after_%s.pdf", sample.Data()));

}
