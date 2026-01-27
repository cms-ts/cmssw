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
#include "TH1.h"
#include "TStyle.h"

#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

using namespace std;

double RelativePhi(double phi_1,double phi_2) {
  double d_phi =  abs(phi_1 - phi_2);
  if (d_phi > acos(-1)) d_phi = 2*acos(-1) - d_phi;
  return d_phi;
}

//To run, root -l analyze_HI_TTreeReader_ZMM.C
//Default isData, for MC root -l 'analyze_HI_TTreeReader_ZMM.C(false)'
void analyze_PbPb_TTreeReader_ZMM(bool isData = true) {

  // Centrality weights
  const float Ncoll[200] = {1893.13, 1867.0, 1834.16, 1805.64, 1770.84, 1744.49, 1699.76, 1661.52, 1615.89, 1579.59, 1540.62, 1499.14, 1469.01, 1432.18, 1402.8, 1368.39, 1338.12, 1302.26, 1274.91, 1245.56, 1215.28, 1183.76, 1160.61, 1131.12, 1107.67, 1078.54, 1055.72, 1026.72, 1000.57, 980.728, 958.777, 936.515, 911.397, 889.182, 869.677, 853.33, 826.999, 808.145, 792.14, 769.639, 753.513, 732.883, 716.817, 697.168, 679.091, 668.056, 650.114, 631.024, 616.203, 597.835, 583.435, 571.454, 555.478, 543.589, 526.328, 511.657, 497.023, 489.255, 471.52, 461.133, 447.767, 436.993, 426.106, 412.626, 403.224, 389.71, 382.595, 371.48, 358.899, 349.179, 339.387, 330.523, 320.094, 313.254, 302.339, 292.421, 282.594, 274.834, 268.847, 259.463, 252.027, 244.561, 236.738, 229.574, 222.898, 215.138, 207.328, 200.879, 196.592, 190.921, 183.942, 176.685, 170.919, 166.96, 161.057, 154.421, 148.816, 144.84, 139.087, 134.448, 128.72, 124.905, 121.166, 116.648, 112.367, 109.012, 104.33, 100.736, 97.3484, 93.2283, 89.3299, 85.9068, 83.6446, 80.2019, 77.5299, 73.9647, 70.7606, 68.2284, 65.793, 63.4532, 60.4738, 58.2406, 55.063, 53.7287, 51.4638, 49.241, 47.0111, 45.5443, 43.1729, 41.5041, 39.5449, 37.9282, 36.8918, 34.9287, 33.1886, 31.9177, 30.756, 29.0803, 27.6721, 26.42, 25.2678, 24.2585, 23.1429, 22.0138, 21.0169, 19.8203, 19.1043, 18.1478, 17.1715, 16.3605, 15.4763, 14.7973, 14.1594, 13.3927, 12.795, 12.1059, 11.5921, 10.9751, 10.3213, 9.94434, 9.3518, 8.94274, 8.37618, 7.94437, 7.48868, 7.06923, 6.71137, 6.31856, 6.03184, 5.67048, 5.43369, 5.13727, 4.83292, 4.58846, 4.37208, 4.15225, 3.84385, 3.63752, 3.45214, 3.24892, 3.02845, 2.81715, 2.66395, 2.5053, 2.29512, 2.13703, 1.93591, 1.79771, 1.64165, 1.54375, 1.45878, 1.36718, 1.2942, 1.23934, 1.18423, 1.14467, 1.11826, 1.0863, 1.06149, 1.04497 };

  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis");

  glob_t globlist;
  if (isData) {
    //glob("/eos/infnts/cms/store/user/rdelliga/HIPhysicsRawPrime*/CRAB3_Analysis_test21_ZMM_PbPb24A_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    glob("/eos/infnts/cms/store/user/rdelliga/HIPhysicsRawPrime*/CRAB3_Analysis_test21_ZMM_PbPb24B_v2_Prime*/*/*/HiForestMiniAOD_DATA_*.root", GLOB_NOSORT, NULL, &globlist);
    //glob("/eos/infnts/cms/store/user/kdeleo/PPRefSingleMuon*/CRAB3_Analysis_test16_ZMM_PPRefSingleMuon*/*/000*.root", GLOB_NOSORT, NULL, &globlist);
    //glob("/eos/infnts/cms/store/user/kdeleo/PPRefSingleMuon3/CRAB3_Analysis_test16_ZMM_PPRefSingleMuon3/250530_082717/0000/HiForestMiniAOD_*.root", GLOB_NOSORT, NULL, &globlist);
  cout << "This is data" << endl;
  }
  else {
    glob("/eos/infnts/cms/store/user/rdelliga/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_DYto2Mu/251217_140307/0000/HiForestMiniAOD_MC_*.root", GLOB_NOSORT, NULL, &globlist);
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
  TTreeReaderValue<Float_t> weight = {fReader, isData ? "hiHF" : "weight"}; // MC event weight, not used in data
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"}; // centralityx2

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
  TTreeReaderArray<Float_t> rawpt = {fReader, "jtpt"};
  //TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};

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

  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_deltaPhi_Zj = new TH1F("h_deltaPhi_Zj", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", 20, 0, 3);

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Output root file
  TFile *file_output_PbPb_mu;

    if (isData) {
      file_output_PbPb_mu = new TFile("./output_PbPb_mu_data.root", "RECREATE");
    }
    if (!isData) {
      file_output_PbPb_mu = new TFile("./output_PbPB_mu_MC.root", "RECREATE");
    }

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  unsigned int iEvfltr = 0;
  double sum_weight = 0;
  double sum_weight_cen = 0;
  while (fReader.Next()) {
  // Check run number
//  cout << "processing run " << *run << endl;
    iEvent++;
    float weight_cent = Ncoll[*hiBin];
    if (!isData) {
      sum_weight += *weight;
      sum_weight_cen += weight_cent;
    }
    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;
    iEvfltr++;
    // Scale MC
    float scale =1;
      if (!isData) {
        scale*=weight_cent*(*weight);
      }
    if(*hiBin>59) continue;
    //if(*HLT_HIL2SingleMu7_v3<=0) continue;
    bool good_pair = false;
    if (*nReco < 2 ) continue;
//    iEvent++;
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
    h_mumu->Fill(Z_mass, scale);
    h_Z_pt->Fill(Z_pt, scale);
    h_cen->Fill((*hiBin)/2, scale);

    // Loop over Jets
    unsigned int njets = 0;
    double detaMinus = 0, dphiMinus = 0, dRMinus = 0;
    double detaPlus = 0, dphiPlus = 0, dRPlus = 0;
    int ijetLeading = -1;
    double jtpt_corr[20000];
    for(int ijet=0; ijet<*nref; ijet++){
      jtpt_corr[ijet] = rawpt[ijet];
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
          h_jet_pt_lj->Fill(jtpt_corr[ijetLeading], scale);
          h_xZj->Fill(jtpt_corr[ijetLeading]/Z_pt, scale);
        }
    }
  }  // end loop events

  // Normalize histogram
  if (!isData) {
  double Xsec = 5.595 * 100 / 1000; //nb-1
  double Lumi = 1.67; // nb-1
  double number_A = 208; // Lead
  double norm_signal = number_A * number_A * Xsec * Lumi * (iEvent/sum_weight_cen) / sum_weight;
  h_mumu->Scale(norm_signal);
  h_Z_pt->Scale(norm_signal);
  h_njet->Scale(norm_signal);
  h_jet_pt_lj->Scale(norm_signal);
  h_deltaPhi_Zj->Scale(norm_signal);
  h_xZj->Scale(norm_signal);
  h_vz->Scale(norm_signal);
  }

  cout << "Tot ev = " << iEvent << " ev with filter = " << iEvfltr << " sum_weight = " << sum_weight << endl;
  cout << "Number of events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) << endl;
  cout << "dphi>7pi/8: " << h_deltaPhi_Zj->Integral(h_deltaPhi_Zj->FindBin(7*TMath::Pi()/8), h_deltaPhi_Zj->GetNbinsX()+1) << endl;
  cout << "dphi<pi/3: " << h_deltaPhi_Zj->Integral(0,h_deltaPhi_Zj->FindBin(TMath::Pi()/3)) << endl;

  c1->cd(1);
  h_mumu->Draw();
//  c1->Print("h_mumu_PbPb.pdf");
  c2->cd(1);
  h_cen->Draw();
  c3->cd(1);
  h_njet->Draw();
//  c3->Print("h_njet_PbPb.pdf");
  c4->cd(1);
  h_jet_pt_lj->Draw();
//  c4->Print("h_jet_pt_lj_PbPb.pdf");
  c6->cd(1);
  h_xZj->Draw();
//  c6->Print("h_XZj_PbPb.pdf");
  c7->cd(1);
  h_deltaPhi_Zj->Draw();
//  c7->Print("h_deltaPhi_Zj_PbPb.pdf");
  // Create the main directory "MC" or "DATA"
  TDirectory *Dir = file_output_PbPb_mu->mkdir("PbPb");
  // Navigate to the directory
  Dir->cd();
  // Create a new directory named "Muons"
  TDirectory *muonsDir = Dir->mkdir("Muons");
  // Navigate to the "MUONS" directory
  muonsDir->cd();
  h_mumu->Write();
  h_Z_pt->Write();
  h_njet->Write();
  h_jet_pt_lj->Write();
  h_deltaPhi_Zj->Write();
  h_xZj->Write();
  h_vz->Write();
  //h_jetgirth->Write();
  //h_jet_deltaR->Write();

  file_output_PbPb_mu->Close();
}
