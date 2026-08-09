#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "TH3D.h"
#include "TDirectory.h"
#include "TPRegexp.h"

#include "../JetSelection_pp.h" // Defines JetSelect for Id selection + jet veto map in pp

// Include your JEC header (we need JEC, but NOT JER)
#include "../JetCorrector.h" 
#include "../helpers.h" // For getDeltaR

using namespace std;

// Initialize Jet Selector as pointers
JetSelect_pp* js_pp = new JetSelect_pp("../Winter24Prompt24_2024BCDEFGHI.root");

void derive_JER_ppref(std::string input_pattern, std::string output_name, int radius) {
    
    // =========================================================================
    // 1. SETUP OUTPUT FILE & HISTOGRAM
    // =========================================================================
    // The downstream scripts (MCJER.C) expect a specific directory path: 
    // "hibin_-1.0_0.0/eta_-5.2_5.2/responses3D"
    
    TFile *fout = new TFile(output_name.c_str(), "RECREATE");
    TDirectory *dir1 = fout->mkdir("hibin_-1.0_0.0");
    dir1->cd();
    TDirectory *dir2 = dir1->mkdir("eta_-5.2_5.2");
    dir2->cd();

    // Exact CMS Eta Rings from the calibration file
    const int n_eta_bins = 12;
    double eta_edges[n_eta_bins + 1] = {0.0, 0.522, 0.783, 1.044, 1.305, 1.566, 2.043, 2.322, 2.65, 2.853, 3.139, 3.485, 5.191};
    
    // Fine pT binning for the expert's scripts to project and fit
    const int n_pt_bins = 20;
    double pt_edges[n_pt_bins + 1] = {15, 18, 21, 24, 28, 32, 37, 43, 49, 56, 64, 74, 84, 97, 114, 133, 153, 174, 200, 300, 1000};
    
    // Define the Z-axis array manually to satisfy ROOT's TH3D constructor
    const int n_z_bins = 150;
    double z_edges[n_z_bins + 1];
    for (int i = 0; i <= n_z_bins; i++) {
        z_edges[i] = 0.0 + i * (3.0 / n_z_bins); // 150 bins from 0.0 to 3.0
    }
    
    // Create the 3D Response Histogram
    // X = Gen pT, Y = Absolute Gen Eta, Z = Reco/Gen Ratio
    TH3D* responses3D = new TH3D("responses3D", "MC Jet Response; p_{T}^{gen}; |#eta^{gen}|; p_{T}^{reco} / p_{T}^{gen}", 
                                 n_pt_bins, pt_edges, n_eta_bins, eta_edges, n_z_bins, z_edges);
    responses3D->Sumw2();

    // =========================================================================
    // 2. SETUP TREES & JEC
    // =========================================================================
    
    // DYNAMIC TREE NAME
    TString treeName = Form("ak%dPFJetAnalyzer/t", radius);
    TChain jetTree(treeName.Data()); 
    TChain evtTree("hiEvtAnalyzer/HiTree");

    // Regex logic to load files
    TString reg_str = input_pattern;
    reg_str.ReplaceAll(".", "\\.");   
    reg_str.ReplaceAll("*", "[^/]*"); 
    TPRegexp re(reg_str);
    
    std::ifstream infile("../samples_root_files.txt");
    std::string line;
    while (std::getline(infile, line)) {
        TString tline(line);
        if (tline.Contains(re)) {
            jetTree.Add(tline);
//            muonTree.Add(tline);
            evtTree.Add(tline);
        }
    }
    infile.close();

    // Safely friend the trees
//    jetTree.AddFriend(&muonTree);
    jetTree.AddFriend(&evtTree);
    cout << "Successfully loaded " << jetTree.GetNtrees() << " files into chains." << endl;

    TTreeReader fReader(&jetTree);
    TTreeReaderValue<Int_t> nref = {fReader, "nref"};
    TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
    TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
    TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};
    TTreeReaderArray<Float_t> jtPfCEF = {fReader, "jtPfCEF"};
    TTreeReaderArray<Float_t> jtPfNEF = {fReader, "jtPfNEF"};
    TTreeReaderArray<Float_t> jtPfMUF = {fReader, "jtPfMUF"};

   // pp-specific variables for id + veto map (pointers) ---
  TTreeReaderArray<Float_t>* jtPfCHF = nullptr;
  TTreeReaderArray<Float_t>* jtPfNHF = nullptr;
  TTreeReaderArray<Int_t>* jtPfCHM = nullptr;
  TTreeReaderArray<Int_t>* jtPfNHM = nullptr;
  TTreeReaderArray<Int_t>* jtPfCEM = nullptr;
  TTreeReaderArray<Int_t>* jtPfNEM = nullptr;
  TTreeReaderArray<Int_t>* jtPfMUM = nullptr;

      jtPfCHF = new TTreeReaderArray<Float_t>(fReader, "jtPfCHF");
      jtPfNHF = new TTreeReaderArray<Float_t>(fReader, "jtPfNHF");
      jtPfCHM = new TTreeReaderArray<Int_t>(fReader, "jtPfCHM");
      jtPfNHM = new TTreeReaderArray<Int_t>(fReader, "jtPfNHM");
      jtPfCEM = new TTreeReaderArray<Int_t>(fReader, "jtPfCEM");
      jtPfNEM = new TTreeReaderArray<Int_t>(fReader, "jtPfNEM");
      jtPfMUM = new TTreeReaderArray<Int_t>(fReader, "jtPfMUM");


    TTreeReaderValue<Int_t> ngen = {fReader, "ngen"};
    TTreeReaderArray<Float_t> genpt = {fReader, "genpt"};
    TTreeReaderArray<Float_t> geneta = {fReader, "geneta"};
    TTreeReaderArray<Float_t> genphi = {fReader, "genphi"};

    TTreeReaderValue<Float_t> weight = {fReader, "weight"};
    // >>> NEW: Muon Branches <<<
