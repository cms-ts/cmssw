/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//   ANALYSIS MACRO: MinBias Library Producer for Mixed Event Subtraction                                                     //
//                                                                                                                            //
//   File:    analyze_MinBias_TTreeReader.C                                                                                   //
//   Author:  Raffaele                                                                                                        //
//   Date:    2024-2026                                                                                                       //
//                                                                                                                            //
//   DESCRIPTION:                                                                                                             //
//   Creates a library of MinBias events to be used for Mixed Event Background Subtraction in the Z+Jet analysis.             //
//   It reads MinBias tuples, processes jets, and stores event info (HF, VZ, Cen) and Leading Jet info into a flat TTree.     //
//                                                                                                                            //
//   CORE WORKFLOW:                                                                                                           //
//   1. Initialization:   Load Chains, JEC, JER, and Jet Selectors.    2. Bin Setup:     Define mixing bins (HF, VZ, VZ+Cen). //
//   3. Event Loop:       Apply filters, calculate Centrality/Rho.     4. Jet Process:   Apply JEC/JER, cuts cleaning.        //
//   5. Storage:          Fill 'jet_tree' with event data and jet_l.                                                          //
//                                                                                                                            //
//   USAGE EXAMPLES:                                                                                                          //
//   root -l 'analyze_MinBias_TTreeReader.C(true, 0)'   // Data (HF Binning)                                                  //
//   root -l 'analyze_MinBias_TTreeReader.C(false, 2)'  // MC (VZ + Cen Binning)                                              //
//                                                                                                                            //
//   PARAMETERS:                                                                                                              //
//   ------------------------------------------------------------------------------                                           //
//   [isData] (bool)        True for Data, False for MC.                                                                      //
//                                                                                                                            //
//   [use_binning_option]   Controls the binning scheme for mixing:                                                           //
//    0: HF binning only           1: VZ binning only             2: VZ + Cen (Combined - Recommended for Analysis)           //
//                                                                                                                            //
//   DEPENDENCIES:                                                                                                            //
//   - JetCorrector.h, JERProvider.h, JetSelection_PbPb.h         - binning_config.h                                          //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector
#include <map>        // For std::map to store events per bin
#include <string>     // For std::string
#include <utility>    // For std::pair
#include <sstream>    // For std::stringstream for string formatting

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
#include "TMath.h" // For TMath::Pi()

#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "binning_config.h"        // Custom binning configuration header
#include "../JetCorrector.h"       // for JEC
#include "../JERProvider.h"        // Include JER Provider
#include "../JetSelection_PbPb.h"  // For Id selection + jet veto map in PbPb

using namespace std;

