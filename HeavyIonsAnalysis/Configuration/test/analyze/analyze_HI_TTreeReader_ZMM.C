/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//   ANALYSIS MACRO: Z Boson (Z->mumu) + Jet Framework for PbPb and pp Collisions                                             //
//                                                                                                                            //
//   File:    analyze_HI_TTreeReader_ZMM.C                                                                                    //
//   Author:  Raffaele Delli Gatti                                                                                            //
//   Date:    2024-2026                                                                                                       //
//                                                                                                                            //
//   DESCRIPTION:                                                                                                             //
//   Performs selection and analysis of Z bosons decaying into dimuons in Heavy Ion collisions. Includes Jet processing,      //
//   Background Subtraction, and Unfolding.                                                                                   //
//                                                                                                                            //
//   CORE WORKFLOW:                                                                                                           //
//   1. Muon Selection:  TightID & HLT Scale Factors (JSON).        2. Z Reconstruction:  Dimuon mass & pT cuts.              //
//   3. Jet Processing:  JEC, JER, and cleaning against Z-muons.    4. Bkg Subtraction:   Event Mixing with MinBias for PbPb. //
//   5. Unfolding Prep:  Response matrices (Reco vs Gen) for xZj.   6. Systematics:       Cen, JEC, JER, SFs, Shape.          //
//                                                                                                                            //
//   USAGE EXAMPLES for PbPb23:                                                                                               //
//   root -l 'analyze_HI_TTreeReader_ZMM.C("PbPb23", "data", 1, 0)'      // Data                                              //
//   root -l 'analyze_HI_TTreeReader_ZMM.C("PbPb23", "signal", 3, 0)'    // MC Nominal                                        //
//   root -l 'analyze_HI_TTreeReader_ZMM.C("PbPb23", "signal", 3, 11)'   // MC Syst (JER Down)                                //
//                                                                                                                            //
//   PARAMETERS:                                                                                                              //
//   ------------------------------------------------------------------------------                                           //
//   [collision_type] Input label (e.g., "PbPb23", "PbPb24", or "ppref24").                                                   //
//                                                                                                                            //
//   [sample_name]  Input label (e.g., "data" or MC label).                                                                   //
//                                                                                                                            //
//   [weight_phase] Control Flag:                                  [systFlag]     Systematic Variations:                      //
//       0: Ncoll weights                                              0: Nominal                    8: Binning               //
//       1: Rho weights only                                           1,2,15-18: Muon SF Down/Up    9,10: JEC Down/Up        //
//       2: Vz weights                                                 4: Prior/Shape                11,12: JER Down/Up       //
//       3: Final Analysis (Rho + Vz + Systematics)                    6,7: Centrality Down/Up       13,14: Shape Down/Up     //
//                                                                                                                            //
//   DEPENDENCIES:                                                                                                            //
//   - helpers.h, MC_samples.h, CorrectionSF.h                                                                                //
//   - JetCorrector.h, JetUncertainty.h, JERProvider.h, JetSelection_PbPb.h, JetSelection_pp.h                                //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
#include "JetSelection_PbPb.h" // Defines JetSelect for Id selection + jet veto map in PbPb
#include "JetSelection_pp.h"   // Defines JetSelect_pp for Id selection + jet veto map in pp
#include "MC_samples.h"        // Include the header file for MC samples
#include "CorrectionSF.h"

#include <nlohmann/json.hpp>   // For JSON parsing
using json = nlohmann::json;   // Use the nlohmann::json namespace

using namespace std;

