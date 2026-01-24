/*
////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//   ANALYSIS MACRO: Z Boson (Z->mumu) + Jet Framework for PbPb and pp Collisions     //
//                                                                                    //
//   File:    analyze_HI_TTreeReader_ZMM.C                                            //
//   Author:  Raffaele Delli Gatti                                                    //
//   Date:    2024-2026                                                               //
//                                                                                    //
//   DESCRIPTION:                                                                     //
//   Performs selection and analysis of Z bosons decaying into dimuons in Heavy Ion   //
//   collisions. Includes Jet processing, Background Subtraction, and Unfolding.      //
//                                                                                    //
//   CORE WORKFLOW:                                                                   //
//   1. Muon Selection:       TightID & HLT Scale Factors (JSON).                     //
//   2. Z Reconstruction:     Dimuon mass & pT cuts.                                  //
//   3. Jet Processing:       JEC, JER, and cleaning against Z-muons.                 //
//   4. Bkg Subtraction:      Event Mixing with MinBias samples for PbPb              //
//   5. Unfolding Prep:       Response matrices (Reco vs Gen) for xZj.                //
//   6. Systematics:          Variations for Centrality, JEC, JER, SFs, Shape.        //
//                                                                                    //
//   USAGE EXAMPLES:                                                                  //
//   root -l 'analyze_HI_TTreeReader_ZMM.C("data", 1, 0)'      // Data                //
//   root -l 'analyze_HI_TTreeReader_ZMM.C("signal", 3, 0)'    // MC Nominal          //
//   root -l 'analyze_HI_TTreeReader_ZMM.C("signal", 3, 11)'   // MC Syst (JER Down)  //
//                                                                                    //
//   PARAMETERS:                                                                      //
//   ------------------------------------------------------------------------------   //
//   [sample_name]  Input label (e.g., "data" or MC label).                           //
//                                                                                    //
//   [weight_phase] Control Flag:                                                     //
//       0: Ncoll weights                                                             //
//       1: Rho weights only                                                          //
//       2: Vz weights                                                                //
//       3: Final Analysis (Rho + Vz + Systematics)                                   //
//                                                                                    //
//   [systFlag]     Systematic Variations:                                            //
//       0: Nominal              8: Binning                                           //
//       1,2: Muon SF Down/Up    9,10: JEC Down/Up                                    //
//       4: Prior/Shape          11,12: JER Down/Up                                   //
//       6,7: Centrality Down/Up 13,14: Shape Down/Up                                 //
//                                                                                    //
//   DEPENDENCIES:                                                                    //
//   - helpers.h, MC_samples.h, CorrectionSF.h                                        //
//   - JetCorrector.h, JetUncertainty.h, JERProvider.h, JetSelection_PbPb.h           //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////
*/

// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector
#include <algorithm>
#include "./MixEvSub/binning_config.h"
#include <string>
#include <fstream>    // For std::ifstream
#include <map>        // For std::map
#include <stdexcept>  // For std::runtime_error
#include <limits>     // For std::numeric_limits
#include <cstdlib>

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
#include "TH1D.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TStyle.h"
#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

// Custom headers
#include "helpers.h"           // for getLumiFromSummary, cen tables, etc.
#include "JetCorrector.h"      // for JEC
#include "JetUncertainty.h"    // for up and down var on JEC
#include "JERProvider.h"       // Include JER Provider
#include "JetSelection_PbPb.h" // For Id selection + jet veto map in PbPb
#include "MC_samples.h"        // Include the header file for MC samples
#include "CorrectionSF.h"

#include <nlohmann/json.hpp>   // For JSON parsing
// Use the nlohmann::json namespace
using json = nlohmann::json;

using namespace std;

