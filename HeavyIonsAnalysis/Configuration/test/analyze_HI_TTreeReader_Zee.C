// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TChain.h"
#include <TLorentzVector.h>
#include <TCanvas.h>
#include "TLatex.h"
#include <TDirectory.h>

#include <glob.h>

using namespace std;

//To run, root -l analyze_HI_TTreeReader
//Dafault isData, for MC root -l 'analyze_HI_TTreeReader(false)'
void analyze_HI_TTreeReader_Zee(bool isData = true) {
  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"), skimanalysis("skimanalysis"), hltanalysis("hltanalysis");
  //data.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0000/HiForestMiniAOD_DATA_*.root/akCs2PFJetAnalyzer/t");
  //data.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0001/HiForestMiniAOD_DATA_*.root/akCs2PFJetAnalyzer/t");
  //data.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0002/HiForestMiniAOD_DATA_*.root/akCs2PFJetAnalyzer/t");
  //data.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0003/HiForestMiniAOD_DATA_*.root/akCs2PFJetAnalyzer/t");
  //data.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0004/HiForestMiniAOD_DATA_*.root/akCs2PFJetAnalyzer/t");

  //data.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0005/HiForestMiniAOD_DATA_*.root/akCs2PFJetAnalyzer/t");
  //EventTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0000/HiForestMiniAOD_DATA_*.root/muonAnalyzer/MuonTree");
  //EventTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0001/HiForestMiniAOD_DATA_*.root/muonAnalyzer/MuonTree");
  //EventTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0002/HiForestMiniAOD_DATA_*.root/muonAnalyzer/MuonTree");
  //EventTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0003/HiForestMiniAOD_DATA_*.root/muonAnalyzer/MuonTree");
  //EventTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0004/HiForestMiniAOD_DATA_*.root/muonAnalyzer/MuonTree");
  //EventTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0005/HiForestMiniAOD_DATA_*.root/muonAnalyzer/MuonTree");

  //TChain HiTree("HiTree"), skimanalysis("skimanalysis"),hltanalysis("hltanalysis");
  //HiTree.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0000/HiForestMiniAOD_DATA_99*.root/hiEvtAnalyzer/HiTree");
  //skimanalysis.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0000/HiForestMiniAOD_DATA_99*.root/skimanalysis/HltTree");
  //hltanalysis.Add("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime0/CRAB3_Analysis_test7/241106_164058/0000/HiForestMiniAOD_DATA_99*.root/hltanalysis/HltTree");

  glob_t globlist;
  //glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/*/*/*/HiForestMiniAOD_DATA_*.root", GLOB_NOSORT, NULL, &globlist);
  if (isData) {
    glob("/eos/infnts/cms/store/user/rdelliga/HIPhysicsRawPrime*/CRAB3_Analysis_test5/*/*.root", GLOB_NOSORT, NULL, &globlist);
  cout << "This is data with 2E" << endl;
  }
  else {
    glob("/eos/infnts/cms/store/user/rdelliga/DYto2E_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/*/*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    cout << "This is a MC simulation for DYto2E" << endl;
  }
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/ggHiNtuplizer/EventTree");
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
    skimanalysis.Add(TString(globlist.gl_pathv[i]) + "/skimanalysis/HltTree");
    hltanalysis.Add(TString(globlist.gl_pathv[i]) + "/hltanalysis/HltTree");
  }
  globfree(&globlist);

  //To associate additional TTrees with a primary TTree. This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  data.AddFriend("HiTree");
  data.AddFriend("skimanalysis");
  data.AddFriend("hltanalysis");

  TTreeReader fReader(&data);

  // Declaration of leaf types
  TTreeReaderValue<Int_t> run = {fReader, "run"};    // Run number
  TTreeReaderValue<Int_t> evt = {fReader, "evt"};    // Event number
  TTreeReaderValue<Int_t> lumi = {fReader, "lumi"};  // Luminosity block
  TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"}; // centralityx2

  // Filters
  //TTreeReaderValue<int> pprimaryVertexFilter = {fReader, "pprimaryVertexFilter"};
  //TTreeReaderValue<int> pclusterCompatibilityFilter = {fReader, "pclusterCompatibilityFilter"};
  //TTreeReaderValue<int> pphfCoincFilter2Th4 = {fReader, "pphfCoincFilter2Th4"};

  // Trigger
  //TTreeReaderValue<Int_t> HLT_HIMinimumBiasHF1AND_v3 = {fReader, "HLT_HIMinimumBiasHF1AND_v3"};

  // Electron
  TTreeReaderValue<Int_t> nEle = {fReader, "nEle"};
  TTreeReaderArray<float> elePt = {fReader, "elePt"};
  TTreeReaderArray<float> eleEta = {fReader, "eleEta"};
  TTreeReaderArray<float> elePhi = {fReader, "elePhi"};
  TTreeReaderArray<int> eleCharge = {fReader, "eleCharge"};
  const double eleMass = 0.00051099895; //Gev, from PDG 2024

  // Jet
  TTreeReaderValue<Int_t> nref = {fReader, "nref"};
  TTreeReaderArray<Float_t> jtpt = {fReader, "jtpt"};
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};

  //Canvas
  TCanvas* c = new TCanvas("c", "c", 1200, 800);
  c->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c1 = new TCanvas("c1", "c1", 1200, 800);
  c1->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c2 = new TCanvas("c2", "c2", 1200, 800);
  c2->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c3 = new TCanvas("c3", "c3", 1200, 800);
  c3->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c4 = new TCanvas("c4", "c4", 1200, 800);
  c4->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c5 = new TCanvas("c5", "c5", 1200, 800);
  c5->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c6 = new TCanvas("c6", "c6", 1200, 800);
  c6->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c7 = new TCanvas("c7", "c7", 1200, 800);
  c7->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c8 = new TCanvas("c8", "c8", 1200, 800);
  c8->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  TCanvas* c9 = new TCanvas("c9", "c9", 1200, 800);
  c9->Divide(1,1);
  gPad->SetTopMargin(0.07);
  gPad->SetBottomMargin(0.17);
  gPad->SetLeftMargin(0.2);
  gPad->SetRightMargin(0.1);
  gStyle->SetOptStat(0);

  //Histograms
  TH1F *h_e_eta = new TH1F("h_e_eta", "Hist;|#eta_{e}|; Entries", 100, 0, TMath::Pi());
  TH1F *h_ee_60to120 = new TH1F("h_ee_60to120", "Hist;m_{ee} [GeV]; Entries", 20, 50, 130);
  TH1F *h_ee_60to120_pt20_eta2_2 = new TH1F("h_ee_60to120_pt20_eta2_2", "Hist;m_{ee} [GeV]; Entries", 20, 50, 130);
  TH1F *h_njet_ee = new TH1F("h_njet_ee", "Hist;Number of jets; Entries", 10, 0, 10);
  TH1F *h_njet_good_ee = new TH1F("h_njet_good_ee", "Hist;Number of jets; Entries", 8, 0, 8);
  TH1F *h_jet_pt_ee = new TH1F("h_jet_pt_ee", "Hist;leading jet p_{T} [GeV]; Entries", 20, 30, 300);
  TH1F *h_jet_pt_good_ee = new TH1F("h_jet_pt_good_ee", "Hist;leading jet p_{T} [GeV]; Entries", 20, 30, 300);

  TH1F *h_ee_m = new TH1F("h_ee_m", "Hist;m_{ee} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt_ee = new TH1F("h_Z_pt_ee", "Hist;p_{t}^{Z} [GeV]; Entries", 20, 40, 300);
  TH1F *h_deltaPhi_ee = new TH1F("h_deltaPhi_ee", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj_ee = new TH1F("h_xZj_ee", "Hist;x_{Zj}; Entries", 20, 0,3);

  TH1F *h_ee_m_dPhiCut = new TH1F("h_ee_m_dPhiCut", "Hist;m_{ee} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt_dPhiCut_ee = new TH1F("h_Z_pt_dPhiCut_ee", "Hist;p_{t}^{Z} [GeV]; Entries", 20, 40, 300);
  TH1F *h_jet_pt_good_dPhiCut_ee = new TH1F("h_jet_pt_good_dPhiCut_ee", "Hist;leading jet p_{T} [GeV]; Entries", 20, 30, 300);
  TH1F *h_deltaPhi_dPhiCut_ee = new TH1F("h_deltaPhi_dPhiCut_ee", "Hist;#Delta#phi_{Zj}; Entries", 20, 0, TMath::Pi());

  // Output root file
  TFile *file_output_HI_ee;
  if (isData) {
    file_output_HI_ee = new TFile("output_HI_ee_data.root", "RECREATE");
  }
  else {
    file_output_HI_ee = new TFile("output_HI_ee_MC.root", "RECREATE");
  }
  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    //if(*pprimaryVertexFilter<=0) continue;
    //if(*pclusterCompatibilityFilter<=0) continue;
    //if(*pphfCoincFilter2Th4<=0) continue;
    if(*hiBin>60) continue;
    //if(*HLT_HIMinimumBiasHF1AND_v3<=0) continue;
    if (*nEle < 2 ) continue;
    iEvent++;
    bool good_pair = false;
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // TLorentzVectors for the electrons
    TLorentzVector ePlus, eMinus;

    // Loop over electrons, save indices of most energetic electron and positron pairs
    int iHighPteMinus = -1;
    int iHighPtePlus = -1;
    //cout << "-----------------------------" << endl;
    for (unsigned int iE = 0; iE < *nEle; ++iE) {
      // Exclude electron candidates in the transition region between the barrel and endcap (1.44 < |η| < 1.57)
      if (abs(eleEta[iE]) > 1.44 && abs(eleEta[iE]) < 1.57) continue;
      h_e_eta->Fill(abs(eleEta[iE]));
      //cout << "iE: " << iE << " pt = " << elePt[iE] <<  " Q = " << eleCharge[iE] << endl;
      if (iHighPteMinus == -1 || elePt[iE] > elePt[iHighPteMinus]) {
        if (eleCharge[iE] == -1) iHighPteMinus = iE;
      }
      if (iHighPtePlus == -1 || elePt[iE] > elePt[iHighPtePlus]) {
        if (eleCharge[iE] == +1) iHighPtePlus = iE;
      }
    }
    //cout << "iHighPteMinus = " << iHighPteMinus << " iHighPtePlus = " << iHighPtePlus << endl;

    // Z from electron-positron pairs
    if (iHighPteMinus == -1 || iHighPtePlus == -1) continue;
    eMinus.SetPtEtaPhiM(elePt[iHighPteMinus], eleEta[iHighPteMinus], elePhi[iHighPteMinus], eleMass);
    ePlus.SetPtEtaPhiM(elePt[iHighPtePlus], eleEta[iHighPtePlus], elePhi[iHighPtePlus], eleMass);
    double Z_mass = (ePlus + eMinus).M();
    double Z_pt = (ePlus + eMinus).Pt();
    double Z_phi = (ePlus + eMinus).Phi();

    // Apply mass cut
    if (Z_mass >= 60 && Z_mass <= 120) {
      h_ee_60to120->Fill(Z_mass);
      if (elePt[iHighPteMinus] > 20 && abs(eleEta[iHighPteMinus]) < 2.2 && elePt[iHighPtePlus] > 20 && abs(eleEta[iHighPtePlus]) < 2.2){
        good_pair = true;
        h_ee_60to120_pt20_eta2_2->Fill(Z_mass);
      }
    }

    // Cut on pt(Z)
    if (Z_pt < 40 ) continue;

    // Loop over Jets
    unsigned int njets = 0, njets_good = 0;
    double detaMinus = 0, dphiMinus = 0, dRMinus = 0;
    double detaPlus = 0, dphiPlus = 0, dRPlus = 0;
    int ijetLeading = -1;
    for(int ijet=0; ijet<*nref; ijet++){
      if(jtpt[ijet]<30) continue;
      if(abs(jteta[ijet])>2.5) continue;
      detaMinus = jteta[ijet] - eMinus.Eta();
      dphiMinus = jtphi[ijet] - eMinus.Phi();
      dRMinus = TMath::Sqrt(detaMinus * detaMinus + dphiMinus * dphiMinus);
      detaPlus = jteta[ijet] - ePlus.Eta();
      dphiPlus = jtphi[ijet] - ePlus.Phi();
      dRPlus = TMath::Sqrt(detaPlus * detaPlus + dphiPlus * dphiPlus);
      if(dRMinus < 0.2 || dRPlus < 0.2 ) continue;
      //cout << "-----------------------------" << endl;
      //cout << "ijet: " << ijet << " pt = " << jtpt[ijet] << " eta = " << jteta[ijet] << " phi = " << jtphi[ijet] << " m = " << jtm[ijet] << endl;
      //cout << "eMinus pt = " << eMinus.Pt() << " eta = " << eMinus.Eta() << " phi = " << eMinus.Phi() << endl;
      //cout << "ePlus pt = " << ePlus.Pt() << " eta = " << ePlus.Eta() << " phi = " << ePlus.Phi() << endl;
      //cout << "dRMinus = " << dRMinus << " dRPlus = " << dRPlus << endl;
      njets++;
      if (ijetLeading == -1 || jtpt[ijet] > jtpt[ijetLeading]) {
          ijetLeading = ijet;
      }
      if(good_pair == true){
	njets_good++;
      }
    }
    h_njet_ee->Fill(njets);
    //cout << "ijetLeading = " << ijetLeading << endl;
    if (ijetLeading != -1) {
      h_jet_pt_ee->Fill(jtpt[ijetLeading]);
      if(good_pair == true) {
        h_njet_good_ee->Fill(njets_good);
        h_jet_pt_good_ee->Fill(jtpt[ijetLeading]);
        h_ee_m->Fill(Z_mass);
        h_Z_pt_ee->Fill(Z_pt);
        h_xZj_ee->Fill(jtpt[ijetLeading]/Z_pt);
        double dPhi_Zj = 0;
        if (abs(Z_phi-jtphi[ijetLeading]) <= TMath::Pi()) dPhi_Zj = abs(Z_phi-jtphi[ijetLeading]);
        else if (abs(Z_phi-jtphi[ijetLeading]) > TMath::Pi()) dPhi_Zj = 2 * TMath::Pi() - abs(Z_phi-jtphi[ijetLeading]);
        h_deltaPhi_ee->Fill(dPhi_Zj);
        if (dPhi_Zj > 2 * TMath::Pi() / 3) {
          h_jet_pt_good_dPhiCut_ee->Fill(jtpt[ijetLeading]);
          h_ee_m_dPhiCut->Fill(Z_mass);
          h_Z_pt_dPhiCut_ee->Fill(Z_pt);
          h_deltaPhi_dPhiCut_ee->Fill(dPhi_Zj);
        }
      }
    }
  }

  cout << "Number of events = " << h_ee_m->Integral() << ", if Z_pt>80: " << h_Z_pt_ee->Integral(h_Z_pt_ee->FindBin(80), h_Z_pt_ee->GetNbinsX()+1) << ", if d_eta>2pi/3: " << h_deltaPhi_dPhiCut_ee->Integral() << endl;
  h_e_eta->SetLineWidth(2);

  c->cd(1);
  h_e_eta->Draw();

  h_ee_60to120->SetLineColor(kBlue);
  h_ee_60to120->SetLineWidth(2);
  h_ee_60to120_pt20_eta2_2->SetLineColor(kBlack);
  h_ee_60to120_pt20_eta2_2->SetLineWidth(2);

  c1->cd(1);
  h_ee_60to120->Draw();
  h_ee_60to120_pt20_eta2_2->Draw("same");

  TLegend *legend = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend->AddEntry(h_ee_60to120, "60<m_{ee}<120 GeV", "l");
  legend->AddEntry(h_ee_60to120_pt20_eta2_2, "60<m_{ee}<120 GeV, p_{T}^{e}>20 GeV, |#eta^{e}|<2.2", "l");
  legend->SetTextSize(0.03);
  legend->SetBorderSize(0);
  legend->Draw("same");

  h_njet_ee->SetLineColor(kRed);
  h_njet_ee->SetLineWidth(2);
  h_njet_good_ee->SetLineColor(kRed);
  h_njet_good_ee->SetLineWidth(2);
  h_jet_pt_ee->SetLineColor(kRed);
  h_jet_pt_ee->SetLineWidth(2);
  h_jet_pt_good_ee->SetLineColor(kRed);
  h_jet_pt_good_ee->SetLineWidth(2);

  h_ee_m->SetLineColor(kRed);
  h_ee_m->SetLineWidth(2);
  h_Z_pt_ee->SetLineColor(kRed);
  h_Z_pt_ee->SetLineWidth(2);
  h_deltaPhi_ee->SetLineColor(kRed);
  h_deltaPhi_ee->SetLineWidth(2);
  h_xZj_ee->SetLineColor(kRed);
  h_xZj_ee->SetLineWidth(2);

  h_jet_pt_good_dPhiCut_ee->SetLineColor(kGreen);
  h_jet_pt_good_dPhiCut_ee->SetLineWidth(2);
  h_ee_m_dPhiCut->SetLineColor(kGreen);
  h_ee_m_dPhiCut->SetLineWidth(2);
  h_Z_pt_dPhiCut_ee->SetLineColor(kGreen);
  h_Z_pt_dPhiCut_ee->SetLineWidth(2);
  h_deltaPhi_dPhiCut_ee->SetLineColor(kGreen);
  h_deltaPhi_dPhiCut_ee->SetLineWidth(2);

  c2->cd(1);
  gPad->SetLogy();
  h_njet_ee->Draw();

  c3->cd(1);
  gPad->SetLogy();
  h_njet_good_ee->Draw();

  c4->cd(1);
  h_jet_pt_ee->Draw();

  c5->cd(1);
  h_jet_pt_good_ee->Draw();
  h_jet_pt_good_dPhiCut_ee->Draw("same");

  TLegend *legend_5 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_5->AddEntry(h_jet_pt_good_ee, "all #Delta#phi_{Zj}", "l");
  legend_5->AddEntry( h_jet_pt_good_dPhiCut_ee, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_5->SetTextSize(0.03);
  legend_5->SetBorderSize(0);
  legend_5->Draw("same");

  c6->cd(1);
  h_ee_m->Draw();
  h_ee_m_dPhiCut->Draw("same");

  TLegend *legend_6 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_6->AddEntry(h_ee_m, "all #Delta#phi_{Zj}", "l");
  legend_6->AddEntry( h_ee_m_dPhiCut, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_6->SetTextSize(0.03);
  legend_6->SetBorderSize(0);
  legend_6->Draw("same");

  c7->cd(1);
  h_Z_pt_ee->Draw();
  h_Z_pt_dPhiCut_ee->Draw("same");

  TLegend *legend_7 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_7->AddEntry(h_Z_pt_ee, "all #Delta#phi_{Zj}", "l");
  legend_7->AddEntry( h_Z_pt_dPhiCut_ee, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_7->SetTextSize(0.03);
  legend_7->SetBorderSize(0);
  legend_7->Draw("same");

  c8->cd(1);
  h_deltaPhi_ee->Draw();
  h_deltaPhi_dPhiCut_ee->Draw("same");

  TLegend *legend_8 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_8->AddEntry(h_deltaPhi_ee, "all #Delta#phi_{Zj}", "l");
  legend_8->AddEntry( h_deltaPhi_dPhiCut_ee, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_8->SetTextSize(0.03);
  legend_8->SetBorderSize(0);
  legend_8->Draw("same");

  c9->cd(1);
  h_xZj_ee->Draw();

  c->SaveAs("h_e_eta.pdf");
  c1->SaveAs("h_ee.pdf");
  c2->SaveAs("h_njets_ee.pdf");
  c3->SaveAs("h_njets_good_ee.pdf");
  c4->SaveAs("h_jet_pt_ee.pdf");
  c5->SaveAs("h_jet_pt_good_ee.pdf");
  c6->SaveAs("h_ee_m.pdf");
  c7->SaveAs("h_Z_pt_ee.pdf");
  c8->SaveAs("h_deltaPhi_ee.pdf");
  c9->SaveAs("h_xZj_ee.pdf");


  // Create the main directory "MC" or "DATA"
  TDirectory *Dir = file_output_HI_ee->mkdir("HI");
  // Navigate to the directory
  Dir->cd();
  // Create a new directory named "Electrons"
  TDirectory *electronsDir = Dir->mkdir("Electrons");
  // Navigate to the "ELECTRONS" directory
  electronsDir->cd();
  h_njet_good_ee->Write();
  h_jet_pt_good_ee->Write();
  h_ee_m->Write();
  h_Z_pt_ee->Write();
  h_deltaPhi_ee->Write();
  h_ee_m_dPhiCut->Write();
  h_Z_pt_dPhiCut_ee->Write();
  h_xZj_ee->Write();

  file_output_HI_ee->Close();
}
