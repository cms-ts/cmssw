#ifndef MC_SAMPLES_H
#define MC_SAMPLES_H

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
    {"/eos/infnts/cms/store/user/kdeleo/WplusToMuplusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_WplusToM/250403_145634/0000.root", "output_HI_mu_MC_WplusToMuplusNu.root", "WplusToMuplusNu", 2.719 * 1000 / 1000, 9985999},
    {"/eos/infnts/cms/store/user/kdeleo/singleT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_singleT/250403_075844/0000.root", "output_HI_mu_MC_singleT.root", "singleT", 1.494 * 10 / 1000, 9928620},
    {"/eos/infnts/cms/store/user/kdeleo/singleTbar_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_singleTbar/250417_082706/0000.root", "output_HI_mu_MC_singleTbar.root", "singleTbar", 1.461 * 10 / 1000, 9990540},
    {"/eos/infnts/cms/store/user/kdeleo/ZZto4L_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_ZZ4L/250401_150230/0000.root", "output_HI_mu_MC_ZZto4L.root", "ZZto4L", 4.027 / 10 / 1000, 974000},
    {"/eos/infnts/cms/store/user/kdeleo/DYto2Tau_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_DYto2Tau/250403_102636/0000.root", "output_HI_mu_MC_DYto2Tau.root", "DYto2Tau", 5.595 * 100 / 1000, 9957125},
    {"/eos/infnts/cms/store/user/kdeleo/WWto2L2Nu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_WW/250331_103647/0000.root", "output_HI_mu_MC_WWto2L2Nu.root", "WWto2L2Nu", 2.851 / 1000, 993056},
    {"/eos/infnts/cms/store/user/kdeleo/WminusToMuminusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_WminusToM/250403_134642/0000.root", "output_HI_mu_MC_WminusToMuminusNu.root", "WminusToMuminusNu", 2.957 * 1000 / 1000, 9981603},
    {"/eos/infnts/cms/store/user/kdeleo/WZto3LNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_WZ/250402_075217/0000.root", "output_HI_mu_MC_WZto3LNu.root", "WZto3LNu", 1.325 / 1000, 975408},
    {"/eos/infnts/cms/store/user/kdeleo/ZZto2L2Q_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_ZZ2L2Q/250401_134408/0000.root", "output_HI_mu_MC_ZZto2L2Q.root", "ZZto2L2Q", 1.881 / 1000, 975640},
    {"/eos/infnts/cms/store/user/kdeleo/WZto2L2Q_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_WZ2L2Q/250402_122759/0000.root", "output_HI_mu_MC_WZto2L2Q.root", "WZto2L2Q", 1.941 / 1000, 1000000},
    {"/eos/infnts/cms/store/user/kdeleo/TT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_TT/250402_140227/0000.root", "output_HI_mu_MC_TT.root", "TT", 6.927 * 10 / 1000, 9977614},
    {"/eos/infnts/cms/store/user/kdeleo/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test13_ZMM_DYto2Mu/250321_154613/0000/HiForestMiniAOD_MC_*.root", "output_HI_mu_MC_signal.root", "signal", 5.595 * 100 / 1000, 9560121}
};

#endif // MC_SAMPLES_H

