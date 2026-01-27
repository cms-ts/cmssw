#ifndef JETSELECTION_PP_H
#define JETSELECTION_PP_H

#include "TMath.h"
#include "TH2.h"
#include "TFile.h"
#include <string>

class JetSelect_pp{
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
        std::cout << "Loaded Jet Veto Map using JetSelect_pp from: " << VetoMapPath << std::endl;
    }
    TH2D* hvetomap_;
public:

    /// Default → file standard on EOS
    JetSelect_pp() {
        std::string defaultPath = "/eos/cms/store/group/phys_heavyions/nbarnett/Winter25Prompt25_RunCDE.root";
        loadVetoMap(defaultPath);
    }

    // Custom constructor
    JetSelect_pp(const std::string &VetoMapPath){loadVetoMap(VetoMapPath);}

    bool JetSelection(Float_t jteta, Float_t jtphi, Float_t CHF, Float_t NHF, Float_t CEF, Float_t NEF, Float_t MUF, Float_t CHM,  Float_t NHM,  Float_t CEM,  Float_t NEM,  Float_t MUM){
        bool KeepJet=false;
        bool KeepJet_map=false;
        bool KeepJet_id=false;
        // multiplicity
        Int_t m = CHM+NHM+CEM+NEM+MUM;
        // charged multiplicity
        Int_t cm = CHM+CEM+MUM;
        // nuetral multiplicity
        Int_t nm = NHM+NEM;
        // suggested requirements by eta range
        if((TMath::Abs(jteta)<=2.6)&&((NHF<0.9)&&(NEF<0.9)&&(m>1)&&(MUF<0.8)&&(CHF>0.01)&&(cm>0)&&(CEF<0.8))){KeepJet_id=true;}
        if(((TMath::Abs(jteta)>2.6)&&(TMath::Abs(jteta)<=2.7))&&((NHF<0.9)&&(NEF<0.99)&&(MUF<0.8)&&(cm>0)&&(CEF<0.8))){KeepJet_id=true;}
        if(((TMath::Abs(jteta)>2.7)&&(TMath::Abs(jteta)<=3.0))&&((NEF<0.99)&&(nm>1))){KeepJet_id=true;}
        if(((TMath::Abs(jteta)>3.0)&&(TMath::Abs(jteta)<=5.0))&&((NHF>0.2)&&(NEF<0.9)&&(nm>10))){KeepJet_id=true;}
        // jet veto map
        Double_t vetomapval = hvetomap_->GetBinContent(hvetomap_->FindBin(jteta, jtphi));
        if(vetomapval==0){KeepJet_map=true;}
        if((KeepJet_id==true)&&(KeepJet_map==true)){KeepJet=true;}
        return KeepJet;
    }
};

#endif

// How to use:
// example of checking if jet passes selections for index 0 in an event

// #include "JetSelection_pp.h"
// JetSelectPP js("/path/to/file/JetVetoMap.root");
// OR
// JetSelectPP js;
// jet veto maps are regularly uploaded to this GitHub repository: https://github.com/cms-jet/JECDatabase/tree/master/jet_veto_maps
// latest veto map as of November 25th, 2025 can be quickly grabbed with wget https://github.com/cms-jet/JECDatabase/raw/refs/heads/master/jet_veto_maps/Winter25Prompt25/Winter25Prompt25_RunCDE.root 
// jet ID requirements suggested by JetMET here https://twiki.cern.ch/twiki/bin/view/CMS/JetID13p6TeV


// if(!js.JetSelectionPP(jteta[0],jtphi[0],jtPfCHF[0],jtPfNHF[0],jtPfCEF[0],jtPfNEF[0],jtPfMUF[0],jtPfCHM[0],jtPfNHM[0],jtPfCEM[0],jtPfNEM[0],jtPfMUM[0])){continue;}
// Do your stuff

