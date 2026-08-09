#include "histograms.h"
#include "plots.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include <iostream>
#include <map>

using namespace std;

void MCRESP(string inFileName = "AK2_ZJet_forjer_FINAL.root", int minpt = 15, string dirname = "MCJER_AK2") {

  map<int, TH1D*> mus;

  // Initialize the 1D histograms to hold the means
  for (int ebin = 1; ebin <= histograms::netaforjer; ++ebin) {
      mus[ebin] = new TH1D(Form("closure_%d", ebin), "", histograms::nptforJER, &histograms::ptforJER[0]);
  }

  TFile *inFile = new TFile(inFileName.c_str(), "READ");
  if (!inFile || inFile->IsZombie()) {
      cout << "Error opening file: " << inFileName << endl;
      return;
  }

  TH3D* responseprofile = (TH3D*)inFile->Get("hibin_-1.0_0.0/eta_-5.2_5.2/responses3D");

  for (int ptbin = 1; ptbin <= responseprofile->GetXaxis()->GetNbins(); ++ptbin) {
      for (int etabin = 1; etabin <= responseprofile->GetYaxis()->GetNbins(); ++etabin) {

        responseprofile->GetXaxis()->SetRange(ptbin, ptbin);    // pt axis
        responseprofile->GetYaxis()->SetRange(etabin, etabin);  // eta axis

        TH1D* resp = (TH1D*)responseprofile->Project3D("z");
        
        // Skip empty or nearly empty bins
        if (!resp || resp->GetEntries() < 50) continue;

        // -------------------------------------------------------------
        // TRUNCATED MEAN CALCULATION (Ignores the asymmetric tail)
        // -------------------------------------------------------------
        double mean = resp->GetMean();
        double rms  = resp->GetRMS();
        
        // Iterate 3 times to lock onto the core peak
        for (int i = 0; i < 3; i++) {
            resp->GetXaxis()->SetRangeUser(mean - 1.5 * rms, mean + 1.5 * rms);
            mean = resp->GetMean();
            rms  = resp->GetRMS();
        }

        mus[etabin]->SetBinContent(ptbin, mean);
        mus[etabin]->SetBinError(ptbin, resp->GetMeanError());
        
        // Reset the axis so we don't break the original histogram
        resp->GetXaxis()->SetRangeUser(0, 3);
      }
  }

  // -------------------------------------------------------------
  // DRAWING THE PLOTS (ALL, BARREL, FORWARD)
  // -------------------------------------------------------------
  
  // Set up the three canvases
  TCanvas *c1 = new TCanvas("c1", "All Eta", 1000, 700); c1->SetLogx();
  TCanvas *c1b = new TCanvas("c1b", "Barrel", 1000, 700); c1b->SetLogx();
  TCanvas *c1f = new TCanvas("c1f", "Forward", 1000, 700); c1f->SetLogx();
  
  // Set up the three legends
  auto leg = new TLegend(0.60, 0.65, 0.90, 0.90); leg->SetNColumns(2);
  auto legb = new TLegend(0.60, 0.65, 0.90, 0.90);
  auto legf = new TLegend(0.60, 0.65, 0.90, 0.90);

  // Create a reference line exactly at 1.0 (Perfect Closure)
  TLine *lineOne = new TLine(minpt, 1.0, 1000, 1.0);
  lineOne->SetLineColor(kBlack);
  lineOne->SetLineStyle(2); // Dashed line
  lineOne->SetLineWidth(2);

  bool firstDraw = true;
  bool firstDrawB = true;
  bool firstDrawF = true;

  for (int ebin = 1; ebin <= histograms::netaforjer; ++ebin) {
      // Check if this histogram actually has points
      if (mus[ebin]->GetEntries() == 0) continue;

      mus[ebin]->SetStats(0);
      mus[ebin]->SetLineColor(cols[ebin-1]);
      mus[ebin]->SetMarkerColor(cols[ebin-1]);
      mus[ebin]->SetMarkerStyle(20);
      
      mus[ebin]->GetXaxis()->SetRangeUser(minpt, 1000);
      
      // Zoom in Y-axis around 1.0 to actually see the precision!
      mus[ebin]->SetMinimum(0.85); 
      mus[ebin]->SetMaximum(1.15); 
      
      mus[ebin]->GetXaxis()->SetTitle("p_{T,gen} [GeV]");
      mus[ebin]->GetYaxis()->SetTitle("JES Closure (p_{T,reco}/p_{T,gen})");
      
      // 1. Draw on the "ALL" canvas
      c1->cd();
      if (firstDraw) { mus[ebin]->Draw("E1"); firstDraw = false; } 
      else { mus[ebin]->Draw("E1 same"); }
      leg->AddEntry(mus[ebin], Form("%.3f < |#eta| < %.3f", histograms::etaforjer[ebin-1], histograms::etaforjer[ebin]), "lp");
      
      // 2. Split into Barrel (ebin <= 8) or Forward (ebin > 8)
      if (ebin <= 8) {
          c1b->cd();
          if (firstDrawB) { mus[ebin]->Draw("E1"); firstDrawB = false; } 
          else { mus[ebin]->Draw("E1 same"); }
          legb->AddEntry(mus[ebin], Form("%.3f < |#eta| < %.3f", histograms::etaforjer[ebin-1], histograms::etaforjer[ebin]), "lp");
      } else {
          c1f->cd();
          if (firstDrawF) { mus[ebin]->Draw("E1"); firstDrawF = false; } 
          else { mus[ebin]->Draw("E1 same"); }
          legf->AddEntry(mus[ebin], Form("%.3f < |#eta| < %.3f", histograms::etaforjer[ebin-1], histograms::etaforjer[ebin]), "lp");
      }
  }
  
  // Finish "All" Canvas
  c1->cd();
  lineOne->Draw("same");
  leg->Draw();
  c1->Print(Form("%s/allmctruthclosures.png", dirname.c_str()));

  // Finish "Barrel" Canvas
  c1b->cd();
  lineOne->Draw("same");
  legb->Draw();
  c1b->Print(Form("%s/allmctruthclosures_barrel.png", dirname.c_str()));

  // Finish "Forward" Canvas
  c1f->cd();
  lineOne->Draw("same");
  legf->Draw();
  c1f->Print(Form("%s/allmctruthclosures_fwd.png", dirname.c_str()));
}
