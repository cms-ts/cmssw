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

#include <glob.h>

using namespace std;

void analyze_HI_TTreeReader() {
  //TTrees
  TChain data("data"), EventTree("EventTree") ;
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
  glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/*/*/*.root", GLOB_NOSORT, NULL, &globlist);

  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/muonAnalyzer/MuonTree");
  }
  globfree(&globlist);

  //To associate additional TTrees with a primary TTree. This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  //data.AddFriend("HiTree");
  //data.AddFriend("skimanalysis");
  //data.AddFriend("hltanalysis");
 
  TTreeReader fReader(&data);
 
  // Declaration of leaf types 
  TTreeReaderValue<Int_t> run = {fReader, "run"};    // Run number
  TTreeReaderValue<Int_t> evt = {fReader, "evt"};    // Event number
  TTreeReaderValue<Int_t> lumi = {fReader, "lumi"};  // Luminosity block

  // Muon
  TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
  TTreeReaderArray<float> recoPt = {fReader, "recoPt"};
  TTreeReaderArray<float> recoEta = {fReader, "recoEta"};
  TTreeReaderArray<float> recoPhi = {fReader, "recoPhi"};
  TTreeReaderArray<int> recoCharge = {fReader, "recoCharge"};
  TTreeReaderValue<vector<bool>> recoIDTight = {fReader, "recoIDTight"};

  // Jet
  TTreeReaderValue<Int_t> nref = {fReader, "nref"};
  TTreeReaderArray<Float_t> jtpt = {fReader, "jtpt"};
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};

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

  TH1F *h_mumu_60to120 = new TH1F("h_mumu_60to120", "Hist;m_{#mu#mu} [GeV]; Entries", 200, 0, 200);
  TH1F *h_mumu_50to130 = new TH1F("h_mumu_50to130", "Hist;m_{#mu#mu} [GeV]; Entries", 200, 0, 200);
  TH1F *h_mumu_60to120_pt10 = new TH1F("h_mumu_60to120_pt10", "Hist;m_{#mu#mu} [GeV]; Entries", 200, 0, 200);
  TH1F *h_njet = new TH1F("h_njet", "Hist;Number of jets; Entries", 20, 0, 20);
  TH1F *h_njet_good = new TH1F("h_njet_good", "Hist;Number of jets; Entries", 20, 0, 20);
  TH1F *h_jet_pt = new TH1F("h_jet_pt", "Hist;leading jet p_{T} [GeV]; Entries", 500, 0, 500);
  TH1F *h_jet_pt_good = new TH1F("h_jet_pt_good", "Hist;leading jet p_{T} [GeV]; Entries", 500, 0, 500);

  // Loop over events
  int iEvent = 0;
  while (fReader.Next()) {
    iEvent++;
    bool good_pair = false;
    //std::cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << std::endl;
    
    // Access and analyze the data
    
    // Create TLorentzVectors for the muons
    TLorentzVector mu1, mu2;

    // Loop over muon pairs
    for (unsigned int iMu1 = 0; iMu1 < *nReco; ++iMu1) {
      for (unsigned int iMu2 = iMu1 + 1; iMu2 < *nReco; ++iMu2) {

      	// Check charge and pT, eta cuts
        if (recoCharge[iMu1] * recoCharge[iMu2] == -1 ){

          mu1.SetPtEtaPhiM(recoPt[iMu1], recoEta[iMu1], recoPhi[iMu1], 0.1057); //to change, read muon mass instead of putting it
          mu2.SetPtEtaPhiM(recoPt[iMu2], recoEta[iMu2], recoPhi[iMu2], 0.1057);
          double mass = (mu1 + mu2).M();

          // Apply mass cut
	  if (mass >= 50 && mass <= 130) {
            h_mumu_50to130->Fill(mass);
          }
          if (mass >= 60 && mass <= 120) {
            h_mumu_60to120->Fill(mass);
	    if (recoPt[iMu1] > 10 && recoPt[iMu2] > 10){
	      good_pair = true;
	      h_mumu_60to120_pt10->Fill(mass);
            }
          }
        }
      }
    }

    // Loop over Jets
    int njets = 0;
    int njets_good = 0;
    for(int ijet=0; ijet<*nref; ijet++){
      if(jtpt[ijet]<30) continue;
      if(std::abs(jteta[ijet])>2.5) continue;
      njets++;
      h_jet_pt->Fill(jtpt[ijet]);
      if(good_pair == true){
	njets_good++; 
        h_jet_pt_good->Fill(jtpt[ijet]);
      }
    }
    h_njet->Fill(njets);
    h_njet_good->Fill(njets_good);

  }


  h_mumu_50to130->SetLineColor(kRed);
  h_mumu_50to130->SetLineWidth(2);
  h_mumu_60to120->SetLineColor(kBlue);
  h_mumu_60to120->SetLineWidth(2);
  h_mumu_60to120_pt10->SetLineColor(kBlack);
  h_mumu_60to120_pt10->SetLineWidth(2);

  c1->cd(1);
  h_mumu_50to130->Draw();
  h_mumu_60to120->Draw("same");
  h_mumu_60to120_pt10->Draw("same");

  TLegend *legend = new TLegend(0.55, 0.7, 0.8, 0.89);  
  legend->AddEntry(h_mumu_50to130, "50<m_{#mu#mu}<130 GeV", "l");  
  legend->AddEntry(h_mumu_60to120, "60<m_{#mu#mu}<120 GeV", "l");  
  legend->AddEntry(h_mumu_60to120_pt10, "60<m_{#mu#mu}<120 GeV, p_{T}^{#mu}>10 GeV", "l");  
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

  c1->SaveAs("h_mumu.pdf");
  c2->SaveAs("h_njets.pdf");
  c3->SaveAs("h_njets_good.pdf");
  c4->SaveAs("h_jet_pt.pdf");
  c5->SaveAs("h_jet_pt_good.pdf");

}  
    
