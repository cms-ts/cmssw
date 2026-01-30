#include "../plot/tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"
#include "../helpers.h"           // for getLumiFromSummary, cen tables, etc.

TCanvas* example_plot( int iPeriod, int iPos, const char* collision_type, bool isData, const char * histo_name, const char * x_title, const char * y_title, int bin, double min, double max);

// Use a map to store histogram parameters
std::map<std::string, std::tuple<const char*, const char*, int, double, double>> histo_params = {
    {"h_jet_pt_lj", {"leading jet p_{T} [GeV]", "Events", 30, 0, 300}},
    {"h_deltaPhi_Zj", {"#Delta#phi_{Zj}", "Events" , 20, 0, TMath::Pi()}},
    {"h_xZj", {"x_{Zj}", "Events", 5, 0, 2}},
};

void plot_MinBias(const char* collision_type = "PbPb23", const char* h_n = "h_deltaPhi_Zj", bool isData = true) {
// Check if the histogram name exists in the map
    if (histo_params.find(h_n) == histo_params.end()) {
        std::cerr << "Error: Histogram '" << h_n << "' not found in parameter map." << std::endl;
        return;
    }
// Retrieve parameters from the map
    const char* x_title;
    const char* y_title;
    int bin;
    double min;
    double max;
    std::tie(x_title, y_title, bin, min, max) = histo_params[h_n];
//std::string h_n = "h_cen", x_title = "cen", y_title = "Events";
//if (nn = 1) {h_n = "h_mumu"; x_title = "m_{#mu#mu} [GeV]"; y_title = "Events";}

    //gROOT->LoadMacro("tdrstyle.C");
  setTDRStyle();

    //gROOT->LoadMacro("CMS_lumi.C");

  writeExtraText = true;       // if extra text
  extraText  = "Preliminary";  // default extra text is "Preliminary"
  //lumi_8TeV  = "19.1 fb^{-1}"; // default is "19.7 fb^{-1}"
  //lumi_7TeV  = "4.9 fb^{-1}";  // default is "5.1 fb^{-1}"
  TString s_coll = collision_type;
  if (s_coll.Contains("PbPb23")) {
      lumi_sqrtS = TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", getLumiFromSummary("../brilcalc_Collisions2023HI.csv"));
  }
  else if (s_coll.Contains("PbPb24")) {
      lumi_sqrtS = TString::Format("PbPb %.2f nb^{-1} (5.36 TeV)", getLumiFromSummary("../brilcalc_Collisions2024_HI.csv"));
  }
  // used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

  int iPeriod = 0;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)

  // second parameter in example_plot is iPos, which drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally : 
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)

  example_plot( iPeriod, 0 , collision_type, isData, h_n, x_title, y_title, bin, min, max);   // out of frame (in exceptional cases)
  //  example_plot( iPeriod, 11 );  // left-aligned
  //  example_plot( iPeriod, 33 );  // right-aligned

  //  writeExtraText = false;       // remove Preliminary
  
  //  example_plot( iPeriod, 0 );   // out of frame (in exceptional cases)

  //  example_plot( iPeriod, 11 );  // default: left-aligned
  //  example_plot( iPeriod, 22 );  // centered
  //  example_plot( iPeriod, 33 );  // right-aligned  
}