void analyze_HI_TTreeReader_ZMM(const char * sample_name = "data", int weight_phase = 1, int systFlag = 0) {

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
        std::cout << "Loaded 'NUM_TightID_DEN_genTracks'" << std::endl;
      } else if (name == "NUM_HLT_HIL2SingleMu7_v_DEN_TightID") {
        hlt_SF.load(corr["data"]);
        hlt_loaded = true;
        std::cout << "Loaded 'NUM_HLT_HIL2SingleMu7_v_DEN_TightID'" << std::endl;
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

  // --- Load Weight Histograms and retrieve histograms ---
  TH1D* h_weight_rho   = loadWeightHist("weights_MC/rho_weights_1/weight_rho.root", "h_weight_rho");
  TH1D* h_weight_vz    = loadWeightHist("weights_MC/vz_weights_2/weight_vz.root", "h_weight_vz");
  TH1D* h_weight_JEWEL = loadWeightHist("weights_MC/final_weight_3/weight_JEWEL.root", "h_weight_JEWEL");
  // --- End Load Weight  ---

  // --- Initialize TTrees ---
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),
         skimanalysis("skimanalysis"), hltanalysis("hltanalysis"),
         hiFJRhoAnalyzerFinerBins("hiFJRhoAnalyzerFinerBins");
  glob_t globlist;

  // Binning_option for mixed event background subtraction, Use VZ + Centrality binning as default
  // 0: HF binning only
  // 1: VZ binning only
  // 2: VZ + Centrality binning
  int binning_option = 2;

  // File with MinBias sample
  TFile *inFile_MinBias;
  TString file_name = sample_name;

  bool isData = false;
  double Xsec = 1;
  double Ngen = 1;

  if (file_name.Contains("data")) {
    isData = true;
    glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test13_ZMM_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_HF.root");
    else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_VZ.root");
    else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_VZ_Cen_Combined.root");
    else { cerr << "Invalid binning_option for data MinBias file." << endl; return; }
    cout << "This is data" << endl;
  }
  else {
    // Loop over files
    for (const auto& file : files) {
      if (file_name.Contains(file.label)) {
        glob(file.path_miniaod, GLOB_NOSORT, NULL, &globlist);
        if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_HF.root");
        else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_VZ.root");
        else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_VZ_Cen_Combined.root");
        else { cerr << "Invalid binning_option for MC MinBias file." << endl; return; }
        Xsec = file.xsec;
        Ngen = file.ngen;
        cout << "This is MC " << file.label << ": ngen = " << Ngen << " xsec = " << Xsec << endl;
      }
    }
  }
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  // --- Initialize JER Provider ---
  JERProvider jer;
  if (!isData) {
    // Is MC
    cout << "Initializing JER..." << endl;
    // Load both SF and Resolution Files
    jer.LoadSF("Autumn18_RunD_V7b_MC_SF_AK4PF.txt");
    jer.LoadResolution("Autumn18_RunD_V7b_MC_PtResolution_AK4PF.txt");
    // Note: We typically don't apply Phi/Eta smearing for standard analysis, so we only load PtResolution.
  }
  // Initialize Jet Selector with specific map file
  JetSelect js("Winter24Prompt24_2024BCDEFGHI.root");

  // --- MC normalization ---
  double number_A = 208; // Lead
  // --- Read Lumi Automatically ---
  double Lumi = getLumiFromSummary("brilcalc_Collisions2023HI.csv"); // nb-1
  std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;
  // Get MC all histogram
  TFile* file_MC_all = TFile::Open("./weights_MC/MC_all_weights/output_HI_mu_MC_all.root", "READ");
  TDirectoryFile* dir_Muons_MC_all = (TDirectoryFile*)file_MC_all->Get("HI/Muons");
  TH1D* h_norm = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights");
  TH1D* h_norm_cen = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights_cen");
  TH1D* h_nev = (TH1D*)dir_Muons_MC_all->Get("h_n_events");
  TH1D* h_cen_after = (TH1D*)dir_Muons_MC_all->Get("h_cen_after");
  double n_ev = h_nev->Integral(0, h_nev->GetNbinsX()+1);
  double sum_w = h_norm->Integral(0, h_norm->GetNbinsX()+1);
  double sum_ncoll = h_norm_cen->Integral(0, h_norm_cen->GetNbinsX()+1);
  double sum_w_and_ncoll = h_cen_after->Integral(0, h_cen_after->GetNbinsX()+1);
  std::cout << "n_ev = " << n_ev << " sum_w = " << sum_w << " sum_ncoll = " << sum_ncoll << std::endl;

  double norm_MC_w_ncoll = number_A*number_A*Lumi*Xsec*(n_ev/sum_ncoll)/sum_w;
  double norm_MC_w = number_A*number_A*Lumi*Xsec/sum_w;
  if (!file_name.Contains("signal")) norm_MC_w_ncoll = number_A*number_A*Lumi*Xsec*n_ev/sum_ncoll/Ngen;
  if (!isData) std::cout << "norm_MC_w = " << norm_MC_w << " norm_MC_w_ncoll = " << norm_MC_w_ncoll << std::endl;

  if (!inFile_MinBias || inFile_MinBias->IsZombie()) {
    std::cerr << "Error: Could not open input file! Check path and file existence." << std::endl;
    return;
  }

  // --- Add files to chains ---
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

  // To associate additional TTrees with a primary TTree.
  // This allows you to access information from the friend trees while looping over the primary tree
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

  // Trigger, no needed because already in production
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
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};
  TTreeReaderArray<Float_t> jtPfCEF = {fReader, "jtPfCEF"};
  TTreeReaderArray<Float_t> jtPfNEF = {fReader, "jtPfNEF"};
  TTreeReaderArray<Float_t> jtPfMUF = {fReader, "jtPfMUF"};
  //TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};
  //TTreeReaderArray<Float_t> jtgirth = {fReader, "jt_girth"};
  //TTreeReaderArray<Float_t> jtdyndeltaR = {fReader, "jtdyn_deltaR"};

  // JEC
  vector<string> Files;
  // L2Relative is applied to BOTH Data and MC (the two files are actually identical)
  // L2Residual applied only to Data
  if (isData) {
    Files.push_back("Spring23Prompt23_PbPb_V1_DATA_L2Relative_AK2PF.txt");
    Files.push_back("Spring23Prompt23_PbPb_V1_DATA_L2Residual_AK2PF.txt");

  } else {
    Files.push_back("Spring23Prompt23_PbPb_V1_MC_L2Relative_AK2PF.txt");
  }
  JetCorrector JEC(Files);
  // --- For debugging ---
  cout << "Initializing JEC..." << endl;
  for (const auto& file : Files) {
      cout << "Loaded JEC File: " << file << endl;
  }
  // ---------------------
  JetUncertainty JEU("Autumn18_HI_V8_MC_Uncertainty_AK2PF.txt"); //!!! Old, update

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
  Float_t HF_MinBias, vz_MinBias, jet_pt_MinBias, jet_phi_MinBias, jet_eta_MinBias;
  Int_t hiBin_MinBias, bin_MinBias;
  // Set branch addresses to link variables to tree branches
  inputTree->SetBranchAddress("HF_MinBias", &HF_MinBias);
  inputTree->SetBranchAddress("vz_MinBias", &vz_MinBias);
  inputTree->SetBranchAddress("hiBin_MinBias", &hiBin_MinBias); // Link new branch
  inputTree->SetBranchAddress("bin_MinBias", &bin_MinBias);
  inputTree->SetBranchAddress("jet_pt_MinBias", &jet_pt_MinBias);
  inputTree->SetBranchAddress("jet_phi_MinBias", &jet_phi_MinBias);
  inputTree->SetBranchAddress("jet_eta_MinBias", &jet_eta_MinBias);

  // Pre-count the MinBias events Before the event loop, count how many events you actually have for each bin.
  std::map<int, int> mb_counts;
  for(int i=0; i < inputTree->GetEntries(); i++) {
    inputTree->GetEntry(i);
    mb_counts[bin_MinBias]++;
  }

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
  // Canvas
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

  // Histograms

  // Define binning for xZj unfolding.
  const int nbins_xZj = 5; // Number of bins (number of edges - 1)
  double xZj_bins[nbins_xZj + 1] = {0, 0.6, 0.9, 1.2, 1.5, 2.};
  const int nbins_xZj_meas = (systFlag != 8) ? nbins_xZj : nbins_xZj-1;

  // Define the bins using a vector so we can initialize conditionally
  std::vector<double> xZj_bins_meas_vec;
  if (systFlag != 8) {
    xZj_bins_meas_vec = {0., 0.6, 0.9, 1.2, 1.5, 2.};
  } else {
    xZj_bins_meas_vec = {0., 0.6, 0.9, 1.2, 1.5};
  }

  // Create a pointer to the vector's data (compatible with TH1F constructors)
  double* xZj_bins_meas = xZj_bins_meas_vec.data();
  //  double xZj_max;

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
  TH1F *h_xZj_fixbinw = new TH1F("h_xZj_fixbinw", "Hist;x_{Zj}; Entries", 30, 0., 3.);
  TH2F *h_jet_etaphi_before = new TH2F("h_jet_etaphi_before", "Jets Before Veto;#eta;#phi", 40, -2.5, 2.5, 40, -TMath::Pi(), TMath::Pi());
  TH2F *h_jet_etaphi_after  = new TH2F("h_jet_etaphi_after",  "Jets After Veto;#eta;#phi",  40, -2.5, 2.5, 40, -TMath::Pi(), TMath::Pi());

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
  TH1D *h_xZj_for_JEWEL_w = new TH1D("h_xZj_for_JEWEL_w", "True x_{Zj};Entries", 60, 0.,3.);
  TH1F* h_mumu_true = new TH1F("h_mumu_true", "True m;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F* h_xZj_reco = new TH1F("h_xZj_reco", "Reco x_{Zj};x_{Zj};Entries", nbins_xZj_meas, xZj_bins_meas);
  TH2F* h_response = new TH2F("h_response", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_unmatched = new TH2F("h_response_unmatched", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas,nbins_xZj, xZj_bins);
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
  TH2F* h_response_closure_unmatched = new TH2F("h_response_closure_unmatched", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias_closure = new TH2F("h_response_MinBias_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  // --- End RooUnfold Histograms ---

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Output root file
  TFile *file_output_HI_mu;

  if (isData && weight_phase != 0) {
   if (systFlag == 0)  file_output_HI_mu = new TFile("./plot/output_HI_mu_data.root", "RECREATE");
   else if (systFlag == 6) {
     cout << "Running Systematic (Centrality) down-variation (systFlag = 6)" << endl;
     file_output_HI_mu = new TFile("./syst_cen/output_HI_mu_data_cen_down.root", "RECREATE");
   }
   else if (systFlag == 7) {
     cout << "Running Systematic (Centrality) up-variation (systFlag = 7)" << endl;
     file_output_HI_mu = new TFile("./syst_cen/output_HI_mu_data_cen_up.root", "RECREATE");
   }
   else if (systFlag == 8) {
     cout << "Running binning variation (systFlag = 8)" << endl;
     file_output_HI_mu = new TFile("./syst_binning/output_HI_mu_data_binning.root", "RECREATE");
   }
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

  // This is just for plotting rho distributions after reweighting
  else if (weight_phase == -1) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_HI_mu_MC_rho_weights_after.root", "RECREATE");
    }
  }

  else if (weight_phase == 2) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_HI_mu_MC_vz_weights.root", "RECREATE");
    }
  }

  else if (weight_phase == 3) {
    if (!isData) {
      if (systFlag == 0) file_output_HI_mu = new TFile("./plot/output_HI_mu_MC_"+file_name+".root", "RECREATE");
      else if (systFlag == 1) {
        cout << "Running Systematic (SF muon) - DOWN variation (systFlag = 1)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_HI_mu_MC_SF_down.root", "RECREATE");
      }
      else if (systFlag == 2) {
        cout << "Running Systematic (SF muon) - UP variation (systFlag = 2)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_HI_mu_MC_SF_up.root", "RECREATE");
      }
      else if (systFlag == 4) {
        cout << "Running Systematic (Prior model) variation (systFlag = 4)" << endl;
        file_output_HI_mu = new TFile("./syst_prior_model/output_HI_mu_MC_prior_model.root", "RECREATE");
      }
      else if (systFlag == 8) {
        cout << "Running  binning variation (systFlag = 8)" << endl;
        file_output_HI_mu = new TFile("./syst_binning/output_HI_mu_MC_binning.root", "RECREATE");
      }
      else if (systFlag == 9) {
        cout << "Running Systematic JEC - DOWN variation (systFlag = 9)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_HI_mu_MC_JEC_down.root", "RECREATE");
      }
      else if (systFlag == 10) {
        cout << "Running Systematic JEC - UP variation (systFlag = 10)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_HI_mu_MC_JEC_up.root", "RECREATE");
      }
      else if (systFlag == 11) {
        cout << "Running Systematic JER - DOWN variation (systFlag = 11)" << endl;
        file_output_HI_mu = new TFile("./syst_JER/output_HI_mu_MC_JER_down.root", "RECREATE");
      }
      else if (systFlag == 12) {
        cout << "Running Systematic JER - UP variation (systFlag = 12)" << endl;
        file_output_HI_mu = new TFile("./syst_JER/output_HI_mu_MC_JER_up.root", "RECREATE");
      }
      else if (systFlag == 13) {
        cout << "Running Systematic shape - DOWN variation (systFlag = 13)" << endl;
        file_output_HI_mu = new TFile("./syst_shape/output_HI_mu_MC_shape_down.root", "RECREATE");
      }
      else if (systFlag == 14) {
        cout << "Running Systematic shape - UP variation (systFlag = 14)" << endl;
        file_output_HI_mu = new TFile("./syst_shape/output_HI_mu_MC_shape_up.root", "RECREATE");
      }
    }
  }

  // =================================================================================
  //   MAIN EVENT LOOP
  // =================================================================================

  unsigned int iEvent = 0;
  unsigned int itotev = 0;
  while (fReader.Next()) {
    itotev++;
    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;
    // Use hiHF to recalculate hiBin for systematics
    int hiBin_to_use = *hiBin; // Start with the nominal hiBin
    // Warning check (Comparison)
    if (isData) {
      int calculated_nominal = getHiBin(*hiHF, cenHF_2023PbPb_nominal);
      if (*hiBin != calculated_nominal) {
        cout << "!!! WARNING: *hiBin = " << *hiBin
             << " hiHF = " << *hiHF
             << " hiBin nominal = " << calculated_nominal
             << " hiBin up = " << getHiBin(*hiHF, cenHF_2023PbPb_up)
             << " hiBin down = " << getHiBin(*hiHF, cenHF_2023PbPb_down)
             << endl;
      }
    }
    // Systematics Assignment for centrality
    if (isData && systFlag == 6) hiBin_to_use = getHiBin(*hiHF, cenHF_2023PbPb_down); // Centrality Down
    if (isData && systFlag == 7) hiBin_to_use = getHiBin(*hiHF, cenHF_2023PbPb_up); // Centrality Up
    // Centrality weight
    float weight_cent = Ncoll[hiBin_to_use];
    // Scale MC
    float scale = 1;
    if (!isData && weight_phase == 0) {
      scale*=norm_MC_w*(*weight);
    }
    if (!isData && weight_phase != 0) {
      if (file_name.Contains("signal")) scale*=norm_MC_w_ncoll*weight_cent*(*weight);
      else scale*=norm_MC_w_ncoll*weight_cent;
    }
    // Selection on centrality bin
    if (weight_phase > 1 ) {
      if(hiBin_to_use>59) continue;
    }
    if (weight_phase == 1 || weight_phase == - 1) {
      if (isData) {
        if(hiBin_to_use>59) continue;
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
      if (weight_phase == 2 || weight_phase == -1) {
        // Apply rho weight
        scale*=h_weight_rho->GetBinContent(bin_rho);
      }
      if (weight_phase == 3) {
       // Apply rho and vz weight
       int bin_vz = h_weight_vz->FindBin(*vz);
       scale*=h_weight_rho->GetBinContent(bin_rho)*h_weight_vz->GetBinContent(bin_vz);
      }
    }

    // --- Fill information for unfolding using gen ---
    TLorentzVector gen_Z;
    double dPhi_Zj_Gen = 0;
    double true_xZj = 0;
    int ijetGenLeading_unfold = -1;
    if (!isData) {
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
        gen_Z = genmuPlus + genmuMinus;
        if (gen_Z.M() >= 60 && gen_Z.M() <= 120 && gen_Z.Pt() >= 40) {
          if (genmuMinus.Pt() >= 20 && abs(genmuMinus.Eta()) <= 2.4 && genmuPlus.Pt() >= 20 && abs(genmuPlus.Eta()) <= 2.4) {
            h_mumu_true->Fill(gen_Z.M(), scale);
            // Loop over gen jets
            for (int ijetGen = 0; ijetGen < *ngen; ++ijetGen) {
              // Apply truth-level cuts
              if (genpt[ijetGen] < 30 || abs(geneta[ijetGen]) > 2.5) continue;
              if (getDeltaR(geneta[ijetGen], genphi[ijetGen], genmuMinus.Eta(), genmuMinus.Phi()) < 0.2) continue;
              if (getDeltaR(geneta[ijetGen], genphi[ijetGen], genmuPlus.Eta(), genmuPlus.Phi()) < 0.2) continue;
              if (ijetGenLeading_unfold == -1 || genpt[ijetGen] > genpt[ijetGenLeading_unfold]) {
                ijetGenLeading_unfold = ijetGen;
              }
            }
            if (ijetGenLeading_unfold != -1) {
              dPhi_Zj_Gen = RelativePhi(gen_Z.Phi(), genphi[ijetGenLeading_unfold]);
              if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                // Calculate true x_Zj
                true_xZj = genpt[ijetGenLeading_unfold] / gen_Z.Pt();
                // Scale MC to JEWEL for check unfolding dependance on shape
                if (systFlag == 4) {
                  int bin_xZj_JEWEL = h_weight_JEWEL->FindBin(true_xZj);
                  double weight_JEWEL = (h_weight_JEWEL->GetBinContent(bin_xZj_JEWEL)>0) ? h_weight_JEWEL->GetBinContent(bin_xZj_JEWEL) : 1;
                  scale*=weight_JEWEL;
                }
                if (systFlag == 13) {
                  if (true_xZj>=0.0 && true_xZj<0.6) scale*=2.2;
                  if (true_xZj>=0.6 && true_xZj<0.9) scale*=0.7;
                  if (true_xZj>=0.9 && true_xZj<1.5) scale*=0.3;
                }
                if (systFlag == 14) {
                  if (true_xZj>=0.0 && true_xZj<0.6) scale*=0.6;
                  if (true_xZj>=0.6 && true_xZj<0.9) scale*=1.4;
                  if (true_xZj>=0.9 && true_xZj<1.5) scale*=1.3;
                }
                h_xZj_for_JEWEL_w->Fill(true_xZj, scale);
                //Remove overflow and put it in the last bin
                //if (true_xZj > xZj_max) true_xZj = xZj_max - 0.01;
                h_xZj_true->Fill(true_xZj, scale);
                if (itotev < 0.7*Ngen) h_xZj_true_train_closure->Fill(true_xZj, scale);
                else h_xZj_true_test_closure->Fill(true_xZj, scale);
              }
            }
          }
        }
      }
    }
    // --- End fill information for unfolding ---

    // --- Reco Z Reconstruction ---
    if (*nReco < 2 ) continue;
    iEvent++;
    //if(*HLT_HIL2SingleMu7_v3<=0) continue; // no needed because already in production
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // TLorentzVectors for the muons
    TLorentzVector muPlus, muMinus;

    // Find Best Muon Pair (most energetic muon and antimuon)
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
    if (iHighPtMu == -1 || iHighPtAntiMu == -1) continue;
    muMinus.SetPtEtaPhiM(recoPt[iHighPtMu], recoEta[iHighPtMu], recoPhi[iHighPtMu], muonMass);
    muPlus.SetPtEtaPhiM(recoPt[iHighPtAntiMu], recoEta[iHighPtAntiMu], recoPhi[iHighPtAntiMu], muonMass);

    // Apply Muon Scale Factors
    if (!isData) {
      std::string sf_flag = (systFlag == 1) ? "systdown" : (systFlag == 2 ? "systup" : "nominal");
      // Apply TightID scale factors for both muons in the Z candidate
      double id_sf_mu_p = tightID_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], sf_flag);
      double id_sf_mu_m = tightID_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], sf_flag);
      scale *= id_sf_mu_p * id_sf_mu_m;
      // Apply HLT scale factor combining them as SF1 + SF2 - (SF1 * SF2)
      // For trigger we only need one to lepton to have fired, so we use the addition rule of probability
      double hlt_sf_mu_p = hlt_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], sf_flag);
      double hlt_sf_mu_m = hlt_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], sf_flag);
      scale *= (hlt_sf_mu_p + hlt_sf_mu_m - (hlt_sf_mu_p * hlt_sf_mu_m));
    }

    // Z from muon-antimuon pairs
    TLorentzVector Z = muPlus + muMinus;
    if (Z.M() < 60 || Z.M() > 120 || Z.Pt() < 40) continue;
    if (muMinus.Pt() < 20 || abs(muMinus.Eta()) > 2.4 || muPlus.Pt() < 20 || abs(muPlus.Eta()) > 2.4) continue;

    h_vz->Fill(*vz, scale);
    h_avg_rho->Fill(avg_rho, scale);
    h_avg_rho_vs_cen->Fill(hiBin_to_use, avg_rho, scale);
    h_cen->Fill((hiBin_to_use)/2, scale);
    h_mumu->Fill(Z.M(), scale);
    h_Z_pt->Fill(Z.Pt(), scale);

    // Pre-calculate GenJet Vectors for easier passing to JER function
    std::vector<float> v_gen_pts, v_gen_etas, v_gen_phis;
    if (!isData) {
      for (int i = 0; i < *ngen; i++) {
        v_gen_pts.push_back(genpt[i]);
        v_gen_etas.push_back(geneta[i]);
        v_gen_phis.push_back(genphi[i]);
      }
    }

    // --- Loop over Jets ---
    unsigned int njets = 0;
    int ijetLeading = -1;
    int iGenjetMatchedtoLeadingReco = -1;
    bool isLeadingJetMatched = false;
    double jtpt_corr[20000];
    for(int ijet=0; ijet<*nref; ijet++){
      // Apply JEC and JEC uncertainty
      //cout << "before JEC: " << rawpt[ijet] << endl;
      JEC.SetJetPT(rawpt[ijet]);
      JEC.SetJetEta(jteta[ijet]);
      JEC.SetJetPhi(jtphi[ijet]);
      double Correction = JEC.GetCorrection();
      double CorrectedPT = JEC.GetCorrectedPT();
      JEU.SetJetPT(CorrectedPT);
      JEU.SetJetEta(jteta[ijet]);
      JEU.SetJetPhi(jtphi[ijet]);
      double pt_jec_applied = CorrectedPT;
      if (!isData && systFlag == 9) pt_jec_applied = CorrectedPT * (1 - JEU.GetUncertainty().first); //down
      if (!isData && systFlag == 10) pt_jec_applied = CorrectedPT * (1 + JEU.GetUncertainty().second); //up
      //cout << "after JEC: jtpt_corr = " << pt_jec_applied << endl;

      // Apply JER (Hybrid Method)
      double pt_final = pt_jec_applied;
      if (!isData) {
        int jer_syst = 0;
        // map systFlag 11/12 to JER Up/Down
        if (systFlag == 11) jer_syst = -1; // Down
        if (systFlag == 12) jer_syst = 1;  // Up

        pt_final = jer.GetSmearedPt(pt_jec_applied, jteta[ijet], jtphi[ijet], avg_rho, v_gen_pts, v_gen_etas, v_gen_phis, jer_syst);
      }
      jtpt_corr[ijet] = pt_final;
      //jtpt_corr[ijet] = rawpt[ijet];

      // Selections and Z-Jet dR Cleaning
      if (jtpt_corr[ijet] < 30 || abs(jteta[ijet]) > 2.5) continue;
      // Apply Combined Jet ID and Veto Map
      h_jet_etaphi_before->Fill(jteta[ijet], jtphi[ijet], scale);
      if (!js.JetSelection(jteta[ijet], jtphi[ijet], jtPfCEF[ijet], jtPfNEF[ijet], jtPfMUF[ijet])) continue;
      h_jet_etaphi_after->Fill(jteta[ijet], jtphi[ijet], scale);
      // Z-Jet dR Cleaning
      if (getDeltaR(jteta[ijet], jtphi[ijet], muMinus.Eta(), muMinus.Phi()) < 0.2) continue;
      if (getDeltaR(jteta[ijet], jtphi[ijet], muPlus.Eta(), muPlus.Phi()) < 0.2) continue;
      njets++;
      //cout << "-----------------------------" << endl;
      //cout << "ijet: " << ijet << " pt = " << jtpt_corr[ijet] << " eta = " << jteta[ijet] << " phi = " << jtphi[ijet] << endl;
      //cout << "muMinus pt = " << muMinus.Pt() << " eta = " << muMinus.Eta() << " phi = " << muMinus.Phi() << endl;
      //cout << "muPlus pt = " << muPlus.Pt() << " eta = " << muPlus.Eta() << " phi = " << muPlus.Phi() << endl;
      //cout << "dRMinus = " << dRMinus << getDeltaR(jteta[ijet], jtphi[ijet], muMinus.Eta(), muMinus.Phi()) <<
      //cout << " dRPlus = " << dRPlus << getDeltaR(jteta[ijet], jtphi[ijet], muPlus.Eta(), muPlus.Phi() << endl;

      // --- Matching Logic for CURRENT Reco Jet (ijet) ---
      double min_dR = 9999.0;
      int matched_gen_jet_idx = -1;

      // Loop over Gen Jets to find the best match for the current reco jet
      if (!isData) {
        for (int igenjet = 0; igenjet < *ngen; igenjet++) {
          // gen jet cuts looser than reco jet cuts
          if(genpt[igenjet]<10. || abs(geneta[igenjet])>3.) continue;
          double dR_gen = getDeltaR(jteta[ijet], jtphi[ijet], geneta[igenjet], genphi[igenjet]);
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
    } //cout << "ijetLeading = " << ijetLeading << endl;
    // --- end loop over jets ---

    // --- Reco leading jet selection ---
    if (ijetLeading != -1) {
      double dPhi_Zj = RelativePhi(Z.Phi(), jtphi[ijetLeading]);
      double xZj = jtpt_corr[ijetLeading]/Z.Pt();
      //Remove overflow and put it in the last bin
      //if (xZj > xZj_max) xZj = xZj_max - 0.01;
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
          if (hiBin_to_use >= bin_range.first && hiBin_to_use < bin_range.second) {
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
        // Loop over the TTree entries for mixing events with MinBias, assumes the 'bin_MinBias' tree
        // corresponds to 'current_global_bin_n' and contains 'events_per_mixed_bin_limit' events for each bin
        double events_filled_for_this_bin_in_MinBias = 0;
        for(int iEntry=0; iEntry< nEntries_MinBias; iEntry++){
          inputTree->GetEntry(iEntry); // Read all branch values for the current entry
          if (current_global_bin_n == bin_MinBias) { // Match by global bin number
            // Determine weight for this specific bin, if n events the weight is 1/n
            double n_mix = mb_counts[current_global_bin_n];
            double mixing_weight = (n_mix > 0) ? (1.0 / n_mix) : 0.0;
            // Apply same jet cuts as for signal jets
            if (jet_pt_MinBias > jtpt_corr[ijetLeading]) {
              if (getDeltaR(jet_eta_MinBias, jet_phi_MinBias, muMinus.Eta(), muMinus.Phi()) >= 0.2 &&
                  getDeltaR(jet_eta_MinBias, jet_phi_MinBias, muPlus.Eta(), muPlus.Phi()) >= 0.2) {
                double dPhi_Zj_MinBias = RelativePhi(Z.Phi(), jet_phi_MinBias);
                double xZj_MinBias = jet_pt_MinBias/Z.Pt();
                h_deltaPhi_Zj_MinBias->Fill(dPhi_Zj_MinBias, scale * mixing_weight);
                //Remove overflow and put it in the last bin
                //if (xZj_MinBias > xZj_max) xZj_MinBias = xZj_max - 0.01;
                if (dPhi_Zj_MinBias > 7 * TMath::Pi() / 8) {
                  h_jet_pt_lj_MinBias->Fill(jet_pt_MinBias, scale * mixing_weight);
                  h_xZj_MinBias->Fill(xZj_MinBias, scale * mixing_weight);
                  if (itotev < 0.7*Ngen) h_xZj_MinBias_train_closure->Fill(xZj_MinBias, scale * mixing_weight);
                  else h_xZj_MinBias_test_closure->Fill(xZj_MinBias, scale * mixing_weight);
                  if (!isData && ijetGenLeading_unfold != -1 && dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                    //if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
                    h_response_MinBias->Fill(xZj_MinBias, true_xZj, scale * mixing_weight);
                    if (itotev < 0.7*Ngen) h_response_MinBias_closure->Fill(xZj_MinBias, true_xZj, scale * mixing_weight);
                    //}
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

      if (!isData && isLeadingJetMatched) {
        double dPhi_Zj_matched = RelativePhi(Z.Phi(), jtphi[ijetLeading]);
        h_deltaPhi_Zj_matched->Fill(dPhi_Zj_matched, scale);
      }

      if (dPhi_Zj > 7 * TMath::Pi() / 8) {
        h_njet->Fill(njets, scale);
        h_mumu_j->Fill(Z.M(), scale);
        h_Z_pt_j->Fill(Z.Pt(), scale);
        h_jet_pt_lj->Fill(jtpt_corr[ijetLeading], scale);
        h_xZj->Fill(xZj, scale);
        h_xZj_fixbinw->Fill(xZj, scale);
        if (!isData && ijetGenLeading_unfold != -1 && dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
          h_response_unmatched->Fill(xZj, true_xZj, scale);
          if (itotev < 0.7*Ngen) h_response_closure_unmatched->Fill(xZj, true_xZj, scale);
        }
        if (itotev < 0.7*Ngen) h_xZj_train_closure->Fill(xZj, scale);
        else h_xZj_test_closure->Fill(xZj, scale);
        h_cen_j->Fill((hiBin_to_use)/2, scale);
        h_HF_j->Fill(*hiHF, scale);

        if (!isData && isLeadingJetMatched) {
          h_jet_pt_lj_matched->Fill(jtpt_corr[ijetLeading], scale);
          h_xZj_matched->Fill(xZj, scale);
        }
      }

      // --- Fill information for unfolding ---
      if (RelativePhi(Z.Phi(), jtphi[ijetLeading]) > 7 * TMath::Pi() / 8) {
        if (!isData && ijetGenLeading_unfold != -1 && dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
          // Calculate true x_Zj
          // Check if the leading gen jet is matched to leading reconstructed jet
          if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
            h_response->Fill(xZj, true_xZj, scale);
            h_xZj_reco->Fill(xZj, scale);
            if (itotev < 0.7*Ngen) {
              h_response_closure->Fill(xZj, true_xZj, scale);
              h_xZj_train_closure_matched->Fill(xZj, scale);
            } else h_xZj_test_closure_matched->Fill(xZj, scale);
          }
        }
      } // --- end filling information for unfolding ---
    } // ---end reco leading jet selection ---
  }

  // =================================================================================
  //   END MAIN EVENT LOOP
  // =================================================================================

  // Finalize histograms by subtracting MinBias
  TH1F* h_deltaPhi_Zj_subtracted = (TH1F*)h_deltaPhi_Zj->Clone("h_deltaPhi_Zj_subtracted");
  h_deltaPhi_Zj_subtracted->SetDirectory(0);
  h_deltaPhi_Zj_subtracted->SetTitle("h_deltaPhi_Zj - h_deltaPhi_Zj_MinBias (rescaled)");
  h_deltaPhi_Zj_subtracted->Add(h_deltaPhi_Zj_MinBias, -1); // The -1 performs the subtraction


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

  TH1F* h_xZj_subtracted = (TH1F*)h_xZj->Clone("h_xZj_subtracted");
  h_xZj_subtracted->SetDirectory(0);
  h_xZj_subtracted->SetTitle("h_xZj - h_xZj_MinBias (rescaled)");
  h_xZj_subtracted->Add(h_xZj_MinBias, -1); // The -1 performs the subtraction

  TH2F* h_response_subtracted = (TH2F*)h_response_unmatched->Clone("h_response_subtracted");
  h_response_subtracted->SetDirectory(0);
  h_response_subtracted->SetTitle("h_response_unmatched - h_response_MinBias (rescaled)");
  h_response_subtracted->Add(h_response_MinBias, -1); // The -1 performs the subtraction

  TH1F* h_xZj_train_closure_subtracted = (TH1F*)h_xZj_train_closure->Clone("h_xZj_train_closure_subtracted");
  h_xZj_train_closure_subtracted->SetDirectory(0);
  h_xZj_train_closure_subtracted->SetTitle("h_xZj_train_closure - h_xZj_MinBias_train_closure (rescaled)");
  h_xZj_train_closure_subtracted->Add(h_xZj_MinBias_train_closure, -1); // The -1 performs the subtraction

  TH1F* h_xZj_test_closure_subtracted = (TH1F*)h_xZj_test_closure->Clone("h_xZj_test_closure_subtracted");
  h_xZj_test_closure_subtracted->SetDirectory(0);
  h_xZj_test_closure_subtracted->SetTitle("h_xZj_test_closure - h_xZj_MinBias_test_closure (rescaled)");
  h_xZj_test_closure_subtracted->Add(h_xZj_MinBias_test_closure, -1); // The -1 performs the subtraction

  TH2F* h_response_closure_subtracted = (TH2F*)h_response_closure_unmatched->Clone("h_response_closure_subtracted");
  h_response_closure_subtracted->SetDirectory(0);
  h_response_closure_subtracted->SetTitle("h_response_closure_unmatched - h_response_MinBias_closure (rescaled)");
  h_response_closure_subtracted->Add(h_response_MinBias_closure, -1); // The -1 performs the subtraction

  cout << "Number of Z+jet events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << ", if Z_pt>80: " << h_Z_pt_j->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << endl;
  cout << "tot ev = " << itotev << endl;

  c1->cd(1);
  h_mumu->Draw();
  c2->cd(1);
  h_Z_pt->Draw();
  c3->cd(1);
  h_HF_j->Draw();
  c4->cd(1);
  h_jet_pt_lj->Draw();
  c5->cd(1);
  h_xZj->Draw();
  if (!isData) h_xZj_true->SetLineColor(3); h_xZj_true->Draw("same");
  c6->cd(1);
  if (isData) h_vz->Draw();
  else h_response->Draw("COLZTEXT");
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
  h_xZj_fixbinw->Write();
  h_jet_etaphi_before->Write();
  h_jet_etaphi_after->Write();
  h_xZj_MinBias->Write();
  h_xZj_subtracted->Write();
  h_xZj_matched->Write();
  h_vz->Write();
  h_avg_rho->Write();

  // Write unfolding specific histograms
  if (!isData) {
    h_xZj_true->Write();
    h_xZj_for_JEWEL_w->Write();
    h_mumu_true->Write();
    h_xZj_reco->Write();
    h_response->Write();
    h_response_unmatched->Write();
    h_response_MinBias->Write();
    h_response_subtracted->Write();
    h_xZj_train_closure->Write();
    h_xZj_train_closure_matched->Write();
    h_xZj_test_closure->Write();
    h_xZj_test_closure_matched->Write();
    h_xZj_true_train_closure->Write();
    h_xZj_true_test_closure->Write();
    h_response_closure->Write();
    h_response_closure_unmatched->Write();
    h_response_MinBias_closure->Write();
    h_response_closure_subtracted->Write();
    h_xZj_MinBias_train_closure->Write();
    h_xZj_MinBias_test_closure->Write();
    h_xZj_train_closure_subtracted->Write();
    h_xZj_test_closure_subtracted->Write();
  }
  file_output_HI_mu->Close();
}

