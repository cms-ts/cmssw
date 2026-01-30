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

#include "../../helpers.h"
#include "../../MC_samples.h"

using namespace std;

//To run, root -l analyze_HI_TTreeReader_ZMM_all.C
void analyze_HI_TTreeReader_ZMM_all(const char* collision_type = "PbPb23") {

  TString s_coll = collision_type;
  bool isPbPb = s_coll.Contains("PbPb");

  TString input_path = "";
  bool found_signal = false;

  // --- 1. Select MC File Vector from MC_samples.h ---
  const std::vector<FileInfo>* targetVector = nullptr;
  if (s_coll.Contains("PbPb23")) {
    targetVector = &files;
  }
  else if (s_coll.Contains("PbPb24")) {
    targetVector = &files_PbPb24;;
  }
  else if (s_coll.Contains("ppref24")) {
    targetVector = &files_ppref;
  } else {
    std::cerr << "Error: Unknown collision type " << s_coll << std::endl;
    return;
  }

  // Loop to find the "signal" label in the selected vector
  if (targetVector) {
    for (const auto& file : *targetVector) {
      if (file.label == "signal") {
        input_path = file.path_miniaod; // This contains the /path/to/*.root
        cout << "Found signal path in MC_samples.h for " << s_coll << ": " << input_path << endl;
        found_signal = true;
        break;
      }
    }
  }

  if (!found_signal) {
      cerr << "[ERROR] Could not find 'signal' label in MC_samples.h for " << s_coll << endl;
      // Fallback/Default paths if needed (optional)
      return;
  }


  //TTrees
  TChain  HiTree("HiTree");

  glob_t globlist;
  glob(input_path.Data(), GLOB_NOSORT, NULL, &globlist);
  if (globlist.gl_pathc == 0) {
      cout << "[ERROR] No files found for path: " << input_path << endl;
      return;
  }
  cout << "Found " << globlist.gl_pathc << " files" << endl;

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
  TH1F *h_weight = new TH1F("h_weight", "Hist; w; Entries", 20, -800, 800);
  TH1F *h_sum_weights = new TH1F("h_sum_weights", "Hist; w; Entries", 1, 0, 20);
  TH1F *h_sum_weights_cen = new TH1F("h_sum_weights_cen", "Hist; w; Entries", 1, 0, 2);
  TH1F *h_cen_before = new TH1F("h_cen_before", "Hist; cen before; Entries", 20, 0, 100);
  TH1F *h_cen_after = new TH1F("h_cen_after", "Hist; cen after; Entries", 20, 0, 100);

  // Output root file
  TString name_output = "HI";
  if (s_coll.Contains("PbPb24")) name_output = "HI24";
  else if (s_coll.Contains("ppref24")) name_output = "ppref";

  TString out_file_path = "./output_" + name_output + "_mu_MC_all.root";
  TFile *file_output_HI_mu = new TFile(out_file_path, "RECREATE");

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

  cout << "Number of events = " << h_n_events->Integral(0, h_n_events->GetNbinsX()+1)
       << ", sumW = " << h_sum_weights->Integral(0, h_sum_weights->GetNbinsX()+1)
       << ", sumW_cen = " << h_sum_weights_cen->Integral(0, h_sum_weights_cen->GetNbinsX()+1) << endl;

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
  TDirectory *Dir = file_output_HI_mu->mkdir(name_output);
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
