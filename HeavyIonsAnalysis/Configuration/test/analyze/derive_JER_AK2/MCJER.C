#include "histograms.h"
#include "TMath.h"
//#include "plots.h"
#include "TLine.h" // Add this at the top for drawing lines

// Helper function to calculate a symmetric trimmed RMS and return the window bounds
double GetTrimmedRMS(TH1D* h, double trim_fraction, double &x_low, double &x_high) {
    if (!h || h->GetEntries() == 0) return 0.0;

    double prob[2] = {trim_fraction / 2.0, 1.0 - (trim_fraction / 2.0)};
    double q[2]    = {0.0, 0.0};

    h->GetQuantiles(2, q, prob);
    x_low  = q[0];
    x_high = q[1];

    h->GetXaxis()->SetRangeUser(x_low, x_high);
    double trimmed_rms = h->GetRMS();
    h->GetXaxis()->SetRangeUser(0, 3.0); // Reset for plotting

    return trimmed_rms;
}

void MCJER(TString inFileName = "AK2_ZJet_forjer.root", int minpt = 15, string dirname = "MCJER", TString outFileName = "MCJER-AK2.root", bool useTrimmedRMS = true) {
     
  TFile *inFile = new TFile(inFileName, "READ");
  TFile *outfile = new TFile(outFileName,"RECREATE");
  TH3D* responseprofile = (TH3D*)inFile->Get("hibin_-1.0_0.0/eta_-5.2_5.2/responses3D");

  // DYNAMIC BINNING: Inherit the exact pT bins from the 3D histogram we just loaded
  map<int, TH1D*> ws;
  const double* pt_edges = responseprofile->GetXaxis()->GetXbins()->GetArray();
  int n_pt_bins = responseprofile->GetXaxis()->GetNbins();
  for (int ebin = 1; ebin <= histograms::netaforjer; ++ebin) {
     ws[ebin] = new TH1D(Form("widths_%d",ebin), "", n_pt_bins, pt_edges);
  }

  TCanvas *c1 = new TCanvas("c1","c1",800,600);

  for (int ptbin = 1; ptbin <= responseprofile->GetXaxis()->GetNbins(); ++ptbin) {
      for (int etabin = 1; etabin <= responseprofile->GetYaxis()->GetNbins(); ++etabin) {
        
        // ---> DIJET HARDCODED LIMITS REMOVED HERE <---

        responseprofile->GetXaxis()->SetRange(ptbin,ptbin);    // pt axis
        responseprofile->GetYaxis()->SetRange(etabin,etabin);  // eta axis

        TH1D* resp = (TH1D*)responseprofile->Project3D("z");
        
        // Safety check: Skip bins with virtually no statistics to prevent MINUIT crashes
        if (resp->GetEntries() < 50) continue;

        resp->Scale(1./resp->Integral(),"width");
        resp->SetTitle("");
        resp->GetXaxis()->SetTitle("p_{T,reco}/p_{T,gen}");

        resp->Draw();

        double final_sigma = 0;
        double final_error = 0;

        if (!useTrimmedRMS) {
            // === METHOD A: DOUBLE GAUSSIAN FIT ===
            TF1 *f1 = new TF1("f1", "gaus");
            f1->SetParameter(0, 1.5);
            resp->Fit("f1", "Q");

            double c = f1->GetParameter(0);
            double m = f1->GetParameter(1);
            double s = f1->GetParameter(2);

            TF1 *f2 = new TF1("f2", "gaus", m - 2*s, m + 2*s);
            f2->SetParameters(c, m, s);
            resp->Fit("f2", "RQ"); // 'R' uses the specified range

            final_sigma = f2->GetParameter(2);
            final_error = f2->GetParError(2);
        } 
        else {
            // === METHOD B: 10% TRUNCATED RMS ===
            double x_low = 0, x_high = 0;
            final_sigma = GetTrimmedRMS(resp, 0.10, x_low, x_high);
            final_error = resp->GetRMSError(); // Approximation for error

            // Visualize the truncation window with two vertical dashed lines
            double max_y = resp->GetMaximum();
            TLine *line_low = new TLine(x_low, 0, x_low, max_y);
            TLine *line_high = new TLine(x_high, 0, x_high, max_y);
            line_low->SetLineColor(kRed); line_low->SetLineStyle(2); line_low->SetLineWidth(2);
            line_high->SetLineColor(kRed); line_high->SetLineStyle(2); line_high->SetLineWidth(2);
            line_low->Draw("same");
            line_high->Draw("same");
        }

        c1->SetLogy();
        auto txt = new TLatex();
        txt->SetNDC();
        txt->SetTextSize(0.03);
        txt->DrawLatex(0.2, 0.35, Form("%.1f < |#eta| < %.1f", histograms::etaforjer[etabin-1], histograms::etaforjer[etabin]));
        txt->DrawLatex(0.2, 0.4, Form("%g < p_{T,gen} < %g", histograms::ptforJER[ptbin-1], histograms::ptforJER[ptbin]));
        
        // Indicate which method was used on the PNG!
        if (useTrimmedRMS) txt->DrawLatex(0.2, 0.45, Form("10%% Trunc RMS = %.5f", final_sigma));
        else txt->DrawLatex(0.2, 0.45, Form("Gaus #sigma = %.5f", final_sigma));

        ws[etabin]->SetBinContent(ptbin, final_sigma);
        ws[etabin]->SetBinError(ptbin, final_error);
	//	ws[etabin]->SetMaximum(0.2);       

	c1->Print(Form("%s/resp_ptbin_%d_etabin_%d.png",dirname.c_str(),ptbin,etabin));
      }
  }

  //  

  auto leg = new TLegend(0.57,0.6,0.85,0.9);
  auto legb = new TLegend(0.57,0.6,0.85,0.9); //barrel(ish) bins
  auto legf = new TLegend(0.57,0.6,0.85,0.9); //forward(ish) bins

  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  c2->SetLogx();
  gStyle->SetOptStat(0);

  TCanvas *c2b = new TCanvas("c2b","c2b",800,600);
  c2b->SetLogx();
  gStyle->SetOptStat(0);

  TCanvas *c2f = new TCanvas("c2f","c2f",800,600);
  c2f->SetLogx();
  gStyle->SetOptStat(0);
  
  for (int ebin = 1; ebin <= histograms::netaforjer; ++ebin) {
    //for (int ebin = 1; ebin <= 5; ++ebin) {

    // SAFETY CHECK: SKIP EMPTY BINS SO THEY AREN'T SAVED <<<
    bool has_data = false;
    for (int b = 1; b <= ws[ebin]->GetNbinsX(); ++b) {
        if (ws[ebin]->GetBinContent(b) > 0) has_data = true;
    }
    if (!has_data) continue; // Instantly skips to the next bin!

    c2->cd();
    
    ws[ebin]->SetLineColor(cols[ebin-1]);
    ws[ebin]->GetXaxis()->SetRangeUser(15,1500);
    ws[ebin]->SetMaximum(0.5);
    ws[ebin]->GetXaxis()->SetTitle("p_{T,ptcl}");
    ws[ebin]->GetYaxis()->SetTitle("#sigma");
    
    ws[ebin]->Draw("same");

    leg->AddEntry(ws[ebin],Form("%.3f < |#eta| < %.3f",histograms::etaforjer[ebin-1],histograms::etaforjer[ebin]));

    ws[ebin]->GetXaxis()->SetRangeUser(minpt,1000);
    ws[ebin]->Write();

    // Fit NSC: sqrt([0]*abs([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])
    //TF1 *nsc = new TF1("nsc", "sqrt([0]*abs([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])");

   TF1 *nsc = new TF1("nsc", "sqrt([0]*([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])");

    // --- Added
    // Give it a gentle nudge in the right direction
    nsc->SetParameter(0, 0.02);
    nsc->SetParameter(1, 0.6);
    nsc->SetParameter(2, 0.05);
    nsc->SetParameter(3, -1.0);
    
    // Leave 0, 1, 2 unbounded like the AK4 file, but strictly clamp 3 so it decays
    nsc->SetParLimits(3, -3.0, -0.01);
    // --- end added

    nsc->SetLineColor(cols[ebin-1]);
    if (ebin ==  histograms::netaforjer) ws[ebin]->GetXaxis()->SetRangeUser(minpt,100);
    // nsc->SetLineColor(kBlue-8+ebin);
    ws[ebin]->Fit("nsc");

    nsc->Write(Form("fit_eta_%.3fto%.3f",histograms::etaforjer[ebin-1],histograms::etaforjer[ebin]));
    
    nsc->Draw("same");
    if (ebin <= 8) {
      c2b->cd();
      ws[ebin]->Draw("same");
      nsc->Draw("same");
      legb->AddEntry(ws[ebin],Form("%.3f < |#eta| < %.3f",histograms::etaforjer[ebin-1],histograms::etaforjer[ebin]));
    }

    else {
      c2f->cd();
       ws[ebin]->Draw("same");
       nsc->Draw("same");
       legf->AddEntry(ws[ebin],Form("%.3f < |#eta| < %.3f",histograms::etaforjer[ebin-1],histograms::etaforjer[ebin]));
    }
    
    
  } 
  leg->Draw();
  c2b->cd();
  legb->Draw();
  
  c2f->cd();
  legf->Draw();

  c2->Print(Form("%s/allmcjers.png",dirname.c_str()));
  c2b->Print(Form("%s/allmcjers_barrel.png",dirname.c_str()));
  c2f->Print(Form("%s/allmcjers_fwd.png",dirname.c_str()));
  
  outfile->Close();
}
