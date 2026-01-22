#include "tdrstyle.C"
#include "CMS_lumi.C"
#include "TH1.h"
#include "TH1F.h"

TCanvas* example_plot( int iPeriod, int iPos, const char * histo_name, const char * x_title, const char * y_title, int bin, double min, double max);

// Use a map to store histogram parameters
std::map<std::string, std::tuple<const char*, const char*, int, double, double>> histo_params = {
    {"h_mumu", {"m_{#mu#mu} [GeV]", "Events", 20, 60, 120}},
    {"h_Z_pt", {"p_{T}^{Z} [GeV]", "Events", 30, 0, 300}},
    {"h_njet", {"n_{jet}", "Events", 10, 0, 10}},
    {"h_cen", {"cen", "Events", 20, 0, 100}},
    {"h_mumu_j", {"m_{#mu#mu} [GeV]", "Events", 20, 60, 120}},
    {"h_Z_pt_j", {"p_{T}^{Z} [GeV]", "Events", 30, 0, 300}},
    {"h_jet_pt_lj", {"leading jet p_{T} [GeV]", "Events", 30, 0, 300}},
    {"h_cen_j", {"cen_j", "Events", 20, 0, 100}},
    {"h_deltaPhi_Zj", {"#Delta#phi_{Zj}", "Events" , 20, 0, TMath::Pi()}},
    {"h_xZj_fixbinw", {"x_{Zj}", "Events", 30, 0, 3.}},
    {"h_vz", {"vz", "Events", 30, -20, 20}},
    {"h_avg_rho", {"<#rho>", "Entries", 50, 0, 400}}
//    {"h_jetgirth", {"girth", "Events", 10, 0, 0.2}},
//    {"h_jet_deltaR", {"R_{g}", "Events", 10, 0, 0.2}}
};

void plot(const char* h_n) {
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
  lumi_sqrtS = "PbPb 1.64 nb^{-1} (5.36 TeV)";       // used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)

  int iPeriod = 0;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)

  // second parameter in example_plot is iPos, which drives the position of the CMS logo in the plot
  // iPos=11 : top-left, left-aligned
  // iPos=33 : top-right, right-aligned
  // iPos=22 : center, centered
  // mode generally : 
  //   iPos = 10*(alignement 1/2/3) + position (1/2/3 = left/center/right)

  example_plot( iPeriod, 0 , h_n, x_title, y_title, bin, min, max);   // out of frame (in exceptional cases)
  //  example_plot( iPeriod, 11 );  // left-aligned
  //  example_plot( iPeriod, 33 );  // right-aligned

  //  writeExtraText = false;       // remove Preliminary
  
  //  example_plot( iPeriod, 0 );   // out of frame (in exceptional cases)

  //  example_plot( iPeriod, 11 );  // default: left-aligned
  //  example_plot( iPeriod, 22 );  // centered
  //  example_plot( iPeriod, 33 );  // right-aligned  
}