TCanvas* example_plot( int iPeriod, int iPos, const char* collision_type, bool isData, const char * histo_name, const char * x_title, const char * y_title, int bin, double min, double max)
{ 
  //  if( iPos==0 ) relPosX = 0.12;

  int W = 800;
  int H = 600;

  // 
  // Simple example of macro: plot with CMS name and lumi text
  //  (this script does not pretend to work in all configurations)
  // iPeriod = 1*(0/1 7 TeV) + 2*(0/1 8 TeV)  + 4*(0/1 13 TeV) 
  // For instance: 
  //               iPeriod = 3 means: 7 TeV + 8 TeV
  //               iPeriod = 7 means: 7 TeV + 8 TeV + 13 TeV 
  // Initiated by: Gautier Hamel de Monchenault (Saclay)
  // Updated by:   Dinko Ferencek (Rutgers)
  //
  int H_ref = 600; 
  int W_ref = 800; 

  // references for T, B, L, R
  float T = 0.08*H_ref;
  float B = 0.12*H_ref; 
  float L = 0.12*W_ref;
  float R = 0.04*W_ref;

  TString canvName = histo_name;
  //canvName += "_";
  //canvName += W;
  //canvName += "-";
  //canvName += H;
  //canvName += "_";  
  //canvName += iPeriod;
  canvName += "_MinBias";
  if (isData) canvName += "_data"; else canvName += "_MC";
  //if( writeExtraText ) canvName += "-prelim";
  //if( iPos%10==0 ) canvName += "-out";
  //else if( iPos%10==1 ) canvName += "-left";
  //else if( iPos%10==2 )  canvName += "-center";
  //else if( iPos%10==3 )  canvName += "-right";

  TCanvas* canv = new TCanvas(canvName,canvName,50,50,W,H);
  canv->SetFillColor(0);
  canv->SetBorderMode(0);
  canv->SetFrameFillStyle(0);
  canv->SetFrameBorderMode(0);
  canv->SetLeftMargin( L/W );
  canv->SetRightMargin( R/W );
  canv->SetTopMargin( T/H );
  canv->SetBottomMargin( B/H );
  canv->SetTickx(0);
  canv->SetTicky(0);
  //canv->SetLogy();
  
  TH1* h = new TH1F("h","h",bin,min,max);
  h->GetXaxis()->SetNdivisions(6,5,0);
  h->GetXaxis()->SetTitle(x_title);  
  h->GetYaxis()->SetNdivisions(6,5,0);
  h->GetYaxis()->SetTitleOffset(1);
  h->GetYaxis()->SetTitle(y_title);  

  //h->SetMaximum( 900 );
  //h->SetMaximum( 20000 );
  //h->SetMinimum( 1.0 );
  //h->SetAxisRange(-1., 1., "X");
  //if( iPos==1 ) h->SetMaximum( 300 );
  h->Draw();

  int histLineColor = TColor::GetColor("#7a21dd");
  int histFillColor = TColor::GetColor("#7a21dd");
  float markerSize  = 0.8;

  {
    // Dynamic Filename
    TString s_coll = collision_type;
    TString fname;
    TString name_prefix;
    if (s_coll.Contains("PbPb23")) name_prefix = "HI";
    else if (s_coll.Contains("PbPb24"))  name_prefix = "HI24";

    if (isData) fname.Form("../plot/output_%s_mu_data.root", name_prefix.Data());
    else        fname.Form("../plot/output_%s_mu_MC_signal.root", name_prefix.Data());

    cout << "Opening " << fname << endl;
    TFile* file = TFile::Open(fname, "READ");
    // and take its directories
    TDirectoryFile* dir_HI = (TDirectoryFile*)file->Get(name_prefix);
    if (!dir_HI)
      cout << "Cannot find dir_HI" << endl;
    TDirectoryFile* dir_Muons = (TDirectoryFile*)dir_HI->Get("Muons");
    if (!dir_Muons)
      cout << "Cannot find dir_Muons" << endl;
    else cout << "Yes Muons";
    if (isData) cout << " data" << endl; else cout << " MC" << endl;

    // Take the trees with the method Get()
    TH1D* h_ = (TH1D*)dir_Muons->Get(histo_name);
    std::string bkg_name = std::string(histo_name) + "_MinBias";
    std::string subtracted_name = std::string(histo_name) + "_subtracted";
    std::string matched_name = std::string(histo_name) + "_matched";
    TH1D* h_MinBias = (TH1D*)dir_Muons->Get(bkg_name.c_str());
    TH1D* h_subtracted = (TH1D*)dir_Muons->Get(subtracted_name.c_str());
    TH1D* h_matched = nullptr;
    if (!isData) h_matched = (TH1D*)dir_Muons->Get(matched_name.c_str());


    //TFile file_("histo.root","READ");
    //Int_t c_blue = TColor::GetColor("#5790fc");
    //Int_t c_red = TColor::GetColor("#e42536");
 
    h_->SetDirectory(0);
    h_MinBias->SetDirectory(0);
    h_subtracted->SetDirectory(0);
    if (!isData) h_matched->SetDirectory(0);
    //h_MC->SetMarkerStyle(23);
    //h_MC->SetMarkerSize(markerSize);
    h_->SetLineColor(kBlack);
    h_MinBias->SetLineColor(histLineColor);
    h_subtracted->SetLineColor(TColor::GetColor("#e42536"));
    h_subtracted->SetMarkerStyle(20);
    h_subtracted->SetMarkerColor(TColor::GetColor("#e42536"));
    h_subtracted->SetMarkerSize(markerSize);
    if (isData) {
      h_->SetMarkerStyle(20);
      h_->SetMarkerColor(kBlack);
      h_->SetMarkerSize(markerSize);
      h_MinBias->SetMarkerStyle(20);
      h_MinBias->SetMarkerColor(histLineColor);
      h_MinBias->SetMarkerSize(markerSize);
    }
    if (!isData) {
      h_matched->SetLineColor(TColor::GetColor("#e42536"));
      h_matched->SetMarkerStyle(20);
      h_matched->SetMarkerColor(TColor::GetColor("#e42536"));
      h_matched->SetMarkerSize(markerSize);
     }
    //h_->SetFillColor(histFillColor); // Choose a suitable color
    //h_->SetFillStyle(1001); // Choose a fill style (solid)
    //h_MC->SetMarkerColor(TColor::GetColor("#e42536"));


    //TH1F *MC   = static_cast<TH1F*>(file_.Get("MC")->Clone());
    //h_data->SetDirectory(0);
    //h_data->SetMarkerStyle(20);
    //h_data->SetMarkerSize(markerSize);
    //h_data->SetLineColor(1);
    //h_data->SetMarkerColor(1);
    //h_data->SetLineColor(histLineColor);
    //h_data->SetFillColor(histFillColor);
    
    cout << "Events : " << h_->Integral(0, h_->GetNbinsX()+1) << endl;

    
    if (isData) h_->Draw("esame"); else h_->Draw("histsame");
    if (isData) h_MinBias->Draw("esame"); else h_MinBias->Draw("histsame");
    h_subtracted->Draw("esame");
    if (!isData) h_matched->Draw("histsame");
    //h_data->Draw("e1samex0");

    double y_max = h_->GetBinContent(h_->GetMaximumBin());
    h->SetMaximum(1.2*y_max);

    double x_min = 0.59;
    if (!canvName.Contains("delta")) x_min = 0.69;
    TLatex* latex1 = new TLatex();
    latex1->SetTextFont(42);
    latex1->SetTextSize(0.036); // Set text size (adjust as needed)
    latex1->DrawLatexNDC(x_min,0.6,"Centrality: 0-30%");
    latex1->DrawLatexNDC(x_min,0.55,"p_{T}^{Z} > 40 GeV");
    latex1->DrawLatexNDC(x_min,0.49,"p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.5");
    if (!canvName.Contains("delta")) latex1->DrawLatexNDC(x_min,0.44,"#Delta#phi_{Zj} > 7#pi/8"); // Use normalized device coordinates NDC (0-1)

    TLegend* legend = new TLegend(x_min-0.01, 0.7, x_min+0.21, 0.86); // Example: Top-right corner
    legend->SetBorderSize(0);
    if (isData) legend->AddEntry(h_, "Raw", "epl"); else legend->AddEntry(h_, "Raw", "l"); // "l" for line
    if (isData) legend->AddEntry(h_MinBias, "Bkg", "epl"); else legend->AddEntry(h_MinBias, "Bkg", "l");
    if (isData) legend->AddEntry(h_subtracted, "Raw - Bkg", "epl"); else legend->AddEntry(h_subtracted, "Raw - Bkg", "epl");
    legend->SetTextFont(42);
    legend->SetTextColor(kBlack);
    legend->SetTextSize(0.036);
    if (!isData) {
      legend->AddEntry(h_matched, "True", "l");
    }
    legend->Draw();

    file->Close();
  }

  // writing the lumi information and the CMS "logo"
  CMS_lumi( canv, iPeriod, iPos );

  canv->Update();
  canv->RedrawAxis();
  canv->GetFrame()->Draw();

  canv->Print(canvName+".pdf",".pdf");
  //canv->Print(canvName+".png",".png");

  return canv;
}
