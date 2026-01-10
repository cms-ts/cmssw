// C++ includes
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TVector2.h" // Essential for DeltaPhi

#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "JetCorrector.h"
#include "JetUncertainty.h"

using namespace std;

// --- Constants ---
const double MUON_MASS = 0.1056583755;
const double PI = TMath::Pi();

// --- Helper Structures to reduce code repetition ---
struct Particle {
    TLorentzVector vec;
    int charge;
    int id; // index in the original tree array
};

struct ZCandidate {
    TLorentzVector vec;
    Particle m1;
    Particle m2;
    bool isValid;
};

// --- Helper Functions ---

// Replaces RelativePhi with standard ROOT math
double GetDeltaPhi(double phi1, double phi2) {
    return TVector2::Phi_mpi_pi(phi1 - phi2);
}

double GetDeltaR(double eta1, double phi1, double eta2, double phi2) {
    double deta = eta1 - eta2;
    double dphi = GetDeltaPhi(phi1, phi2);
    return TMath::Sqrt(deta * deta + dphi * dphi);
}

// Generic function to find Z candidate from a collection of muons
ZCandidate FindBestZ(const vector<Particle>& muons) {
    ZCandidate z;
    z.isValid = false;
    
    int idxPlus = -1;
    int idxMinus = -1;
    double maxPtPlus = -1;
    double maxPtMinus = -1;

    // Find highest Pt + and - muons
    for(const auto& mu : muons) {
        if (mu.charge > 0) {
            if (mu.vec.Pt() > maxPtPlus) { maxPtPlus = mu.vec.Pt(); idxPlus = mu.id; z.m1 = mu; }
        } else {
            if (mu.vec.Pt() > maxPtMinus) { maxPtMinus = mu.vec.Pt(); idxMinus = mu.id; z.m2 = mu; }
        }
    }

    if (idxPlus != -1 && idxMinus != -1) {
        z.vec = z.m1.vec + z.m2.vec;
        double mass = z.vec.M();
        // Basic Kinematic Cuts
        bool kineCuts = (z.m1.vec.Pt() > 20 && abs(z.m1.vec.Eta()) < 2.4 &&
                         z.m2.vec.Pt() > 20 && abs(z.m2.vec.Eta()) < 2.4);
        
        if (mass >= 60 && mass <= 120 && kineCuts && z.vec.Pt() > 40) {
            z.isValid = true;
        }
    }
    return z;
}