void analyze_HI_TTreeReader_ZMM(const char * collision_type = "PbPb23", const char * sample_name = "data",
                                int weight_phase = 1, int systFlag = 0,
                                int cent_min = 0, int cent_max = 30,
                                double ptZ_min = 40.0, double ptZ_max = 9999.0) {

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

  // Collision name
  TString collision_name = collision_type;
  // Sample name
  TString file_name = sample_name;
  // File with MinBias sample
  TFile *inFile_MinBias = nullptr;

  bool isData = false;
  bool isPbPb = collision_name.Contains("PbPb");
  bool is2023 = collision_name.Contains("23");
  bool isSignal = file_name.Contains("signal");
  bool isAlternative = file_name.Contains("alternative");
  double Xsec = 1;
  double Ngen = 1;

  // --- ADD RUN TAG FOR DYNAMIC CUTS ---
  TString run_tag;
  if (isPbPb) {
      if (ptZ_max > 9000) {
          run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
      } else {
          run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
      }
  } else {
      // For pp collisions, omit the centrality tags
      if (ptZ_max > 9000) {
          run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
      } else {
          run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
      }
  }

  // Create a pointer to the vector of MC samples we want to use
  const std::vector<FileInfo>* targetVector = nullptr;
  if (collision_name.Contains("PbPb24")) {
      targetVector = &files_PbPb24;
  } else if (collision_name.Contains("PbPb23")) {
      targetVector = &files;
  } else {
      targetVector = &files_ppref;
  }

  std::cout << "------------------------------------------------" << std::endl;
  if (collision_name.Contains("PbPb23")) std::cout << "Running 2023 PbPb collisions" << endl;
  else if (collision_name.Contains("PbPb24")) std::cout << "Running 2024 PbPb collisions" << endl;
  else if (collision_name.Contains("ppref24")) std::cout << "Running 2024 ppref collisions" << endl;
  std::cout << "isPbPb = " << isPbPb << " is2023 = " << is2023 << " isSignal = " << isSignal << std::endl;

  if (file_name.Contains("data")) {
    isData = true;
    if (isPbPb) {
      if (is2023)
        glob("/eos/infnts/cms/store/user/rdelliga/HIPhysicsRawPrime*/CRAB3_Analysis_test24_ZMM_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
      else
          // Pattern "PbPb24[AB]_" matches "PbPb24A_..." and "PbPb24B_..." but EXCLUDES "PbPb24_..."
        glob("/eos/infnts/cms/store/user/rdelliga/HIPhysicsRawPrime*/CRAB3_Analysis_test21_ZMM_PbPb24[AB]_*/*/*.root", GLOB_NOSORT, NULL, &globlist);

      // Dynamic Prefix for MinBias files
      TString mb_prefix = (is2023) ? "" : "HI24_";

      TString cent_tag = "";
      if (binning_option == 1 || binning_option == 2) {
          cent_tag = Form("_Cen%d_%d", cent_min, cent_max);
      }
      
      if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_" + mb_prefix + "leading_jets_data_HF.root");
      else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_" + mb_prefix + "leading_jets_data_VZ" + cent_tag + ".root");
      else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_" + mb_prefix + "leading_jets_data_VZ_Cen_Combined" + cent_tag + ".root");
      else { cerr << "Invalid binning_option for data MinBias file." << endl; return; }
      cout << "This is PbPb data (" << (is2023 ? "2023" : "2024") << ")" << endl;
    } else {
      glob("/eos/infnts/cms/store/user/kdeleo/PPRefSingleMuon*/CRAB3_Analysis_test16_ZMM_PPRefSingleMuon*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    }
  } else { //MC
    // Loop over files
    for (const auto& file : *targetVector) {
      if (file_name.Contains(file.label)) {
        glob(file.path_miniaod, GLOB_NOSORT, NULL, &globlist);
        Xsec = file.xsec;
        Ngen = file.ngen;
        std::cout << "This is MC " << file.label << ": ngen = " << Ngen;
        if (isPbPb) std::cout << " xsec = " << Xsec << " nb-1" << std::endl;
        else std::cout << " xsec = " <<  Xsec << " pb^-1" << std::endl;
      }
    }
    if (isPbPb) {
      TString mb_prefix = (is2023) ? "" : "HI24_";

      TString cent_tag = "";
      if (binning_option == 1 || binning_option == 2) {
          cent_tag = Form("_Cen%d_%d", cent_min, cent_max);
      }

      if (binning_option == 0)      inFile_MinBias = TFile::Open("./MixEvSub/MinBias_" + mb_prefix + "leading_jets_MC_HF.root");
      else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_" + mb_prefix + "leading_jets_MC_VZ" + cent_tag + ".root");
      else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_" + mb_prefix + "leading_jets_MC_VZ_Cen_Combined" + cent_tag + ".root");
      else { cerr << "Invalid binning_option for MC MinBias file." << endl; return; }
    }
  }
  // Print only the first file to avoid spamming the console
  cout << "Found " << globlist.gl_pathc << " files" << endl;
  size_t n_print = 1;
  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    if (i < n_print) {
      std::cout << "[" << i << "] " << globlist.gl_pathv[i] << std::endl;
    } else {
      // Calculate how many remain
      size_t remaining = globlist.gl_pathc - n_print;
      std::cout << "... and " << remaining << " more files." << std::endl;
      break; // Stop the loop
    }
  }
  // --- SAFETY CHECK: Did glob find any files? ---
  if (globlist.gl_pathc == 0) {
      std::cerr << "\n[ERROR] No files found for sample: " << file_name << std::endl;
      std::cerr << "  -> This usually means the path in MC_samples.h is incorrect" << std::endl;
      std::cerr << "  -> Or the files haven't been produced/transferred yet." << std::endl;
      std::cerr << "  -> Skipping this sample to avoid crash.\n" << std::endl;
      return; // Exit the function cleanly
  }
  // ----------------------------------------------

  if (isPbPb) {
    if (inFile_MinBias && !inFile_MinBias->IsZombie()) {
      cout << "Opened MinBias file: " << inFile_MinBias->GetName() << endl;
    } else {
      std::cerr << "Error: Could not open input file! Check path and file existence." << std::endl;
      return;
    }
  }

  // --- Load Weight Histograms and retrieve histograms ---
  TH1D* h_weight_rho   = nullptr;
  TH1D* h_weight_vz    = nullptr;
  TH1D* h_weight_JEWEL = nullptr;

  bool use_data_driven_cen = true;

  // Only load weights if running on MC

  if (!isData && weight_phase!=0) {
    // 1. Rho Weight (Only for PbPb MC, specific to year)
    if (isPbPb && weight_phase!=1) {
      std::string name_weight_rho = (is2023) ? Form("weights_MC/rho_weights_1/weight_HI_rho%s.root", run_tag.Data())
                                             : Form("weights_MC/rho_weights_1/weight_HI24_rho%s.root", run_tag.Data());
      h_weight_rho = loadWeightHist(name_weight_rho, "h_weight_rho");
    }

    // 2. Vz Weight (Specific to each campaign: PbPb23, PbPb24, ppref)
    if (weight_phase!=2) {
      std::string name_weight_vz;
      if (collision_name.Contains("PbPb23"))           name_weight_vz = Form("weights_MC/vz_weights_2/weight_HI_vz%s.root", run_tag.Data());
      else if (collision_name.Contains("PbPb24"))      name_weight_vz = Form("weights_MC/vz_weights_2/weight_HI24_vz%s.root", run_tag.Data());
      else if (collision_name.Contains("ppref24"))     name_weight_vz = Form("weights_MC/vz_weights_2/weight_ppref_vz%s.root", run_tag.Data());
      h_weight_vz = loadWeightHist(name_weight_vz, "h_weight_vz");
    }

    // 3. JEWEL Weight (Only for Systematics)
    if (systFlag == 4) {
      std::string name_weight_JEWEL;
      if (collision_name.Contains("PbPb23"))       name_weight_JEWEL = Form("weights_MC/final_weight_3/weight_HI_JEWEL%s.root", run_tag.Data());
      else if (collision_name.Contains("PbPb24"))  name_weight_JEWEL = Form("weights_MC/final_weight_3/weight_HI24_JEWEL%s.root", run_tag.Data());
      else if (collision_name.Contains("ppref24")) name_weight_JEWEL = Form("weights_MC/final_weight_3/weight_ppref_JEWEL%s.root", run_tag.Data());
      h_weight_JEWEL = loadWeightHist(name_weight_JEWEL, "h_weight_JEWEL");
      std::cout << "JEWEL file needed for syst variation" << std::endl;
    }
  }
  // --- End Load Weight  ---

  // --- MC normalization ---
  double number_A = 208; // Lead
  // --- Read Lumi Automatically ---
  double Lumi = 1.;
  if (collision_name.Contains("PbPb23")) Lumi = getLumiFromSummary("brilcalc_Collisions2023HI.csv"); // nb-1
  else if (collision_name.Contains("PbPb24")) Lumi = getLumiFromSummary("brilcalc_Collisions2024_HI.csv"); // nb-1
  else if (collision_name.Contains("ppref24")) Lumi = getLumiFromSummary("brilcalc_Collisions2024_ppref.csv"); // pb-1

  if (isPbPb) std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;
  else        std::cout << "Parsed Lumi  : " << Lumi << " pb^-1" << std::endl;

  // Calculate MC Normalization factors ONLY if MC
  double norm_MC_w = 1.0;
  double norm_MC_w_ncoll = 1.0;

  double n_ev_alternative = 1.0;
  double sum_w_alternative = 1.0;

  if (!isData) {
    // Get MC all histogram
    TString name_MC_all;
    TString name_dir_all;

    if (collision_name.Contains("PbPb23")) {
      name_MC_all = "./weights_MC/MC_all_weights/output_HI_mu_MC_all.root";
      name_dir_all = "HI/Muons";
    }
    else if (collision_name.Contains("PbPb24")) {
      name_MC_all = "./weights_MC/MC_all_weights/output_HI24_mu_MC_all.root";
      name_dir_all = "HI24/Muons";
    }
    else if (collision_name.Contains("ppref24")) {
      name_MC_all = "./weights_MC/MC_all_weights/output_ppref_mu_MC_all.root";
      name_dir_all = "ppref/Muons";
      if (isAlternative){
        TFile* file_MC_all_alternative = TFile::Open("./weights_MC/MC_all_weights/output_ppref_mu_MC_all_alternative.root", "READ");
        if (!file_MC_all_alternative || file_MC_all_alternative->IsZombie()) {
          std::cerr << "Error: Cannot open alternative MC norm file " << std::endl;
          return;
        }
        TDirectoryFile* dir_Muons_MC_all_alternative = (TDirectoryFile*)file_MC_all_alternative->Get(name_dir_all);
        if (!dir_Muons_MC_all_alternative) {
          std::cerr << "Error: Cannot find directory " << name_dir_all << " in alternative MC norm file." << std::endl;
          return;
        }
        TH1D* h_norm_alternative = (TH1D*)dir_Muons_MC_all_alternative->Get("h_sum_weights");
        TH1D* h_nev_alternative  = (TH1D*)dir_Muons_MC_all_alternative->Get("h_n_events");
        sum_w_alternative = h_norm_alternative->Integral(0, h_norm_alternative->GetNbinsX()+1);
        double n_ev_alternative = h_nev_alternative->Integral(0, h_nev_alternative->GetNbinsX()+1);

        std::cout << "Alternative n_ev = " << n_ev_alternative << " sum_w = " << sum_w_alternative << std::endl;
      }
    }
    std::cout << "Opening MC norm file: " << name_MC_all << std::endl;

    TFile* file_MC_all = TFile::Open(name_MC_all, "READ");
    if (!file_MC_all || file_MC_all->IsZombie()) {
      std::cerr << "Error: Cannot open MC norm file " << name_MC_all << std::endl;
      return;
    }
    TDirectoryFile* dir_Muons_MC_all = (TDirectoryFile*)file_MC_all->Get(name_dir_all);
    if (!dir_Muons_MC_all) {
      std::cerr << "Error: Cannot find directory " << name_dir_all << " in MC norm file." << std::endl;
      return;
    }

    TH1D* h_norm      = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights");
    TH1D* h_norm_cen  = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights_cen");
    TH1D* h_nev       = (TH1D*)dir_Muons_MC_all->Get("h_n_events");
    TH1D* h_cen_after = (TH1D*)dir_Muons_MC_all->Get("h_cen_after");

    double n_ev = h_nev->Integral(0, h_nev->GetNbinsX()+1);
    double sum_w = h_norm->Integral(0, h_norm->GetNbinsX()+1);
    double sum_ncoll = h_norm_cen->Integral(0, h_norm_cen->GetNbinsX()+1);
    double sum_w_and_ncoll = h_cen_after->Integral(0, h_cen_after->GetNbinsX()+1);

    std::cout << "n_ev = " << n_ev << " sum_w = " << sum_w << " sum_ncoll = " << sum_ncoll << std::endl;

    // Calculate Normalization Factors
    // Since TTree weights are ~Xsec [pb], we normalize by (Lumi / N_gen) if !isSignal, since we don't have sum_w total
    norm_MC_w_ncoll = isSignal ? number_A*number_A*Lumi*Xsec*(n_ev/sum_ncoll)/sum_w
                               : number_A*number_A*Lumi*(n_ev/sum_ncoll)/Ngen/1000; // :1000 since weights are ~Xsec in pb
    norm_MC_w = isPbPb ? ( isSignal ? number_A*number_A*Lumi*Xsec/sum_w
                                    : number_A*number_A*Lumi/Ngen/1000 )
                       : ( isSignal ? Lumi*Xsec/sum_w
                                    : ( isAlternative ? Lumi*Xsec/sum_w_alternative : Lumi/Ngen ) );

    std::cout << "norm_MC_w = " << norm_MC_w << " norm_MC_w_ncoll = " << norm_MC_w_ncoll << std::endl;
  }
  std::cout << "------------------------------------------------" << std::endl;

  // --- Add files to chains ---
  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    //data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzerSubstructure/t");
    if (isPbPb) data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    else data.Add(TString(globlist.gl_pathv[i]) + "/ak2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/muonAnalyzer/MuonTree");
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
    skimanalysis.Add(TString(globlist.gl_pathv[i]) + "/skimanalysis/HltTree");
    if (isPbPb) hiFJRhoAnalyzerFinerBins.Add(TString(globlist.gl_pathv[i]) + "/hiFJRhoAnalyzerFinerBins/t");
    hltanalysis.Add(TString(globlist.gl_pathv[i]) + "/hltanalysis/HltTree");
  }
  globfree(&globlist);

  // To associate additional TTrees with a primary TTree.
  // This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  data.AddFriend("HiTree");
  data.AddFriend("skimanalysis");
  if (isPbPb) data.AddFriend("hiFJRhoAnalyzerFinerBins");
  data.AddFriend("hltanalysis");

  // Calculate Total Events BEFORE initializing the Reader
  Long64_t total_events = data.GetEntries();

  // Initialize Reader AFTER touching the Chain
  TTreeReader fReader(&data);

  // Declaration of leaf types
  TTreeReaderValue<Int_t> run = {fReader, "run"};    // Run number
  TTreeReaderValue<Int_t> evt = {fReader, "evt"};    // Event number
  TTreeReaderValue<Int_t> lumi = {fReader, "lumi"};  // Luminosity block
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderValue<Float_t> hiHF = {fReader, "hiHF"};
  //TTreeReaderValue<float> Ncoll = {fReader, "Ncoll"}; // Ncoll

  // Declare leaves as pointers initialized to nullptr
  TTreeReaderValue<Int_t>* hiBin = nullptr;
  TTreeReaderValue<Float_t>* weight = nullptr;
  TTreeReaderArray<double>* rho = nullptr;
  TTreeReaderArray<double>* etaMin = nullptr; // Eta bin edges for jet rho calculation
  TTreeReaderArray<double>* etaMax = nullptr;
  if (isPbPb) {
    hiBin = new TTreeReaderValue<Int_t>(fReader, "hiBin"); // centralityx2, not used in ppref
    rho    = new TTreeReaderArray<double>(fReader, "rho");
    etaMin = new TTreeReaderArray<double>(fReader, "etaMin");
    etaMax = new TTreeReaderArray<double>(fReader, "etaMax");
  }
  if (!isData) weight = new TTreeReaderValue<Float_t>(fReader, "weight"); // MC event weight, not used in data

  // Filters
  TTreeReaderValue<int>* pprimaryVertexFilter = nullptr;
  TTreeReaderValue<int>* pclusterCompatibilityFilter = nullptr;
  TTreeReaderValue<int>* pphfCoincFilter2Th4 = nullptr;
  TTreeReaderValue<int>* goodvertex = nullptr;
  if (isPbPb) {
    pprimaryVertexFilter = new TTreeReaderValue<int>(fReader, "pprimaryVertexFilter");
    pclusterCompatibilityFilter = new TTreeReaderValue<int>(fReader, "pclusterCompatibilityFilter");
    pphfCoincFilter2Th4 = new TTreeReaderValue<int>(fReader, "pphfCoincFilter2Th4");
  }
  else goodvertex = new TTreeReaderValue<int>(fReader, "goodvertex");

  //Trigger for MC, no needed in data because already in production
  TTreeReaderValue<Int_t>* HLT_L2SingleMu = nullptr;
  if (!isData) {
    if (collision_name.Contains("PbPb23")) HLT_L2SingleMu = new TTreeReaderValue<Int_t>(fReader, "HLT_HIL2SingleMu7_v3");
    else if (collision_name.Contains("PbPb24")) HLT_L2SingleMu = new TTreeReaderValue<Int_t>(fReader, "HLT_HIL2SingleMu7_v7");
    else if (collision_name.Contains("ppref24")) HLT_L2SingleMu = new TTreeReaderValue<Int_t>(fReader, "HLT_PPRefL2SingleMu7_v6");
  }

  // Muon
  TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
  TTreeReaderArray<Float_t> recoPt = {fReader, "recoPt"};
  TTreeReaderArray<Float_t> recoEta = {fReader, "recoEta"};
  TTreeReaderArray<Float_t> recoPhi = {fReader, "recoPhi"};
  TTreeReaderArray<Int_t> recoCharge = {fReader, "recoCharge"};
  TTreeReaderArray<bool> recoIDTight = {fReader, "recoIDTight"};
  TTreeReaderArray<bool>* recoMVAIsoWP95 = nullptr;
  TTreeReaderArray<Float_t>* recoMVAIso = nullptr;

  // --- ADDED THIS BLOCK FOR PP ISOLATION BRANCHES ---
  TTreeReaderArray<Float_t>* recoPFChIso = nullptr;
  TTreeReaderArray<Float_t>* recoPFNeuIso = nullptr;
  TTreeReaderArray<Float_t>* recoPFPhoIso = nullptr;
  TTreeReaderArray<Float_t>* recoPFPUIso = nullptr;
  // --------------------------------------------------

  if (isPbPb) {
      recoMVAIsoWP95 = new TTreeReaderArray<bool>(fReader, "recoMVAIsoWP95");
      recoMVAIso = new TTreeReaderArray<Float_t>(fReader, "recoMVAIso");
  }
  else {
      // --- ADDED THIS BLOCK FOR PP ISOLATION INITIALIZATION ---
      recoPFChIso = new TTreeReaderArray<Float_t>(fReader, "recoPFChIso");
      recoPFNeuIso = new TTreeReaderArray<Float_t>(fReader, "recoPFNeuIso");
      recoPFPhoIso = new TTreeReaderArray<Float_t>(fReader, "recoPFPhoIso");
      recoPFPUIso = new TTreeReaderArray<Float_t>(fReader, "recoPFPUIso");
      // --------------------------------------------------------
  }
  const double muonMass = 0.1056583755; //From PDG 2024

  // --- Load Muon Scale Factors from JSON ---
  std::cout << "Loading Muon Scale Factors from JSON..." << std::endl;
  CorrectionSF tightID_SF;
  CorrectionSF hlt_SF;
  CorrectionSF iso_SF;

  std::string json_filename;
  if (collision_name.Contains("PbPb23")) json_filename = "HLT_HIL2SingleMu7_and_TightID_abseta1_pt1_cutAndCount_schemaV2.json";
  else if (collision_name.Contains("PbPb24")) json_filename = "HLT_HIL2SingleMu7_and_TightID_abseta1_pt1_cutAndCount_schemaV2.json"; // !!! to update
  else if (collision_name.Contains("ppref24")) json_filename = "HLT_HIL2SingleMu7_and_TightID_abseta1_pt1_cutAndCount_schemaV2.json"; // !!! to update
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
    std::cout << "Loaded '" << json_filename << "'" << std::endl;
    for (const auto& corr : sf_data["corrections"]) {
      std::string name = corr["name"];
      if (name == "NUM_TightID_DEN_genTracks") {
        tightID_SF.load(corr["data"]);
        tightID_loaded = true;
        std::cout << "Loaded '" << name << "'" << std::endl;
      } else if (name == "NUM_HLT_HIL2SingleMu7_v_DEN_TightID") {
        hlt_SF.load(corr["data"]);
        hlt_loaded = true;
        std::cout << "Loaded '" << name << "'" << std::endl;
      }
    }
  }

  if (!tightID_loaded || !hlt_loaded) {
    std::cerr << "Error: Failed to load required corrections from JSON." << std::endl;
    if (!tightID_loaded) std::cerr << "  'NUM_TightID_DEN_genTracks' was not found." << std::endl;
    if (!hlt_loaded) std::cerr << "  'NUM_HLT_HIL2SingleMu7_v_DEN_TightID' was not found." << std::endl;
    return;
  }

  // --- ADDED THIS BLOCK TO LOAD ISO SF ---
  std::string iso_json_filename = "NUM_Iso_DEN_TightID_abseta_pt_schemaV2.json"; // <--- CHANGED FILENAME
  std::ifstream iso_json_file(iso_json_filename);
  if (iso_json_file.is_open()) {
      json iso_data;
      try {
          iso_data = json::parse(iso_json_file);
          for (const auto& corr : iso_data["corrections"]) {
              // Extract ONLY the WP95 scale factors, ignoring WP80, WP85, etc.
              if (corr["name"] == "NUM_IsoWP95_DEN_TightID") {
                  iso_SF.load(corr["data"]);
                  std::cout << "Loaded 'NUM_IsoWP95_DEN_TightID' SFs from " << iso_json_filename << std::endl;
              }
          }
      } catch (json::parse_error& e) {
          std::cerr << "[Error] Failed to parse Isolation JSON file: " << iso_json_filename << std::endl;
          std::cerr << e.what() << std::endl;
      }
  } else {
      std::cerr << "Warning: Could not open Isolation JSON file: " << iso_json_filename << std::endl;
  }
  // ----------------------------------------

  std::cout << "JSON Scale Factors loaded successfully." << std::endl;
  // --- End Load Muon Scale Factors from JSON ---

  // Jet
  TTreeReaderValue<Int_t> nref = {fReader, "nref"};
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};
  TTreeReaderArray<Float_t> jtpt = {fReader, "jtpt"};
  TTreeReaderArray<Float_t> jtPfCEF = {fReader, "jtPfCEF"};
  TTreeReaderArray<Float_t> jtPfNEF = {fReader, "jtPfNEF"};
  TTreeReaderArray<Float_t> jtPfMUF = {fReader, "jtPfMUF"};
  //TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};
  //TTreeReaderArray<Float_t> jtgirth = {fReader, "jt_girth"};
  //TTreeReaderArray<Float_t> jtdyndeltaR = {fReader, "jtdyn_deltaR"};

  // pp-specific variables for id + veto map (pointers to avoid crash in PbPb) ---
  TTreeReaderArray<Float_t>* jtPfCHF = nullptr;
  TTreeReaderArray<Float_t>* jtPfNHF = nullptr;
  TTreeReaderArray<Int_t>* jtPfCHM = nullptr;
  TTreeReaderArray<Int_t>* jtPfNHM = nullptr;
  TTreeReaderArray<Int_t>* jtPfCEM = nullptr;
  TTreeReaderArray<Int_t>* jtPfNEM = nullptr;
  TTreeReaderArray<Int_t>* jtPfMUM = nullptr;

  if (!isPbPb) {
      jtPfCHF = new TTreeReaderArray<Float_t>(fReader, "jtPfCHF");
      jtPfNHF = new TTreeReaderArray<Float_t>(fReader, "jtPfNHF");
      jtPfCHM = new TTreeReaderArray<Int_t>(fReader, "jtPfCHM");
      jtPfNHM = new TTreeReaderArray<Int_t>(fReader, "jtPfNHM");
      jtPfCEM = new TTreeReaderArray<Int_t>(fReader, "jtPfCEM");
      jtPfNEM = new TTreeReaderArray<Int_t>(fReader, "jtPfNEM");
      jtPfMUM = new TTreeReaderArray<Int_t>(fReader, "jtPfMUM");
  }

  // JEC and JER
  vector<string> Files;
  // L2Relative is applied to BOTH Data and MC (the two files are actually identical)
  // L2Residual applied only to Data
  if (isData) {
    if (collision_name.Contains("PbPb23")) {
      Files.push_back("Spring23Prompt23_PbPb_V1_DATA_L2Relative_AK2PF.txt");
      Files.push_back("Spring23Prompt23_PbPb_V1_DATA_L2Residual_AK2PF.txt");
    }
    else if (collision_name.Contains("PbPb24")) {
     Files.push_back("PbPb_2024_noPUcorr_L2Relative_AK4PF.txt"); // !!! to update
     // Missing L2Residual
    }
    else if (collision_name.Contains("ppref24")) {
      std::cout << "Warning!!! Using jtpt instead of JEC for ppref" << endl;
      // 1. Pileup Correction
      Files.push_back("Spring18_ppRef5TeV_V6_DATA_L1FastJet_AK2PF.txt");
      // 2. Relative Response (or MC truth)
      Files.push_back("Spring18_ppRef5TeV_V6_DATA_L2Relative_AK2PF.txt");
      // 3. Absolute Response
      Files.push_back("Spring18_ppRef5TeV_V6_DATA_L3Absolute_AK2PF.txt");
      // 4. Data Residuals (Only for Data)
      Files.push_back("Spring18_ppRef5TeV_V6_DATA_L2L3Residual_AK2PF.txt");
    }
  }
  else {
    if (collision_name.Contains("PbPb23")) {
      Files.push_back("Spring23Prompt23_PbPb_V1_MC_L2Relative_AK2PF.txt");
    }
    else if (collision_name.Contains("PbPb24")) {
      Files.push_back("PbPb_2024_noPUcorr_L2Relative_AK4PF.txt"); // !!! to update
    }
    else if (collision_name.Contains("ppref24")) { /// !!! old
      std::cout << "Warning!!! Using jtpt instead of JEC for ppref" << endl;
      Files.push_back("Spring18_ppRef5TeV_V6_MC_L1FastJet_AK2PF.txt");
      Files.push_back("Spring18_ppRef5TeV_V6_MC_L2Relative_AK2PF.txt");
      Files.push_back("Spring18_ppRef5TeV_V6_MC_L3Absolute_AK2PF.txt");
      // MC does NOT use Residuals
    }
  }
  JetCorrector JEC(Files);
  // --- For debugging ---
  cout << "Initializing JEC..." << endl;
  for (const auto& file : Files) {
      cout << "Loaded JEC File: " << file << endl;
  }
  // ---------------------
  std::string name_Uncertainty_file;
  if (collision_name.Contains("PbPb23")) name_Uncertainty_file = "Autumn18_HI_V8_MC_Uncertainty_AK2PF.txt"; //!!! Old, update
  else if (collision_name.Contains("PbPb24")) name_Uncertainty_file = "Autumn18_HI_V8_MC_Uncertainty_AK2PF.txt"; //!!! Old, update
  else if (collision_name.Contains("ppref24")) name_Uncertainty_file = "Spring18_ppRef5TeV_V6_MC_Uncertainty_AK2PF.txt"; //!!! Old, update
  JetUncertainty JEU(name_Uncertainty_file);
  std::cout << "Loaded JEC Uncertainty File: " << name_Uncertainty_file << endl;

  // --- Initialize JER Provider ---
  JERProvider jer;
  if (!isData) {
    // Is MC
    cout << "Initializing JER..." << endl;
    // Load both SF and Resolution Files
    std::string jer_sf_file = "";
    std::string jer_res_file = "";
    // Define files based on collision type
    if (collision_name.Contains("PbPb23")) {
      jer_sf_file = "Autumn18_RunD_V7b_MC_SF_AK4PF.txt";  //!!! Old, update
      jer_res_file = "Autumn18_RunD_V7b_MC_PtResolution_AK4PF.txt"; //!!! Old, update
    }
    else if (collision_name.Contains("PbPb24")) {
      jer_sf_file = "Autumn18_RunD_V7b_MC_SF_AK4PF.txt"; //!!! Old, update
      jer_res_file = "Autumn18_RunD_V7b_MC_PtResolution_AK4PF.txt"; //!!! Old, update
    }
    else if (collision_name.Contains("ppref24")) {
      jer_sf_file = "Fall17_V3b_MC_SF_AK4PF.txt"; //!!! Old, update
      jer_res_file = "Fall17_V3b_MC_PtResolution_AK4PF.txt"; //!!! Old, update
    }
    // Print and Load
    if (!jer_sf_file.empty()) {
        std::cout << "Loading JER SF: " << jer_sf_file << std::endl;
        jer.LoadSF(jer_sf_file);
    }
    if (!jer_res_file.empty()) {
        std::cout << "Loading JER Resolution: " << jer_res_file << std::endl;
        jer.LoadResolution(jer_res_file);
    }
    // Note: We typically don't apply Phi/Eta smearing for standard analysis, so we only load PtResolution.
  }

  // Pre-calculate GenJet Vectors for easier passing to JER function
  // Declare BEFORE the event loop
  std::vector<float> v_gen_pts, v_gen_etas, v_gen_phis;
  v_gen_pts.reserve(100); // Reserve memory once to avoid re-allocations, reserve(100) does not set a hard limit.
  v_gen_etas.reserve(100);
  v_gen_phis.reserve(100);

  // Initialize Jet Selector as pointers
  JetSelect* js_PbPb = nullptr;
  JetSelect_pp* js_pp = nullptr; // Assumes class name is JetSelect_pp

  // Load specific map file
  if (isPbPb) {
    js_PbPb = new JetSelect("Winter24Prompt24_2024BCDEFGHI.root");
  } else {
    js_pp = new JetSelect_pp("Winter24Prompt24_2024BCDEFGHI.root");
  }
  std::cout << "------------------------------------------------" << std::endl;

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
  TTree *inputTree;
  if (isPbPb) {
    inputTree = (TTree*)inFile_MinBias->Get("jet_tree");
    if (!inputTree) {
      std::cerr << "Error: Could not find TTree 'jet_tree' in the input file!" << std::endl;
      inFile_MinBias->Close();
      return;
    }
  }
  else {inputTree = nullptr;}

  // Declare variables to hold the branch data
  Float_t HF_MinBias, vz_MinBias, jet_pt_MinBias, jet_phi_MinBias, jet_eta_MinBias;
  Int_t hiBin_MinBias, bin_MinBias;
  // Set branch addresses to link variables to tree branches
  if (isPbPb) {
    inputTree->SetBranchAddress("HF_MinBias", &HF_MinBias);
    inputTree->SetBranchAddress("vz_MinBias", &vz_MinBias);
    inputTree->SetBranchAddress("hiBin_MinBias", &hiBin_MinBias); // Link new branch
    inputTree->SetBranchAddress("bin_MinBias", &bin_MinBias);
    inputTree->SetBranchAddress("jet_pt_MinBias", &jet_pt_MinBias);
    inputTree->SetBranchAddress("jet_phi_MinBias", &jet_phi_MinBias);
    inputTree->SetBranchAddress("jet_eta_MinBias", &jet_eta_MinBias);

    // --- OPTIMIZATION START: Cache MinBias events into RAM ---
    std::cout << "Caching MinBias events into memory..." << std::endl;

    std::cout << "Found " << inputTree->GetEntries() << " MinBias entries in 'jet_tree'." << std::endl;
    std::cout << "Caching into memory... " << std::flush; // Flush ensures text appears immediately

  }
  std::map<int, std::vector<MinBiasJetInfo>> minBiasCache; // Map: Key = Bin ID, Value = Vector of jets in that bin
  std::map<int, int> mb_counts;
  if (isPbPb) {
    // Read the MinBias tree exactly ONCE
    for(int iEntry=0; iEntry < inputTree->GetEntries(); iEntry++){
      inputTree->GetEntry(iEntry);
      // Pre-count the MinBias events before event loop, count how many events you actually have for each bin.
      mb_counts[bin_MinBias]++;
      // Store only the necessary info
      MinBiasJetInfo info;
      info.pt = jet_pt_MinBias;
      info.eta = jet_eta_MinBias;
      info.phi = jet_phi_MinBias;

      // Push into the specific bin vector
      minBiasCache[bin_MinBias].push_back(info);
    }
  std::cout << "Done." << std::endl;
  std::cout << "Cached " << minBiasCache.size() << " unique bins." << std::endl;
  std::cout << "Caching complete " << std::endl;
  // --- OPTIMIZATION END ---
  }

  // --- Bin definition for MinBias matching (based on selected option) ---
  std::vector<std::pair<float, float>> primary_bins; // For HF or VZ only
  std::vector<std::pair<float, float>> vz_bins_combined;     // For combined VZ+Centrality
  std::vector<std::pair<float, float>> centrality_bins_combined; // For combined VZ+Centrality

  int total_mixed_bins_expected = 0; // Total expected bins for the selected option in MinBias
  int events_per_mixed_bin_limit = 0; // The max events collected per bin in MinBias

  if (isPbPb) {
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
      // --- FIXED: Calculate bins based on user arguments ---
      float min_hiBin_limit = cent_min * 2;
      float max_hiBin_limit = cent_max * 2;

      // Define how many bins to divide the range into. 
      // If you want 1% bins, the number of bins is just (cent_max - cent_min)
      int n_cen_bins = (cent_max - cent_min); 
      float cen_bin_width_hiBin = (max_hiBin_limit - min_hiBin_limit) / n_cen_bins;

      for (int i = 0; i < n_cen_bins; ++i) {
        float min_hiBin_edge = min_hiBin_limit + i * cen_bin_width_hiBin;
        float max_hiBin_edge = min_hiBin_edge + cen_bin_width_hiBin;
        centrality_bins_combined.push_back({min_hiBin_edge, max_hiBin_edge});
      }

      // Vz bins based on explicit edges
      for (int i = 0; i < BinningConfig_Combined_Vz_Centrality::num_vz_bins; ++i) {
        vz_bins_combined.push_back({BinningConfig_Combined_Vz_Centrality::vz_bin_edges[i], BinningConfig_Combined_Vz_Centrality::vz_bin_edges[i+1]});
      }

      total_mixed_bins_expected = n_cen_bins * BinningConfig_Combined_Vz_Centrality::num_vz_bins;
      events_per_mixed_bin_limit = BinningConfig_Combined_Vz_Centrality::ev_per_combined_bin;
      std::cout << "Defined combined Centrality-VZ bins for MinBias matching (" << total_mixed_bins_expected << " total)." << std::endl;
    } else {
      std::cerr << "Invalid binning_option: " << binning_option << std::endl;
      return;
    }
    std::cout << "------------------------------------------------" << std::endl;
  }
  // --- End of bin definition ---

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
  const double pi_value = std::acos(-1);;
  // Define binning for xZj unfolding.
  const int nbins_xZj = 6; // Number of bins (number of edges - 1)
  double xZj_bins[nbins_xZj + 1] = {0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 2.};
  const int nbins_xZj_meas = (systFlag != 8) ? nbins_xZj : nbins_xZj-1;

  // Define the bins using a vector so we can initialize conditionally
  std::vector<double> xZj_bins_meas_vec;
  if (systFlag != 8) {
    xZj_bins_meas_vec = {0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 2.};
  } else {
    xZj_bins_meas_vec = {0.5, 0.7, 0.9, 1.1, 1.3, 1.5};
  }

  // Create a pointer to the vector's data (compatible with TH1D constructors)
  double* xZj_bins_meas = xZj_bins_meas_vec.data();
  //  double xZj_max;

  TH1D *h_mumu = new TH1D("h_mumu", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1D *h_Z_pt = new TH1D("h_Z_pt", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1D *h_njet = new TH1D("h_njet", "Hist;Number of jets; Entries", 10, 0, 10);
  TH1D *h_cen = new TH1D("h_cen", "Hist; centrality bin; Entries", 20, 0, 100);

  TH1D *h_mumu_j = new TH1D("h_mumu_j", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1D *h_Z_pt_j = new TH1D("h_Z_pt_j", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1D *h_jet_pt_lj = new TH1D("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1D *h_jet_pt_lj_nocut = new TH1D("h_jet_pt_lj_nocut", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1D *h_jet_pt_lj_2pi_3 = new TH1D("h_jet_pt_lj_2pi_3", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1D *h_cen_j = new TH1D("h_cen_j", "Hist; centrality bin; Entries", 20, 0, 100);
  TH1D *h_HF_j = new TH1D("h_HF_j", "Hist; HF; Entries", 80, 0, 8000);
  TH1D *h_deltaPhi_Zj = new TH1D("h_deltaPhi_Zj", "Hist;#Delta#phi_{Zj}; Entries", 20, 0, pi_value);
  TH1D *h_xZj = new TH1D("h_xZj", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D *h_xZj_fixbinw = new TH1D("h_xZj_fixbinw", "Hist;x_{Zj}; Entries", 30, 0., 3.);
  TH2F *h_jet_etaphi_before = new TH2F("h_jet_etaphi_before", "Jets Before Veto;#eta;#phi", 40, -2.5, 2.5, 40, -pi_value, pi_value);
  TH2F *h_jet_etaphi_after  = new TH2F("h_jet_etaphi_after",  "Jets After Veto;#eta;#phi",  40, -2.5, 2.5, 40, -pi_value, pi_value);
  TH1D *h_muon_iso_nocut = new TH1D("h_muon_iso_nocut", "Muon Isolation (before cut); recoMVAIso; Entries", 20, 0, 1.0);

  TH1D *h_deltaPhi_Zj_all = new TH1D("h_deltaPhi_Zj_all", "Hist;#Delta#phi_{Zj} (All Jets); Entries", 20, 0, pi_value);
  TH1D *h_jet_pt_all = new TH1D("h_jet_pt_all", "Hist;inclusive jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1D *h_xZj_all = new TH1D("h_xZj_all", "Hist;x_{Zj} (All Jets); Entries", nbins_xZj_meas, xZj_bins_meas);

  TH1D *h_vz = new TH1D("h_vz", "Hist; vz; Entries", 30, -20, 20);
  TH1D *h_avg_rho = new TH1D("h_avg_rho", "Hist; <#rho>; Entries", 50, 0, 400);
  auto *h_avg_rho_vs_cen = new TProfile("h_avg_rho_vs_cen", "Profile of <#rho> vs centrality bin", 200, 0, 200, 0, 400);

  TH1D *h_deltaPhi_Zj_MinBias = new TH1D("h_deltaPhi_Zj_MinBias", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,pi_value);
  TH1D *h_jet_pt_lj_MinBias = new TH1D("h_jet_pt_lj_MinBias", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1D *h_xZj_MinBias = new TH1D("h_xZj_MinBias", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  TH1D *h_deltaPhi_Zj_matched = new TH1D("h_deltaPhi_Zj_matched", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,pi_value);
  TH1D *h_jet_pt_lj_matched = new TH1D("h_jet_pt_lj_matched", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1D *h_xZj_matched = new TH1D("h_xZj_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  // --- RooUnfold Histograms ---

  TH1D *h_deltaR_gen_reco = new TH1D("h_deltaR_gen_reco", "Matching Distance;#DeltaR(Reco, Gen);Entries", 50, 0, 0.5);

  TH1D* h_xZj_true = new TH1D("h_xZj_true", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);     // For true MC
  TH1D *h_xZj_for_JEWEL_w = new TH1D("h_xZj_for_JEWEL_w", "True x_{Zj};Entries", 60, 0.,3.);
  TH1D* h_mumu_true = new TH1D("h_mumu_true", "True m;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1D* h_xZj_reco = new TH1D("h_xZj_reco", "Reco x_{Zj};x_{Zj};Entries", nbins_xZj_meas, xZj_bins_meas);
  TH2F* h_response = new TH2F("h_response", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_unmatched = new TH2F("h_response_unmatched", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas,nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias = new TH2F("h_response_MinBias", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  TH1D *h_xZj_train_closure = new TH1D("h_xZj_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D *h_xZj_train_closure_matched = new TH1D("h_xZj_train_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D *h_xZj_test_closure = new TH1D("h_xZj_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D *h_xZj_test_closure_matched = new TH1D("h_xZj_test_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D *h_xZj_MinBias_train_closure = new TH1D("h_xZj_MinBias_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D *h_xZj_MinBias_test_closure = new TH1D("h_xZj_MinBias_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1D* h_xZj_true_train_closure = new TH1D("h_xZj_true_train_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH1D* h_xZj_true_test_closure = new TH1D("h_xZj_true_test_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH2F* h_response_closure = new TH2F("h_response_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_closure_unmatched = new TH2F("h_response_closure_unmatched", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias_closure = new TH2F("h_response_MinBias_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  // --- End RooUnfold Histograms ---

  // --- Z Acceptance Histograms for Theorists ---
  TH1D* h_Z_eta_gen_total = new TH1D("h_Z_eta_gen_total", "Gen Z Eta (All);#eta^{Z};Entries", 30, -3.0, 3.0);
  TH1D* h_Z_eta_gen_accepted = new TH1D("h_Z_eta_gen_accepted", "Gen Z Eta (Accepted);#eta^{Z};Entries", 30, -3.0, 3.0);

  //TH1D *h_jetgirth = new TH1D("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1D *h_jet_deltaR = new TH1D("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Output root file
  TFile *file_output_HI_mu;

  TString name_output = "HI";
  if (collision_name.Contains("PbPb24")) name_output = "HI24";
  else if (collision_name.Contains("ppref24")) name_output = "ppref";

  if (isData && weight_phase != 0) {
    if (systFlag == 0) file_output_HI_mu = new TFile("./plot/output_"+name_output+"_mu_data" + run_tag + ".root", "RECREATE");
    if (isPbPb) {
      if (systFlag == 6) {
        cout << "Running Systematic (Centrality) down-variation (systFlag = 6)" << endl;
        file_output_HI_mu = new TFile("./syst_cen/output_"+name_output+"_mu_data_cen_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 7) {
        cout << "Running Systematic (Centrality) up-variation (systFlag = 7)" << endl;
        file_output_HI_mu = new TFile("./syst_cen/output_"+name_output+"_mu_data_cen_up" + run_tag + ".root", "RECREATE");
      }
    }
    if (systFlag == 8) {
      cout << "Running binning variation (systFlag = 8)" << endl;
      file_output_HI_mu = new TFile("./syst_binning/output_"+name_output+"_mu_data_binning" + run_tag + ".root", "RECREATE");
    }
  }

  if (weight_phase == 0 && isPbPb) {
  cout << "Running weight_phase " << weight_phase << " for computing Ncoll_weights" << endl;
    if (isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_"+name_output+"_mu_data_Ncoll_weights" + run_tag + ".root", "RECREATE");
    }
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_"+name_output+"_mu_MC_Ncoll_weights" + run_tag + ".root", "RECREATE");
    }
  }

  if (weight_phase == 1 && isPbPb) {
    if (!isData) {
      cout << "Running weight_phase " << weight_phase << " for computing rho_weights" << endl;
      file_output_HI_mu = new TFile("./weights_MC/rho_weights_1/output_"+name_output+"_mu_MC_rho_weights" + run_tag + ".root", "RECREATE");
    }
  }

  // This is just for plotting rho distributions after reweighting
  if (weight_phase == -1 && isPbPb) {
    if (!isData) {
      cout << "Running weight_phase " << weight_phase << " for plotting rho distributions after reweighting" << endl;
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_"+name_output+"_mu_MC_rho_weights_after" + run_tag + ".root", "RECREATE");
    }
  }

  if (weight_phase == 2) {
    if (!isData) {
      cout << "Running weight_phase " << weight_phase << " for computing vz_weights" << endl;
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_"+name_output+"_mu_MC_vz_weights" + run_tag + ".root", "RECREATE");
    }
  }

  else if (weight_phase == 3) {
    if (!isData) {
      if (systFlag == 0) file_output_HI_mu = new TFile("./plot/output_"+name_output+"_mu_MC_"+file_name + run_tag + ".root", "RECREATE");
      else if (systFlag == 1) {
        cout << "Running Systematic (SF ID) - DOWN variation (systFlag = 1)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_"+name_output+"_mu_MC_SF_ID_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 2) {
        cout << "Running Systematic (SF ID) - UP variation (systFlag = 2)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_"+name_output+"_mu_MC_SF_ID_up" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 4) {
        cout << "Running Systematic (Prior model) variation (systFlag = 4)" << endl;
        file_output_HI_mu = new TFile("./syst_prior_model/output_"+name_output+"_mu_MC_prior_model" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 8) {
        cout << "Running  binning variation (systFlag = 8)" << endl;
        file_output_HI_mu = new TFile("./syst_binning/output_"+name_output+"_mu_MC_binning" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 9) {
        cout << "Running Systematic JEC - DOWN variation (systFlag = 9)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_"+name_output+"_mu_MC_JEC_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 10) {
        cout << "Running Systematic JEC - UP variation (systFlag = 10)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_"+name_output+"_mu_MC_JEC_up" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 11) {
        cout << "Running Systematic JER - DOWN variation (systFlag = 11)" << endl;
        file_output_HI_mu = new TFile("./syst_JER/output_"+name_output+"_mu_MC_JER_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 12) {
        cout << "Running Systematic JER - UP variation (systFlag = 12)" << endl;
        file_output_HI_mu = new TFile("./syst_JER/output_"+name_output+"_mu_MC_JER_up" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 13) {
        cout << "Running Systematic shape - DOWN variation (systFlag = 13)" << endl;
        file_output_HI_mu = new TFile("./syst_shape/output_"+name_output+"_mu_MC_shape_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 14) {
        cout << "Running Systematic shape - UP variation (systFlag = 14)" << endl;
        file_output_HI_mu = new TFile("./syst_shape/output_"+name_output+"_mu_MC_shape_up" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 15) {
        cout << "Running Systematic (SF iso) - DOWN variation (systFlag = 15)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_"+name_output+"_mu_MC_Iso_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 16) {
        cout << "Running Systematic (SF iso) - UP variation (systFlag = 16)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_"+name_output+"_mu_MC_Iso_up" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 17) {
        cout << "Running Systematic (SF HLT) - DOWN variation (systFlag = 17)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_"+name_output+"_mu_MC_SF_HLT_down" + run_tag + ".root", "RECREATE");
      }
      else if (systFlag == 18) {
        cout << "Running Systematic (SF HLT) - UP variation (systFlag = 18)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_"+name_output+"_mu_MC_SF_HLT_up" + run_tag + ".root", "RECREATE");
      }
    }
  }
  std::cout << "File created: " << file_output_HI_mu->GetName() << std::endl;
  std::cout << "------------------------------------------------" << std::endl;

  // =================================================================================
  //   MAIN EVENT LOOP
  // =================================================================================

  unsigned int iEvent = 0;
  unsigned int itotev = 0;
  double weight_totev = 0;
  std::cout << "Starting Analysis Loop over " << total_events << " events..." << std::endl;
  int report_step = static_cast<unsigned int>(total_events / 100); // Update 100 times
  while (fReader.Next()) {
    // Clear GenJet Vectors
    v_gen_pts.clear();
    v_gen_etas.clear();
    v_gen_phis.clear();
    itotev++;
    if (!isData) weight_totev += (**weight); else weight_totev++;
    // --- Progress Bar ---
    if (itotev % report_step == 0 || itotev == 1) {
        double progress = 100.0 * itotev / total_events;
        // \r returns cursor to start of line, allowing us to overwrite the previous number
        std::cout << "\r[Analysis] Processing: " << itotev << " / " << total_events
                  << " (" << std::fixed << std::setprecision(2) << progress << "%)" << std::flush;
    }
    // --- end of Progress Bar ---

    // Stop at 1/10 of statistics, ONLY if it is MC and stop_early is true
    bool stop_early = false;
    if (stop_early && !isData && itotev >= total_events / 10) {
        std::cout << "\n\033[1;31mSTOPPING EARLY ACTIVATED\033[0m" << std::endl;
        break;
    }

    if (isPbPb) {
      if(**pprimaryVertexFilter<=0) continue;
      if(**pclusterCompatibilityFilter<=0) continue;
      if(**pphfCoincFilter2Th4<=0) continue;
    }
    else {
      if(**goodvertex<=0) continue;
    }
    //vz cut
    if (*vz < -15.0 || *vz > 15.0) continue;
    // Use hiHF to recalculate hiBin for systematics
    int hiBin_to_use = 1;
    if (isPbPb) hiBin_to_use = **hiBin; // Start with the nominal hiBin
    // Warning check (Comparison)
    if (isData && isPbPb) {
      int calculated_nominal = is2023 ? getHiBin(*hiHF, cenHF_2023PbPb_nominal)
                                      : getHiBin(*hiHF, cenHF_2024PbPb_nominal);

      if (**hiBin != calculated_nominal) {
        cout << "!!! WARNING: hiBin = " << **hiBin
             << " hiHF = " << *hiHF
             << " hiBin nominal = " << calculated_nominal
             << " hiBin up = " << getHiBin(*hiHF, cenHF_2023PbPb_up)
             << " hiBin down = " << getHiBin(*hiHF, cenHF_2023PbPb_down)
             << endl;
      }
    }
    // Systematics Assignment for centrality
    if (isData && isPbPb && systFlag == 6)
      hiBin_to_use = is2023 ? getHiBin(*hiHF, cenHF_2023PbPb_down)
                            : getHiBin(*hiHF, cenHF_2024PbPb_down);
    if (isData && isPbPb && systFlag == 7)
      hiBin_to_use = is2023 ? getHiBin(*hiHF, cenHF_2023PbPb_up)
                            : getHiBin(*hiHF, cenHF_2024PbPb_up);

    // Centrality weight
    float weight_cent = isPbPb ? Ncoll[hiBin_to_use] : 1;
    // Scale MC
    float scale = 1;
    if (stop_early && !isData) scale*=10;
    if (use_data_driven_cen == true) {
      // --- PURE DATA-DRIVEN PATH ---
      // No Ncoll, no centrality weights. 
      // Just the base cross-section and generator weight for all phases.
      if (!isData) {
        scale *= norm_MC_w * (**weight);
      }
    } else {
      // --- GLAUBER PATH ---
      if (isPbPb) {
        if (!isData && weight_phase == 0)
          scale*=norm_MC_w*(**weight);
        if (!isData && weight_phase != 0)
          scale*=norm_MC_w_ncoll*weight_cent*(**weight);
      }
      else {
        if (!isData) scale*=norm_MC_w*(**weight);
      }
    }

    // Selection on centrality bin only for PbPb
    if (isPbPb) {
      if (weight_phase > 1 ) {
        if (hiBin_to_use < cent_min * 2 || hiBin_to_use >= cent_max * 2) continue;
      }
      if (weight_phase == 1 || weight_phase == -1) {
        if (isData) {
          if (hiBin_to_use < cent_min * 2 || hiBin_to_use >= cent_max * 2) continue;
        }
      }
    }

    // Calculate average rho
    double sum_rho = 0;
    double avg_rho = 0;
    // Check if rho exists (pointer is not null)
    if (rho) {
      for (unsigned int i = 0; i < rho->GetSize(); i++) {
        if ((*rho)[i] > 0) sum_rho += (*rho)[i]; //rho[i] > 0 to skip invalid bins, anyway HiFJRhoProducer.cc initializes rho with 1e-6
      }
      if (rho->GetSize() > 0) avg_rho = sum_rho / rho->GetSize();
    }

    // use binning to get the value of the weight
    if (isPbPb) {
      if (!isData) {
        if (weight_phase == 2 || weight_phase == -1) {
          int bin_rho = h_weight_rho->FindBin(avg_rho);
          // Apply rho weight
          scale*=h_weight_rho->GetBinContent(bin_rho);
        }
        if (weight_phase == 3) {
         // Apply rho and vz weight
         int bin_rho = h_weight_rho->FindBin(avg_rho);
         int bin_vz = h_weight_vz->FindBin(*vz);
         scale*=h_weight_rho->GetBinContent(bin_rho)*h_weight_vz->GetBinContent(bin_vz);
        }
      }
    }
    else {
      if (!isData && weight_phase == 3) {
        // Apply only vz weight for ppref
        int bin_vz = h_weight_vz->FindBin(*vz);
        scale*=h_weight_vz->GetBinContent(bin_vz);
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
      for (unsigned int igenMu = 0; igenMu < genMuPt.GetSize(); ++igenMu) {
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
        // Z kinematic cuts (Mass and pT)
        if (gen_Z.M() >= 60 && gen_Z.M() <= 120 && gen_Z.Pt() >= ptZ_min && gen_Z.Pt() < ptZ_max) {
          // 1. Fill TOTAL Z bosons (before checking if muons hit the detector)
          h_Z_eta_gen_total->Fill(gen_Z.Eta(), scale);
          // Check if BOTH muons fall into CMS acceptance
          if (genmuMinus.Pt() >= 20 && abs(genmuMinus.Eta()) <= 2.4 && genmuPlus.Pt() >= 20 && abs(genmuPlus.Eta()) <= 2.4) {
            // 2. Fill ACCEPTED Z bosons (muons are seen by CMS)
            h_Z_eta_gen_accepted->Fill(gen_Z.Eta(), scale);
            h_mumu_true->Fill(gen_Z.M(), scale);
            // Loop over gen jets
            for (int ijetGen = 0; ijetGen < genpt.GetSize(); ++ijetGen) {
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
              if (dPhi_Zj_Gen > 7 * pi_value / 8) {
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
                if (itotev < 0.6*Ngen) h_xZj_true_train_closure->Fill(true_xZj, scale);
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
    if (!isData) {
      if(**HLT_L2SingleMu<=0) continue; // no needed for data because already in production
    }
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // TLorentzVectors for the muons
    TLorentzVector muPlus, muMinus;

    // Find Best Muon Pair (most energetic muon and antimuon)
    int iHighPtMu = -1;
    int iHighPtAntiMu = -1;
    //cout << "-----------------------------" << endl;
    for (unsigned int iMu = 0; iMu < *nReco; ++iMu) {
      if (recoIDTight[iMu]) {
        if (isPbPb && recoMVAIso) {
             h_muon_iso_nocut->Fill((*recoMVAIso)[iMu], scale);
        }
        if (isPbPb && !(*recoMVAIsoWP95)[iMu]) continue;
        // --- ADDED THIS BLOCK FOR PP ISOLATION ---
        if (!isPbPb && recoPFChIso && recoPFNeuIso && recoPFPhoIso && recoPFPUIso) {
            float chIso = (*recoPFChIso)[iMu];
            float neuIso = (*recoPFNeuIso)[iMu];
            float phoIso = (*recoPFPhoIso)[iMu];
            float puIso = (*recoPFPUIso)[iMu];
            float pt = recoPt[iMu];

            // Formula: (chHad + max(0, neutHad + phot - 0.5*PU)) / pt
            float relIso = (chIso + std::max(0.0f, neuIso + phoIso - 0.5f * puIso)) / pt;

            // Fill the histogram so you can check Data vs MC for ppref too!
            h_muon_iso_nocut->Fill(relIso, scale);

            // Apply Tight WP cut (0.15) for ~95% efficiency
            if (relIso > 0.15) continue;
        }
        // -----------------------------------------
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
      // 1. Define separate flags for each source
      std::string id_sf_flag  = (systFlag == 1) ? "systdown" : (systFlag == 2 ? "systup" : "nominal");
      std::string iso_sf_flag = (systFlag == 15) ? "systdown" : (systFlag == 16 ? "systup" : "nominal");
      std::string hlt_sf_flag = (systFlag == 17) ? "systdown" : (systFlag == 18 ? "systup" : "nominal");

      // 2. Apply TightID scale factors (using id_sf_flag) for both muons in the Z candidate
      double id_sf_mu_p = tightID_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], id_sf_flag);
      double id_sf_mu_m = tightID_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], id_sf_flag);
      scale *= id_sf_mu_p * id_sf_mu_m;

      // 3. Apply HLT scale factor (using hlt_sf_flag) combining them as SF1 + SF2 - (SF1 * SF2)
      // For trigger we only need one to lepton to have fired, so we use the addition rule of probability
      double hlt_sf_mu_p = hlt_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], hlt_sf_flag);
      double hlt_sf_mu_m = hlt_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], hlt_sf_flag);
      scale *= (hlt_sf_mu_p + hlt_sf_mu_m - (hlt_sf_mu_p * hlt_sf_mu_m));

      // 4. Apply ISO SF (using iso_sf_flag)
      double iso_sf_mu_p = iso_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], iso_sf_flag);
      double iso_sf_mu_m = iso_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], iso_sf_flag);
      scale *= iso_sf_mu_p * iso_sf_mu_m;
    }

    // Z from muon-antimuon pairs
    TLorentzVector Z = muPlus + muMinus;
    if (Z.M() < 60 || Z.M() > 120 || Z.Pt() < ptZ_min || Z.Pt() >= ptZ_max) continue;
    if (muMinus.Pt() < 20 || abs(muMinus.Eta()) > 2.4 || muPlus.Pt() < 20 || abs(muPlus.Eta()) > 2.4) continue;

    h_vz->Fill(*vz, scale);
    h_avg_rho->Fill(avg_rho, scale);
    h_avg_rho_vs_cen->Fill(hiBin_to_use, avg_rho, scale);
    h_cen->Fill((hiBin_to_use)/2, scale);
    h_mumu->Fill(Z.M(), scale);
    h_Z_pt->Fill(Z.Pt(), scale);

    // Fill GenJet Vectors pre-calcuted for easier passing to JER function
    if (!isData) {
      for (int i = 0; i < genpt.GetSize(); i++) {
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
//!!!    For ppref we use for now jtpt
      double CorrectedPT = isPbPb ? JEC.GetCorrectedPT() : jtpt[ijet];
      JEU.SetJetPT(CorrectedPT);
      JEU.SetJetEta(jteta[ijet]);
      JEU.SetJetPhi(jtphi[ijet]);
      double pt_jec_applied = CorrectedPT;

      if (!isData && systFlag == 9) pt_jec_applied = CorrectedPT * (1 - JEU.GetUncertainty().first); //down
      if (!isData && systFlag == 10) pt_jec_applied = CorrectedPT * (1 + JEU.GetUncertainty().second); //up
      //cout << "after JEC: jtpt_corr = " << pt_jec_applied << endl;

      // For JER, we need to find the rho value in the bin that contains the jet
      double jetRho = 1e-6; // Consider that in HiFJRhoProducer.cc, they initialize rho with value of 1e-6
      // Only run this if the pointers are valid
      if (rho && etaMin && etaMax) {
        for (unsigned int iEta = 0; iEta < etaMin->GetSize(); iEta++) {
          if (jteta[ijet] >= (*etaMin)[iEta] && jteta[ijet] < (*etaMax)[iEta]) {
            jetRho = (*rho)[iEta];
              break;
          }
        }
      }

      // Apply JER (Hybrid Method)
      double pt_final = pt_jec_applied;
      if (!isData) {
        int jer_syst = 0;
        // map systFlag 11/12 to JER Up/Down
        if (systFlag == 11) jer_syst = -1; // Down
        if (systFlag == 12) jer_syst = 1;  // Up

        pt_final = jer.GetSmearedPt(pt_jec_applied, jteta[ijet], jtphi[ijet], jetRho, v_gen_pts, v_gen_etas, v_gen_phis, jer_syst);
      }
      jtpt_corr[ijet] = pt_final;
      //jtpt_corr[ijet] = rawpt[ijet];

      // Selections
      if (jtpt_corr[ijet] < 30 || abs(jteta[ijet]) > 2.5) continue;
      // Apply Combined Jet ID and Veto Map
      h_jet_etaphi_before->Fill(jteta[ijet], jtphi[ijet], scale);
      bool passJetID = false;
      if (isPbPb) {
        passJetID = js_PbPb->JetSelection(jteta[ijet], jtphi[ijet], jtPfCEF[ijet], jtPfNEF[ijet], jtPfMUF[ijet]);
      } else {
        // For pp, we dereference the pointers (*ptr)[index]
        passJetID = js_pp->JetSelection(jteta[ijet], jtphi[ijet],
                                       (*jtPfCHF)[ijet], (*jtPfNHF)[ijet], jtPfCEF[ijet], jtPfNEF[ijet], jtPfMUF[ijet],
                                       (*jtPfCHM)[ijet], (*jtPfNHM)[ijet], (*jtPfCEM)[ijet], (*jtPfNEM)[ijet], (*jtPfMUM)[ijet]);
      }
      if (!passJetID) continue;
      h_jet_etaphi_after->Fill(jteta[ijet], jtphi[ijet], scale);
      // Z-Jet dR Cleaning
      if (getDeltaR(jteta[ijet], jtphi[ijet], muMinus.Eta(), muMinus.Phi()) < 0.2) continue;
      if (getDeltaR(jteta[ijet], jtphi[ijet], muPlus.Eta(), muPlus.Phi()) < 0.2) continue;
      njets++;

      // --- Inclusive Jet Kinematics ---
      double dPhi_Zj_current = RelativePhi(Z.Phi(), jtphi[ijet]);
      double xZj_current = jtpt_corr[ijet] / Z.Pt();

      // Fill delta phi for all valid jets
      h_deltaPhi_Zj_all->Fill(dPhi_Zj_current, scale);

      // Apply the exact same back-to-back cut as the leading jet
      if (dPhi_Zj_current > 7 * pi_value / 8) {
          h_jet_pt_all->Fill(jtpt_corr[ijet], scale);
          h_xZj_all->Fill(xZj_current, scale);
      }
      // --------------------------------

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
        for (int igenjet = 0; igenjet < genpt.GetSize(); igenjet++) {
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
          h_deltaR_gen_reco->Fill(min_dR, scale);
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
    h_njet->Fill(njets, scale);
    // --- end loop over jets ---

    // --- Reco leading jet selection ---
    if (ijetLeading != -1) {
      double dPhi_Zj = RelativePhi(Z.Phi(), jtphi[ijetLeading]);
      double xZj = jtpt_corr[ijetLeading]/Z.Pt();
      //Remove overflow and put it in the last bin
      //if (xZj > xZj_max) xZj = xZj_max - 0.01;
      h_deltaPhi_Zj->Fill(dPhi_Zj, scale);
      if (isPbPb) {
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

        // Mixing events with MinBias, assumes 'current_global_bin_n' and 'events_per_mixed_bin_limit' events for each bin
        if (current_global_bin_n != -1) { // Only proceed with MinBias matching if a valid bin was found
          // Check if this bin exists in our cache
          if (minBiasCache.count(current_global_bin_n)) {
            // Access the specific vector of jets for this bin directly
            const std::vector<MinBiasJetInfo>& cachedJets = minBiasCache[current_global_bin_n];
            double events_filled_for_this_bin_in_MinBias = 0;
            // Determine weight (using the size of the cached vector)
            double n_mix = mb_counts[current_global_bin_n]; // Or use cachedJets.size() if they are 1-to-1
            double mixing_weight = (n_mix > 0) ? (1.0 / n_mix) : 0.0;
            // Loop ONLY over the jets that belong to this bin
            for (const auto& mbJet : cachedJets) {
              // Use mbJet.pt, mbJet.eta, mbJet.phi instead of tree variables
              // Apply same jet cuts as for signal jets
              if (mbJet.pt > jtpt_corr[ijetLeading]) {
                if (getDeltaR(mbJet.eta, mbJet.phi, muMinus.Eta(), muMinus.Phi()) >= 0.2 &&
                    getDeltaR(mbJet.eta, mbJet.phi, muPlus.Eta(), muPlus.Phi()) >= 0.2) {
                  double dPhi_Zj_MinBias = RelativePhi(Z.Phi(), mbJet.phi);
                  double xZj_MinBias = mbJet.pt / Z.Pt();
                  h_deltaPhi_Zj_MinBias->Fill(dPhi_Zj_MinBias, scale * mixing_weight);
                  //Remove overflow and put it in the last bin
                  //if (xZj_MinBias > xZj_max) xZj_MinBias = xZj_max - 0.01;
                  if (dPhi_Zj_MinBias > 7 * pi_value / 8) {
                    h_jet_pt_lj_MinBias->Fill(mbJet.pt, scale * mixing_weight);
                    h_xZj_MinBias->Fill(xZj_MinBias, scale * mixing_weight);
                    if (itotev < 0.6*Ngen) h_xZj_MinBias_train_closure->Fill(xZj_MinBias, scale * mixing_weight);
                    else h_xZj_MinBias_test_closure->Fill(xZj_MinBias, scale * mixing_weight);
                    if (!isData && ijetGenLeading_unfold != -1 && dPhi_Zj_Gen > 7 * pi_value / 8) {
                      //if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
                      h_response_MinBias->Fill(xZj_MinBias, true_xZj, scale * mixing_weight);
                      if (itotev < 0.6*Ngen) h_response_MinBias_closure->Fill(xZj_MinBias, true_xZj, scale * mixing_weight);
                      //}
                    }
                  }
                }
              }
              events_filled_for_this_bin_in_MinBias++;
            }
            if (events_filled_for_this_bin_in_MinBias != events_per_mixed_bin_limit) {
            std::cout << "--- Warning! MinBias bin " << current_global_bin_n << " has only " << events_filled_for_this_bin_in_MinBias
                      << " events (expected " << events_per_mixed_bin_limit << "). ---" << std::endl;
            }
          }
        }
      } // end isPbPb

      if (!isData && isLeadingJetMatched) {
        double dPhi_Zj_matched = RelativePhi(Z.Phi(), jtphi[ijetLeading]);
        h_deltaPhi_Zj_matched->Fill(dPhi_Zj_matched, scale);
      }

      if (dPhi_Zj > 7 * pi_value / 8) {
        h_mumu_j->Fill(Z.M(), scale);
        h_Z_pt_j->Fill(Z.Pt(), scale);
        h_jet_pt_lj->Fill(jtpt_corr[ijetLeading], scale);
        h_xZj->Fill(xZj, scale);
        h_xZj_fixbinw->Fill(xZj, scale);
        if (!isData && ijetGenLeading_unfold != -1 && dPhi_Zj_Gen > 7 * pi_value / 8) {
          h_response_unmatched->Fill(xZj, true_xZj, scale);
          if (itotev < 0.6*Ngen) h_response_closure_unmatched->Fill(xZj, true_xZj, scale);
        }
        if (itotev < 0.6*Ngen) h_xZj_train_closure->Fill(xZj, scale);
        else h_xZj_test_closure->Fill(xZj, scale);
        h_cen_j->Fill((hiBin_to_use)/2, scale);
        h_HF_j->Fill(*hiHF, scale);

        if (!isData && isLeadingJetMatched) {
          h_jet_pt_lj_matched->Fill(jtpt_corr[ijetLeading], scale);
          h_xZj_matched->Fill(xZj, scale);
        }
      }

      if (dPhi_Zj > 2 * pi_value / 3) h_jet_pt_lj_2pi_3->Fill(jtpt_corr[ijetLeading], scale);
      h_jet_pt_lj_nocut->Fill(jtpt_corr[ijetLeading], scale);

      // --- Fill information for unfolding ---
      if (RelativePhi(Z.Phi(), jtphi[ijetLeading]) > 7 * pi_value / 8) {
        if (!isData && ijetGenLeading_unfold != -1 && dPhi_Zj_Gen > 7 * pi_value / 8) {
          // Calculate true x_Zj
          // Check if the leading gen jet is matched to leading reconstructed jet
          if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
            h_response->Fill(xZj, true_xZj, scale);
            h_xZj_reco->Fill(xZj, scale);
            if (itotev < 0.6*Ngen) {
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

  // Create a plot of "Efficiency vs DeltaR Cut"
  TH1 * h_eff_curve = h_deltaR_gen_reco->GetCumulative();
  h_eff_curve->Scale(1.0 / h_deltaR_gen_reco->Integral(0, h_deltaR_gen_reco->GetNbinsX() + 1));
  h_eff_curve->SetTitle("Matching Efficiency vs. #DeltaR Cut; #DeltaR Cut; Efficiency");

  // Finalize histograms by subtracting MinBias
  TH1D* h_deltaPhi_Zj_subtracted = (TH1D*)h_deltaPhi_Zj->Clone("h_deltaPhi_Zj_subtracted");
  h_deltaPhi_Zj_subtracted->SetDirectory(0);
  h_deltaPhi_Zj_subtracted->SetTitle("h_deltaPhi_Zj - h_deltaPhi_Zj_MinBias (rescaled)");
  h_deltaPhi_Zj_subtracted->Add(h_deltaPhi_Zj_MinBias, -1); // The -1 performs the subtraction


  TH1D* h_jet_pt_lj_subtracted = (TH1D*)h_jet_pt_lj->Clone("h_jet_pt_lj_subtracted");
  h_jet_pt_lj_subtracted->SetDirectory(0);
  h_jet_pt_lj_subtracted->SetTitle("h_jet_pt_lj - h_jet_pt_lj_MinBias (rescaled)");
  h_jet_pt_lj_subtracted->Add(h_jet_pt_lj_MinBias, -1); // The -1 performs the subtraction

  TH1D* h_xZj_subtracted = (TH1D*)h_xZj->Clone("h_xZj_subtracted");
  h_xZj_subtracted->SetDirectory(0);
  h_xZj_subtracted->SetTitle("h_xZj - h_xZj_MinBias (rescaled)");
  h_xZj_subtracted->Add(h_xZj_MinBias, -1); // The -1 performs the subtraction

  TH2F* h_response_subtracted = (TH2F*)h_response_unmatched->Clone("h_response_subtracted");
  h_response_subtracted->SetDirectory(0);
  h_response_subtracted->SetTitle("h_response_unmatched - h_response_MinBias (rescaled)");
  h_response_subtracted->Add(h_response_MinBias, -1); // The -1 performs the subtraction

  TH1D* h_xZj_train_closure_subtracted = (TH1D*)h_xZj_train_closure->Clone("h_xZj_train_closure_subtracted");
  h_xZj_train_closure_subtracted->SetDirectory(0);
  h_xZj_train_closure_subtracted->SetTitle("h_xZj_train_closure - h_xZj_MinBias_train_closure (rescaled)");
  h_xZj_train_closure_subtracted->Add(h_xZj_MinBias_train_closure, -1); // The -1 performs the subtraction

  TH1D* h_xZj_test_closure_subtracted = (TH1D*)h_xZj_test_closure->Clone("h_xZj_test_closure_subtracted");
  h_xZj_test_closure_subtracted->SetDirectory(0);
  h_xZj_test_closure_subtracted->SetTitle("h_xZj_test_closure - h_xZj_MinBias_test_closure (rescaled)");
  h_xZj_test_closure_subtracted->Add(h_xZj_MinBias_test_closure, -1); // The -1 performs the subtraction

  TH2F* h_response_closure_subtracted = (TH2F*)h_response_closure_unmatched->Clone("h_response_closure_subtracted");
  h_response_closure_subtracted->SetDirectory(0);
  h_response_closure_subtracted->SetTitle("h_response_closure_unmatched - h_response_MinBias_closure (rescaled)");
  h_response_closure_subtracted->Add(h_response_MinBias_closure, -1); // The -1 performs the subtraction

  // --- Output summary ---
  // Clear the progress bar line
  std::cout << "\r[Analysis] Processing: " << total_events << " / " << total_events << " (100.0%) - Complete." << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  std::cout << "--- Analysis Summary ---" << std::endl;
  std::cout << "Total Events Processed: " << itotev << " average_weight = " << weight_totev/itotev << std::endl;

  double raw_dPhi_integral = h_deltaPhi_Zj->Integral(0, h_deltaPhi_Zj->GetNbinsX()+1);
  double raw_pt_integral = h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1);

  std::cout << "[Raw Info]" << std::endl;
  std::cout << "Raw Integral (dPhi):   " << raw_dPhi_integral
            << "        Raw Integral (pT):    " << raw_pt_integral << std::endl;

  if (isPbPb) {
  double bkg_dPhi_integral = h_deltaPhi_Zj_MinBias->Integral(0, h_deltaPhi_Zj_MinBias->GetNbinsX()+1);
  double bkg_pt_integral = h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1);
  std::cout << "[Background Subtraction Info]" << std::endl;
  std::cout << "Bkg Integral (dPhi):   " << bkg_dPhi_integral
            << "        Bkg Integral (pT):    " << bkg_pt_integral << std::endl;
  std::cout << "Bkg Fraction (dPhi):   " << (raw_dPhi_integral > 0 ? 100*bkg_dPhi_integral/raw_dPhi_integral : 0) << " %"
            << "        Bkg Fraction (pT):    " << (raw_pt_integral > 0 ? 100*bkg_pt_integral/raw_pt_integral : 0) << " %" << std::endl;

  }
  if (!isData) {
      double matched_dPhi_integral = h_deltaPhi_Zj_matched->Integral(0, h_deltaPhi_Zj_matched->GetNbinsX()+1);
      double matched_pt_integral = h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1);
      std::cout << "[MC Matching Info]" << std::endl;
      std::cout << "Raw - Matched (dPhi):  " << raw_dPhi_integral - matched_dPhi_integral
                << "        Raw - Matched (pT):   " << raw_pt_integral - matched_pt_integral << std::endl;
      std::cout << "Fraction (dPhi):       " << (raw_dPhi_integral > 0 ? 100*(raw_dPhi_integral - matched_dPhi_integral)/raw_dPhi_integral : 0) << " %"
                << "        Fraction (pT):        " << (raw_pt_integral > 0 ? 100*(raw_pt_integral - matched_pt_integral)/raw_pt_integral : 0) << " %" <<std::endl;
  }

  std::cout << "[Z Boson Info]" << std::endl;
  std::cout << "Z+Jet Events found:    " << raw_pt_integral << std::endl;
  std::cout << "Z+Jet (pT_Z > 60):     " << h_Z_pt_j->Integral(h_Z_pt->FindBin(60), h_Z_pt->GetNbinsX()+1)
            << "        Z+Jet (pT_Z > 80):     " << h_Z_pt_j->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << std::endl;
  std::cout << "--- End Analysis Sum ---" << std::endl;
  std::cout << "------------------------------------------------" << std::endl;
  // --- End output summary ---

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
  TDirectory *Dir = file_output_HI_mu->mkdir(name_output);
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
  h_jet_pt_lj_2pi_3->Write();
  h_jet_pt_lj_nocut->Write();
  h_cen_j->Write();
  h_deltaPhi_Zj->Write();
  h_xZj->Write();
  h_xZj_fixbinw->Write();

  h_deltaPhi_Zj_all->Write();
  h_jet_pt_all->Write();
  h_xZj_all->Write();

  h_jet_etaphi_before->Write();
  h_jet_etaphi_after->Write();
  h_muon_iso_nocut->Write();
  h_vz->Write();
  h_avg_rho->Write();
  if (isPbPb) {
    h_jet_pt_lj_MinBias->Write();
    h_jet_pt_lj_subtracted->Write();
    h_deltaPhi_Zj_MinBias->Write();
    h_deltaPhi_Zj_subtracted->Write();
    h_xZj_MinBias->Write();
    h_xZj_subtracted->Write();
  }

  // Write unfolding specific histograms
  if (!isData) {
    h_deltaR_gen_reco->Write();
    h_eff_curve->Write();
    h_jet_pt_lj_matched->Write();
    h_deltaPhi_Zj_matched->Write();
    h_xZj_matched->Write();
    h_xZj_true->Write();
    h_xZj_for_JEWEL_w->Write();
    h_mumu_true->Write();
    h_xZj_reco->Write();
    h_response->Write();
    h_response_unmatched->Write();
    h_xZj_train_closure->Write();
    h_xZj_train_closure_matched->Write();
    h_xZj_test_closure->Write();
    h_xZj_test_closure_matched->Write();
    h_xZj_true_train_closure->Write();
    h_xZj_true_test_closure->Write();
    h_response_closure->Write();
    h_response_closure_unmatched->Write();
    h_Z_eta_gen_total->Write();
    h_Z_eta_gen_accepted->Write();
    if (isPbPb) {
      h_response_MinBias->Write();
      h_response_subtracted->Write();
      h_response_MinBias_closure->Write();
      h_response_closure_subtracted->Write();
      h_xZj_MinBias_train_closure->Write();
      h_xZj_MinBias_test_closure->Write();
      h_xZj_train_closure_subtracted->Write();
      h_xZj_test_closure_subtracted->Write();
    }
  }

  file_output_HI_mu->Close();

  // Manually delete pointers (clean up)
  //if (hiBin) delete hiBin;
  //if (weight) delete weight;
  //if (rho) delete rho;
  //if (etaMin) delete etaMin;
  //if (etaMax) delete etaMax;
  //if (jtPfCHF) delete jtPfCHF;
  //if (jtPfNHF) delete jtPfNHF;
  //if (jtPfCHM) delete jtPfCHM;
  //if (jtPfNHM) delete jtPfNHM;
  //if (jtPfCEM) delete jtPfCEM;
  //if (jtPfNEM) delete jtPfNEM;
  //if (jtPfMUM) delete jtPfMUM;
  //if (js_PbPb) delete js_PbPb;
  //if (js_pp) delete js_pp;
  //if (pprimaryVertexFilter) delete pprimaryVertexFilter;
  //if (pclusterCompatibilityFilter) delete pclusterCompatibilityFilter;
  //if (pphfCoincFilter2Th4) delete pphfCoincFilter2Th4;
  //if (goodvertex) delete goodvertex;
  //if (HLT_L2SingleMu) delete HLT_L2SingleMu;
}

