#include <iostream>
#include <vector>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TRatioPlot.h"

void compareIso() {
    // 1. Open the file and get the tree
    TFile *f = TFile::Open("HiForestMiniAOD.root");
    if (!f || f->IsZombie()) {
        std::cout << "Error: Could not open HiForestMiniAOD.root" << std::endl;
        return;
    }
    TTree *tree = (TTree*)f->Get("muonAnalyzer/MuonTree");

    // 2. Set up pointers to read the std::vectors from the branches
    std::vector<float> *recoPt = nullptr;
    std::vector<float> *recoPFChIso = nullptr;
    std::vector<float> *recoPFNeuIso = nullptr;
    std::vector<float> *recoPFPhoIso = nullptr;
    std::vector<float> *recoPFPUIso = nullptr;
    std::vector<bool>  *recoPFIsoTight = nullptr;

    tree->SetBranchAddress("recoPt", &recoPt);
    tree->SetBranchAddress("recoPFChIso", &recoPFChIso);
    tree->SetBranchAddress("recoPFNeuIso", &recoPFNeuIso);
    tree->SetBranchAddress("recoPFPhoIso", &recoPFPhoIso);
    tree->SetBranchAddress("recoPFPUIso", &recoPFPUIso);
    tree->SetBranchAddress("recoPFIsoTight", &recoPFIsoTight);

    // 3. Define Histograms (0 = Fail, 1 = Pass)
    TH1F *h_precomputed = new TH1F("h_pre", "Isolation Comparison; Pass(1) or Fail(0); Number of Muons", 2, -0.5, 1.5);
    TH1F *h_manual = new TH1F("h_man", "Manual Calc; Pass(1) or Fail(0); Number of Muons", 2, -0.5, 1.5);

    // Styling
    h_precomputed->SetLineColor(kBlue);
    h_precomputed->SetLineWidth(2);
    h_manual->SetLineColor(kRed);
    h_manual->SetLineWidth(2);
    h_manual->SetLineStyle(2); // Dashed so you can see it if they overlap perfectly

    int nEvents = tree->GetEntries();
    int totalMismatches = 0;

    // 4. Event Loop
    for (int i = 0; i < nEvents; i++) {
        tree->GetEntry(i);

        // Muon Loop
        for (size_t j = 0; j < recoPt->size(); j++) {
            if (recoPt->at(j) <= 15.0) continue; // Only check muons with pt > 15

            float pt = recoPt->at(j);
            float chIso = recoPFChIso->at(j);
            float neuIso = recoPFNeuIso->at(j);
            float phoIso = recoPFPhoIso->at(j);
            float puIso = recoPFPUIso->at(j);

            // Calculate manual flag
            bool manualPass = (chIso + std::max(0.0f, neuIso + phoIso - 0.5f * puIso)) / pt < 0.15;
            
            // Get precomputed flag
            bool precomputedPass = recoPFIsoTight->at(j);

            h_precomputed->Fill(precomputedPass);
            h_manual->Fill(manualPass);

            // Print warning if they don't match
            if (manualPass != precomputedPass) {
                std::cout << "WARNING [Event " << i << ", Muon " << j << "]: Mismatch! "
                          << "Manual = " << manualPass << ", Precomputed = " << precomputedPass
                          << " (pt = " << pt << ")" << std::endl;
                totalMismatches++;
            }
        }
    }

    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "Total events processed: " << nEvents << std::endl;
    std::cout << "Total mismatches found: " << totalMismatches << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    // 5. Create Ratio Plot
    TCanvas *c1 = new TCanvas("c1", "Isolation Comparison", 800, 800);
    
    // TRatioPlot takes care of splitting the canvas and drawing the ratio
    TRatioPlot *rp = new TRatioPlot(h_precomputed, h_manual);
    rp->Draw();
    
    // Add a legend to the upper pad
    rp->GetUpperPad()->cd();
    TLegend *leg = new TLegend(0.15, 0.75, 0.45, 0.85);
    leg->AddEntry(h_precomputed, "Precomputed PFIsoTight", "l");
    leg->AddEntry(h_manual, "Manual Calc", "l");
    leg->SetBorderSize(0);
    leg->Draw();

    // Save as PDF
    c1->SaveAs("iso_comparison.pdf");
}
