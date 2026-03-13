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

// Create a vector of FileInfo structs for PbPb23 an PbPb24, xsec in nb

// PbPb23
const std::vector<FileInfo> files = {
    {"/eos/infnts/cms/store/user/rdelliga/WplusToMuplusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_WplusToMuplusNu/*/000*/*.root", "output_HI_mu_MC_WplusToMuplusNu.root", "WplusToMuplusNu", 2.719 * 1000 / 1000, 9985999},
    {"/eos/infnts/cms/store/user/rdelliga/singleT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_singleT/260219_070923/0000.root", "output_HI_mu_MC_singleT.root", "singleT", 3.14 / 1000, 9928620},
    {"/eos/infnts/cms/store/user/rdelliga/singleTbar_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_singleTbar/260219_091039/0000.root", "output_HI_mu_MC_Tbar.root", "Tbar", 3.14 / 1000, 9990540},
    {"/eos/infnts/cms/store/user/rdelliga/ZZto4L_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_ZZto4L/260218_163811/0000.root", "output_HI_mu_MC_ZZto4L.root", "ZZto4L", 4.027 / 10 / 1000, 974000},
    {"/eos/infnts/cms/store/user/rdelliga/DYto2Tau_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_DYto2Tau/260219_105055/000*.root", "output_HI_mu_MC_DYto2Tau.root", "DYto2Tau", 5.595 * 100 / 1000, 9957125},
    {"/eos/infnts/cms/store/user/rdelliga/WWto2L2Nu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_WWto2L2Nu/260218_210330/0000.root", "output_HI_mu_MC_WWto2L2Nu.root", "WWto2L2Nu", 2.851 / 1000, 993056},
    {"/eos/infnts/cms/store/user/rdelliga/WminusToMuminusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_WminusToMuminusNu/260219_143930/000*.root", "output_HI_mu_MC_WminusToMuminusNu.root", "WminusToMuminusNu", 2.957 * 1000 / 1000, 9981603},
    {"/eos/infnts/cms/store/user/rdelliga/WZto3LNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_WZto3LNu/260219_020833/0000.root", "output_HI_mu_MC_WZto3LNu.root", "WZto3LNu", 1.325 / 1000, 975408},
    {"/eos/infnts/cms/store/user/rdelliga/ZZto2L2Q_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_ZZto2L2Q/260218_224800/0000.root", "output_HI_mu_MC_ZZto2L2Q.root", "ZZto2L2Q", 1.881 / 1000, 975640},
    {"/eos/infnts/cms/store/user/rdelliga/ZZto2L2Nu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_ZZto2L2Nu/260219_002817/0000.root", "output_HI_mu_MC_ZZto2L2Nu.root", "ZZto2L2Nu", 2.66 / 10/ 1000, 987950},
    {"/eos/infnts/cms/store/user/rdelliga/WZto2L2Q_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_WZto2L2Q/260219_034848/0000.root", "output_HI_mu_MC_WZto2L2Q.root", "WZto2L2Q", 1.941 / 1000, 1000000},
    {"/eos/infnts/cms/store/user/rdelliga/TT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_TT/260219_052905/0000.root", "output_HI_mu_MC_TT.root", "TT", 6.927 * 10 / 1000, 9977614},
    //{"/eos/infnts/cms/store/user/rdelliga/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test24_ZMM_DYto2Mu/260210_102810/0000/HiForestMiniAOD_*.root", "output_HI_mu_MC_signal.root", "signal", 5.595 * 100 / 1000, 9560121}
    {"/eos/infnts/cms/store/user/rdelliga/DYto2L-2Jets_MLL-50_TuneCP5_5p36TeV_amcatnloFXFX-pythia8/CRAB3_Analysis_test24_ZMM_DYto2L2Jets/260210_215130/000*/*.root", "output_HI_mu_MC_signal.root", "signal", 2.040 , 29444782}
};

