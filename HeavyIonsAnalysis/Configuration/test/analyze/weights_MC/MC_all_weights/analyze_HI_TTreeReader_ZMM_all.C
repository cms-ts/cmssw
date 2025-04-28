// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TDirectory.h"

#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"


using namespace std;

//To run, root -l analyze_HI_TTreeReader_ZMM_all.C
void analyze_HI_TTreeReader_ZMM_all() {

  // Centrality weights
  const float Ncoll[200] = {1893.13, 1867.0, 1834.16, 1805.64, 1770.84, 1744.49, 1699.76, 1661.52, 1615.89, 1579.59, 1540.62, 1499.14, 1469.01, 1432.18, 1402.8, 1368.39, 1338.12, 1302.26, 1274.91, 1245.56, 1215.28, 1183.76, 1160.61, 1131.12, 1107.67, 1078.54, 1055.72, 1026.72, 1000.57, 980.728, 958.777, 936.515, 911.397, 889.182, 869.677, 853.33, 826.999, 808.145, 792.14, 769.639, 753.513, 732.883, 716.817, 697.168, 679.091, 668.056, 650.114, 631.024, 616.203, 597.835, 583.435, 571.454, 555.478, 543.589, 526.328, 511.657, 497.023, 489.255, 471.52, 461.133, 447.767, 436.993, 426.106, 412.626, 403.224, 389.71, 382.595, 371.48, 358.899, 349.179, 339.387, 330.523, 320.094, 313.254, 302.339, 292.421, 282.594, 274.834, 268.847, 259.463, 252.027, 244.561, 236.738, 229.574, 222.898, 215.138, 207.328, 200.879, 196.592, 190.921, 183.942, 176.685, 170.919, 166.96, 161.057, 154.421, 148.816, 144.84, 139.087, 134.448, 128.72, 124.905, 121.166, 116.648, 112.367, 109.012, 104.33, 100.736, 97.3484, 93.2283, 89.3299, 85.9068, 83.6446, 80.2019, 77.5299, 73.9647, 70.7606, 68.2284, 65.793, 63.4532, 60.4738, 58.2406, 55.063, 53.7287, 51.4638, 49.241, 47.0111, 45.5443, 43.1729, 41.5041, 39.5449, 37.9282, 36.8918, 34.9287, 33.1886, 31.9177, 30.756, 29.0803, 27.6721, 26.42, 25.2678, 24.2585, 23.1429, 22.0138, 21.0169, 19.8203, 19.1043, 18.1478, 17.1715, 16.3605, 15.4763, 14.7973, 14.1594, 13.3927, 12.795, 12.1059, 11.5921, 10.9751, 10.3213, 9.94434, 9.3518, 8.94274, 8.37618, 7.94437, 7.48868, 7.06923, 6.71137, 6.31856, 6.03184, 5.67048, 5.43369, 5.13727, 4.83292, 4.58846, 4.37208, 4.15225, 3.84385, 3.63752, 3.45214, 3.24892, 3.02845, 2.81715, 2.66395, 2.5053, 2.29512, 2.13703, 1.93591, 1.79771, 1.64165, 1.54375, 1.45878, 1.36718, 1.2942, 1.23934, 1.18423, 1.14467, 1.11826, 1.0863, 1.06149, 1.04497 };
  //TTrees
  TChain  HiTree("HiTree");

  glob_t globlist;
  glob("condor/HiForestMiniAOD_MC_all.root", GLOB_NOSORT, NULL, &globlist);
  //glob("/eos/infnts/cms/store/user/kdeleo/TT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test11_Zmumu_TT/250114_143300/0000.root", GLOB_NOSORT, NULL, &globlist);
  cout << "This is a MC" << endl;
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
  }
  globfree(&globlist);

  TTreeReader fReader(&HiTree);

  // Declaration of leaf types
  TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"}; // centralityx2
  TTreeReaderValue<Float_t> weight = {fReader, "weight"}; // MC event weight

  //Canvas
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 1200, 800);
  c1->Divide(1,1);
  TCanvas* c2 = new TCanvas("c2", "c2", 1200, 800);
  c2->Divide(1,1);
  TCanvas* c3 = new TCanvas("c3", "c3", 1200, 800);
  c3->Divide(1,1);
  TCanvas* c4 = new TCanvas("c4", "c4", 1200, 800);
  c4->Divide(1,1);
  TCanvas* c5 = new TCanvas("c5", "c5", 1200, 800);
  c5->Divide(1,1);
  TCanvas* c6 = new TCanvas("c6", "c6", 1200, 800);
  c6->Divide(1,1);

  //Histograms
  TH1F *h_n_events = new TH1F("h_n_events", "Hist; n; Entries", 1, 0, 2);
  TH1F *h_weight = new TH1F("h_weight", "Hist; w; Entries", 20, -600, 600);
  TH1F *h_sum_weights = new TH1F("h_sum_weights", "Hist; w; Entries", 1, 0, 20);
  TH1F *h_sum_weights_cen = new TH1F("h_sum_weights_cen", "Hist; w; Entries", 1, 0, 2);
  TH1F *h_cen_before = new TH1F("h_cen_before", "Hist; cen before; Entries", 20, 0, 100);
  TH1F *h_cen_after = new TH1F("h_cen_after", "Hist; cen after; Entries", 20, 0, 100);

  // Output root file
  TFile *file_output_HI_mu;
    file_output_HI_mu = new TFile("./output_HI_mu_MC_all.root", "RECREATE");

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    float weight_cent = Ncoll[*hiBin];
    float scale =1;
    scale*=weight_cent*(*weight);
    h_n_events->Fill(1);
    h_weight->Fill((*weight));
    h_sum_weights->Fill(1,(*weight));
    h_sum_weights_cen->Fill(1,weight_cent);
    h_cen_before->Fill((*hiBin)/2);
    h_cen_after->Fill((*hiBin)/2, scale);
    iEvent++;
  }  // end loop events

  cout << "Number of events = " << h_n_events->Integral(0, h_n_events->GetNbinsX()+1) << ", sumW = " << h_sum_weights->Integral(0, h_sum_weights->GetNbinsX()+1) << ", sumW_cen = " << h_sum_weights_cen->Integral(0, h_sum_weights_cen->GetNbinsX()+1) << endl;

  c1->cd(1);
  h_n_events->Draw();
  c2->cd(1);
  h_weight->Draw();
  c3->cd(1);
  h_sum_weights->Draw();
  c4->cd(1);
  h_sum_weights_cen->Draw();
  c5->cd(1);
  h_cen_before->Draw();
  c6->cd(1);
  h_cen_after->Draw();

  // Create the main directory "MC"
  TDirectory *Dir = file_output_HI_mu->mkdir("HI");
  // Navigate to the directory
  Dir->cd();
  // Create a new directory named "Muons"
  TDirectory *muonsDir = Dir->mkdir("Muons");
  // Navigate to the "MUONS" directory
  muonsDir->cd();
  h_n_events->Write();
  h_weight->Write();
  h_sum_weights->Write();
  h_sum_weights_cen->Write();
  h_cen_before->Write();
  h_cen_after->Write();

  file_output_HI_mu->Close();
}
