#ifndef JETSELECTION_PBPB_H
#define JETSELECTION_PBPB_H

#include "TMath.h"
#include "TH2.h"
#include "TFile.h"
#include <string>

class JetSelect{
private:
    void loadVetoMap(const std::string VetoMapPath) {
        TFile *fvm = TFile::Open(VetoMapPath.c_str(),"read");
        if(!fvm){throw std::runtime_error("Error: Could not open Jet Veto Map file: " + VetoMapPath);}
        TH2D *hvetomap = (TH2D*)fvm->Get("jetvetomap_all");
        if(!hvetomap){throw std::runtime_error("Error: Veto Map histogram 'jetvetomap_all' not in file");}
        hvetomap_ = (TH2D*)hvetomap->Clone("hvetomap_");
        hvetomap_->SetDirectory(0);
        fvm->Close();

        // showing that the veto map was obtained
        //std::cout << "\033[1;32mLoaded Jet Veto Map from: " << VetoMapPath << "\033[0m" << std::endl;
        std::cout << "Loaded Jet Veto Map from: " << VetoMapPath << "\033[0m" << std::endl;
    }
    TH2D* hvetomap_;
public:

    /// Default → file standard on EOS
    JetSelect() {
        std::string defaultPath = "/eos/cms/store/group/phys_heavyions/nbarnett/Winter25Prompt25_RunCDE.root";
        loadVetoMap(defaultPath);
    }

    // Custom constructor
    JetSelect(const std::string &VetoMapPath){loadVetoMap(VetoMapPath);}

    bool JetSelection(Float_t jteta, Float_t jtphi, Float_t CEF, Float_t NEF, Float_t MUF){
        bool KeepJet=false;
        bool KeepJet_map=false;
        bool KeepJet_id=false;
        if(TMath::Abs(jteta)<=2.6){if((NEF<0.9)&&(MUF<0.8)&&(CEF<0.8)){KeepJet_id=true;}}
        if((TMath::Abs(jteta)>2.6)&&(TMath::Abs(jteta)<=2.7)){if((NEF<0.99)&&(MUF<0.8)&&(CEF<0.8)){KeepJet_id=true;}}
        if((TMath::Abs(jteta)>2.7)&&(TMath::Abs(jteta)<=3.0)){if((NEF<0.99)){KeepJet_id=true;}}
        if((TMath::Abs(jteta)>3)&&(TMath::Abs(jteta)<=5.0)){if((NEF<0.9)){KeepJet_id=true;}}
        Double_t vetomapval = hvetomap_->GetBinContent(hvetomap_->FindBin(jteta, jtphi));
        if(vetomapval==0){KeepJet_map=true;}
        if((KeepJet_id==true)&&(KeepJet_map==true)){KeepJet=true;}
        return KeepJet;
    }
};

#endif

// How to use:
// example of checking if jet passes selections for index 0 in an event

// #include "JetSelection_PbPb.h"
// JetSelect js("/path/to/file/JetVetoMap.root");
// OR
// JetSelect js;
// jet veto maps are regularly uploaded to this GitHub repository: https://github.com/cms-jet/JECDatabase/tree/master/jet_veto_maps
// latest veto map as of November 25th, 2025 can be quickly grabbed with wget https://github.com/cms-jet/JECDatabase/raw/refs/heads/master/jet_veto_maps/Winter25Prompt25/Winter25Prompt25_RunCDE.root 


// if(!js.JetSelection(jteta[0], jtphi[0], jtPfCEF[0], jtPfNEF[0],jtPfMUF[0])){continue;}
// Do your stuff
