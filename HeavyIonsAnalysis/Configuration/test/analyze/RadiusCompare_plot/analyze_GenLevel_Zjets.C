/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                            //
//   ANALYSIS MACRO: Gen-Level ONLY Z Boson + Jet Framework for Jet Radius Comparison (R=0.2, 0.3, 0.4)                       //
//                                                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TH1D.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TPRegexp.h"

// Custom headers
#include "../helpers.h"           // for getLumiFromSummary, getDeltaR, RelativePhi, etc.
#include "../MC_samples.h"        // MC sample definitions

using namespace std;

void analyze_GenLevel_Zjets(const char * collision_type = "PbPb23", 
                            double jet_R = 0.2, // Pass 0.2, 0.3, or 0.4
                            int cent_min = 0, int cent_max = 30,
                            double ptZ_min = 40.0, double ptZ_max = 9999.0) {

  // --- Configuration ---
  TString collision_name = collision_type;
  bool isPbPb = collision_name.Contains("PbPb");
  bool is2023 = collision_name.Contains("23");
  
  // Format the jet tree name dynamically based on the requested radius
  int r_int = std::round(jet_R * 10); // 0.2 -> 2, 0.3 -> 3, 0.4 -> 4
  TString jetTreeName = isPbPb ? Form("akCs%dPFJetAnalyzer/t", r_int) : Form("ak%dPFJetAnalyzer/t", r_int);

  TString run_tag;
  if (isPbPb) {
      if (ptZ_max > 9000) run_tag = Form("_Cen%d_%d_ptZ%.0f_Inf", cent_min, cent_max, ptZ_min);
      else run_tag = Form("_Cen%d_%d_ptZ%.0f_%.0f", cent_min, cent_max, ptZ_min, ptZ_max);
  } else {
      if (ptZ_max > 9000) run_tag = Form("_ptZ%.0f_Inf", ptZ_min);
      else run_tag = Form("_ptZ%.0f_%.0f", ptZ_min, ptZ_max);
  }

  std::cout << "================================================================" << std::endl;
  std::cout << " Running Pure Gen-Level Analysis" << std::endl;
  std::cout << " Collision: " << collision_type << " | Jet Radius: R=" << jet_R << std::endl;
  std::cout << " Target Jet Tree: " << jetTreeName << std::endl;
  std::cout << "================================================================" << std::endl;

  // --- Initialize TTrees ---
  TChain EventTree("EventTree"), HiTree("HiTree"), JetTree("JetTree");
  
  // Point to the correct vector of MC samples
  const std::vector<FileInfo>* targetVector = nullptr;
  if (collision_name.Contains("PbPb24")) targetVector = &files_PbPb24;
  else if (collision_name.Contains("PbPb23")) targetVector = &files;
  else targetVector = &files_ppref;

  // Find signal sample properties
  TString search_pattern = "";
  double Xsec = 1;
  double Ngen = 1;

  for (const auto& file : *targetVector) {
      if (TString(file.label).Contains("signal")) {
          search_pattern = file.path_miniaod;
          Xsec = file.xsec;
          Ngen = file.ngen;
          std::cout << " Found signal: ngen = " << Ngen << ", xsec = " << Xsec << std::endl;
          break;
      }
  }

  // --- File Matching ---
  std::ifstream infile("../samples_root_files.txt");
  if (!infile.is_open()) {
      std::cerr << "[ERROR] Cannot open ../samples_root_files.txt!" << std::endl;
      return;
  }

  TString reg_str = search_pattern;
  reg_str.ReplaceAll(".", "\\."); 
  reg_str.ReplaceAll("*", "[^/]*");
  TPRegexp re(reg_str);
  
  std::vector<std::string> matched_files;
  std::string line;
  while (std::getline(infile, line)) {
      if (TString(line).Contains(re)) matched_files.push_back(line);
  }
  infile.close();
  
  std::cout << " Found " << matched_files.size() << " files matching pattern." << std::endl;

  for (size_t i = 0; i < matched_files.size(); i++) {
      EventTree.Add(TString(matched_files[i]) + "/muonAnalyzer/MuonTree");
      HiTree.Add(TString(matched_files[i]) + "/hiEvtAnalyzer/HiTree");
      JetTree.Add(TString(matched_files[i]) + "/" + jetTreeName);
  }
  
  EventTree.AddFriend("HiTree");
  EventTree.AddFriend("JetTree");

  Long64_t total_events = EventTree.GetEntries();
  TTreeReader fReader(&EventTree);

  // --- Branch Setup ---
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderValue<Float_t> weight = {fReader, "weight"};
  
  // Gen Muons
  TTreeReaderValue<Int_t> nGen = {fReader, "nGen"};
  TTreeReaderArray<Float_t> genPt = {fReader, "genPt"};
  TTreeReaderArray<Float_t> genEta = {fReader, "genEta"};
  TTreeReaderArray<Float_t> genPhi = {fReader, "genPhi"};
  TTreeReaderArray<Int_t> genPID = {fReader, "genPID"};

  // Gen Jets
  TTreeReaderValue<Int_t> ngen = {fReader, "ngen"};
  TTreeReaderArray<Float_t> genpt = {fReader, "genpt"};
  TTreeReaderArray<Float_t> geneta = {fReader, "geneta"};
  TTreeReaderArray<Float_t> genphi = {fReader, "genphi"};

  // --- Load Weights ---
  TH1D* h_weight_vz = nullptr;
  TString name_weight_vz;
  if (collision_name.Contains("PbPb23"))       name_weight_vz = Form("../weights_MC/vz_weights_2/weight_HI_vz%s.root", run_tag.Data());
  else if (collision_name.Contains("PbPb24"))  name_weight_vz = Form("../weights_MC/vz_weights_2/weight_HI24_vz%s.root", run_tag.Data());
  else if (collision_name.Contains("ppref24")) name_weight_vz = Form("../weights_MC/vz_weights_2/weight_ppref_vz%s.root", run_tag.Data());
  
  h_weight_vz = loadWeightHist(name_weight_vz.Data(), "h_weight_vz");

  // --- MC Normalization (Same as main macro) ---
  double Lumi = 1.;
  if (collision_name.Contains("PbPb23")) Lumi = getLumiFromSummary("../brilcalc_Collisions2023HI.csv");
  else if (collision_name.Contains("PbPb24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_HI.csv");
  else if (collision_name.Contains("ppref24")) Lumi = getLumiFromSummary("../brilcalc_Collisions2024_ppref.csv");

  TString name_MC_all = "";
  TString name_dir_all = "";
  if (collision_name.Contains("PbPb23")) { name_MC_all = "../weights_MC/MC_all_weights/output_HI_mu_MC_all.root"; name_dir_all = "HI/Muons"; }
  else if (collision_name.Contains("PbPb24")) { name_MC_all = "../weights_MC/MC_all_weights/output_HI24_mu_MC_all.root"; name_dir_all = "HI24/Muons"; }
  else if (collision_name.Contains("ppref24")) { name_MC_all = "../weights_MC/MC_all_weights/output_ppref_mu_MC_all.root"; name_dir_all = "ppref/Muons"; }

  TFile* file_MC_all = TFile::Open(name_MC_all, "READ");
  TH1D* h_norm = (TH1D*)((TDirectoryFile*)file_MC_all->Get(name_dir_all))->Get("h_sum_weights");
  double sum_w = h_norm->Integral(0, h_norm->GetNbinsX()+1);
  double number_A = 208;
  
  double norm_MC_w = isPbPb ? (number_A * number_A * Lumi * Xsec / sum_w) : (Lumi * Xsec / sum_w);
  std::cout << " norm_MC_w = " << norm_MC_w << std::endl;

  // --- Histograms ---
  const double pi_value = std::acos(-1);
  const int nbins_xZj = 6;
  double xZj_bins[nbins_xZj + 1] = {0.5, 0.7, 0.9, 1.1, 1.3, 1.5, 2.};

  TH1D *h_Z_pt_gen = new TH1D("h_Z_pt_gen", "Gen Z p_{T};p_{T}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1D *h_mumu_gen = new TH1D("h_mumu_gen", "Gen Z Mass;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1D *h_Z_eta_gen = new TH1D("h_Z_eta_gen", "Gen Z #eta;#eta^{Z}; Entries", 30, -3.0, 3.0);
  
  TH1D *h_jet_pt_gen = new TH1D("h_jet_pt_gen", "Gen Leading Jet p_{T};p_{T}^{jet} [GeV]; Entries", 30, 0, 300);
  TH1D *h_dphi_gen = new TH1D("h_dphi_gen", "Gen #Delta#phi_{Zj};#Delta#phi_{Zj}; Entries", 20, 0, pi_value);
  TH1D *h_xZj_gen = new TH1D("h_xZj_gen", "Gen x_{Zj};x_{Zj}; Entries", nbins_xZj, xZj_bins);

  // --- Event Loop ---
  std::cout << " Starting Event Loop over " << total_events << " events..." << std::endl;
  int report_step = total_events / 100;
  unsigned int itotev = 0;
  const double muonMass = 0.1056583755;

  while (fReader.Next()) {
      itotev++;
      if (itotev % report_step == 0 || itotev == 1) {
          std::cout << "\r Processing: " << itotev << " / " << total_events 
                    << " (" << std::fixed << std::setprecision(1) << (100.0 * itotev / total_events) << "%)" << std::flush;
      }

      // Vz cut
      if (*vz < -15.0 || *vz > 15.0) continue;

      // Base weight
      double scale = norm_MC_w * (*weight);
      if (h_weight_vz) {
          int bin_vz = h_weight_vz->FindBin(*vz);
          scale *= h_weight_vz->GetBinContent(bin_vz);
      }

      // --- Z Reconstruction (Gen) ---
      int iHighPtMu = -1, iHighPtAntiMu = -1;
      for (unsigned int i = 0; i < genPt.GetSize(); ++i) {
          if (genPID[i] == 13) {
              if (iHighPtMu == -1 || genPt[i] > genPt[iHighPtMu]) iHighPtMu = i;
          } else if (genPID[i] == -13) {
              if (iHighPtAntiMu == -1 || genPt[i] > genPt[iHighPtAntiMu]) iHighPtAntiMu = i;
          }
      }

      if (iHighPtMu == -1 || iHighPtAntiMu == -1) continue;

      TLorentzVector muMinus, muPlus;
      muMinus.SetPtEtaPhiM(genPt[iHighPtMu], genEta[iHighPtMu], genPhi[iHighPtMu], muonMass);
      muPlus.SetPtEtaPhiM(genPt[iHighPtAntiMu], genEta[iHighPtAntiMu], genPhi[iHighPtAntiMu], muonMass);

      // CMS Acceptance cuts
      if (muMinus.Pt() < 20 || std::abs(muMinus.Eta()) > 2.4 || muPlus.Pt() < 20 || std::abs(muPlus.Eta()) > 2.4) continue;

      TLorentzVector Z = muMinus + muPlus;
      if (Z.M() < 60 || Z.M() > 120 || Z.Pt() < ptZ_min || Z.Pt() >= ptZ_max) continue;

      h_mumu_gen->Fill(Z.M(), scale);
      h_Z_pt_gen->Fill(Z.Pt(), scale);
      h_Z_eta_gen->Fill(Z.Eta(), scale);

      // --- Jet Selection (Gen) ---
      int ijetLeading = -1;
      for (int ijet = 0; ijet < genpt.GetSize(); ++ijet) {
          if (genpt[ijet] < 30 || std::abs(geneta[ijet]) > 2.1) continue;
          
          // Z-muon cleaning
          if (getDeltaR(geneta[ijet], genphi[ijet], muMinus.Eta(), muMinus.Phi()) < 0.2) continue;
          if (getDeltaR(geneta[ijet], genphi[ijet], muPlus.Eta(), muPlus.Phi()) < 0.2) continue;

          if (ijetLeading == -1 || genpt[ijet] > genpt[ijetLeading]) {
              ijetLeading = ijet;
          }
      }

      // --- Z-Jet Kinematics ---
      if (ijetLeading != -1) {
          double dPhi_Zj = RelativePhi(Z.Phi(), genphi[ijetLeading]);
          double xZj = genpt[ijetLeading] / Z.Pt();

          h_dphi_gen->Fill(dPhi_Zj, scale);

          // Apply back-to-back cut for jet pt and xZj observables
          if (dPhi_Zj > 7 * pi_value / 8) {
              h_jet_pt_gen->Fill(genpt[ijetLeading], scale);
              h_xZj_gen->Fill(xZj, scale);
          }
      }
  }
  std::cout << "\n================================================================" << std::endl;

  // --- Save Output ---
  TString name_prefix = "HI";
  if (collision_name.Contains("PbPb24")) name_prefix = "HI24";
  else if (collision_name.Contains("ppref24")) name_prefix = "ppref";

  TString output_filename = Form("./output_%s_GenLevel_Zjet_R0%d%s.root", name_prefix.Data(), r_int, run_tag.Data());
  TFile *f_out = new TFile(output_filename, "RECREATE");
  
  TDirectory *Dir = f_out->mkdir(name_prefix);
  Dir->cd();
  TDirectory *muonsDir = Dir->mkdir("Muons");
  muonsDir->cd();

  h_Z_pt_gen->Write();
  h_mumu_gen->Write();
  h_Z_eta_gen->Write();
  h_jet_pt_gen->Write();
  h_dphi_gen->Write();
  h_xZj_gen->Write();

  f_out->Close();
  std::cout << " Successfully saved Gen-Level distributions for R=" << jet_R << " to: " << output_filename << std::endl;
}