// PbPb24
const std::vector<FileInfo> files_PbPb24 = {
    {"/eos/infnts/cms/store/user/rdelliga/WplusToMuplusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_WplusToMuplusNu/260103_153554/000*.root", "output_HI24_mu_MC_WplusToMuplusNu.root", "WplusToMuplusNu", 2.719 * 1000 / 1000, 10000000},
    {"/eos/infnts/cms/store/user/rdelliga/singleT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_singleT/251222_181118/000*.root", "output_HI24_mu_MC_singleT.root", "singleT", 3.14 / 1000, 10000000},
    {"/eos/infnts/cms/store/user/rdelliga/singleTbar_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_singleTbar/260127_081319/000*.root", "output_HI24_mu_MC_Tbar.root", "Tbar", 3.14 / 1000, 10000000},
//    {"", "output_HI24_mu_MC_ZZto4L.root", "ZZto4L", , },
    {"/eos/infnts/cms/store/user/rdelliga/DYto2Tau_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_DYto2Tau/251218_143402/000*.root", "output_HI24_mu_MC_DYto2Tau.root", "DYto2Tau", 5.595 * 100 / 1000, 10000000},
    {"/eos/infnts/cms/store/user/rdelliga/WWto2L2Nu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_WWto2L2Nu/251219_125005/000*.root", "output_HI24_mu_MC_WWto2L2Nu.root", "WWto2L2Nu", 2.851 / 1000, 1000000},
    {"/eos/infnts/cms/store/user/rdelliga/WminusToMuminusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_WminusToMuminusNu/251230_134544/000*.root", "output_HI24_mu_MC_WminusToMuminusNu.root", "WminusToMuminusNu", 2.957 * 1000 / 1000, 9998999},
//    {"", "output_HI24_mu_MC_WZto3LNu.root", "WZto3LNu", , },
//    {"", "output_HI24_mu_MC_ZZto2L2Q.root", "ZZto2L2Q", , },
    {"/eos/infnts/cms/store/user/rdelliga/ZZto2L2Nu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_ZZto2L2Nu/260127_111955/000*.root", "output_HI24_mu_MC_ZZto2L2Nu.root", "ZZto2L2Nu", 2.66 / 10/ 1000, 980098},
    {"/eos/infnts/cms/store/user/rdelliga/WZto2L2Q_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_WZto2L2Q/251219_153216/000*.root", "output_HI24_mu_MC_WZto2L2Q.root", "WZto2L2Q", 1.941 / 1000, 995142},
    {"/eos/infnts/cms/store/user/rdelliga/TT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_TT/251220_081650/000*.root", "output_HI24_mu_MC_TT.root", "TT", 6.927 * 10 / 1000, 9999000},
    {"/eos/infnts/cms/store/user/rdelliga/DYto2Mu_MLL-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test19_ZMM_PbPb24_DYto2Mu/251217_140307/0000/HiForestMiniAOD_MC_*.root", "output_HI24_mu_MC_signal.root", "signal", 5.595 * 100 / 1000, 10000000}
};

// PPref 24
// xsec in pb
const std::vector<FileInfo> files_ppref = {
    {"/eos/infnts/cms/store/user/kdeleo/WplusToMuplusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_WplusToMu/251121_153250/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_WplusToMuplusNu.root", "WplusToMuplusNu", 4.09 * 1000, 9730000},
    {"/eos/infnts/cms/store/user/kdeleo/T_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_SingleT/251124_103245/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_singleT.root", "singleT", 3.06, 10000000},
    {"/eos/infnts/cms/store/user/kdeleo/Tbar_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_SingleAntiT/251124_140017/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_Tbar.root", "Tbar", 3.07, 9972000},
    {"/eos/infnts/cms/store/user/kdeleo/DYToTauTau_M-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_DYtoTauTau/251119_161755/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_DYto2Tau.root", "DYto2Tau", 6.57*100, 9946251},
    {"/eos/infnts/cms/store/user/kdeleo/WminusToMuminusNu_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_WminusToMu/251120_161830/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_WminusToMuminusNu.root", "WminusToMuminusNu", 2.73 * 1000, 9844580},
    {"/eos/infnts/cms/store/user/kdeleo/TT_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_TT/251120_101043/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_TT.root", "TT", 6.62 * 10, 10000000},
    {"/eos/infnts/cms/store/user/rdelliga/ZZTo4L_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test22_run3_ppref_MC_ZMM_ZZTo4L/260308_162724/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_ZZto4L.root", "ZZto4L", 5.04 / 10, 999040},
//    {"/eos/infnts/cms/store/user/kdeleo/DYToMuMu_M-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test18_mc_ppRef_ZMM_DYto2Mu/251115_123708/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_signal.root", "signal", 6.57 * 100, 9999999} //old
    {"/eos/infnts/cms/store/user/rdelliga/DYToMuMu_M-50_TuneCP5_5p36TeV_powheg-pythia8/CRAB3_Analysis_test22_run3_ppref_MC_ZMM_DYtoMuMu/260127_120900/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_alternative.root", "alternative", 6.57 * 100, 9999999},
    {"/eos/infnts/cms/store/user/rdelliga/DYto2L-2Jets_MLL-50_TuneCP5_5p36TeV_amcatnloFXFX-pythia8/CRAB3_Analysis_test22_run3_ppref_MC_ZMM_DYto2L_2Jets/260130_142319/0000/HiForestMiniAOD_*.root", "output_ppref_mu_MC_signal.root", "signal", 2.328 * 1000, 30156049}
};

#endif // MC_SAMPLES_H

