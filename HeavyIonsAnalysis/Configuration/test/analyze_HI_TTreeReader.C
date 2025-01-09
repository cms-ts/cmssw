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

//To run, root -l analyze_HI_TTreeReader.C
//Dafault isData, for MC root -l 'analyze_HI_TTreeReader.C(false)'
void analyze_HI_TTreeReader(bool isData = true) {
  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis");
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
    glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test7/*/*.root", GLOB_NOSORT, NULL, &globlist);
  }
  else {
    glob("/eos/infnts/cms/store/user/kdeleo/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/*/*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    cout << "This is a MC" << endl;
  }
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/muonAnalyzer/MuonTree");
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
    //skimanalysis.Add(TString(globlist.gl_pathv[i]) + "/skimanalysis/HltTree");
    //hltanalysis.Add(TString(globlist.gl_pathv[i]) + "/hltanalysis/HltTree");

  }
  globfree(&globlist);

  //To associate additional TTrees with a primary TTree. This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  data.AddFriend("HiTree");
  //data.AddFriend("skimanalysis");
  //data.AddFriend("hltanalysis");

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
  //TTreeReaderValue<Int_t> HLT_HIL2SingleMu7_v3 = {fReader, "HLT_HIL2SingleMu7_v3"};

  // Muon
  TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
  TTreeReaderArray<float> recoPt = {fReader, "recoPt"};
  TTreeReaderArray<float> recoEta = {fReader, "recoEta"};
  TTreeReaderArray<float> recoPhi = {fReader, "recoPhi"};
  TTreeReaderArray<int> recoCharge = {fReader, "recoCharge"};
  TTreeReaderArray<bool> recoIDTight = {fReader, "recoIDTight"};
  //TTreeReaderValue<vector<bool>> recoIDTight = {fReader, "recoIDTight"};
  const double muonMass = 0.1056583755; //From PDG 2024

  // Jet
  TTreeReaderValue<Int_t> nref = {fReader, "nref"};
  TTreeReaderArray<Float_t> jtpt = {fReader, "jtpt"};
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};

  //Canvas
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
  TH1F *h_mumu_60to120 = new TH1F("h_mumu_60to120", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 50, 130);
  TH1F *h_mumu_60to120_pt20_eta2_4 = new TH1F("h_mumu_60to120_pt20_eta2_4", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 50, 130);
  TH1F *h_njet = new TH1F("h_njet", "Hist;Number of jets; Entries", 10, 0, 10);
  TH1F *h_njet_good = new TH1F("h_njet_good", "Hist;Number of jets; Entries", 8, 0, 8);
  TH1F *h_jet_pt = new TH1F("h_jet_pt", "Hist;leading jet p_{T} [GeV]; Entries", 20, 30, 300);
  TH1F *h_jet_pt_good = new TH1F("h_jet_pt_good", "Hist;leading jet p_{T} [GeV]; Entries", 20, 30, 300);

  TH1F *h_mumu_m = new TH1F("h_mumu_m", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt = new TH1F("h_Z_pt", "Hist;p_{t}^{Z} [GeV]; Entries", 20, 40, 300);
  TH1F *h_deltaPhi = new TH1F("h_deltaPhi", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", 20, 0,3);

  TH1F *h_mumu_m_dPhiCut = new TH1F("h_mumu_m_dPhiCut", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt_dPhiCut = new TH1F("h_Z_pt_dPhiCut", "Hist;p_{t}^{Z} [GeV]; Entries", 20, 40, 300);
  TH1F *h_jet_pt_good_dPhiCut = new TH1F("h_jet_pt_good_dPhiCut", "Hist;leading jet p_{T} [GeV]; Entries", 20, 30, 300);
  TH1F *h_deltaPhi_dPhiCut = new TH1F("h_deltaPhi_dPhiCut", "Hist;#Delta#phi_{Zj}; Entries", 20, 0, TMath::Pi());

  // Output root file
  TFile *file_output_HI_mu;
  if (isData) {
    file_output_HI_mu = new TFile("output_HI_mu_data.root", "RECREATE");
  }
  else {
    file_output_HI_mu = new TFile("output_HI_mu_MC.root", "RECREATE");
  }
  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  while (fReader.Next()) {
    //if(*pprimaryVertexFilter<=0) continue;
    //if(*pclusterCompatibilityFilter<=0) continue;
    //if(*pphfCoincFilter2Th4<=0) continue;
    if(*hiBin>60) continue;
    //if(*HLT_HIL2SingleMu7_v3<=0) continue;
    //if (*nReco < 2 ) continue;
    iEvent++;
    bool good_pair = false;
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // TLorentzVectors for the muons
    TLorentzVector muPlus, muMinus;

    // Loop over muons, save indices of most energetic muon and antimuon pairs
    int iHighPtMu = -1;
    int iHighPtAntiMu = -1;
    //cout << "-----------------------------" << endl;
    for (unsigned int iMu = 0; iMu < *nReco; ++iMu) {
      if (!recoIDTight[iMu]) continue;
      //cout << "iMu: " << iMu << " pt = " << recoPt[iMu] <<  " Q = " << recoCharge[iMu] << endl;
      if (iHighPtMu == -1 || recoPt[iMu] > recoPt[iHighPtMu]) {
        if (recoCharge[iMu] == -1) iHighPtMu = iMu;
      }
      if (iHighPtAntiMu == -1 || recoPt[iMu] > recoPt[iHighPtAntiMu]) {
        if (recoCharge[iMu] == +1) iHighPtAntiMu = iMu;
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
      h_mumu_60to120->Fill(Z_mass);
      if (recoPt[iHighPtMu] > 20 && abs(recoEta[iHighPtMu]) < 2.4 && recoPt[iHighPtAntiMu] > 20 && abs(recoEta[iHighPtAntiMu]) < 2.4){
        good_pair = true;
        h_mumu_60to120_pt20_eta2_4->Fill(Z_mass);
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
      detaMinus = jteta[ijet] - muMinus.Eta();
      dphiMinus = jtphi[ijet] - muMinus.Phi();
      dRMinus = TMath::Sqrt(detaMinus * detaMinus + dphiMinus * dphiMinus);
      detaPlus = jteta[ijet] - muPlus.Eta();
      dphiPlus = jtphi[ijet] - muPlus.Phi();
      dRPlus = TMath::Sqrt(detaPlus * detaPlus + dphiPlus * dphiPlus);
      if(dRMinus < 0.2 || dRPlus < 0.2 ) continue;
      //cout << "-----------------------------" << endl;
      //cout << "ijet: " << ijet << " pt = " << jtpt[ijet] << " eta = " << jteta[ijet] << " phi = " << jtphi[ijet] << " m = " << jtm[ijet] << endl;
      //cout << "muMinus pt = " << muMinus.Pt() << " eta = " << muMinus.Eta() << " phi = " << muMinus.Phi() << endl;
      //cout << "muPlus pt = " << muPlus.Pt() << " eta = " << muPlus.Eta() << " phi = " << muPlus.Phi() << endl;
      //cout << "dRMinus = " << dRMinus << " dRPlus = " << dRPlus << endl;
      njets++;
      if (ijetLeading == -1 || jtpt[ijet] > jtpt[ijetLeading]) {
          ijetLeading = ijet;
      }
      if(good_pair == true){
	njets_good++;
      }
    }
    h_njet->Fill(njets);
    //cout << "ijetLeading = " << ijetLeading << endl;
    if (ijetLeading != -1) {
      h_jet_pt->Fill(jtpt[ijetLeading]);
      if(good_pair == true) {
        h_njet_good->Fill(njets_good);
        h_jet_pt_good->Fill(jtpt[ijetLeading]);
        h_mumu_m->Fill(Z_mass);
        h_Z_pt->Fill(Z_pt);
        h_xZj->Fill(jtpt[ijetLeading]/Z_pt);
        double dPhi_Zj = 0;
        if (abs(Z_phi-jtphi[ijetLeading]) <= TMath::Pi()) dPhi_Zj = abs(Z_phi-jtphi[ijetLeading]);
        else if (abs(Z_phi-jtphi[ijetLeading]) > TMath::Pi()) dPhi_Zj = 2 * TMath::Pi() - abs(Z_phi-jtphi[ijetLeading]);
        h_deltaPhi->Fill(dPhi_Zj);
        if (dPhi_Zj > 2 * TMath::Pi() / 3) {
          h_jet_pt_good_dPhiCut->Fill(jtpt[ijetLeading]);
          h_mumu_m_dPhiCut->Fill(Z_mass);
          h_Z_pt_dPhiCut->Fill(Z_pt);
          h_deltaPhi_dPhiCut->Fill(dPhi_Zj);
        }
      }
    }
  }

  cout << "Number of events = " << h_mumu_m->Integral() << ", if Z_pt>80: " << h_Z_pt->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << ", if d_eta>2pi/3: " << h_deltaPhi_dPhiCut->Integral() << endl;

  h_mumu_60to120->SetLineColor(kBlue);
  h_mumu_60to120->SetLineWidth(2);
  h_mumu_60to120_pt20_eta2_4->SetLineColor(kBlack);
  h_mumu_60to120_pt20_eta2_4->SetLineWidth(2);

  c1->cd(1);
  h_mumu_60to120->Draw();
  h_mumu_60to120_pt20_eta2_4->Draw("same");

  TLegend *legend = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend->AddEntry(h_mumu_60to120, "60<m_{#mu#mu}<120 GeV", "l");
  legend->AddEntry(h_mumu_60to120_pt20_eta2_4, "60<m_{#mu#mu}<120 GeV, p_{T}^{#mu}>20 GeV, |#eta^{#mu}|<2.4", "l");
  legend->SetTextSize(0.03);
  legend->SetBorderSize(0);
  legend->Draw("same");

  h_njet->SetLineColor(kRed);
  h_njet->SetLineWidth(2);
  h_njet_good->SetLineColor(kRed);
  h_njet_good->SetLineWidth(2);
  h_jet_pt->SetLineColor(kRed);
  h_jet_pt->SetLineWidth(2);
  h_jet_pt_good->SetLineColor(kRed);
  h_jet_pt_good->SetLineWidth(2);

  h_mumu_m->SetLineColor(kRed);
  h_mumu_m->SetLineWidth(2);
  h_Z_pt->SetLineColor(kRed);
  h_Z_pt->SetLineWidth(2);
  h_deltaPhi->SetLineColor(kRed);
  h_deltaPhi->SetLineWidth(2);
  h_xZj->SetLineColor(kRed);
  h_xZj->SetLineWidth(2);

  h_jet_pt_good_dPhiCut->SetLineColor(kGreen);
  h_jet_pt_good_dPhiCut->SetLineWidth(2);
  h_mumu_m_dPhiCut->SetLineColor(kGreen);
  h_mumu_m_dPhiCut->SetLineWidth(2);
  h_Z_pt_dPhiCut->SetLineColor(kGreen);
  h_Z_pt_dPhiCut->SetLineWidth(2);
  h_deltaPhi_dPhiCut->SetLineColor(kGreen);
  h_deltaPhi_dPhiCut->SetLineWidth(2);

  c2->cd(1);
  gPad->SetLogy();
  h_njet->Draw();

  c3->cd(1);
  gPad->SetLogy();
  h_njet_good->Draw();

  c4->cd(1);
  h_jet_pt->Draw();

  c5->cd(1);
  h_jet_pt_good->Draw();
  h_jet_pt_good_dPhiCut->Draw("same");

  TLegend *legend_5 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_5->AddEntry(h_jet_pt_good, "all #Delta#phi_{Zj}", "l");
  legend_5->AddEntry( h_jet_pt_good_dPhiCut, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_5->SetTextSize(0.03);
  legend_5->SetBorderSize(0);
  legend_5->Draw("same");

  c6->cd(1);
  h_mumu_m->Draw();
  h_mumu_m_dPhiCut->Draw("same");

  TLegend *legend_6 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_6->AddEntry(h_mumu_m, "all #Delta#phi_{Zj}", "l");
  legend_6->AddEntry( h_mumu_m_dPhiCut, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_6->SetTextSize(0.03);
  legend_6->SetBorderSize(0);
  legend_6->Draw("same");

  c7->cd(1);
  h_Z_pt->Draw();
  h_Z_pt_dPhiCut->Draw("same");

  TLegend *legend_7 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_7->AddEntry(h_Z_pt, "all #Delta#phi_{Zj}", "l");
  legend_7->AddEntry( h_Z_pt_dPhiCut, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_7->SetTextSize(0.03);
  legend_7->SetBorderSize(0);
  legend_7->Draw("same");

  c8->cd(1);
  h_deltaPhi->Draw();
  h_deltaPhi_dPhiCut->Draw("same");

  TLegend *legend_8 = new TLegend(0.55, 0.7, 0.8, 0.89);
  legend_8->AddEntry(h_deltaPhi, "all #Delta#phi_{Zj}", "l");
  legend_8->AddEntry( h_deltaPhi_dPhiCut, "#Delta#phi_{Zj}>2#pi/3", "l");
  legend_8->SetTextSize(0.03);
  legend_8->SetBorderSize(0);
  legend_8->Draw("same");

  c9->cd(1);
  h_xZj->Draw();

  c1->SaveAs("h_mumu.pdf");
  c2->SaveAs("h_njets.pdf");
  c3->SaveAs("h_njets_good.pdf");
  c4->SaveAs("h_jet_pt.pdf");
  c5->SaveAs("h_jet_pt_good.pdf");
  c6->SaveAs("h_mumu_m.pdf");
  c7->SaveAs("h_Z_pt.pdf");
  c8->SaveAs("h_deltaPhi.pdf");
  c9->SaveAs("h_xZj.pdf");

  // Create the main directory "MC" or "DATA"
  TDirectory *Dir = file_output_HI_mu->mkdir("HI");
  // Navigate to the directory
  Dir->cd();
  // Create a new directory named "Muons"
  TDirectory *muonsDir = Dir->mkdir("Muons");
  // Navigate to the "MUONS" directory
  muonsDir->cd();
  h_njet_good->Write();
  h_jet_pt_good->Write();
  h_mumu_m->Write();
  h_Z_pt->Write();
  h_deltaPhi->Write();
  h_mumu_m_dPhiCut->Write();
  h_Z_pt_dPhiCut->Write();
  h_xZj->Write();

  file_output_HI_mu->Close();
}
