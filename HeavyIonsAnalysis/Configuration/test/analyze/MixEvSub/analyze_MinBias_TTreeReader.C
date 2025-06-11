// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector
#include <map>        // For std::map to store events per bin
#include <string>     // For std::string
#include <utility>    // For std::pair
#include "binning_config.h"

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
#include "TH1.h"
#include "TStyle.h"

#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "../JetCorrector.h" // for JEC

using namespace std;

//To run, root -l analyze_HI_MinBias_TTreeReader.C
//Default isData, for MC root -l 'analyze_MinBias_TTreeReader.C(false)'
void analyze_MinBias_TTreeReader(bool isData = true) {

  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis");

  glob_t globlist;
  if (isData) {
    glob("/eos/infnts/cms/store/user/kdeleo/HIMinimumBias0/CRAB3_Analysis_test15_MinBias0/250522_121447/0000/HiForestMiniAOD_DATA_*.root", GLOB_NOSORT, NULL, &globlist);
  cout << "This is data" << endl;
  }
  else {
    glob("/eos/infnts/cms/store/user/kdeleo/MinBias_Drum5F_5p36TeV_hydjet/CRAB3_Analysis_test15_mc_MinBias/250523_135118/0000/HiForestMiniAOD_MC_*.root", GLOB_NOSORT, NULL, &globlist);
    cout << "This is MC" << endl;
  }
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    //data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzerSubstructure/t");
    data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/muonAnalyzer/MuonTree");
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
    skimanalysis.Add(TString(globlist.gl_pathv[i]) + "/skimanalysis/HltTree");
    //hltanalysis.Add(TString(globlist.gl_pathv[i]) + "/hltanalysis/HltTree");

  }
  globfree(&globlist);

  //To associate additional TTrees with a primary TTree. This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  data.AddFriend("HiTree");
  data.AddFriend("skimanalysis");
  //data.AddFriend("hltanalysis");

  TTreeReader fReader(&data);

  // Declaration of leaf types
  TTreeReaderValue<Int_t> run = {fReader, "run"};    // Run number
  TTreeReaderValue<Int_t> evt = {fReader, "evt"};    // Event number
  TTreeReaderValue<Int_t> lumi = {fReader, "lumi"};  // Luminosity block
  TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"}; // centralityx2
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderValue<Float_t> hiHF = {fReader, "hiHF"};

  //TTreeReaderValue<float> Ncoll = {fReader, "Ncoll"}; // Ncoll

  // Filters
  TTreeReaderValue<int> pprimaryVertexFilter = {fReader, "pprimaryVertexFilter"};
  TTreeReaderValue<int> pclusterCompatibilityFilter = {fReader, "pclusterCompatibilityFilter"};
  TTreeReaderValue<int> pphfCoincFilter2Th4 = {fReader, "pphfCoincFilter2Th4"};

  // Trigger
  //TTreeReaderValue<Int_t> HLT_HIL2SingleMu7_v3 = {fReader, "HLT_HIL2SingleMu7_v3"};

  // Muon
  TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
  TTreeReaderArray<Float_t> recoPt = {fReader, "recoPt"};
  TTreeReaderArray<Float_t> recoEta = {fReader, "recoEta"};
  TTreeReaderArray<Float_t> recoPhi = {fReader, "recoPhi"};
  TTreeReaderArray<Int_t> recoCharge = {fReader, "recoCharge"};
  TTreeReaderArray<bool> recoIDTight = {fReader, "recoIDTight"};
  const double muonMass = 0.1056583755; //From PDG 2024

  // Jet
  TTreeReaderValue<Int_t> nref = {fReader, "nref"};
  //TTreeReaderArray<Float_t> rawpt = {fReader, "jtptUncorrected"};
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  //TTreeReaderArray<Float_t> jtgirth = {fReader, "jt_girth"};
  //TTreeReaderArray<Float_t> jtdyndeltaR = {fReader, "jtdyn_deltaR"};
  TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};
  //TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};

  // To apply corrections on jets
  vector<string> Files;
  Files.push_back("../ParallelMC_L2Relative_AK2PF_PbPb_Reco_v0_2_13_2024.txt");
  JetCorrector JEC(Files);

  // --- Define HF bins ---
  std::map<std::string, std::vector<std::pair<double, double>>> leading_jets_by_hf_bin;

  std::vector<std::pair<float, float>> hf_bins;
  const int total_bins = BinningConfig::tot_bins;
  const int events_per_bin = BinningConfig::ev_per_bin;
  const float first_bin_min = BinningConfig::frst_bin_min;

  float current_min_hf = first_bin_min;
  for (int i = 0; i < total_bins; ++i) {
      float current_max_hf = current_min_hf * 1.1;
      hf_bins.push_back({current_min_hf, current_max_hf});
      current_min_hf = current_max_hf;
  }

  std::cout << "Defined HF bins (" << hf_bins.size() << " total):" << std::endl;
  for (const auto& bin : hf_bins) {
      std::cout << "[" << bin.first << ", " << bin.second << ")" << std::endl;
  }
  // --- End of HF bin definition ---

  // Save in a ttree
  TTree *jet_tree;
  jet_tree = new TTree("jet_tree","jet_tree");
  Float_t jet_tree_HF = 0;
  Int_t jet_tree_HF_bin = 0;
  Float_t jet_tree_pt = 0;
  Float_t jet_tree_phi = 0;
  Float_t jet_tree_eta = 0;

  jet_tree->Branch("HF_MinBias", &jet_tree_HF);
  jet_tree->Branch("HF_bin_MinBias", &jet_tree_HF_bin);
  jet_tree->Branch("jet_pt_MinBias", &jet_tree_pt);
  jet_tree->Branch("jet_phi_MinBias", &jet_tree_phi);
  jet_tree->Branch("jet_eta_MinBias", &jet_tree_eta);

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

  //Histograms
  TH1F *h_cen = new TH1F("h_cen", "Hist; centrality bin; Entries", 20, 0, 100);
  TH1F *h_HF = new TH1F("h_HF", "Hist; HF; Entries", 80, 0, 8000);

  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_Phi_lj = new TH1F("h_Phi_lj", "Hist;#phi_{lj}; Entries", 20, -TMath::Pi(),TMath::Pi());

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Track how many bins have reached 100 events to potentially stop early
  int filled_bins_count = 0;

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    // Optional: if all bins are filled, we can stop processing events early
    if (filled_bins_count == total_bins) {
        std::cout << "All " << total_bins << " HF bins have collected " << events_per_bin << " events. Stopping event loop early." << std::endl;
        break;
    }
    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;
     // Selection on centrality bin
     //if(*hiBin>59) continue;

     float current_hiHF_val = *hiHF;
     std::string current_bin_label = "";
     int bin_n = 0;
     int current_bin_n = 0;
     bool found_bin = false;
     for (const auto& bin_range : hf_bins) {
       if (current_hiHF_val >= bin_range.first && current_hiHF_val < bin_range.second) {
         current_bin_label = Form("%.0f-%.0f [%d]", bin_range.first, bin_range.second, bin_n);
         found_bin = true;
         current_bin_n = bin_n;
         break;
       }
     bin_n++;
     }

     if (!found_bin) {
       continue;
     }

    //if(*HLT_HIL2SingleMu7_v3<=0) continue;
