// MC JET PHI/ETA RESOLUTION
// ADJUST ETA, PHI BINS?

//#include "../fillhistograms/histograms.h"
#include "TMath.h"


void MCJPR(TString inFileName = "../../../results_RERECO/RERECOMC_AK4_PFTRIG_nojetid.root", bool doeta = 0,  string dirname = "MCJPR", TString outFileName = "testingMCpfires-rereco.root") {

  string varlabel = (doeta ? "eta" : "phi");

  int pts[] = {40, 55, 80, 120, 170, 1000};
  float etabins[] = {0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};

  float ptsforwidths[] = {80, 120, 170, 1000};
  TH1D* widths = new TH1D("widths","widths",3,&ptsforwidths[0]); 

  map<int, TH1D*> ws;

  for (int ebin = 1; ebin <= histograms::netaforjer; ++ebin) {
     ws[ebin] =  new TH1D(Form("widths_%d",ebin),"",histograms::nptforJER,&histograms::ptforJER[0]); 
    }

  TFile *inFile = new TFile(inFileName, "READ");

  TFile *outfile = new TFile(outFileName,"RECREATE");

  TH3D* responseprofile = (TH3D*)inFile->Get((doeta ? "hibin_-1.0_0.0/eta_-5.2_5.2/etaresponses3D" : "hibin_-1.0_0.0/eta_-5.2_5.2/phiresponses3D"));

  //responseprofile->Draw();
  //cout << responseprofile->GetXaxis()->GetNbins() << endl;
  //cout << responseprofile->GetYaxis()->GetNbins() << endl;
  TCanvas *c1 = new TCanvas("c1","c1",800,600);

  for (int ptbin = 1; ptbin <= responseprofile->GetXaxis()->GetNbins(); ++ptbin) {
      for (int etabin = 1; etabin <= responseprofile->GetYaxis()->GetNbins(); ++etabin) {
       
	// Manually limit bins for 2023
	if (ptbin == 8 and etabin > 11) continue;
	if (ptbin == 9 and etabin > 10) continue;
	if (ptbin == 10 and etabin > 9) continue;
	if (ptbin == 11 and etabin > 8) continue;
	if (ptbin == 12 and etabin > 8) continue;
	if (ptbin == 13 and etabin > 8) continue;
	if (ptbin == 14 and etabin > 7) continue;
	if (ptbin == 15 and etabin > 6) continue;
	if (ptbin == 16 and etabin > 5) continue;
	if (ptbin == 17 and etabin > 4) continue;

	
	cout << "etabin " << etabin << endl;
	responseprofile->GetXaxis()->SetRange(ptbin,ptbin);    // pt axis - 2to5
	responseprofile->GetYaxis()->SetRange(etabin,etabin);    // eta axis

	TH1D* resp = (TH1D*)responseprofile->Project3D("z");
	if (resp->GetEntries() < 500) continue;

	resp->Scale(1./resp->Integral(),"width");
	resp->SetTitle("");

	resp->GetXaxis()->SetTitle((doeta ? "#eta_{reco}-#eta_{gen}" : "#phi_{reco}-#phi_{gen}"));

	// JER should come from a gaussian fit here
	resp->Draw();

	// Fit twice
	/*	TF1 *f1 = new TF1("f1", "gaus");
	f1->SetParameter(0,1.5);
	resp->Fit("f1");


	double c = f1->GetParameter(0);
	double m = f1->GetParameter(1);
	double s = f1->GetParameter(2); */

	//TF1 *f2 = new TF1("f2", "gaus", m-2*s, m+2*s );
	//f2->SetParameters(c,m,s);
	TF1 *f2 = new TF1("f2", "gaus");

       
	//	resp->Fit("f2","R");
	resp->Fit("f2");

	c1->SetLogy();
	auto txt = new TLatex();
	txt->SetNDC();
	txt->SetTextSize(0.03);
	txt->DrawLatex( 0.2, 0.35, Form("%.1f < |#eta| < %.1f",histograms::etaforjer[etabin-1],histograms::etaforjer[etabin]));
	txt->DrawLatex( 0.2, 0.4, Form("%g < p_{T,gen} < %g",histograms::ptforJER[ptbin-1],histograms::ptforJER[ptbin]));
	txt->DrawLatex( 0.2, 0.45, Form("#sigma = %.5f #pm %.5f",f2->GetParameter(2), f2->GetParError(2)));

	ws[etabin]->SetBinContent(ptbin,f2->GetParameter(2)); // Was ptbin-2?
	ws[etabin]->SetBinError(ptbin,f2->GetParError(2));
	//	ws[etabin]->SetMaximum(0.2);       

	c1->Print(Form("%s/%sresp_ptbin_%d_etabin_%d.png",dirname.c_str(),varlabel.c_str(),ptbin,etabin));
      }
  }

  //  

  auto leg = new TLegend(0.57,0.6,0.85,0.9);
  TCanvas *c2 = new TCanvas("c2","c2",800,600);
  c2->SetLogx();
  gStyle->SetOptStat(0);

  cout << "Fitting curve" << endl;
  for (int ebin = 1; ebin <= histograms::netaforjer; ++ebin) {
    //for (int ebin = 1; ebin <= 5; ++ebin) {
    ws[ebin]->SetLineColor(cols[ebin-1]);
    //    ws[ebin]->GetXaxis()->SetRangeUser(15,1500);
    ws[ebin]->GetXaxis()->SetRangeUser(28,1500);
    ws[ebin]->SetMaximum(0.25);

    ws[ebin]->GetXaxis()->SetTitle("#p_{T,gen}");

    ws[ebin]->GetYaxis()->SetTitle("#sigma");
    ws[ebin]->Draw("same");
    leg->AddEntry(ws[ebin],Form("%.3f < |#eta| < %.3f",histograms::etaforjer[ebin-1],histograms::etaforjer[ebin]));

    ws[ebin]->Write();

    // (x)=sqrt(pow(p0,2)+pow(p1,2)/x+pow((p2/x),2)+pow((p3/x),3)) was used by uttam
    
    
    // Fit NSC: sqrt([0]*abs([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])
    /*TF1 *nsc = new TF1("nsc", "[0]+[1]*exp(-x/[2])+[3]*exp(-x/[4])"); // JERC files UL18
    nsc->SetParameter(0,0.025);
    nsc->SetParameter(1,0.02);
    nsc->SetParameter(2,30);
    nsc->SetParameter(3,0.02);
    nsc->SetParameter(4,10); */
   
    // TF1 *nsc = new TF1("nsc", "sqrt([0]*([0])/(x*x)+[1]*[1]*pow(x,[3])+[2]*[2])"); // JER
    /*  nsc->SetParameter(0,0.5);
    nsc->SetParameter(1,-0.2);
    */   

     
    TF1 *nsc = new TF1("nsc", "sqrt(pow([0],2) + pow([1],2)/x + pow([2]/x,2) + pow([3]/x,3))");
    nsc->SetParameter(0,0.02);
    nsc->SetParameter(1,-0.1);
    nsc->SetParameter(2,-1);
    nsc->SetParameter(3,0.02);

    nsc->SetLineColor(cols[ebin-1]);
    ws[ebin]->Fit("nsc");

    nsc->Draw("same");
    nsc->Write(Form("fit_eta_%.3fto%.3f",histograms::etaforjer[ebin-1],histograms::etaforjer[ebin]));
    
  }
  leg->Draw();

  c2->Print(Form("%s/allmc%sres.png",dirname.c_str(),varlabel.c_str()));

  outfile->Close();

}
