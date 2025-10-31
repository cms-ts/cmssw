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

#include <fstream>      // For std::ifstream
#include <map>          // For std::map
#include <stdexcept>    // For std::runtime_error
#include <limits>       // For std::numeric_limits
#include <nlohmann/json.hpp> // For JSON parsing
#include "CorrectionSF.h"
// Use the nlohmann::json namespace
using json = nlohmann::json;

using namespace std;

double RelativePhi(double phi_1,double phi_2) {
  double d_phi =  abs(phi_1 - phi_2);
  if (d_phi > acos(-1)) d_phi = 2*acos(-1) - d_phi;
  return d_phi;
}

//To run, root -l analyze_HI_TTreeReader_ZMM.C
//Default isData, for MC root -l 'analyze_HI_TTreeReader_ZMM.C(false, 3)'
void analyze_HI_TTreeReader_ZMM(bool isData = true, unsigned int weight_phase = 1) {

  // --- Load Muon Scale Factors from JSON ---
  std::cout << "Loading Muon Scale Factors from JSON..." << std::endl;
  CorrectionSF tightID_SF;
  CorrectionSF hlt_SF;
  
  std::string json_filename = "HLT_HIL2SingleMu7_and_TightID_abseta1_pt1_cutAndCount_schemaV2.json";
  std::ifstream json_file_stream(json_filename);
  if (!json_file_stream.is_open()) {
      std::cerr << "Error: Cannot open JSON file: " << json_filename << std::endl;
      return;
  }

  json sf_data;
  try {
      sf_data = json::parse(json_file_stream);
  } catch (json::parse_error& e) {
      std::cerr << "[Error] Failed to parse JSON file: " << json_filename << std::endl;
      std::cerr << e.what() << std::endl;
      return;
  }

  bool tightID_loaded = false;
  bool hlt_loaded = false;

  if (sf_data.contains("corrections") && sf_data["corrections"].is_array()) {
      for (const auto& corr : sf_data["corrections"]) {
          std::string name = corr["name"];
          if (name == "NUM_TightID_DEN_genTracks") {
              tightID_SF.load(corr["data"]);
              tightID_loaded = true;
              std::cout << "  Loaded 'NUM_TightID_DEN_genTracks'" << std::endl;
          } else if (name == "NUM_HLT_HIL2SingleMu7_v_DEN_TightID") {
              hlt_SF.load(corr["data"]);
              hlt_loaded = true;
              std::cout << "  Loaded 'NUM_HLT_HIL2SingleMu7_v_DEN_TightID'" << std::endl;
          }
      }
  }

  if (!tightID_loaded || !hlt_loaded) {
      std::cerr << "Error: Failed to load required corrections from JSON." << std::endl;
      if (!tightID_loaded) std::cerr << "  'NUM_TightID_DEN_genTracks' was not found." << std::endl;
      if (!hlt_loaded) std::cerr << "  'NUM_HLT_HIL2SingleMu7_v_DEN_TightID' was not found." << std::endl;
      return;
  }
  std::cout << "JSON Scale Factors loaded successfully." << std::endl;
  // --- End Load Muon Scale Factors from JSON ---

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


  // Binning_option for mixed event background subtraction, Use HF binning as default
  // 0: HF binning only
  // 1: VZ binning only
  // 2: VZ + Centrality binning
  int binning_option = 2;

  // File with MinBias sample
  TFile *inFile_MinBias;

  if (isData) {
    glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test13_ZMM_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_HF.root");
    else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_VZ.root");
    else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_VZ_Cen_Combined.root");
    else { cerr << "Invalid binning_option for data MinBias file." << endl; return; }
    cout << "This is data" << endl;
  }
  else {
    glob("/eos/infnts/cms/store/user/kdeleo/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test17_ZMM_DYto2Mu/250625_144848/0000/HiForestMiniAOD_MC_*.root", GLOB_NOSORT, NULL, &globlist);
    if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_HF.root");
    else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_VZ.root");
    else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_VZ_Cen_Combined.root");
    else { cerr << "Invalid binning_option for MC MinBias file." << endl; return; }
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

  // Gen Muon
  TTreeReaderValue<Int_t> ngenMu = {fReader, isData ? "nReco" : "nGen"};
  TTreeReaderArray<Float_t> genMuPt = {fReader, isData ? "recoPt" : "genPt"};
  TTreeReaderArray<Float_t> genMuEta = {fReader, isData ? "recoEta" : "genEta"};
  TTreeReaderArray<Float_t> genMuPhi = {fReader, isData ? "recoPhi" : "genPhi"};
  TTreeReaderArray<Int_t> genMuPID = {fReader, isData ? "recoCharge" : "genPID"};

  // Access MinBias sample
  TTree *inputTree = (TTree*)inFile_MinBias->Get("jet_tree");
    if (!inputTree) {
        std::cerr << "Error: Could not find TTree 'jet_tree' in the input file!" << std::endl;
        inFile_MinBias->Close();
        return;
  }

  // Declare variables to hold the branch data
  Float_t HF_MinBias;
  Float_t vz_MinBias;
  Int_t hiBin_MinBias;
  Int_t bin_MinBias; // Global bin number in MinBias tree
  Float_t jet_pt_MinBias;
  Float_t jet_phi_MinBias;
  Float_t jet_eta_MinBias;

  // Set branch addresses to link variables to tree branches
  inputTree->SetBranchAddress("HF_MinBias", &HF_MinBias);
  inputTree->SetBranchAddress("vz_MinBias", &vz_MinBias);
  inputTree->SetBranchAddress("hiBin_MinBias", &hiBin_MinBias); // Link new branch
  inputTree->SetBranchAddress("bin_MinBias", &bin_MinBias);
  inputTree->SetBranchAddress("jet_pt_MinBias", &jet_pt_MinBias);
  inputTree->SetBranchAddress("jet_phi_MinBias", &jet_phi_MinBias);
  inputTree->SetBranchAddress("jet_eta_MinBias", &jet_eta_MinBias);

  // --- Bin definition for MinBias matching (based on selected option) ---
  std::vector<std::pair<float, float>> primary_bins; // For HF or VZ only
  std::vector<std::pair<float, float>> vz_bins_combined;     // For combined VZ+Centrality
  std::vector<std::pair<float, float>> centrality_bins_combined; // For combined VZ+Centrality

  int total_mixed_bins_expected = 0; // Total expected bins for the selected option in MinBias
  int events_per_mixed_bin_limit = 0; // The max events collected per bin in MinBias

  if (binning_option == 0) { // HF binning
      total_mixed_bins_expected = BinningConfig::tot_bins;
      events_per_mixed_bin_limit = BinningConfig::ev_per_bin;
      float current_min = BinningConfig::frst_bin_min;
      for (int i = 0; i < total_mixed_bins_expected; ++i) {
          float current_max = (current_min * 1.1);
          primary_bins.push_back({current_min, current_max});
          current_min = current_max;
      }
      std::cout << "Defined HF bins for MinBias matching (" << primary_bins.size() << " total)." << std::endl;
  } else if (binning_option == 1) { // VZ binning only
      total_mixed_bins_expected = BinningConfig_vz::tot_bins;
      events_per_mixed_bin_limit = BinningConfig_vz::ev_per_bin;
      float current_min = BinningConfig_vz::frst_bin_min;
      for (int i = 0; i < total_mixed_bins_expected; ++i) {
          float current_max = (current_min + 10.0f); // 10cm wide vz bins
          primary_bins.push_back({current_min, current_max});
          current_min = current_max;
      }
      std::cout << "Defined VZ bins for MinBias matching (" << primary_bins.size() << " total)." << std::endl;
  } else if (binning_option == 2) { // VZ + Centrality binning
      // Centrality bins based on hiBin
      float cen_bin_width_hiBin = (BinningConfig_Combined_Vz_Centrality::centrality_max_hiBin - BinningConfig_Combined_Vz_Centrality::centrality_min_hiBin) / BinningConfig_Combined_Vz_Centrality::num_centrality_bins;
      for (int i = 0; i < BinningConfig_Combined_Vz_Centrality::num_centrality_bins; ++i) {
          float min_hiBin = BinningConfig_Combined_Vz_Centrality::centrality_min_hiBin + i * cen_bin_width_hiBin;
          float max_hiBin = min_hiBin + cen_bin_width_hiBin;
          centrality_bins_combined.push_back({min_hiBin, max_hiBin});
      }

      // Vz bins based on explicit edges
      for (int i = 0; i < BinningConfig_Combined_Vz_Centrality::num_vz_bins; ++i) {
          vz_bins_combined.push_back({BinningConfig_Combined_Vz_Centrality::vz_bin_edges[i], BinningConfig_Combined_Vz_Centrality::vz_bin_edges[i+1]});
      }

      total_mixed_bins_expected = BinningConfig_Combined_Vz_Centrality::num_centrality_bins * BinningConfig_Combined_Vz_Centrality::num_vz_bins;
      events_per_mixed_bin_limit = BinningConfig_Combined_Vz_Centrality::ev_per_combined_bin;
      std::cout << "Defined combined Centrality-VZ bins for MinBias matching (" << total_mixed_bins_expected << " total)." << std::endl;
  } else {
      std::cerr << "Invalid binning_option: " << binning_option << std::endl;
      return;
  }
  // --- End of bin definition ---

  // TTree entries
  Int_t nEntries_MinBias = inputTree->GetEntries();
  std::cout << "Reading " << nEntries_MinBias << " entries from 'jet_tree'..." << std::endl;
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

  // Define binning for xZj unfolding.
  const int nbins_xZj = 5; // Number of bins (number of edges - 1)
  const int nbins_xZj_meas = nbins_xZj-1;
  double xZj_bins[nbins_xZj + 1] = {0., 0.4, 0.8, 1.2, 1.6, 2.};
  double xZj_bins_meas[nbins_xZj_meas + 1] = {0., 0.4, 0.8, 1.2, 1.6};

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
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);
  TH1F *h_avg_rho = new TH1F("h_avg_rho", "Hist; <#rho>; Entries", 50, 0, 400);
  auto *h_avg_rho_vs_cen = new TProfile("h_avg_rho_vs_cen", "Profile of <#rho> vs centrality bin", 200, 0, 200, 0, 400);

  TH1F *h_deltaPhi_Zj_MinBias = new TH1F("h_deltaPhi_Zj_MinBias", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_jet_pt_lj_MinBias = new TH1F("h_jet_pt_lj_MinBias", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_xZj_MinBias = new TH1F("h_xZj_MinBias", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  TH1F *h_deltaPhi_Zj_matched = new TH1F("h_deltaPhi_Zj_matched", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_jet_pt_lj_matched = new TH1F("h_jet_pt_lj_matched", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_xZj_matched = new TH1F("h_xZj_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  // --- RooUnfold Histograms ---

  TH1F* h_xZj_true = new TH1F("h_xZj_true", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);     // For true MC
  TH1F* h_xZj_reco = new TH1F("h_xZj_reco", "Reco x_{Zj};x_{Zj};Entries", nbins_xZj_meas, xZj_bins_meas);
  TH2F* h_response = new TH2F("h_response", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias = new TH2F("h_response_MinBias", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  TH1F *h_xZj_train_closure = new TH1F("h_xZj_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_train_closure_matched = new TH1F("h_xZj_train_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_test_closure = new TH1F("h_xZj_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_test_closure_matched = new TH1F("h_xZj_test_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_MinBias_train_closure = new TH1F("h_xZj_MinBias_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_MinBias_test_closure = new TH1F("h_xZj_MinBias_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F* h_xZj_true_train_closure = new TH1F("h_xZj_true_train_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH1F* h_xZj_true_test_closure = new TH1F("h_xZj_true_test_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH2F* h_response_closure = new TH2F("h_response_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias_closure = new TH2F("h_response_MinBias_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  // --- End RooUnfold Histograms ---

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
  unsigned int itotev = 0;
  while (fReader.Next()) {
    itotev++;
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

    // --- Fill information for unfolding ---
    double gen_Z_pt = 0;
    double gen_Z_phi = 0;
    double dPhi_Zj_Gen = 0;
    double true_xZj = 0;
    int ijetGenLeading_unfold = -1;
    if (!isData) {
      // TLorentzVectors for the gen muons
      TLorentzVector genmuPlus, genmuMinus;
      // Loop over muons, save indices of most energetic muon and antimuon pairs
      int iHighPtgenMu = -1;
      int iHighPtgenAntiMu = -1;
      for (unsigned int igenMu = 0; igenMu < *ngenMu; ++igenMu) {
        if (iHighPtgenMu == -1 || genMuPt[igenMu] > genMuPt[iHighPtgenMu]) {
          if (genMuPID[igenMu] == 13) iHighPtgenMu = igenMu;
        }
        if (iHighPtgenAntiMu == -1 || genMuPt[igenMu] > genMuPt[iHighPtgenAntiMu]) {
          if (genMuPID[igenMu] == -13) iHighPtgenAntiMu = igenMu;
        }
      }
      // Z from gen muon-antimuon pairs
      if (iHighPtgenMu != -1 && iHighPtgenAntiMu != -1) {
        genmuMinus.SetPtEtaPhiM(genMuPt[iHighPtgenMu], genMuEta[iHighPtgenMu], genMuPhi[iHighPtgenMu], muonMass);
        genmuPlus.SetPtEtaPhiM(genMuPt[iHighPtgenAntiMu], genMuEta[iHighPtgenAntiMu], genMuPhi[iHighPtgenAntiMu], muonMass);
        double gen_Z_mass = (genmuPlus + genmuMinus).M();
        gen_Z_pt = (genmuPlus + genmuMinus).Pt();
        gen_Z_phi = (genmuPlus + genmuMinus).Phi();
        // Apply mass cut
        if (gen_Z_mass >= 60 && gen_Z_mass <= 120) {
          if (genMuPt[iHighPtgenMu] > 20 && abs(genMuEta[iHighPtgenMu]) < 2.4 && genMuPt[iHighPtgenAntiMu] > 20 && abs(genMuEta[iHighPtgenAntiMu]) < 2.4) {
            // Cut on pt(Z)
            if (gen_Z_pt > 40 ) {
              // Loop over gen jets
              for (int ijetGen = 0; ijetGen < *ngen; ++ijetGen) {
                // Apply truth-level cuts
                if (genpt[ijetGen] < 30 || abs(geneta[ijetGen]) > 2.5) continue;
                double detaMinusGen = geneta[ijetGen] - genmuMinus.Eta();
                double dphiMinusGen = RelativePhi(genphi[ijetGen], genmuMinus.Phi());
                double dRMinusGen = TMath::Sqrt(detaMinusGen * detaMinusGen + dphiMinusGen * dphiMinusGen);
                double detaPlusGen = geneta[ijetGen] - genmuPlus.Eta();
                double dphiPlusGen = RelativePhi(genphi[ijetGen], genmuPlus.Phi());
                double dRPlusGen = TMath::Sqrt(detaPlusGen * detaPlusGen + dphiPlusGen * dphiPlusGen);
                if(dRMinusGen < 0.2 || dRPlusGen < 0.2 ) continue;
                if (ijetGenLeading_unfold == -1 || genpt[ijetGen] > genpt[ijetGenLeading_unfold]) {
                  ijetGenLeading_unfold = ijetGen;
                }
              }
              if (ijetGenLeading_unfold != -1) {
                dPhi_Zj_Gen = RelativePhi(gen_Z_phi, genphi[ijetGenLeading_unfold]);
                if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                  // Calculate true x_Zj
                  true_xZj = genpt[ijetGenLeading_unfold] / gen_Z_pt;
                  // Scale MC to data for check unfolding dependance on shape
                  //if (true_xZj>0.4 && true_xZj<0.8) scale*=2;
                  //if (true_xZj>0.8 && true_xZj<1.2) scale*=0.5;
                  h_xZj_true->Fill(true_xZj, scale);
                  if (itotev < 0.7*9560121) h_xZj_true_train_closure->Fill(true_xZj, scale);
                  else h_xZj_true_test_closure->Fill(true_xZj, scale);
                }
              }
            }
          }
        }
      }
    }
    // --- End fill information for unfolding ---

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
      double sf_id_mu_plus = tightID_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], "nominal");
      double sf_id_mu_minus = tightID_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], "nominal");
      scale *= sf_id_mu_plus * sf_id_mu_minus;
      // Apply HLT scale factor combining them as SF1 + SF2 - (SF1 * SF2)
      // For trigger we only need one to lepton to have fired, so we use the addition rule of probability
      double sf_hlt_mu_plus = hlt_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], "nominal");
      double sf_hlt_mu_minus = hlt_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], "nominal");
      scale *= (sf_hlt_mu_plus + sf_hlt_mu_minus - (sf_hlt_mu_plus * sf_hlt_mu_minus));
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
    int iGenjetMatchedtoLeadingReco = -1;
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
              iGenjetMatchedtoLeadingReco = matched_gen_jet_idx;
            }
            else {
              isLeadingJetMatched = false;
              iGenjetMatchedtoLeadingReco = -1; // Reset if no match
            }
          }
      }
    } //end loop over jets
    //cout << "ijetLeading = " << ijetLeading << endl;

    if (ijetLeading != -1) {
        double dPhi_Zj = RelativePhi(Z_phi, jtphi[ijetLeading]);
        h_deltaPhi_Zj->Fill(dPhi_Zj, scale);

        // --- Determine the current bin number for MinBias matching ---
        int current_global_bin_n = -1;
        
        if (binning_option == 0) { // HF binning
            float current_val = *hiHF;
            int bin_n = 0;
            for (const auto& bin_range : primary_bins) {
                if (current_val >= bin_range.first && current_val < bin_range.second) {
                    current_global_bin_n = bin_n;
                    break;
                }
                bin_n++;
            }
        } else if (binning_option == 1) { // VZ binning only
            float current_val = *vz;
            int bin_n = 0;
            for (const auto& bin_range : primary_bins) {
                if (current_val >= bin_range.first && current_val < bin_range.second) {
                    current_global_bin_n = bin_n;
                    break;
                }
                bin_n++;
            }
        } else if (binning_option == 2) { // VZ + Centrality binning
            int cen_bin_idx = -1;
            int vz_bin_idx = -1;

            for (size_t c_bin_n = 0; c_bin_n < centrality_bins_combined.size(); ++c_bin_n) {
                const auto& bin_range = centrality_bins_combined[c_bin_n];
                if (*hiBin >= bin_range.first && *hiBin < bin_range.second) {
                    cen_bin_idx = c_bin_n;
                    break;
                }
            }

            for (size_t v_bin_n = 0; v_bin_n < vz_bins_combined.size(); ++v_bin_n) {
                const auto& bin_range = vz_bins_combined[v_bin_n];
                if (*vz >= bin_range.first && *vz < bin_range.second) {
                    vz_bin_idx = v_bin_n;
                    break;
                }
            }
            if (cen_bin_idx != -1 && vz_bin_idx != -1) {
                current_global_bin_n = cen_bin_idx * BinningConfig_Combined_Vz_Centrality::num_vz_bins + vz_bin_idx;
            } else {
                // This event doesn't fall into a defined combined bin, skip background subtraction for it
                current_global_bin_n = -1; 
            }
        }
        // --- End of bin determination ---

        if (current_global_bin_n != -1) { // Only proceed with MinBias matching if a valid bin was found
            // Loop over the TTree entries for mixing events with MinBias
            // This assumes the MinBias tree 'bin_MinBias' corresponds to 'current_global_bin_n'
            // and contains 'events_per_mixed_bin_limit' events for each of these bins.
            double events_filled_for_this_bin_in_MinBias = 0; 
            for(int iEntry=0; iEntry< nEntries_MinBias; iEntry++){
                inputTree->GetEntry(iEntry); // Read all branch values for the current entry
                if (current_global_bin_n == bin_MinBias) { // Match by global bin number
                    // Apply same jet cuts as for signal jets
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
                            if (dPhi_Zj_MinBias > 7 * TMath::Pi() / 8) {
                                h_jet_pt_lj_MinBias->Fill(jet_pt_MinBias, scale);
                                h_xZj_MinBias->Fill(jet_pt_MinBias/Z_pt, scale);
                                if (itotev < 0.7*9560121) h_xZj_MinBias_train_closure->Fill(jet_pt_MinBias/Z_pt, scale);
                                else h_xZj_MinBias_test_closure->Fill(jet_pt_MinBias/Z_pt, scale);
                                if (!isData) {
                                  if (ijetGenLeading_unfold != -1) {
                                    if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                                      if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
                                        h_response_MinBias->Fill(jet_pt_MinBias/Z_pt, true_xZj, scale);
                                        if (itotev < 0.7*9560121) h_response_MinBias_closure->Fill(jet_pt_MinBias/Z_pt, true_xZj, scale);
                                      }
                                    }
                                  }
                                }
                            }
                        }
                    }
                    events_filled_for_this_bin_in_MinBias++;
                }
            }
            if (events_filled_for_this_bin_in_MinBias != events_per_mixed_bin_limit) {
                std::cout << "--- Warning! MinBias bin " << current_global_bin_n << " has only " << events_filled_for_this_bin_in_MinBias
                          << " events (expected " << events_per_mixed_bin_limit << "). ---" << std::endl;
            }
        }

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
          if (itotev < 0.7*9560121) h_xZj_train_closure->Fill(jtpt_corr[ijetLeading]/Z_pt, scale);
          else h_xZj_test_closure->Fill(jtpt_corr[ijetLeading]/Z_pt, scale);
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
    }  // end reco leading jet selection

    // --- Fill information for unfolding ---
    if (ijetLeading != -1 && RelativePhi(Z_phi, jtphi[ijetLeading]) > 7 * TMath::Pi() / 8) {
      if (!isData) {
        if (ijetGenLeading_unfold != -1) {
          if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
            // Calculate true x_Zj
            // Check if the leading gen jet is matched to leading reconstructed jet
            if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
              h_response->Fill(jtpt_corr[ijetLeading] / Z_pt, true_xZj, scale);
              h_xZj_reco->Fill(jtpt_corr[ijetLeading] / Z_pt, scale);
              if (itotev < 0.7*9560121) {
                h_response_closure->Fill(jtpt_corr[ijetLeading] / Z_pt, true_xZj, scale);
                h_xZj_train_closure_matched->Fill(jtpt_corr[ijetLeading] / Z_pt, scale);
              }
              else h_xZj_test_closure_matched->Fill(jtpt_corr[ijetLeading] / Z_pt, scale);
            }
          }
        }
      }
    }
    // --- end filling information for unfolding ---

  }  // end loop events

  // Finalize histograms for Mixed event subtraction
  // Scale by the number of events per bin that were collected in the MinBias file
  int scale_binning = 1;
  if (binning_option == 0) {scale_binning = BinningConfig::ev_per_bin; cout << "HF matching for mixed event bkg subtraction" << endl;}
  else if (binning_option == 1) {scale_binning = BinningConfig_vz::ev_per_bin; cout << "vz matching for mixed event bkg subtraction" << endl;}
  else if (binning_option == 2) {scale_binning = BinningConfig_Combined_Vz_Centrality::ev_per_combined_bin; cout << "Combined vz + Centrality matching for mixed event bkg subtraction" << endl;}

  h_deltaPhi_Zj_MinBias->Scale(1. / scale_binning);

  TH1F* h_deltaPhi_Zj_subtracted = (TH1F*)h_deltaPhi_Zj->Clone("h_deltaPhi_Zj_subtracted");
  h_deltaPhi_Zj_subtracted->SetDirectory(0);
  h_deltaPhi_Zj_subtracted->SetTitle("h_deltaPhi_Zj - h_deltaPhi_Zj_MinBias (rescaled)");
  h_deltaPhi_Zj_subtracted->Add(h_deltaPhi_Zj_MinBias, -1); // The -1 performs the subtraction

  h_jet_pt_lj_MinBias->Scale(1. / scale_binning);

  TH1F* h_jet_pt_lj_subtracted = (TH1F*)h_jet_pt_lj->Clone("h_jet_pt_lj_subtracted");
  h_jet_pt_lj_subtracted->SetDirectory(0);
  h_jet_pt_lj_subtracted->SetTitle("h_jet_pt_lj - h_jet_pt_lj_MinBias (rescaled)");
  h_jet_pt_lj_subtracted->Add(h_jet_pt_lj_MinBias, -1); // The -1 performs the subtraction

  cout << "Bkg Integral (dPhi): " << h_deltaPhi_Zj_MinBias->Integral(0, h_deltaPhi_Zj_MinBias->GetNbinsX()+1) << endl;
  cout << "Bkg Integral (pT): " << h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1) << " fraction: "
       << h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1)/h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << endl;

  if (!isData) {
    cout << "Raw - True (pT): " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) - h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1)
         << " fraction: " << (h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) - h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1))/h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
         << endl;
  }

  h_xZj_MinBias->Scale(1. / scale_binning);

  TH1F* h_xZj_subtracted = (TH1F*)h_xZj->Clone("h_xZj_subtracted");
  h_xZj_subtracted->SetDirectory(0);
  h_xZj_subtracted->SetTitle("h_xZj - h_xZj_MinBias (rescaled)");
  h_xZj_subtracted->Add(h_xZj_MinBias, -1); // The -1 performs the subtraction

  h_response_MinBias->Scale(1. / scale_binning);

  TH2F* h_response_subtracted = (TH2F*)h_response->Clone("h_response_subtracted");
  h_response_subtracted->SetDirectory(0);
  h_response_subtracted->SetTitle("h_response - h_response_MinBias (rescaled)");
  h_response_subtracted->Add(h_response_MinBias, -1); // The -1 performs the subtraction

  h_xZj_MinBias_train_closure->Scale(1. / scale_binning);
  h_xZj_MinBias_test_closure->Scale(1. / scale_binning);

  TH1F* h_xZj_subtracted_train_closure = (TH1F*)h_xZj_train_closure->Clone("h_xZj_subtracted_train_closure");
  h_xZj_subtracted_train_closure->SetDirectory(0);
  h_xZj_subtracted_train_closure->SetTitle("h_xZj_train_closure - h_xZj_MinBias_train_closure (rescaled)");
  h_xZj_subtracted_train_closure->Add(h_xZj_MinBias_train_closure, -1); // The -1 performs the subtraction

  TH1F* h_xZj_subtracted_test_closure = (TH1F*)h_xZj_test_closure->Clone("h_xZj_subtracted_test_closure");
  h_xZj_subtracted_test_closure->SetDirectory(0);
  h_xZj_subtracted_test_closure->SetTitle("h_xZj_test_closure - h_xZj_MinBias_test_closure (rescaled)");
  h_xZj_subtracted_test_closure->Add(h_xZj_MinBias_test_closure, -1); // The -1 performs the subtraction

  h_response_MinBias_closure->Scale(1. / scale_binning);

  TH2F* h_response_subtracted_closure = (TH2F*)h_response_closure->Clone("h_response_subtracted_closure");
  h_response_subtracted_closure->SetDirectory(0);
  h_response_subtracted_closure->SetTitle("h_response_closure - h_response_MinBias_closure (rescaled)");
  h_response_subtracted_closure->Add(h_response_MinBias_closure, -1); // The -1 performs the subtraction

  cout << "Number of events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << ", if Z_pt>80: " << h_Z_pt_j->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << endl;
  cout << "tot ev = " << itotev << endl;

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
//  h_cen->Draw();
  h_xZj->Draw();
  if (!isData) h_xZj_true->SetLineColor(3); h_xZj_true->Draw("same");
  c6->cd(1);
  //h_vz->Draw();
  if (!isData) h_response->Draw("COLZTEXT");
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

  // Write unfolding specific histograms - NEW
  if (!isData) {
    h_xZj_true->Write();
    h_xZj_reco->Write();
    h_response->Write();
    h_response_MinBias->Write();
    h_response_subtracted->Write();
    h_xZj_train_closure->Write();
    h_xZj_train_closure_matched->Write();
    h_xZj_test_closure->Write();
    h_xZj_test_closure_matched->Write();
    h_xZj_true_train_closure->Write();
    h_xZj_true_test_closure->Write();
    h_response_closure->Write();
    h_response_MinBias_closure->Write();
    h_response_subtracted_closure->Write();
    h_xZj_MinBias_train_closure->Write();
    h_xZj_MinBias_test_closure->Write();
    h_xZj_subtracted_train_closure->Write();
    h_xZj_subtracted_test_closure->Write();
  }
  file_output_HI_mu->Close();
}