//    bool good_pair = false;
//    if (*nReco < 2 ) continue;
    iEvent++;
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // Check if this bin already has 100 events
    if (leading_jets_by_hf_bin[current_bin_label].size() >= events_per_bin) {
      continue; // Skip this event, this bin is already full
    }

    // Loop over Jets
    int ijetLeading = -1;
    double jtpt_corr[20000];
    for(int ijet=0; ijet<*nref; ijet++){
      JEC.SetJetPT(rawpt[ijet]);
      JEC.SetJetEta(jteta[ijet]);
      JEC.SetJetPhi(jtphi[ijet]);
      double Correction = JEC.GetCorrection();
      double CorrectedPT = JEC.GetCorrectedPT();
      jtpt_corr[ijet] = CorrectedPT;
      //jtpt_corr[ijet] = rawpt[ijet];
      //cout << "after JEC: jtpt_corr = " << jtpt_corr[ijet] << " CorrectedPT = " << CorrectedPT << endl;
      //if(jtpt_corr[ijet]<30) continue;
      //if(abs(jteta[ijet])>2.5) continue;
      //cout << "-----------------------------" << endl;
      //cout << "ijet: " << ijet << " pt = " << jtpt_corr[ijet] << " eta = " << jteta[ijet] << " phi = " << jtphi[ijet] << " m = " << jtm[ijet] << endl;
      //cout << "muMinus pt = " << muMinus.Pt() << " eta = " << muMinus.Eta() << " phi = " << muMinus.Phi() << endl;
      //cout << "muPlus pt = " << muPlus.Pt() << " eta = " << muPlus.Eta() << " phi = " << muPlus.Phi() << endl;
      //cout << "dRMinus = " << dRMinus << " dRPlus = " << dRPlus << endl;
      if (ijetLeading == -1 || jtpt_corr[ijet] > jtpt_corr[ijetLeading]) {
          ijetLeading = ijet;
      }
    } //end loop over jets
    //cout << "ijetLeading = " << ijetLeading << endl;

    if (ijetLeading != -1) {
      h_vz->Fill(*vz);
      h_HF->Fill(*hiHF);
      h_cen->Fill((*hiBin)/2);
      h_Phi_lj->Fill(jtphi[ijetLeading]);
      h_jet_pt_lj->Fill(jtpt_corr[ijetLeading]);

      leading_jets_by_hf_bin[current_bin_label].push_back({jtpt_corr[ijetLeading], jtphi[ijetLeading]});
      jet_tree_HF = *hiHF;
      jet_tree_HF_bin = current_bin_n;
      jet_tree_pt = jtpt_corr[ijetLeading];
      jet_tree_phi = jtphi[ijetLeading];
      jet_tree_eta = jteta[ijetLeading];
      jet_tree->Fill();

      // Check if this push_back just filled the bin to 100
      if (leading_jets_by_hf_bin[current_bin_label].size() == events_per_bin) {
        filled_bins_count++;
        std::cout << "Bin " << current_bin_label << " is now full with " << events_per_bin << " events. Total filled bins: " << filled_bins_count << std::endl;
      }
    }
  }  // end loop events

  std::cout << "\n--- Finished event collection. Final status of leading jet data per HF bin: ---" << std::endl;
  for (const auto& pair : leading_jets_by_hf_bin) {
      std::cout << "Bin " << pair.first << ": " << pair.second.size() << " leading jets collected." << std::endl;
      // Optional: Print first few collected events for verification
      // if (pair.second.size() > 0) {
      //     std::cout << "  First event: Pt = " << pair.second[0].first << ", Phi = " << pair.second[0].second << std::endl;
      // }
  }

  cout << "N events (h_Phi_lj): " << h_Phi_lj->Integral()
       << " (h_jet_pt_lj); " << h_jet_pt_lj->Integral() << endl;

   // --- Store to a ROOT file ---
   TFile *outputFile;
   if (isData) {
      outputFile = new TFile("./MinBias_leading_jets_data.root", "RECREATE");
   } else {
      outputFile = new TFile("./MinBias_leading_jets_MC.root", "RECREATE");
   }
   jet_tree->Write("",TObject::kOverwrite);

   outputFile->Close();

  c1->cd(1);
  h_cen->Draw();
  c2->cd(1);
  h_vz->Draw();
  c3->cd(1);
  h_HF->Draw();
  c4->cd(1);
  h_jet_pt_lj->Draw();
  c5->cd(1);
  h_Phi_lj->Draw();
}

