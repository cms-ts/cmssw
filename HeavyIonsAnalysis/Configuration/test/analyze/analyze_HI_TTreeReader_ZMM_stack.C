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

#include "JetCorrector.h" // for JEC
#include "MC_samples.h" // Include the header file for MC samples

using namespace std;

double RelativePhi(double phi_1,double phi_2) {
  double d_phi =  abs(phi_1 - phi_2);
  if (d_phi > acos(-1)) d_phi = 2*acos(-1) - d_phi;
  return d_phi;
}

//To run on data, root -l 'analyze_HI_TTreeReader_ZMM_stack.C("data")'
//for MC see MC_samples.h, for ex. root -l 'analyze_HI_TTreeReader_ZMM_stack.C("signal")'
void analyze_HI_TTreeReader_ZMM_stack(const char * argument_name) {

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
  bool isData = false;
  TString file_name = argument_name;
  if (file_name.Contains("data")) {
    isData = true;
    cout << "This is data: " << file_name << endl;
    glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test13_ZMM_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
  }
  else {
    // Loop over files
    for (const auto& file : files) {
      if (file_name.Contains(file.label)) {
        glob(file.path_miniaod, GLOB_NOSORT, NULL, &globlist);
        cout << "This is MC " << file.label << ": ngen = " << file.ngen << " xsec = " << file.xsec << endl;
      }
    }
  }
  cout << "Found " << globlist.gl_pathc << " files" << endl;

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

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  //TH2F *h_jet_etaphi = new TH2F("h_jet_etaphi", "Hist; #eta; #phi", 40, -2.5, 2.5, 40, -TMath::Pi(), TMath::Pi());

  // Output root file
  TFile *file_output_HI_mu;
  if (isData) {
    file_output_HI_mu = new TFile("./stack/output_HI_mu_data.root", "RECREATE");
  }
  else {
    file_output_HI_mu = new TFile("./stack/output_HI_mu_MC_"+file_name+".root", "RECREATE");
  }
  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;
    float weight_cent = Ncoll[*hiBin];
    // Scale MC
    float scale =1;
    if (!isData) {
      if (file_name.Contains("signal")) scale*=weight_cent*(*weight);
      else scale*=weight_cent;
    }
    if(*hiBin>59) continue;
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
      // Apply rho and vz weight
      int bin_vz = h_weight_vz->FindBin(*vz);
      scale*=h_weight_rho->GetBinContent(bin_rho)*h_weight_vz->GetBinContent(bin_vz);
    }

    h_vz->Fill(*vz, scale);
    h_avg_rho->Fill(avg_rho, scale);
    h_cen->Fill((*hiBin)/2, scale);
    h_mumu->Fill(Z_mass, scale);
    h_Z_pt->Fill(Z_pt, scale);

    // Loop over Jets
    unsigned int njets = 0;
    double detaMinus = 0, dphiMinus = 0, dRMinus = 0;
    double detaPlus = 0, dphiPlus = 0, dRPlus = 0;
    int ijetLeading = -1;
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
      if (ijetLeading == -1 || jtpt_corr[ijet] > jtpt_corr[ijetLeading]) {
          ijetLeading = ijet;
      }
    } //end loop over jets
    //cout << "ijetLeading = " << ijetLeading << endl;

    if (ijetLeading != -1) {
        double dPhi_Zj = RelativePhi(Z_phi, jtphi[ijetLeading]);
        h_deltaPhi_Zj->Fill(dPhi_Zj, scale);
        if (dPhi_Zj > 7 * TMath::Pi() / 8) {
          h_njet->Fill(njets, scale);
          h_mumu_j->Fill(Z_mass, scale);
          h_Z_pt_j->Fill(Z_pt, scale);
          h_jet_pt_lj->Fill(jtpt_corr[ijetLeading], scale);
          h_xZj->Fill(jtpt_corr[ijetLeading]/Z_pt, scale);
          h_cen_j->Fill((*hiBin)/2, scale);
          h_HF_j->Fill(*hiHF, scale);
          //h_jetgirth->Fill(jtgirth[ijetLeading], scale);
          //h_jet_deltaR->Fill(jtdyndeltaR[ijetLeading], scale);
          //h_jet_etaphi->Fill(jteta[ijetLeading], jtphi[ijetLeading], scale);
        }
    }
  }  // end loop events

  cout << "Number of events = " << h_mumu_j->Integral() << ", if Z_pt>80: " << h_Z_pt_j->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << endl;

  c1->cd(1);
  h_mumu_j->Draw();
  c2->cd(1);
  h_Z_pt_j->Draw();
  c3->cd(1);
  h_njet->Draw();
  c4->cd(1);
  h_jet_pt_lj->Draw();
  c5->cd(1);
  h_cen->Draw();
  c6->cd(1);
  h_vz->Draw();
  c7->cd(1);
  h_avg_rho->Draw();
  //h_jet_etaphi->Draw("colz");
  //c6->SaveAs("h_jet_etaphi.pdf");

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
  h_cen_j->Write();
  h_deltaPhi_Zj->Write();
  h_xZj->Write();
  h_vz->Write();
  h_avg_rho->Write();
  //h_jetgirth->Write();
  //h_jet_deltaR->Write();
  //h_jet_etaphi->Write();

  file_output_HI_mu->Close();
}
