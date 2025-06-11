// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector
#include "./MixEvSub/binning_config.h"

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

#include "JetCorrector.h" // for JEC
#include "MC_samples.h" // Include the header file for MC samples

using namespace std;

double RelativePhi(double phi_1,double phi_2) {
  double d_phi =  abs(phi_1 - phi_2);
  if (d_phi > acos(-1)) d_phi = 2*acos(-1) - d_phi;
  return d_phi;
}

//To run, root -l analyze_HI_TTreeReader_ZMM.C
//Default isData, for MC root -l 'analyze_HI_TTreeReader_ZMM.C(false, 3)'
void analyze_HI_TTreeReader_ZMM(bool isData = true, unsigned int weight_phase = 1) {

  // --- Load Muon Scale Factor Histograms ---
  // TightID SF
  TFile* tightID_sf_file = TFile::Open("NUM_TightID_DEN_genTracks_abseta1_pt2_cutAndCount.root", "READ");
  if (!tightID_sf_file) {
      std::cerr << "Error: Cannot open NUM_TightID_DEN_genTracks_abseta1_pt2_cutAndCount.root file!" << std::endl;
      return;
  }
  TH2F* h_tightID_sf = (TH2F*)tightID_sf_file->Get("NUM_TightID_DEN_genTracks_abseta1_pt2");
  if (!h_tightID_sf) {
      std::cerr << "Error: Cannot retrieve NUM_TightID_DEN_genTracks_abseta1_pt2 histogram!" << std::endl;
      tightID_sf_file->Close();
      return;
  }
  h_tightID_sf->SetDirectory(0); // Detach histogram from file
  tightID_sf_file->Close();

  // HLT SF
  TFile* hlt_sf_file = TFile::Open("NUM_HLT_HIL2SingleMu7_v_DEN_TightID_abseta1_pt1_cutAndCount.root", "READ");
  if (!hlt_sf_file) {
      std::cerr << "Error: Cannot open NUM_HLT_HIL2SingleMu7_v_DEN_TightID_abseta1_pt1_cutAndCount.root file!" << std::endl;
      return;
  }
  TH2F* h_hlt_sf = (TH2F*)hlt_sf_file->Get("NUM_HLT_HIL2SingleMu7_v_DEN_TightID_abseta1_pt1");
  if (!h_hlt_sf) {
      std::cerr << "Error: Cannot retrieve NUM_HLT_HIL2SingleMu7_v_DEN_TightID_abseta1_pt1 histogram!" << std::endl;
      hlt_sf_file->Close();
      return;
  }
  h_hlt_sf->SetDirectory(0); // Detach histogram from file
  hlt_sf_file->Close();
  // --- End Load Muon Scale Factor Histograms ---

  // Centrality weights
  const float Ncoll[200] = {1893.13, 1867.0, 1834.16, 1805.64, 1770.84, 1744.49, 1699.76, 1661.52, 1615.89, 1579.59, 1540.62, 1499.14, 1469.01, 1432.18, 1402.8, 1368.39, 1338.12, 1302.26, 1274.91, 1245.56, 1215.28, 1183.76, 1160.61, 1131.12, 1107.67, 1078.54, 1055.72, 1026.72, 1000.57, 980.728, 958.777, 936.515, 911.397, 889.182, 869.677, 853.33, 826.999, 808.145, 792.14, 769.639, 753.513, 732.883, 716.817, 697.168, 679.091, 668.056, 650.114, 631.024, 616.203, 597.835, 583.435, 571.454, 555.478, 543.589, 526.328, 511.657, 497.023, 489.255, 471.52, 461.133, 447.767, 436.993, 426.106, 412.626, 403.224, 389.71, 382.595, 371.48, 358.899, 349.179, 339.387, 330.523, 320.094, 313.254, 302.339, 292.421, 282.594, 274.834, 268.847, 259.463, 252.027, 244.561, 236.738, 229.574, 222.898, 215.138, 207.328, 200.879, 196.592, 190.921, 183.942, 176.685, 170.919, 166.96, 161.057, 154.421, 148.816, 144.84, 139.087, 134.448, 128.72, 124.905, 121.166, 116.648, 112.367, 109.012, 104.33, 100.736, 97.3484, 93.2283, 89.3299, 85.9068, 83.6446, 80.2019, 77.5299, 73.9647, 70.7606, 68.2284, 65.793, 63.4532, 60.4738, 58.2406, 55.063, 53.7287, 51.4638, 49.241, 47.0111, 45.5443, 43.1729, 41.5041, 39.5449, 37.9282, 36.8918, 34.9287, 33.1886, 31.9177, 30.756, 29.0803, 27.6721, 26.42, 25.2678, 24.2585, 23.1429, 22.0138, 21.0169, 19.8203, 19.1043, 18.1478, 17.1715, 16.3605, 15.4763, 14.7973, 14.1594, 13.3927, 12.795, 12.1059, 11.5921, 10.9751, 10.3213, 9.94434, 9.3518, 8.94274, 8.37618, 7.94437, 7.48868, 7.06923, 6.71137, 6.31856, 6.03184, 5.67048, 5.43369, 5.13727, 4.83292, 4.58846, 4.37208, 4.15225, 3.84385, 3.63752, 3.45214, 3.24892, 3.02845, 2.81715, 2.66395, 2.5053, 2.29512, 2.13703, 1.93591, 1.79771, 1.64165, 1.54375, 1.45878, 1.36718, 1.2942, 1.23934, 1.18423, 1.14467, 1.11826, 1.0863, 1.06149, 1.04497 };


  // Open the weight file and retrieve the h_weight_rho histogram
    TFile* rho_weightFile = TFile::Open("weights_MC/rho_weights_1/weight_rho.root", "READ");
    if (!rho_weightFile) {
        std::cerr << "Error: Cannot open weights_rho.root file!" << std::endl;
        return;
    }
    TH1D* h_weight_rho = (TH1D*)rho_weightFile->Get("h_weight_rho");
    if (!h_weight_rho) {
        std::cerr << "Error: Cannot retrieve h_weight_rho histogram!" << std::endl;
        return;
    }
    h_weight_rho->SetDirectory(0); // Detach histogram from file to avoid auto-deletion
    rho_weightFile->Close();

  // Open the weight file and retrieve the h_weight_vz histogram
    TFile* vz_weightFile = TFile::Open("weights_MC/vz_weights_2/weight_vz.root", "READ");
    if (!vz_weightFile) {
        std::cerr << "Error: Cannot open weights_vz.root file!" << std::endl;
        return;
    }
    TH1D* h_weight_vz = (TH1D*)vz_weightFile->Get("h_weight_vz");
    if (!h_weight_vz) {
        std::cerr << "Error: Cannot retrieve h_weight_vz histogram!" << std::endl;
        return;
    }
    h_weight_vz->SetDirectory(0); // Detach histogram from file to avoid auto-deletion
    vz_weightFile->Close();

  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis"), hiFJRhoAnalyzerFinerBins("hiFJRhoAnalyzerFinerBins");

  glob_t globlist;

  // File with MinBias sample
  TFile *inFile_MinBias;

  if (isData) {
    glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test13_ZMM_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data.root");
  cout << "This is data" << endl;
  }
  else {
    glob("/eos/infnts/cms/store/user/kdeleo/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_DYto2Mu/250321_154613/0000/HiForestMiniAOD_MC_*.root", GLOB_NOSORT, NULL, &globlist);
    inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC.root");
    cout << "This is MC" << endl;
  }
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  if (!inFile_MinBias || inFile_MinBias->IsZombie()) {
        std::cerr << "Error: Could not open input file! Check path and file existence." << std::endl;
        return;
  }

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    //data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzerSubstructure/t");
    data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/muonAnalyzer/MuonTree");
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
    skimanalysis.Add(TString(globlist.gl_pathv[i]) + "/skimanalysis/HltTree");
    hiFJRhoAnalyzerFinerBins.Add(TString(globlist.gl_pathv[i]) + "/hiFJRhoAnalyzerFinerBins/t");
    //hltanalysis.Add(TString(globlist.gl_pathv[i]) + "/hltanalysis/HltTree");

  }
  globfree(&globlist);

  //To associate additional TTrees with a primary TTree. This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  data.AddFriend("HiTree");
  data.AddFriend("skimanalysis");
  data.AddFriend("hiFJRhoAnalyzerFinerBins");
  //data.AddFriend("hltanalysis");

  TTreeReader fReader(&data);

  // Declaration of leaf types
  TTreeReaderValue<Int_t> run = {fReader, "run"};    // Run number
  TTreeReaderValue<Int_t> evt = {fReader, "evt"};    // Event number
  TTreeReaderValue<Int_t> lumi = {fReader, "lumi"};  // Luminosity block
  TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"}; // centralityx2
  TTreeReaderValue<Float_t> weight = {fReader, isData ? "hiHF" : "weight"}; // MC event weight, not used in data
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderArray<double> rho = {fReader, "rho"};
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
  Files.push_back("ParallelMC_L2Relative_AK2PF_PbPb_Reco_v0_2_13_2024.txt");
  JetCorrector JEC(Files);

  // Gen jets
  TTreeReaderValue<Int_t> ngen = {fReader, isData ? "nref" : "ngen"};
  TTreeReaderArray<Float_t> genpt = {fReader, isData ? "rawpt" : "genpt"};
  TTreeReaderArray<Float_t> geneta = {fReader, isData ? "jteta" : "geneta"};
  TTreeReaderArray<Float_t> genphi = {fReader, isData ? "jtphi" : "genphi"};

  // Access MinBias sample
  TTree *inputTree = (TTree*)inFile_MinBias->Get("jet_tree");
    if (!inputTree) {
        std::cerr << "Error: Could not find TTree 'jet_tree' in the input file!" << std::endl;
        inFile_MinBias->Close();
        return;
  }

  // Declare variables to hold the branch data
  Float_t HF_MinBias;
  Int_t HF_bin_MinBias;
  Float_t jet_pt_MinBias;
  Float_t jet_phi_MinBias;
  Float_t jet_eta_MinBias;

  // Set branch addresses to link variables to tree branches
  inputTree->SetBranchAddress("HF_MinBias", &HF_MinBias);
  inputTree->SetBranchAddress("HF_bin_MinBias", &HF_bin_MinBias);
  inputTree->SetBranchAddress("jet_pt_MinBias", &jet_pt_MinBias);
  inputTree->SetBranchAddress("jet_phi_MinBias", &jet_phi_MinBias);
  inputTree->SetBranchAddress("jet_eta_MinBias", &jet_eta_MinBias);

  // --- Define HF bins ---
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

  //std::cout << "Defined HF bins (" << hf_bins.size() << " total):" << std::endl;
  //for (const auto& bin : hf_bins) {
  //    std::cout << "[" << bin.first << ", " << bin.second << ")" << std::endl;
  //}
  // --- End of HF bin definition ---

  // TTree entries
  Int_t nEntries = inputTree->GetEntries();
  std::cout << "Reading " << nEntries << " entries from 'jet_tree'..." << std::endl;
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
  TCanvas* c7 = new TCanvas("c7", "c7", 1200, 800);
  c7->Divide(1,1);

  //Histograms
  TH1F *h_mumu = new TH1F("h_mumu", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt = new TH1F("h_Z_pt", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1F *h_njet = new TH1F("h_njet", "Hist;Number of jets; Entries", 10, 0, 10);
  TH1F *h_cen = new TH1F("h_cen", "Hist; centrality bin; Entries", 20, 0, 100);

  TH1F *h_mumu_j = new TH1F("h_mumu_j", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt_j = new TH1F("h_Z_pt_j", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_cen_j = new TH1F("h_cen_j", "Hist; centrality bin; Entries", 20, 0, 100);
  TH1F *h_HF_j = new TH1F("h_HF_j", "Hist; HF; Entries", 80, 0, 8000);
  TH1F *h_deltaPhi_Zj = new TH1F("h_deltaPhi_Zj", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", 20, 0, 3);

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);
  TH1F *h_avg_rho = new TH1F("h_avg_rho", "Hist; <#rho>; Entries", 50, 0, 400);
  auto *h_avg_rho_vs_cen = new TProfile("h_avg_rho_vs_cen", "Profile of <#rho> vs centrality bin", 200, 0, 200, 0, 400);

  TH1F *h_deltaPhi_Zj_MinBias = new TH1F("h_deltaPhi_Zj_MinBias", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_jet_pt_lj_MinBias = new TH1F("h_jet_pt_lj_MinBias", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_xZj_MinBias = new TH1F("h_xZj_MinBias", "Hist;x_{Zj}; Entries", 20, 0, 3);

  TH1F *h_deltaPhi_Zj_matched = new TH1F("h_deltaPhi_Zj_matched", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_jet_pt_lj_matched = new TH1F("h_jet_pt_lj_matched", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_xZj_matched = new TH1F("h_xZj_matched", "Hist;x_{Zj}; Entries", 20, 0, 3);

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Output root file
  TFile *file_output_HI_mu;

  if (isData && weight_phase != 0) {
      file_output_HI_mu = new TFile("./plot/output_HI_mu_data.root", "RECREATE");
  }

  if (weight_phase == 0) {
    if (isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_HI_mu_data_Ncoll_weights.root", "RECREATE");
    }
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_HI_mu_MC_Ncoll_weights.root", "RECREATE");
    }
  }

  if (weight_phase == 1) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/rho_weights_1/output_HI_mu_MC_rho_weights.root", "RECREATE");
    }
  }

  else if (weight_phase == 2) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_HI_mu_MC_vz_weights.root", "RECREATE");
//      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_HI_mu_MC_rho_weights_after.root", "RECREATE");
    }
  }

  else if (weight_phase == 3) {
    if (!isData) {
      file_output_HI_mu = new TFile("./plot/output_HI_mu_MC.root", "RECREATE");
    }
  }

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;
    float weight_cent = Ncoll[*hiBin];
    // Scale MC
    float scale = 1;
    if (weight_phase == 0) {
      if (!isData) {
        scale*=(*weight);
      }
    }
    else if (weight_phase != 0) {
      if (!isData) {
        scale*=weight_cent*(*weight);
      }
    }
    // Selection on centrality bin
    if (weight_phase != 0 && weight_phase != 1) {
      if(*hiBin>59) continue;
    }
    if (weight_phase == 1) {
      if (isData) {
        if(*hiBin>59) continue;
      }
    }
    //if(*HLT_HIL2SingleMu7_v3<=0) continue;
    bool good_pair = false;
    if (*nReco < 2 ) continue;
    iEvent++;
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // TLorentzVectors for the muons
    TLorentzVector muPlus, muMinus;

    // Loop over muons, save indices of most energetic muon and antimuon pairs
    int iHighPtMu = -1;
    int iHighPtAntiMu = -1;
    //cout << "-----------------------------" << endl;
    for (unsigned int iMu = 0; iMu < *nReco; ++iMu) {
      if (recoIDTight[iMu]) {
        //cout << "iMu: " << iMu << " pt = " << recoPt[iMu] <<  " Q = " << recoCharge[iMu] << endl;
        if (iHighPtMu == -1 || recoPt[iMu] > recoPt[iHighPtMu]) {
          if (recoCharge[iMu] == -1) iHighPtMu = iMu;
        }
        if (iHighPtAntiMu == -1 || recoPt[iMu] > recoPt[iHighPtAntiMu]) {
          if (recoCharge[iMu] == +1) iHighPtAntiMu = iMu;
        }
      }
    }
    //cout << "iHighPtMu = " << iHighPtMu << " iHighPtAntiMu = " << iHighPtAntiMu << endl;

    // Z from muon-antimuon pairs
    if (iHighPtMu == -1 || iHighPtAntiMu == -1) continue;
    muMinus.SetPtEtaPhiM(recoPt[iHighPtMu], recoEta[iHighPtMu], recoPhi[iHighPtMu], muonMass);
    muPlus.SetPtEtaPhiM(recoPt[iHighPtAntiMu], recoEta[iHighPtAntiMu], recoPhi[iHighPtAntiMu], muonMass);
    double Z_mass = (muPlus + muMinus).M();
    double Z_pt = (muPlus + muMinus).Pt();
    double Z_phi = (muPlus + muMinus).Phi();
    // Apply Muon Scale Factors from TH2F histograms
    if (!isData) {
      // Apply TightID scale factors for both muons in the Z candidate
      double sf_id_mu_plus = h_tightID_sf->GetBinContent(h_tightID_sf->FindBin(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu]));
      double sf_id_mu_minus = h_tightID_sf->GetBinContent(h_tightID_sf->FindBin(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu]));
      scale *= sf_id_mu_plus * sf_id_mu_minus;
      // Apply HLT scale factor combining them as SF1 + SF2 - (SF1 * SF2)
      // For trigger we only need one to lepton to have fired, so we use the addition rule of probability
      double sf_hlt_mu_plus = h_hlt_sf->GetBinContent(h_hlt_sf->FindBin(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu]));
      double sf_hlt_mu_minus = h_hlt_sf->GetBinContent(h_hlt_sf->FindBin(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu]));
      scale *= sf_hlt_mu_plus + sf_hlt_mu_minus - (sf_hlt_mu_plus * sf_hlt_mu_minus);
    }

    // Apply mass cut
    if (Z_mass >= 60 && Z_mass <= 120) {
      if (recoPt[iHighPtMu] > 20 && abs(recoEta[iHighPtMu]) < 2.4 && recoPt[iHighPtAntiMu] > 20 && abs(recoEta[iHighPtAntiMu]) < 2.4){
        good_pair = true;
      }
    }
    if (!good_pair) continue;

    // Cut on pt(Z)
    if (Z_pt < 40 ) continue;

    // Calculate average rho
    double sum_rho = 0;
    for (unsigned int i = 0; i < rho.GetSize(); i++) {
      sum_rho += rho[i];
    }
    double avg_rho = 0;
    if (rho.GetSize() > 0) {
      avg_rho = sum_rho / rho.GetSize();
    }
    // use binning to get the value of the weight
    int bin_rho = h_weight_rho->FindBin(avg_rho);

    if (!isData) {
      if (weight_phase == 2) {
        // Apply rho weight
        scale*=h_weight_rho->GetBinContent(bin_rho);
      }
      if (weight_phase == 3) {
       // Apply rho and vz weight
       int bin_vz = h_weight_vz->FindBin(*vz);
       scale*=h_weight_rho->GetBinContent(bin_rho)*h_weight_vz->GetBinContent(bin_vz);
      }
    }

    h_vz->Fill(*vz, scale);
    h_avg_rho->Fill(avg_rho, scale);
    h_avg_rho_vs_cen->Fill(*hiBin, avg_rho, scale);
    h_cen->Fill((*hiBin)/2, scale);
    h_mumu->Fill(Z_mass, scale);
    h_Z_pt->Fill(Z_pt, scale);

    // Loop over Jets
    unsigned int njets = 0;
    double detaMinus = 0, dphiMinus = 0, dRMinus = 0;
    double detaPlus = 0, dphiPlus = 0, dRPlus = 0;
    int ijetLeading = -1;
    bool isLeadingJetMatched = false;
    double jtpt_corr[20000];
    for(int ijet=0; ijet<*nref; ijet++){
      //cout << "before JEC: " << rawpt[ijet] << endl;
      JEC.SetJetPT(rawpt[ijet]);
      JEC.SetJetEta(jteta[ijet]);
      JEC.SetJetPhi(jtphi[ijet]);
      double Correction = JEC.GetCorrection();
      double CorrectedPT = JEC.GetCorrectedPT();
      jtpt_corr[ijet] = CorrectedPT;
      //jtpt_corr[ijet] = rawpt[ijet];
      //cout << "after JEC: jtpt_corr = " << jtpt_corr[ijet] << " CorrectedPT = " << CorrectedPT << endl;

      if(jtpt_corr[ijet]<30) continue;
      if(abs(jteta[ijet])>2.5) continue;
      detaMinus = jteta[ijet] - muMinus.Eta();
      dphiMinus = RelativePhi(jtphi[ijet], muMinus.Phi());
      dRMinus = TMath::Sqrt(detaMinus * detaMinus + dphiMinus * dphiMinus);
      detaPlus = jteta[ijet] - muPlus.Eta();
      dphiPlus = RelativePhi(jtphi[ijet], muPlus.Phi());
      dRPlus = TMath::Sqrt(detaPlus * detaPlus + dphiPlus * dphiPlus);
      if(dRMinus < 0.2 || dRPlus < 0.2 ) continue;
      njets++;
      //cout << "-----------------------------" << endl;
      //cout << "ijet: " << ijet << " pt = " << jtpt_corr[ijet] << " eta = " << jteta[ijet] << " phi = " << jtphi[ijet] << " m = " << jtm[ijet] << endl;
      //cout << "muMinus pt = " << muMinus.Pt() << " eta = " << muMinus.Eta() << " phi = " << muMinus.Phi() << endl;
      //cout << "muPlus pt = " << muPlus.Pt() << " eta = " << muPlus.Eta() << " phi = " << muPlus.Phi() << endl;
      //cout << "dRMinus = " << dRMinus << " dRPlus = " << dRPlus << endl;

      // --- Matching Logic for CURRENT Reco Jet (ijet) ---
      double min_dR = 9999.0;
      int matched_gen_jet_idx = -1;

      // Loop over Gen Jets to find the best match for the current reco jet
      if (!isData) {
        for (int igenjet = 0; igenjet < *ngen; igenjet++) {
          // gen jet cuts looser than reco jet cuts
          if(genpt[igenjet]<20.) continue;
          if(abs(geneta[igenjet])>3.) continue;

          double deta_gen = jteta[ijet] - geneta[igenjet];
          double dphi_gen = RelativePhi(jtphi[ijet], genphi[igenjet]);
          double dR_gen = TMath::Sqrt(deta_gen * deta_gen + dphi_gen * dphi_gen);

          // min_dR tracks the smallest dR found for the current reco jet
          // matched_gen_jet_idx stores the index of the gen jet that yielded the min_dR.
          if (dR_gen < min_dR) {
            min_dR = dR_gen;
            matched_gen_jet_idx = igenjet;
          }
        }
      }

      if (ijetLeading == -1 || jtpt_corr[ijet] > jtpt_corr[ijetLeading]) {
          ijetLeading = ijet;
          // Check if a match was found within a reasonable dR cone
          if (!isData) {
            if (min_dR < 0.1) {
              isLeadingJetMatched = true;
            }
            else {isLeadingJetMatched = false;}
          }
      }
    } //end loop over jets
    //cout << "ijetLeading = " << ijetLeading << endl;

    if (ijetLeading != -1) {
        double dPhi_Zj = RelativePhi(Z_phi, jtphi[ijetLeading]);
        h_deltaPhi_Zj->Fill(dPhi_Zj, scale);

        // Check the current bin of HF
        float current_hiHF_val = *hiHF;
        int bin_n = 0;
        int current_bin_n = 0;
        for (const auto& bin_range : hf_bins) {
          if (current_hiHF_val >= bin_range.first && current_hiHF_val < bin_range.second) {
           current_bin_n = bin_n;
           break;
           }
           bin_n++;
        }

        // Loop over the TTree entries for mixing events with MinBias
        double check_ev_per_bin = 0; //check if each bin of HF was filled with 100MinBias events
        for(int iEntry=0; iEntry< nEntries; iEntry++){
          inputTree->GetEntry(iEntry); // Read all branch values for the current entry
          if (current_bin_n == HF_bin_MinBias) {
            if (jet_pt_MinBias >= 30 && abs(jet_eta_MinBias) <= 2.5) {
              double detaMinus_MinBias = jet_eta_MinBias - muMinus.Eta();
              double dphiMinus_MinBias = RelativePhi(jet_phi_MinBias, muMinus.Phi());
              double dRMinus_MinBias = TMath::Sqrt(detaMinus_MinBias * detaMinus_MinBias + dphiMinus_MinBias * dphiMinus_MinBias);
              double detaPlus_MinBias = jet_eta_MinBias - muPlus.Eta();
              double dphiPlus_MinBias = RelativePhi(jet_phi_MinBias,muPlus.Phi());
              double dRPlus_MinBias = TMath::Sqrt(detaPlus_MinBias * detaPlus_MinBias + dphiPlus_MinBias * dphiPlus_MinBias);
              if (dRMinus_MinBias >= 0.2 && dRPlus_MinBias >= 0.2 ) {
                double dPhi_Zj_MinBias = RelativePhi(Z_phi, jet_phi_MinBias);
                h_deltaPhi_Zj_MinBias->Fill(dPhi_Zj_MinBias, scale);
//              std::cout << "current_bin_n: " << current_bin_n << "  current_hiHF_val: " << current_hiHF_val <<
//                           "  jet_pt_MinBias: " << jet_pt_MinBias << "  HF_MinBias: " << HF_MinBias << std::endl;
                if (dPhi_Zj_MinBias> 7 * TMath::Pi() / 8) {
                  h_jet_pt_lj_MinBias->Fill(jet_pt_MinBias, scale);
                  h_xZj_MinBias->Fill(jet_pt_MinBias/Z_pt, scale);
                }
              }
            }
            check_ev_per_bin++;
          }
        }
        if (check_ev_per_bin != events_per_bin) std::cout << "--- Warning! There are only " << check_ev_per_bin
                                                          << " events per bin ( < " << events_per_bin << " )"
                                                          << " current_hiHF_val = " << current_hiHF_val << std::endl;

        if (!isData) {
            if (isLeadingJetMatched) {
              double dPhi_Zj_matched = RelativePhi(Z_phi, jtphi[ijetLeading]);
              h_deltaPhi_Zj_matched->Fill(dPhi_Zj_matched, scale);
          }
        }

        if (dPhi_Zj > 7 * TMath::Pi() / 8) {
          h_njet->Fill(njets, scale);
          h_mumu_j->Fill(Z_mass, scale);
          h_Z_pt_j->Fill(Z_pt, scale);
          h_jet_pt_lj->Fill(jtpt_corr[ijetLeading], scale);
          h_xZj->Fill(jtpt_corr[ijetLeading]/Z_pt, scale);
          h_cen_j->Fill((*hiBin)/2, scale);
          h_HF_j->Fill(*hiHF, scale);

          if (!isData) {
            if (isLeadingJetMatched) {
              h_jet_pt_lj_matched->Fill(jtpt_corr[ijetLeading], scale);
              h_xZj_matched->Fill(jtpt_corr[ijetLeading]/Z_pt, scale);
            }
          }

          //h_jetgirth->Fill(jtgirth[ijetLeading], scale);
          //h_jet_deltaR->Fill(jtdyndeltaR[ijetLeading], scale);
        }
    }
  }  // end loop events

  // Finalize histograms for Mixed event subtraction
  h_deltaPhi_Zj_MinBias->Scale(1./BinningConfig::ev_per_bin);
  TH1F* h_deltaPhi_Zj_subtracted = (TH1F*)h_deltaPhi_Zj->Clone("h_deltaPhi_Zj_subtracted");
  h_deltaPhi_Zj_subtracted->SetDirectory(0);
  h_deltaPhi_Zj_subtracted->SetTitle("h_deltaPhi_Zj - h_deltaPhi_Zj_MinBias (rescaled)");
  h_deltaPhi_Zj_subtracted->Add(h_deltaPhi_Zj_MinBias, -1); // The -1 performs the subtraction

  h_jet_pt_lj_MinBias->Scale(1./BinningConfig::ev_per_bin);
  TH1F* h_jet_pt_lj_subtracted = (TH1F*)h_jet_pt_lj->Clone("h_jet_pt_lj_subtracted");
  h_jet_pt_lj_subtracted->SetDirectory(0);
  h_jet_pt_lj_subtracted->SetTitle("h_jet_pt_lj - h_jet_pt_lj_MinBias (rescaled)");
  h_jet_pt_lj_subtracted->Add(h_jet_pt_lj_MinBias, -1); // The -1 performs the subtraction
  cout << "Bkg: " << h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1) << " fraction: "
       << h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1)/h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << endl;
  if (!isData) {
    cout << "Raw - True: " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) - h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1)
         << " fraction: " << (h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) - h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1))/h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
         << endl;
  }
  h_xZj_MinBias->Scale(1./BinningConfig::ev_per_bin);
  TH1F* h_xZj_subtracted = (TH1F*)h_xZj->Clone("h_xZj_subtracted");
  h_xZj_subtracted->SetDirectory(0);
  h_xZj_subtracted->SetTitle("h_xZj - h_xZj (rescaled)");
  h_xZj_subtracted->Add(h_xZj_MinBias, -1); // The -1 performs the subtraction

  cout << "Number of events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << ", if Z_pt>80: " << h_Z_pt_j->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << endl;

  c1->cd(1);
  h_mumu->Draw();
  c2->cd(1);
  h_Z_pt->Draw();
  c3->cd(1);
  //h_njet->Draw();
  //h_avg_rho_vs_cen->Draw();
  h_HF_j->Draw();
  c4->cd(1);
  h_jet_pt_lj->Draw();
  c5->cd(1);
  h_cen->Draw();
  c6->cd(1);
  h_vz->Draw();
  c7->cd(1);
  h_avg_rho->Draw();
  //c1->SaveAs("h_mumu_j.pdf");

  // Create the main directory "MC" or "DATA"
  TDirectory *Dir = file_output_HI_mu->mkdir("HI");
  // Navigate to the directory
  Dir->cd();
  // Create a new directory named "Muons"
  TDirectory *muonsDir = Dir->mkdir("Muons");
  // Navigate to the "MUONS" directory
  muonsDir->cd();
  h_mumu->Write();
  h_Z_pt->Write();
  h_njet->Write();
  h_cen->Write();
  h_mumu_j->Write();
  h_Z_pt_j->Write();
  h_jet_pt_lj->Write();
  h_jet_pt_lj_MinBias->Write();
  h_jet_pt_lj_subtracted->Write();
  h_jet_pt_lj_matched->Write();
  h_cen_j->Write();
  h_deltaPhi_Zj->Write();
  h_deltaPhi_Zj_MinBias->Write();
  h_deltaPhi_Zj_subtracted->Write();
  h_deltaPhi_Zj_matched->Write();
  h_xZj->Write();
  h_xZj_MinBias->Write();
  h_xZj_subtracted->Write();
  h_xZj_matched->Write();
  h_vz->Write();
  h_avg_rho->Write();
  //h_jetgirth->Write();
  //h_jet_deltaR->Write();

  file_output_HI_mu->Close();
}