void simple_analyze_HI_TTreeReader_ZMM(bool isData = true, unsigned int weight_phase = 1, int systFlag = 0) {

  //MC normalization
  double Lumi = 1.64; // nb-1
  double number_A = 208; // Lead
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
    if (!isData) std::cout << "n_ev = " << n_ev << " sum_w = " << sum_w << " sum_ncoll = " << sum_ncoll << std::endl;
    double Xsec = 5.595 * 100 / 1000;
    double Ngen = 9560121;
    double norm_signal = number_A*number_A*Lumi*Xsec*(n_ev/sum_ncoll)/sum_w;

  // Centrality weights
  const float Ncoll[200] = {1893.13, 1867.0, 1834.16, 1805.64, 1770.84, 1744.49, 1699.76, 1661.52, 1615.89, 1579.59, 1540.62, 1499.14, 1469.01, 1432.18, 1402.8, 1368.39, 1338.12, 1302.26, 1274.91, 1245.56, 1215.28, 1183.76, 1160.61, 1131.12, 1107.67, 1078.54, 1055.72, 1026.72, 1000.57, 980.728, 958.777, 936.515, 911.397, 889.182, 869.677, 853.33, 826.999, 808.145, 792.14, 769.639, 753.513, 732.883, 716.817, 697.168, 679.091, 668.056, 650.114, 631.024, 616.203, 597.835, 583.435, 571.454, 555.478, 543.589, 526.328, 511.657, 497.023, 489.255, 471.52, 461.133, 447.767, 436.993, 426.106, 412.626, 403.224, 389.71, 382.595, 371.48, 358.899, 349.179, 339.387, 330.523, 320.094, 313.254, 302.339, 292.421, 282.594, 274.834, 268.847, 259.463, 252.027, 244.561, 236.738, 229.574, 222.898, 215.138, 207.328, 200.879, 196.592, 190.921, 183.942, 176.685, 170.919, 166.96, 161.057, 154.421, 148.816, 144.84, 139.087, 134.448, 128.72, 124.905, 121.166, 116.648, 112.367, 109.012, 104.33, 100.736, 97.3484, 93.2283, 89.3299, 85.9068, 83.6446, 80.2019, 77.5299, 73.9647, 70.7606, 68.2284, 65.793, 63.4532, 60.4738, 58.2406, 55.063, 53.7287, 51.4638, 49.241, 47.0111, 45.5443, 43.1729, 41.5041, 39.5449, 37.9282, 36.8918, 34.9287, 33.1886, 31.9177, 30.756, 29.0803, 27.6721, 26.42, 25.2678, 24.2585, 23.1429, 22.0138, 21.0169, 19.8203, 19.1043, 18.1478, 17.1715, 16.3605, 15.4763, 14.7973, 14.1594, 13.3927, 12.795, 12.1059, 11.5921, 10.9751, 10.3213, 9.94434, 9.3518, 8.94274, 8.37618, 7.94437, 7.48868, 7.06923, 6.71137, 6.31856, 6.03184, 5.67048, 5.43369, 5.13727, 4.83292, 4.58846, 4.37208, 4.15225, 3.84385, 3.63752, 3.45214, 3.24892, 3.02845, 2.81715, 2.66395, 2.5053, 2.29512, 2.13703, 1.93591, 1.79771, 1.64165, 1.54375, 1.45878, 1.36718, 1.2942, 1.23934, 1.18423, 1.14467, 1.11826, 1.0863, 1.06149, 1.04497 };

    // --- TChain Setup (Simplified) ---
    TChain data("akCs2PFJetAnalyzer/t");
    TChain EventTree("muonAnalyzer/MuonTree");
    TChain HiTree("hiEvtAnalyzer/HiTree");
    TChain skimanalysis("skimanalysis/HltTree");
    TChain hiFJRhoAnalyzerFinerBins("hiFJRhoAnalyzerFinerBins/t");

    glob_t globlist;
    string path = isData ? 
        "/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test13_ZMM_Prime*/*/*.root" : 
        "/eos/infnts/cms/store/user/kdeleo/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test17_ZMM_DYto2Mu/250625_144848/0000/HiForestMiniAOD_MC_*.root";
    
    glob(path.c_str(), GLOB_NOSORT, NULL, &globlist);
    if (globlist.gl_pathc == 0) { cout << "No files found!" << endl; return; }
    
    for (size_t i = 0; i < globlist.gl_pathc; i++) {
        data.Add(globlist.gl_pathv[i]);
        EventTree.Add(globlist.gl_pathv[i]);
        HiTree.Add(globlist.gl_pathv[i]);
        skimanalysis.Add(globlist.gl_pathv[i]);
        hiFJRhoAnalyzerFinerBins.Add(globlist.gl_pathv[i]);
    }
    globfree(&globlist);

    data.AddFriend(&EventTree);
    data.AddFriend(&HiTree);
    data.AddFriend(&skimanalysis);
    data.AddFriend(&hiFJRhoAnalyzerFinerBins);

    TTreeReader fReader(&data);

    // --- Reader Variables ---
    TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"};
    TTreeReaderValue<Float_t> weight = {fReader, isData ? "hiHF" : "weight"};
    TTreeReaderValue<Float_t> vz = {fReader, "vz"};
    TTreeReaderValue<Float_t> hiHF = {fReader, "hiHF"};

    // Filters
    TTreeReaderValue<int> pprimaryVertexFilter = {fReader, "pprimaryVertexFilter"};
    TTreeReaderValue<int> pclusterCompatibilityFilter = {fReader, "pclusterCompatibilityFilter"};
    TTreeReaderValue<int> pphfCoincFilter2Th4 = {fReader, "pphfCoincFilter2Th4"};

    // Reco Muons
    TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
    TTreeReaderArray<Float_t> recoPt = {fReader, "recoPt"};
    TTreeReaderArray<Float_t> recoEta = {fReader, "recoEta"};
    TTreeReaderArray<Float_t> recoPhi = {fReader, "recoPhi"};
    TTreeReaderArray<Int_t> recoCharge = {fReader, "recoCharge"};
    TTreeReaderArray<bool> recoIDTight = {fReader, "recoIDTight"};

    // Reco Jets
    TTreeReaderValue<Int_t> nref = {fReader, "nref"};
    TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
    TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
    TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};

    // Gen Particles (Only init if MC to be safe, though Ternary is OK if careful)
    // NOTE: Using pointers or Optional could be cleaner, but ternary is used to keep structure similar
    TTreeReaderValue<Int_t> ngen = {fReader, isData ? "nref" : "ngen"}; // Careful: on data this is nref
    TTreeReaderArray<Float_t> genpt = {fReader, isData ? "rawpt" : "genpt"};
    TTreeReaderArray<Float_t> geneta = {fReader, isData ? "jteta" : "geneta"};
    TTreeReaderArray<Float_t> genphi = {fReader, isData ? "jtphi" : "genphi"};

    TTreeReaderValue<Int_t> ngenMu = {fReader, isData ? "nReco" : "nGen"};
    TTreeReaderArray<Float_t> genMuPt = {fReader, isData ? "recoPt" : "genPt"};
    TTreeReaderArray<Float_t> genMuEta = {fReader, isData ? "recoEta" : "genEta"};
    TTreeReaderArray<Float_t> genMuPhi = {fReader, isData ? "recoPhi" : "genPhi"};
    TTreeReaderArray<Int_t> genMuPID = {fReader, isData ? "recoCharge" : "genPID"};

    // JEC
    vector<string> JECFiles = {"ParallelMC_L2Relative_AK2PF_PbPb_Reco_v0_2_13_2024.txt"};
    JetCorrector JEC(JECFiles);
    JetUncertainty JEU("Autumn18_HI_V8_MC_Uncertainty_AK2PF.txt");

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

  //Histograms

  // Define binning for xZj unfolding.
  const int nbins_xZj = 6; // Number of bins (number of edges - 1)
  const int nbins_xZj_meas = nbins_xZj-1;
  double xZj_bins[nbins_xZj + 1] = {0., 0.4, 0.65, 0.9, 1.15, 1.4, 2.};
  double xZj_bins_meas[nbins_xZj_meas + 1] = {0., 0.4,  0.65, 0.9, 1.15, 1.4};