void analyze_MinBias_TTreeReader(bool isData = true, int use_binning_option = 0) {

  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis"), hiFJRhoAnalyzerFinerBins("hiFJRhoAnalyzerFinerBins");

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
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderValue<Float_t> hiHF = {fReader, "hiHF"};

  //TTreeReaderValue<float> Ncoll = {fReader, "Ncoll"}; // Ncoll

  TTreeReaderArray<double> rho = {fReader, "rho"};

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
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};
  TTreeReaderArray<Float_t> jtPfCEF = {fReader, "jtPfCEF"};
  TTreeReaderArray<Float_t> jtPfNEF = {fReader, "jtPfNEF"};
  TTreeReaderArray<Float_t> jtPfMUF = {fReader, "jtPfMUF"};

  // Gen Jets (for hybrid JER smearing in MC)
  // We use "rawpt" as dummy for data to avoid crash, but logic inside loop handles isData check
  TTreeReaderValue<Int_t> ngen = {fReader, isData ? "nref" : "ngen"};
  TTreeReaderArray<Float_t> genpt = {fReader, isData ? "rawpt" : "genpt"};
  TTreeReaderArray<Float_t> geneta = {fReader, isData ? "jteta" : "geneta"};
  TTreeReaderArray<Float_t> genphi = {fReader, isData ? "jtphi" : "genphi"};

  // To apply corrections on jets
  vector<string> Files;
  // L2Relative is applied to BOTH Data and MC (the two files are actually identical)
  // L2Residual applied only to Data
  if (isData) {
    Files.push_back("../Spring23Prompt23_PbPb_V1_DATA_L2Relative_AK2PF.txt");
    Files.push_back("../Spring23Prompt23_PbPb_V1_DATA_L2Residual_AK2PF.txt");

  } else {
    Files.push_back("../Spring23Prompt23_PbPb_V1_MC_L2Relative_AK2PF.txt");
  }
  JetCorrector JEC(Files);
  // For debugging
  cout << "Initializing JEC..." << endl;
  for (const auto& file : Files) {
      cout << "  Loaded JEC File: " << file << endl;
  }

  // Initialize JER Provider
  JERProvider jer;
  if (!isData) {
    cout << "Initializing JER..." << endl;
    // Ensure these text files exist in the path or update path accordingly
    jer.LoadSF("../Autumn18_RunD_V7b_MC_SF_AK4PF.txt");
    jer.LoadResolution("../Autumn18_RunD_V7b_MC_PtResolution_AK4PF.txt");
  }

  // Initialize Jet Selector with your specific 2024 map file
  JetSelect js("../Winter24Prompt24_2024BCDEFGHI.root");

  // --- Define bins ---
  std::map<std::string, std::vector<std::pair<double, double>>> leading_jets_by_bin;

  // These will hold the actual bin ranges for iteration, based on the selected option
  std::vector<std::pair<float, float>> primary_bins;         // For HF or VZ only
  std::vector<std::pair<float, float>> vz_bins_combined;     // For combined VZ+Centrality
  std::vector<std::pair<float, float>> centrality_bins_combined; // For combined VZ+Centrality

  int total_bins_count = 0; // Total expected bins for the selected option
  int events_per_bin_limit = 0; // The maximum number of events to collect per individual bin

  if (use_binning_option == 0) { // HF binning
    total_bins_count = BinningConfig::tot_bins;
    events_per_bin_limit = BinningConfig::ev_per_bin;
    float current_min = BinningConfig::frst_bin_min;
    for (int i = 0; i < total_bins_count; ++i) {
      float current_max = (current_min * 1.1);
      primary_bins.push_back({current_min, current_max});
      current_min = current_max;
    }
    std::cout << "Defined HF bins (" << primary_bins.size() << " total):" << std::endl;
    for (const auto& bin : primary_bins) {
      std::cout << "[" << bin.first << ", " << bin.second << ")" << std::endl;
    }
  } else if (use_binning_option == 1) { // VZ binning only
    total_bins_count = BinningConfig_vz::tot_bins;
    events_per_bin_limit = BinningConfig_vz::ev_per_bin;
    float current_min = BinningConfig_vz::frst_bin_min;
    for (int i = 0; i < total_bins_count; ++i) {
      float current_max = (current_min + 10.0f); // Example: 10cm wide vz bins
      primary_bins.push_back({current_min, current_max});
      current_min = current_max;
    }
    std::cout << "Defined vz bins (" << primary_bins.size() << " total):" << std::endl;
    for (const auto& bin : primary_bins) {
      std::cout << "[" << bin.first << ", " << bin.second << ")" << std::endl;
    }
  } else if (use_binning_option == 2) { // VZ + Centrality binning
    // Centrality bins based on hiBin (e.g., 30 bins from 0-30% centrality)
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

    total_bins_count = BinningConfig_Combined_Vz_Centrality::num_centrality_bins * BinningConfig_Combined_Vz_Centrality::num_vz_bins;
    events_per_bin_limit = BinningConfig_Combined_Vz_Centrality::ev_per_combined_bin;

    std::cout << "Defined combined Centrality-VZ bins (" << total_bins_count << " total):" << std::endl;
    for (const auto& cen_bin : centrality_bins_combined) {
      for (const auto& vz_bin : vz_bins_combined) {
        // Convert hiBin to % for display: hiBin / 2
        std::cout << "Centrality [" << cen_bin.first/2 << "-" << cen_bin.second/2 << "]%, VZ [" << vz_bin.first << ", " << vz_bin.second << ")" << std::endl;
      }
    }
  } else {
    std::cerr << "Invalid use_binning_option: " << use_binning_option << std::endl;
    return; // Exit if invalid option
  }

  // --- End of bin definition ---

  // Save in a ttree
  TTree *jet_tree;
  jet_tree = new TTree("jet_tree","jet_tree");
  Float_t jet_tree_HF = 0;
  Float_t jet_tree_vz = 0;
  Int_t jet_tree_hiBin = 0;
  Int_t jet_tree_bin = 0;
  Float_t jet_tree_pt = 0;
  Float_t jet_tree_phi = -999.0;
  Float_t jet_tree_eta = -999.0;

  jet_tree->Branch("HF_MinBias", &jet_tree_HF);
  jet_tree->Branch("vz_MinBias", &jet_tree_vz);
  jet_tree->Branch("hiBin_MinBias", &jet_tree_hiBin);
  jet_tree->Branch("bin_MinBias", &jet_tree_bin);
  jet_tree->Branch("jet_pt_MinBias", &jet_tree_pt);
  jet_tree->Branch("jet_phi_MinBias", &jet_tree_phi);
  jet_tree->Branch("jet_eta_MinBias", &jet_tree_eta);

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

  // Histograms
  TH1F *h_cen = new TH1F("h_cen", "Hist; centrality bin; Entries", 20, 0, 100);
  TH1F *h_HF = new TH1F("h_HF", "Hist; HF; Entries", 80, 0, 8000);

  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_Phi_lj = new TH1F("h_Phi_lj", "Hist;#phi_{lj}; Entries", 20, -TMath::Pi(),TMath::Pi());

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);

  // Track how many bins have reached 'events_per_bin_limit' events
  std::map<std::string, int> bin_event_counts; // Keeps count for each specific bin label
  int overall_filled_bins_count = 0; // Counts how many distinct bins have reached their limit

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    // Optional: if all bins are filled, we can stop processing events early
    if (overall_filled_bins_count == total_bins_count) {
      std::cout << "All " << total_bins_count << " bins have collected " << events_per_bin_limit << " events. Stopping event loop early." << std::endl;
      break;
    }

    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;

    std::string current_bin_label = "";
    int current_global_bin_n = -1; // Unique integer ID for the current bin
    bool found_bin = false;

    if (use_binning_option == 0) { // HF binning
      float current_val = *hiHF;
      int bin_n = 0;
      for (const auto& bin_range : primary_bins) {
        if (current_val >= bin_range.first && current_val < bin_range.second) {
          current_bin_label = Form("HF_%.0f-%.0f [%d]", bin_range.first, bin_range.second, bin_n);
          current_global_bin_n = bin_n;
          found_bin = true;
          break;
        }
        bin_n++;
      }
    } else if (use_binning_option == 1) { // VZ binning only
      float current_val = *vz;
      // Apply centrality cut for VZ only (e.g., 0-30%)
      if(*hiBin > 59) continue; // hiBin is centrality*2, so >59 means >29.5%
      int bin_n = 0;
      for (const auto& bin_range : primary_bins) {
        if (current_val >= bin_range.first && current_val < bin_range.second) {
          current_bin_label = Form("VZ_%.1f-%.1f [%d]", bin_range.first, bin_range.second, bin_n);
          current_global_bin_n = bin_n;
          found_bin = true;
          break;
        }
        bin_n++;
      }
    } else if (use_binning_option == 2) { // VZ + Centrality binning
      // Apply overall centrality cut relevant to the combined binning scheme (e.g., 0-30%)
      if(*hiBin > 59) continue;

      int cen_bin_idx = -1;
      int vz_bin_idx = -1;

      // Find Centrality Bin
      for (size_t c_bin_n = 0; c_bin_n < centrality_bins_combined.size(); ++c_bin_n) {
        const auto& bin_range = centrality_bins_combined[c_bin_n];
        if (*hiBin >= bin_range.first && *hiBin < bin_range.second) {
          cen_bin_idx = c_bin_n;
          break;
        }
      }

      // Find VZ Bin
      for (size_t v_bin_n = 0; v_bin_n < vz_bins_combined.size(); ++v_bin_n) {
        const auto& bin_range = vz_bins_combined[v_bin_n];
        if (*vz >= bin_range.first && *vz < bin_range.second) {
          vz_bin_idx = v_bin_n;
          break;
        }
      }

      if (cen_bin_idx != -1 && vz_bin_idx != -1) {
        // Construct a unique bin label for the map key
        std::stringstream ss;
        ss << "Cen_" << centrality_bins_combined[cen_bin_idx].first/2 << "-" << centrality_bins_combined[cen_bin_idx].second/2 << "%_VZ_"
           << vz_bins_combined[vz_bin_idx].first << "-" << vz_bins_combined[vz_bin_idx].second;
        current_bin_label = ss.str();

        // Calculate a unique global bin number for the TTree
        // This assumes centrality_bins_combined.size() and vz_bins_combined.size() are constant
        current_global_bin_n = cen_bin_idx * BinningConfig_Combined_Vz_Centrality::num_vz_bins + vz_bin_idx;
        found_bin = true;
      }
    }

    if (!found_bin) {
      continue; // Event does not fall into any defined bin for the selected option
    }

    // Check if this specific bin already has enough events
    if (bin_event_counts[current_bin_label] >= events_per_bin_limit) {
      continue; // Skip this event, this bin is already full
    }

    iEvent++;
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // Calculate average rho for JER
    double sum_rho = 0;
    for (unsigned int i = 0; i < rho.GetSize(); i++) sum_rho += rho[i];
    double avg_rho = (rho.GetSize() > 0) ? sum_rho / rho.GetSize() : 0;

    // Pre-calculate GenJet Vectors for JER
    std::vector<float> v_gen_pts, v_gen_etas, v_gen_phis;
    if (!isData) {
      for (int i = 0; i < *ngen; i++) {
        v_gen_pts.push_back(genpt[i]);
        v_gen_etas.push_back(geneta[i]);
        v_gen_phis.push_back(genphi[i]);
      }
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
      // Apply JER if MC
      double pt_final = CorrectedPT;
      if (!isData) {
        pt_final = jer.GetSmearedPt(CorrectedPT, jteta[ijet], jtphi[ijet], avg_rho, v_gen_pts, v_gen_etas, v_gen_phis, 0);
      }
      jtpt_corr[ijet] = pt_final;
      // Selections
      if(jtpt_corr[ijet]<30 || abs(jteta[ijet])>2.5) continue;
      // Apply Combined Jet ID and Veto Map
      // Pass the current jet index [ijet] to the arrays
      if (!js.JetSelection(jteta[ijet], jtphi[ijet], jtPfCEF[ijet], jtPfNEF[ijet], jtPfMUF[ijet])) continue;
      if (ijetLeading == -1 || jtpt_corr[ijet] > jtpt_corr[ijetLeading]) {
          ijetLeading = ijet;
      }
    } // end loop over jets

    if (ijetLeading != -1) {
      jet_tree_pt = jtpt_corr[ijetLeading];
      jet_tree_phi = jtphi[ijetLeading];
      jet_tree_eta = jteta[ijetLeading];
      // Fill histograms only if you have a jet
      h_Phi_lj->Fill(jtphi[ijetLeading]);
      h_jet_pt_lj->Fill(jtpt_corr[ijetLeading]);
      // Store leading jet information for the specific bin
      leading_jets_by_bin[current_bin_label].push_back({jtpt_corr[ijetLeading], jtphi[ijetLeading]});
    }
    // Fill general event histograms, ALWAYS Fill the Tree (Even if jet_tree_pt is 0)
    jet_tree_HF = *hiHF;
    jet_tree_vz = *vz;
    jet_tree_hiBin = *hiBin;
    jet_tree_bin = current_global_bin_n;
    // Fill global histograms
    h_vz->Fill(*vz);
    h_HF->Fill(*hiHF);
    h_cen->Fill((*hiBin)/2.0);

    jet_tree->Fill();

    // Increment event count for this specific bin
    bin_event_counts[current_bin_label]++;
    // Check if this push_back just filled the bin to 'events_per_bin_limit'
      if (bin_event_counts[current_bin_label] == events_per_bin_limit) {
        overall_filled_bins_count++;
        std::cout << "Bin " << current_bin_label << " is now full with " << events_per_bin_limit << " events. Total filled bins: " << overall_filled_bins_count << std::endl;
    }
  }  // end loop events

  // We iterate over bin_event_counts to tracks bins not completely full
  for (const auto& pair : bin_event_counts) {
    std::string label = pair.first;
    int count = pair.second;
    if (count < events_per_bin_limit) {
      std::cout << "Bin " << label << " only reached: "
                << count << " / " << events_per_bin_limit << " events. [INCOMPLETE]" << std::endl;
    }
  }

  std::cout << "\n--- Finished event collection. Final status of leading jet data per bin: ---" << std::endl;
  for (const auto& pair : leading_jets_by_bin) {
    std::cout << "Bin " << pair.first << ": " << pair.second.size() << " leading jets collected." << std::endl;
  }

  cout << "Total events saved : " << iEvent << endl;
  cout << "N leading jets (h_jet_pt_lj integral): " << h_Phi_lj->GetEntries() << endl;

  // --- Store to a ROOT file ---
  TFile *outputFile;
  TString output_filename;
  if (isData) {
    if (use_binning_option == 0) output_filename = "./MinBias_leading_jets_data_HF.root";
    else if (use_binning_option == 1) output_filename = "./MinBias_leading_jets_data_VZ.root";
    else if (use_binning_option == 2) output_filename = "./MinBias_leading_jets_data_VZ_Cen_Combined.root";
    else output_filename = "./MinBias_leading_jets_data_UnknownOption.root";
  } else {
    if (use_binning_option == 0) output_filename = "./MinBias_leading_jets_MC_HF.root";
    else if (use_binning_option == 1) output_filename = "./MinBias_leading_jets_MC_VZ.root";
    else if (use_binning_option == 2) output_filename = "./MinBias_leading_jets_MC_VZ_Cen_Combined.root";
    else output_filename = "./MinBias_leading_jets_MC_UnknownOption.root";
  }
  outputFile = new TFile(output_filename, "RECREATE");

  // Write the TTree
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