/*    TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
    TTreeReaderArray<Float_t> recoPt = {fReader, "recoPt"};
    TTreeReaderArray<Float_t> recoEta = {fReader, "recoEta"};
    TTreeReaderArray<Float_t> recoPhi = {fReader, "recoPhi"};
    TTreeReaderArray<bool> recoIDTight = {fReader, "recoIDTight"};é*/

    // DYNAMIC JEC FILE
    TString jecName = Form("../Prompt24HIpp_V2_MC_L2Relative_AK%dPF.txt", radius);
    vector<string> jecFiles = {jecName.Data()};
    JetCorrector JEC(jecFiles);

    // --- For debugging ---
    cout << "Initializing JEC..." << endl;
    for (const auto& file : jecFiles) {
      cout << "Loaded JEC File: " << file << endl;
    }
    // ---------------------

    // =========================================================================
    // 3. EVENT LOOP (Filling the 3D Histogram)
    // =========================================================================
    cout << "Starting Event Loop..." << endl;
    Long64_t total_events = jetTree.GetEntries();
    Long64_t iEvent = 0;
    Long64_t successfully_filled_jets = 0; // Tracking counter
    Long64_t cleaned_jets = 0;

    while (fReader.Next()) {
        if (iEvent % 10000 == 0) cout << "Processing event " << iEvent << " / " << total_events << std::endl;
        iEvent++;

        for (int ijet = 0; ijet < *nref; ijet++) {

            // >>> NEW: MUON CLEANING <<<
   /*         bool overlaps_with_muon = false;
            for (int imu = 0; imu < *nReco; imu++) {
                // Only clean against high-quality, high-pT signal muons
                if (!recoIDTight[imu]) continue;
                if (recoPt[imu] < 20.0 || std::abs(recoEta[imu]) > 2.4) continue;
                
                // Calculate distance between Jet and Muon
                double dR_muon = getDeltaR(jteta[ijet], jtphi[ijet], recoEta[imu], recoPhi[imu]);
                
                // Strict isolation cone
                if (dR_muon < 0.2) {
                    overlaps_with_muon = true;
                    break; 
                }
            }
            
            // Skip this jet if it is sitting on top of a muon!
            if (overlaps_with_muon) {
                cleaned_jets++;
                continue;
            }*/
            // >>> END MUON CLEANING <<<

            // Apply JEC
            JEC.SetJetPT(rawpt[ijet]);
            JEC.SetJetEta(jteta[ijet]);
            JEC.SetJetPhi(jtphi[ijet]);
            double pt_reco = JEC.GetCorrectedPT();
            
            // Apply Combined Jet ID and Veto Map
            bool passJetID = false;
            // For pp, we dereference the pointers (*ptr)[index]
            passJetID = js_pp->JetSelection(jteta[ijet], jtphi[ijet],
                                       (*jtPfCHF)[ijet], (*jtPfNHF)[ijet], 
 jtPfCEF[ijet], jtPfNEF[ijet], jtPfMUF[ijet],
                                       (*jtPfCHM)[ijet], (*jtPfNHM)[ijet], 
 (*jtPfCEM)[ijet], (*jtPfNEM)[ijet], (*jtPfMUM)[ijet]);
            if (!passJetID) continue;

            // Match to closest Gen Jet
            double min_dR = 999.0;
            int matched_gen_idx = -1;
            
            for (int igen = 0; igen < *ngen; igen++) {
                double dR = getDeltaR(jteta[ijet], jtphi[ijet], geneta[igen], genphi[igen]);
                if (dR < min_dR) {
                    min_dR = dR;
                    matched_gen_idx = igen;
                }
            }
            
            // DYNAMIC MATCHING CUT
            double max_dR = radius * 0.05; 
            
            if (min_dR < max_dR && matched_gen_idx != -1) {
                double pt_gen = genpt[matched_gen_idx];
                double abs_eta = std::abs(geneta[matched_gen_idx]);
                if (pt_gen > 0) {
                    double response = pt_reco / pt_gen;
                    responses3D->Fill(pt_gen, abs_eta, response, *weight);
                    successfully_filled_jets++;
                }
            }
        }
    }
    
    // Save and close
    dir2->cd();
    responses3D->Write();
    fout->Close();
    
    cout << "=====================================================================" << endl;
    cout << "DONE! The file '" << output_name << "' has been created." << endl;
    cout << "Successfully filled " << successfully_filled_jets << " matched, cleaned jets into the 3D histogram!" << endl;
//    cout << "Removed " << cleaned_jets << " fake jets that overlapped with Z-boson muons." << endl;
    cout << "Please pass this file to the expert's runMCJER.C script to extract the parameters." << endl;
}