//  double xZj_max;

  TH1F *h_mumu = new TH1F("h_mumu", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt = new TH1F("h_Z_pt", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1F *h_cen = new TH1F("h_cen", "Hist; centrality bin; Entries", 20, 0, 100);

  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_deltaPhi_Zj = new TH1F("h_deltaPhi_Zj", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);

  // --- RooUnfold Histograms ---

  TH1F* h_xZj_true = new TH1F("h_xZj_true", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);     // For true MC
  TH1D *h_xZj_for_JEWEL_w = new TH1D("h_xZj_for_JEWEL_w", "True x_{Zj};Entries", 60, 0.,3.);
  TH1F* h_mumu_true = new TH1F("h_mumu_true", "True m;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F* h_xZj_reco = new TH1F("h_xZj_reco", "Reco x_{Zj};x_{Zj};Entries", nbins_xZj_meas, xZj_bins_meas);
  TH2F* h_response = new TH2F("h_response", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  TH1F *h_xZj_train_closure = new TH1F("h_xZj_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_train_closure_matched = new TH1F("h_xZj_train_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_test_closure = new TH1F("h_xZj_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_test_closure_matched = new TH1F("h_xZj_test_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F* h_xZj_true_train_closure = new TH1F("h_xZj_true_train_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH1F* h_xZj_true_test_closure = new TH1F("h_xZj_true_test_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH2F* h_response_closure = new TH2F("h_response_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  // --- End RooUnfold Histograms ---

  // Output root file
  TFile *file_output_HI_mu;

  if (weight_phase == 0) {
    if (isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_HI_mu_data_Ncoll_weights.root", "RECREATE");
    }
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_HI_mu_MC_Ncoll_weights.root", "RECREATE");
    }
  }

  if (weight_phase != 0) {
    if (isData) {
      if (systFlag == 0)  file_output_HI_mu = new TFile("./plot/output_HI_mu_data.root", "RECREATE");
    }
    if (!isData) {
      if (systFlag == 0) file_output_HI_mu = new TFile("./plot/output_HI_mu_MC.root", "RECREATE");
      else if (systFlag == 9) {
        cout << "Running Systematic JEC - DOWN variation (systFlag = 9)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_HI_mu_MC_JEC_down.root", "RECREATE");
      }
      else if (systFlag == 10) {
        cout << "Running Systematic JEC - UP variation (systFlag = 10)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_HI_mu_MC_JEC_up.root", "RECREATE");
      }
    }
  }


    // --- Event Loop ---
    unsigned int itotev = 0;
    while (fReader.Next()) {
        itotev++;
        if(*pprimaryVertexFilter<=0 || *pclusterCompatibilityFilter<=0 || *pphfCoincFilter2Th4<=0) continue;
        if (*hiBin > 59) continue;

        // Weights
        float weight_cent = Ncoll[*hiBin];
    // Scale MC
    float scale = 1;
    if (!isData) scale*=norm_signal;
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

        // ------------------------------
        // 1. Gen Level Analysis (MC Only)
        // ------------------------------
        ZCandidate genZ;
        int ijetGenLeading = -1;
        double true_xZj = -1;
        bool hasGenZ = false;

        if (!isData) {
            vector<Particle> genMuons;
            for (int i = 0; i < *ngenMu; ++i) {
                if (abs(genMuPID[i]) != 13) continue; // Skip non-muons
                Particle p;
                p.vec.SetPtEtaPhiM(genMuPt[i], genMuEta[i], genMuPhi[i], MUON_MASS);
                p.charge = (genMuPID[i] == -13) ? 1 : -1; // PID -13 is anti-muon (+)
                p.id = i;
                genMuons.push_back(p);
            }

            genZ = FindBestZ(genMuons);

            if (genZ.isValid) {
                hasGenZ = true;
                h_mumu_true->Fill(genZ.vec.M(), scale);

                // Find Leading Gen Jet (Cleaning against Gen Muons)
                double maxGenJetPt = -1;
                for (int i = 0; i < *ngen; ++i) {
                    if (genpt[i] < 30 || abs(geneta[i]) > 2.5) continue;
                    if (GetDeltaR(geneta[i], genphi[i], genZ.m1.vec.Eta(), genZ.m1.vec.Phi()) < 0.2) continue;
                    if (GetDeltaR(geneta[i], genphi[i], genZ.m2.vec.Eta(), genZ.m2.vec.Phi()) < 0.2) continue;

                    if (genpt[i] > maxGenJetPt) {
                        maxGenJetPt = genpt[i];
                        ijetGenLeading = i;
                    }
                }

                if (ijetGenLeading != -1) {
                    double dphi = GetDeltaPhi(genZ.vec.Phi(), genphi[ijetGenLeading]);
                    if (abs(dphi) > 7 * PI / 8) {
                        true_xZj = genpt[ijetGenLeading] / genZ.vec.Pt();
                        h_xZj_for_JEWEL_w->Fill(true_xZj, scale);
                        h_xZj_true->Fill(true_xZj, scale);
                        if (itotev < 0.7*Ngen) h_xZj_true_train_closure->Fill(true_xZj, scale);
                        else h_xZj_true_test_closure->Fill(true_xZj, scale);
                    }
                }
            }
        }

        // ------------------------------
        // 2. Reco Level Analysis
        // ------------------------------
        if (*nReco < 2) continue;

        vector<Particle> recoMuons;
        for (int i = 0; i < *nReco; ++i) {
            if (!recoIDTight[i]) continue;
            Particle p;
            p.vec.SetPtEtaPhiM(recoPt[i], recoEta[i], recoPhi[i], MUON_MASS);
            p.charge = recoCharge[i];
            p.id = i;
            recoMuons.push_back(p);
        }

        ZCandidate recoZ = FindBestZ(recoMuons);
        if (!recoZ.isValid) continue;

        h_vz->Fill(*vz, scale);
        h_cen->Fill((*hiBin)/2, scale);
        h_mumu->Fill(recoZ.vec.M(), scale);
        h_Z_pt->Fill(recoZ.vec.Pt(), scale);

        // ------------------------------
        // 3. Reco Jet Loop & Optimization
        // ------------------------------
        int ijetRecoLeading = -1;
        double maxRecoJetPt = -1;

        // Find Leading Reco Jet first!
        for(int i=0; i<*nref; i++){
            // Apply JEC
            JEC.SetJetPT(rawpt[i]);
            JEC.SetJetEta(jteta[i]);
            JEC.SetJetPhi(jtphi[i]);
            double corrPt = JEC.GetCorrectedPT();

            // Apply Syst variations if needed
            JEU.SetJetPT(JEC.GetCorrectedPT());
            JEU.SetJetEta(jteta[i]);
            JEU.SetJetPhi(jtphi[i]);

            if (!isData && systFlag == 9) corrPt = JEC.GetCorrectedPT() * (1 - JEU.GetUncertainty().first); //down
            if (!isData && systFlag == 10) corrPt = JEC.GetCorrectedPT() * (1 + JEU.GetUncertainty().second); //up
            // Cuts
            if (corrPt < 30 || abs(jteta[i]) > 2.5) continue;

            // Cleaning against Reco Muons
            if (GetDeltaR(jteta[i], jtphi[i], recoZ.m1.vec.Eta(), recoZ.m1.vec.Phi()) < 0.2) continue;
            if (GetDeltaR(jteta[i], jtphi[i], recoZ.m2.vec.Eta(), recoZ.m2.vec.Phi()) < 0.2) continue;

            if (corrPt > maxRecoJetPt) {
                maxRecoJetPt = corrPt;
                ijetRecoLeading = i;
            }
        }

        // If we have a Z and a Leading Jet
        if (ijetRecoLeading != -1) {
            double dPhi_Zj = GetDeltaPhi(recoZ.vec.Phi(), jtphi[ijetRecoLeading]);
            double xZj = maxRecoJetPt / recoZ.vec.Pt();
            h_deltaPhi_Zj->Fill(dPhi_Zj, scale);
            // Fill Z+Jet Histograms
            if (abs(dPhi_Zj) > 7 * PI / 8) {
                h_jet_pt_lj->Fill(maxRecoJetPt, scale);
                h_xZj->Fill(xZj, scale);
                if (itotev < 0.7*Ngen) h_xZj_train_closure->Fill(xZj, scale);
                else h_xZj_test_closure->Fill(xZj, scale);

                // ------------------------------
                // 4. IMPROVED MATCHING LOGIC
                // ------------------------------
                if (!isData && hasGenZ) {
                    // We have a valid Reco Z+Jet and a valid Gen Z
                    // We specifically want to know if the Leading Reco Jet matches the Leading Gen Jet

                    bool matchesLeadingGen = false;

                    // Only perform matching if we actually identified a leading gen jet earlier
                    if (ijetGenLeading != -1) {
                        double dR = GetDeltaR(jteta[ijetRecoLeading], jtphi[ijetRecoLeading], 
                                              geneta[ijetGenLeading], genphi[ijetGenLeading]);

                        // Check if the Leading Reco matches the Leading Gen
                        if (dR < 0.1) {
                            matchesLeadingGen = true;
                        }
                    }

                    if (matchesLeadingGen && true_xZj > 0) {
                         // Fill Matched Histograms
                         h_response->Fill(xZj, true_xZj, scale);
                         h_xZj_reco->Fill(xZj, scale);
                         if (itotev < 0.7*Ngen) {
                           h_response_closure->Fill(xZj, true_xZj, scale);
                           h_xZj_train_closure_matched->Fill(xZj, scale);
                         }
                         else h_xZj_test_closure_matched->Fill(xZj, scale);
                    }
                }
            }
        }

    } // End Event Loop

    // Save and Close files
     cout << "Number of events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) << endl;
     cout << "tot ev = " << itotev << endl;
c1->cd(1);
  h_mumu->Draw();
  c2->cd(1);
  h_Z_pt->Draw();
c3->cd(1);
  h_jet_pt_lj->Draw();
  c4->cd(1);
//  h_cen->Draw();
  h_xZj->Draw();
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
  h_cen->Write();
  h_jet_pt_lj->Write();
  h_deltaPhi_Zj->Write();
  h_xZj->Write();
  h_vz->Write();

// Write unfolding specific histograms - NEW
  if (!isData) {
    h_xZj_true->Write();
    h_xZj_for_JEWEL_w->Write();
    h_mumu_true->Write();
    h_xZj_reco->Write();
    h_response->Write();
    h_xZj_train_closure->Write();
    h_xZj_train_closure_matched->Write();
    h_xZj_test_closure->Write();
    h_xZj_test_closure_matched->Write();
    h_xZj_true_train_closure->Write();
    h_xZj_true_test_closure->Write();
    h_response_closure->Write();
  }
  file_output_HI_mu->Close();
}
