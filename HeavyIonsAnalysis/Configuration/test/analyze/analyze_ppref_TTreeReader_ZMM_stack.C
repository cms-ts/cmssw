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

#include "MC_samples_ppref.h" // Include the header file for MC samples

using namespace std;

double RelativePhi(double phi_1,double phi_2) {
  double d_phi =  abs(phi_1 - phi_2);
  if (d_phi > acos(-1)) d_phi = 2*acos(-1) - d_phi;
  return d_phi;
}

//To run, root -l analyze_HI_TTreeReader_ZMM.C
//Default isData, for MC root -l 'analyze_HI_TTreeReader_ZMM.C(false)'
void analyze_ppref_TTreeReader_ZMM_stack(const char * argument_name) {

  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis");

  glob_t globlist;
  bool isData = false;
  double Xsec = 1;
  double total_ngen = 1;
  TString file_name = argument_name;
  if (file_name.Contains("data")) {
    isData = true;
    cout << "This is data: " << file_name << endl;
    glob("/eos/infnts/cms/store/user/kdeleo/PPRefSingleMuon*/CRAB3_Analysis_test16_ZMM_PPRefSingleMuon*/*/000*.root", GLOB_NOSORT, NULL, &globlist);
  }
  else {
    // Loop over files
    for (const auto& file : files) {
      if (file_name.Contains(file.label)) {
        glob(file.path_miniaod, GLOB_NOSORT, NULL, &globlist);
        Xsec = file.xsec; //pb
        total_ngen = file.ngen;
        cout << "This is MC " << file.label << ": ngen = " << file.ngen << " xsec = " << file.xsec << endl;
      }
    }
  }
  cout << "Found " << globlist.gl_pathc << " files" << endl;

  //glob("/eos/infnts/cms/store/user/kdeleo/DYToMuMu_M-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_DYto2Mu/251115_123708/0000/HiForestMiniAOD_*.root", GLOB_NOSORT, NULL, &globlist);

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    //data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzerSubstructure/t");
    data.Add(TString(globlist.gl_pathv[i]) + "/ak2PFJetAnalyzer/t");
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
  //TTreeReaderValue<float> Ncoll = {fReader, "Ncoll"}; // Ncoll

  // Filters
  TTreeReaderValue<int> goodvertex = {fReader, "goodvertex"}; //pprimaryVertexFilter+noscrape

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

  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_deltaPhi_Zj = new TH1F("h_deltaPhi_Zj", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", 20, 0, 3);

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Output root file
  TFile *file_output_ppref_mu;

    if (isData) {
      file_output_ppref_mu = new TFile("./stack/output_ppref_mu_data.root", "RECREATE");
    }
    if (!isData) {
      file_output_ppref_mu = new TFile("./stack/output_ppref_mu_MC_"+file_name+".root", "RECREATE");
    }

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  unsigned int iEvfltr = 0;
  double sum_weight = 0;
  while (fReader.Next()) {
  iEvent++;
  if (!isData) sum_weight += *weight;
    if(*goodvertex<=0) continue;
  iEvfltr++;
    // Scale MC
    float scale =1;
      if (!isData) {
        scale*=(*weight);
      }

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
  double Lumi = 479; // pb-1
//  double norm_signal = Xsec * Lumi / sum_weight;
  // FIXED NORMALIZATION FORMULA
  // Since your TTree weights are ~Xsec, we normalize by (Lumi / N_gen)
  // This ensures: Final_Yield = Sum(Weight) * Norm = (N_pass * Xsec) * (Lumi / N_gen) = sigma * L * eff
  double norm_signal = Lumi / total_ngen;
  h_mumu->Scale(norm_signal);
  h_Z_pt->Scale(norm_signal);
  h_njet->Scale(norm_signal);
  h_jet_pt_lj->Scale(norm_signal);
  h_deltaPhi_Zj->Scale(norm_signal);
  h_xZj->Scale(norm_signal);
  h_vz->Scale(norm_signal);
  }

  cout << "Tot ev = " << iEvent << " ev with filter = " << iEvfltr << " sum_weight = " << sum_weight << endl;
  cout << "average weight = " << sum_weight/iEvent << endl;
  cout << "Number of events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) << endl;
  cout << "dphi<pi/3: " << h_deltaPhi_Zj->Integral(0,h_deltaPhi_Zj->FindBin(TMath::Pi()/3)) << endl;
  c1->cd(1);
  h_mumu->Draw();
  c3->cd(1);
  h_njet->Draw();
  c4->cd(1);
  h_jet_pt_lj->Draw();
  c6->cd(1);
  h_xZj->Draw();
  c7->cd(1);
  h_deltaPhi_Zj->Draw();
  // Create the main directory "MC" or "DATA"
  TDirectory *Dir = file_output_ppref_mu->mkdir("pp");
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

  file_output_ppref_mu->Close();
}
