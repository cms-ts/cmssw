#ifndef MC_SAMPLES_PPREF_H
#define MC_SAMPLES_PPREF_H

#include <string>
#include <vector>

// Define a struct to hold file information
struct FileInfo {
    const char * path_miniaod;
    std::string out_filename;
    std::string label;
    double xsec;
    int ngen;
};

// Create a vector of FileInfo structs
const std::vector<FileInfo> files = {
    {"/eos/infnts/cms/store/user/kdeleo/WplusToMuplusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_WplusToMu/251121_153250/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_WplusToMuplusNu.root", "WplusToMuplusNu", 4.09 * 1000, 9730000},
    {"/eos/infnts/cms/store/user/kdeleo/T_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_SingleT/251124_103245/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_singleT.root", "singleT", 3.06, 10000000},
    {"/eos/infnts/cms/store/user/kdeleo/Tbar_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_SingleAntiT/251124_140017/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_Tbar.root", "Tbar", 3.07, 9972000},
    {"/eos/infnts/cms/store/user/kdeleo/DYToTauTau_M-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_DYtoTauTau/251119_161755/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_DYto2Tau.root", "DYto2Tau", 6.57*100, 9946251},
    {"/eos/infnts/cms/store/user/kdeleo/WminusToMuminusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_WminusToMu/251120_161830/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_WminusToMuminusNu.root", "WminusToMuminusNu", 2.73 * 1000, 9844580},
    {"/eos/infnts/cms/store/user/kdeleo/TT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_TT/251120_101043/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_TT.root", "TT", 6.62 * 10, 10000000},
    {"/eos/infnts/cms/store/user/kdeleo/DYToMuMu_M-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_DYto2Mu/251115_123708/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_signal.root", "signal", 6.57 * 100, 9999999}
};

#endif // MC_SAMPLES_PPREF_H