TCanvas* example_plot( int iPeriod, int iPos, const char * histo_name, const char * x_title, const char * y_title, int bin, double min, double max)
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
  if( writeExtraText ) canvName += "-prelim";
  if( iPos%10==0 ) canvName += "-out";
  else if( iPos%10==1 ) canvName += "-left";
  else if( iPos%10==2 )  canvName += "-center";
  else if( iPos%10==3 )  canvName += "-right";

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
  h->SetMinimum( 1.0 );
  //h->SetAxisRange(-1., 1., "X");
  //if( iPos==1 ) h->SetMaximum( 300 );
  h->Draw();

  int histLineColor = TColor::GetColor("#e42536");
  int histFillColor = TColor::GetColor("#e42536");
  float markerSize  = 0.8;

  {
    TLatex latex;
				
    int n_ = 2;

    float x1_l = 0.4;
    float y1_l = 0.916;

    float dx_l = 0.26;
    float dy_l = 0.16;
    float x0_l = x1_l-dx_l;
    float y0_l = y1_l-dy_l;

    TPad* legend = new TPad("legend_0","legend_0",x0_l,y0_l,x1_l, y1_l );
    //    legend->SetFillColor( kGray );
    legend->Draw();
    legend->cd();
		
    float ar_l = dy_l/dx_l;
		
    float x_l[1];
    float ex_l[1];
    float y_l[1];
    float ey_l[1];
		
    //    float gap_ = 0.09/ar_l;
    float gap_ = 1./(n_+1);
		
    float bwx_ = 0.12;
    float bwy_ = gap_/1.5;
		
    x_l[0] = 1.2*bwx_;
    //    y_l[0] = 1-(1-0.10)/ar_l;
    y_l[0] = 1-gap_;
    ex_l[0] = 0;
    ey_l[0] = 0.04/ar_l;
		
    TGraph* gr_data = new TGraphErrors(1, x_l, y_l, ex_l, ey_l );
    
    //gStyle->SetEndErrorSize(0);
    
    gr_data->SetMarkerSize(markerSize);
    gr_data->SetMarkerStyle(20);
    gr_data->SetLineColor(1);
    gr_data->SetMarkerColor(1);
    gr_data->Draw("0P");
  		
    latex.SetTextFont(42);
    latex.SetTextAngle(0);
    latex.SetTextColor(kBlack);    
    latex.SetTextSize(0.25);    
    latex.SetTextAlign(12); 
		

    float xx_ = x_l[0];
    float yy_ = y_l[0];
    latex.DrawLatex(xx_+1.*bwx_,yy_,"Data");
		
    yy_ -= gap_;
    y_l[0] -= gap_;
    
    /*TGraph* gr_MC = new TGraphErrors(1, x_l, y_l, ex_l, ey_l );
    gr_MC->SetMarkerSize(markerSize);
    gr_MC->SetMarkerStyle(23);
    gr_MC->SetLineColor(TColor::GetColor("#e42536"));
    gr_MC->SetMarkerColor(TColor::GetColor("#e42536"));
    gr_MC->Draw("0P");*/
    
    TLine line_;
    line_.SetLineWidth( 1 );
    line_.SetLineColor(histLineColor);
    line_.DrawLine(xx_-bwx_/2, yy_, xx_+bwx_/2, yy_ );
    TBox  box_;
    box_.SetLineStyle( kSolid );
    box_.SetLineWidth( 1 );
    //		box_.SetLineColor( kBlack );
    box_.SetLineColor(histLineColor);
    box_.SetFillColor(histFillColor);
    box_.DrawBox( xx_-bwx_/2, yy_-bwy_/2, xx_+bwx_/2, yy_+bwy_/2 );
    box_.SetFillStyle(0);
    box_.DrawBox( xx_-bwx_/2, yy_-bwy_/2, xx_+bwx_/2, yy_+bwy_/2 );
    latex.DrawLatex(xx_+1.*bwx_,yy_,"Drell-Yan");

    canv->cd();
  }

  {
    TFile* file_MC = TFile::Open("./output_HI_mu_MC_signal.root", "READ");
    // and take its directories
    TDirectoryFile* dir_HI_MC = (TDirectoryFile*)file_MC->Get("HI");
    if (!dir_HI_MC)
      cout << "Cannot find dir_HI_MC" << endl;
    TDirectoryFile* dir_Muons_MC = (TDirectoryFile*)dir_HI_MC->Get("Muons");
    if (!dir_Muons_MC)
      cout << "Cannot find dir_Muons_MC" << endl;
    else cout << "Yes Muons MC" << endl;
    
    TFile* file_data = TFile::Open("./output_HI_mu_data.root", "READ");
    // and take its directories
    TDirectoryFile* dir_HI_data = (TDirectoryFile*)file_data->Get("HI");
    if (!dir_HI_data)
      cout << "Cannot find dir_HI_data" << endl;
    TDirectoryFile* dir_Muons_data = (TDirectoryFile*)dir_HI_data->Get("Muons");
    if (!dir_Muons_data)
      cout << "Cannot find dir_Muons_data" << endl;
    else cout << "Yes Muons data" << endl;
    
  
    // Take the trees with the method Get()
    TH1D* h_MC = (TH1D*)dir_Muons_MC->Get(histo_name);
    TH1D* h_data = (TH1D*)dir_Muons_data->Get(histo_name);

    //TFile file_("histo.root","READ");

    //Int_t c_blue = TColor::GetColor("#5790fc");
    //Int_t c_red = TColor::GetColor("#e42536");
 
    h_MC->SetDirectory(0);
    //h_MC->SetMarkerStyle(23);
    //h_MC->SetMarkerSize(markerSize);
    h_MC->SetLineColor(histLineColor);
    h_MC->SetFillColor(histFillColor); // Choose a suitable color
    h_MC->SetFillStyle(1001); // Choose a fill style (solid)
    //h_MC->SetMarkerColor(TColor::GetColor("#e42536"));


    //TH1F *MC   = static_cast<TH1F*>(file_.Get("MC")->Clone());
    h_data->SetDirectory(0);
    h_data->SetMarkerStyle(20);
    h_data->SetMarkerSize(markerSize);
    h_data->SetLineColor(1);
    h_data->SetMarkerColor(1);
    //h_data->SetLineColor(histLineColor);
    //h_data->SetFillColor(histFillColor);
    
    h_MC->Draw("histsame");
    h_data->Draw("e1samex0");

    double y_max=0;
    if (h_data->GetBinContent(h_data->GetMaximumBin())>=h_MC->GetBinContent(h_MC->GetMaximumBin())) y_max=h_data->GetBinContent(h_data->GetMaximumBin());
    else y_max=h_MC->GetBinContent(h_MC->GetMaximumBin());
    if (canvName.Contains("delta")) h->SetMaximum(2.2*y_max);
    else h->SetMaximum(1.4*y_max);

    // 4. Create TLatex object for the integral value
    TLatex* latex1 = new TLatex();
    //latex->SetNDC(); // Use normalized device coordinates (0-1)
    latex1->SetTextSize(0.025); // Set text size (adjust as needed)
    latex1->SetTextColor(kBlue); // Set text color (optional)

    latex1->DrawLatexNDC(0.84,0.5,TString::Format("#int data = %.0f", h_data->Integral(0, h_data->GetNbinsX()+1)));
    latex1->DrawLatexNDC(0.7,0.5,TString::Format("#int MC = %.0f", h_MC->Integral(0, h_MC->GetNbinsX()+1)));

    file_MC->Close();
    file_data->Close();
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
