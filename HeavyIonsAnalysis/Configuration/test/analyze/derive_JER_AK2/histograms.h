#ifndef __histograms_h__
#define __histograms_h__

#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TProfile3D.h"
#include "TDirectory.h"


class histograms {

 public:
  Float_t ptmin;
  Float_t ptmax;
  Float_t etamin;
  Float_t etamax;
  Float_t isMC;
  Float_t hibinmin;
  Float_t hibinmax;
  
// Jet histograms

  TH1D* jet_pt;
  TH1D* jet_pt_now;
  TH1D* jet_uncorr_pt;
  TH1D* jet_pt_genweight;
  TH1D* jet_eta;
  TH1D* jet_phi;
  
  TH1D* tag_pt;
  TH1D* tag_eta;
  TH1D* tag_phi;

  TH1D* probe_pt;
  TH1D* probe_eta;
  TH1D* probe_phi;

  TH1D* alphas;
  
  // Trigger
  TH1D* HLTZB;
  TH1D* HLT40;
  TH1D* HLT60;
  TH1D* HLT80;
  TH1D* HLT100;
  TH1D* HLT120;

  TH1D* HLTZB_ptav;
  TH1D* HLT40_ptav;
  TH1D* HLT60_ptav;
  TH1D* HLT80_ptav;
  TH1D* HLT100_ptav;
  TH1D* HLT120_ptav;

// PF composition
  TProfile* jet_nhf;
  TProfile* jet_chf;
  TProfile* jet_nef; 
  TProfile* jet_cef;
  TProfile* jet_muf;

  TH2D* jetetaphi;

  // Gen jet histograms
  TH1D* genjet_pt;
  TH1D* genjet_eta;
  TH1D* genjet_phi;

  TH1D* genjetdyn_kt;
  TH1D* genjetdyn_deltaR;
  TH1D* genjetdyn_z;
  TH1D* genjetdyn_z_cutdeltaR;

  
// Lund plane
  TH2D* plane_inclusive;

// JES related controls etc

  TProfile* jetresponse;

// Definition: (reco-true)/true
  TH1D* ptres;
  TH1D* ktres;
  TH1D* deltaRres;
  TH1D* zres;


// Dijets
  TH1D* dijetasymmetry;
  TH1D* dijetasymmetry_now;
  TH1D* dijetdeltaphi;
  TH1D* dijetdeltaeta;


// JERC dijets
  TProfile* dijetasymmetry_a01;
  TProfile* dijetasymmetry_a02;
  TProfile* dijetasymmetry_a035;
  TProfile* dijetasymmetry_a04;
  TProfile* dijetasymmetry_a05;
  TProfile* dijetasymmetry_a06;
  
  TH1D* dijetbalance_a03;
  TProfile* dijetasymmetry_a03;
  TH1D* dijetbalance_a1;
  TProfile* dijetasymmetry_a1;

  // low pT quick xsec
  TH2D* dijetbalance_a03_pt30to40;
  TH2D* dijetbalance_a03_pt40to80;

// 2D profiles for the derivation
  TProfile2D* dijetasymmetry2D_a01;
  TProfile2D* dijetasymmetry2D_a02;
  TProfile2D* dijetasymmetry2D_a03;
  TProfile2D* dijetasymmetry2D_a04;
  TProfile2D* dijetasymmetry2D_a05;
  TProfile2D* dijetasymmetry2D_a06;


  // 3D profile for the derivation 
  TProfile3D* dijetasymmetry3D;
  TProfile3D* dijetasymmetry3Dwide;
  TProfile3D* dijetasymmetry3Dnarrow;

  TProfile3D* dijetasymmetry3Dabseta;
  TProfile3D* dijetasymmetry3Dabsetawide;
  TProfile3D* dijetasymmetry3Dabsetanarrow;

  // 3D histograms for JER
  TH3D* responses3D;
  TH3D* asymmdist3D;
  TH3D* absasymmdist3D;

  // Phi and eta resolution
  TH3D* phiresponse;
  TH3D* etaresponse;

  // JER needs asymmetries as function of alpha
  TH3D* asymmdist3D_a10;
  TH3D* absasymmdist3D_a10;
  TH3D* asymmdist3D_a15;
  TH3D* absasymmdist3D_a15;
  TH3D* asymmdist3D_a20;
  TH3D* absasymmdist3D_a20;
  TH3D* asymmdist3D_a25;
  TH3D* absasymmdist3D_a25;
  TH3D* asymmdist3D_a30;
  TH3D* absasymmdist3D_a30;
  TH3D* asymmdist3D_a35;
  TH3D* absasymmdist3D_a35;
  TH3D* asymmdist3D_a40;
  TH3D* absasymmdist3D_a40;
  TH3D* asymmdist3D_a45;
  TH3D* absasymmdist3D_a45;

  TH3D* absasymmdist3D_gen_a10;
  TH3D* absasymmdist3D_gen_a15;
  TH3D* absasymmdist3D_gen_a20;
  TH3D* absasymmdist3D_gen_a25;
  TH3D* absasymmdist3D_gen_a30;
  TH3D* absasymmdist3D_gen_a35;
  TH3D* absasymmdist3D_gen_a40;
  TH3D* absasymmdist3D_gen_a45;

// Weights etc
  TH2D* ptgenvsptreco;
  TH2D* ptrecovsweight;
  TH2D* ptgenvsweight;

  TDirectory *dir;

  static constexpr double etarange[] = {    -5.191,-4.889,-4.716,-4.538,-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,-3.314,-3.139,-2.964,-2.853,-2.65,-2.5,-2.322,-2.172,-2.043,-1.93,-1.83,-1.74,-1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,-0.957,-0.870,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,-0.087,
     0.000, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.870, 0.957, 1.044, 1.131, 1.218,1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5,2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013, 4.191, 4.363,4.538, 4.716, 4.889, 5.191
  };
  static constexpr unsigned int netas = sizeof(etarange)/sizeof(etarange[0])-1;

    static constexpr double absetarange[] = {0.000, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.870, 0.957, 1.044, 1.131, 1.218,1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5,2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013, 4.191, 4.363,4.538, 4.716, 4.889, 5.191  };
  static constexpr unsigned int nabsetas = sizeof(absetarange)/sizeof(absetarange[0])-1;

  // This is L2res binning used also in ppJEC
  static constexpr double wetarange[] = {    -5.191, -3.839, -3.489, -3.139, -2.964,-2.853, -2.65, -2.5,-2.322,-2.172,-1.93,-1.653,-1.479,-1.305,-1.044,-0.783,-0.522,-0.261, 0.000,
                                              0.261,  0.522,  0.783,  1.044,  1.305, 1.479, 1.653, 1.93, 2.172, 2.322,  2.5,  2.65, 2.853, 2.964, 3.139, 3.489, 3.839, 5.191
  };
  static constexpr unsigned int nwetas = sizeof(wetarange)/sizeof(wetarange[0])-1;

  
  static constexpr double wabsetarange[] = {0.0, 0.261,  0.522,  0.783,  1.044,  1.305, 1.479, 1.653, 1.93, 2.172, 2.322,  2.5,  2.65, 2.853, 2.964, 3.139, 3.489, 3.839, 5.191
  };
  static constexpr unsigned int nwabsetas = sizeof(wabsetarange)/sizeof(wabsetarange[0])-1;

  static constexpr double dwabsetarange[] = {0.0, 0.522, 1.044,  1.479, 1.93, 2.322,  2.65, 2.964, 5.191 // Super wide binning for 2023 PbPb JEC in high pT
  };
  static constexpr unsigned int ndwabsetas = sizeof(dwabsetarange)/sizeof(dwabsetarange[0])-1;


  static constexpr double asymrange[] = {-2, -1.9, -1.8, -1.7, -1.6, -1.5, -1.4, -1.3, -1.2, -1.1, -1, -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1, 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2};
  static constexpr unsigned int nasym = sizeof(asymrange)/sizeof(asymrange[0])-1;

  
  //  static constexpr double jeretarange[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
  static constexpr double jeretarange[] = {0.0, 1.3, 2.5, 3.0};
  static constexpr unsigned int njeretas = sizeof(jeretarange)/sizeof(jeretarange[0])-1;

  static constexpr double phirange[] = {    -3.14159265360, -3.0543261909902775, -2.9670597283905553, -2.879793265790833, -2.792526803191111, -2.705260340591389, -2.6179938779916667, -2.5307274153919446, -2.443460952792222, -2.3561944901925, -2.2689280275927777, -2.1816615649930555, -2.0943951023933334, -2.0071286397936112, -1.9198621771938889, -1.8325957145941667, -1.7453292519944443, -1.6580627893947222, -1.570796326795, -1.4835298641952777, -1.3962634015955555, -1.3089969389958334, -1.221730476396111, -1.1344640137963888, -1.0471975511966667, -0.9599310885969444, -0.8726646259972222, -0.7853981633975, -0.6981317007977778, -0.6108652381980555, -0.5235987755983333, -0.4363323129986111, -0.3490658503988889, -0.26179938779916667, -0.17453292519944444, -0.08726646259972222,
    0.0, 0.08726646259972222, 0.17453292519944444, 0.26179938779916667, 0.3490658503988889, 0.4363323129986111, 0.5235987755983333, 0.6108652381980555, 0.6981317007977778, 0.7853981633975, 0.8726646259972222, 0.9599310885969444, 1.0471975511966667, 1.1344640137963888, 1.221730476396111, 1.3089969389958334, 1.3962634015955555, 1.4835298641952777, 1.570796326795, 1.6580627893947222, 1.7453292519944443, 1.8325957145941667, 1.9198621771938889, 2.0071286397936112, 2.0943951023933334, 2.1816615649930555, 2.2689280275927777, 2.3561944901925, 2.443460952792222, 2.5307274153919446, 2.6179938779916667, 2.705260340591389, 2.792526803191111, 2.879793265790833, 2.9670597283905553, 3.0543261909902775, 3.14159265360
  };
  static constexpr unsigned int nphis = sizeof(phirange)/sizeof(phirange[0])-1;
  
  // These are the bins for JEC statistics etc checks
  static constexpr float etaforjec[] = {-5.2, -3.9, -2.6, -1.3, 0.0, 1.3, 2.6, 3.9, 5.2};
  static constexpr unsigned int netaforjec = sizeof(etaforjec)/sizeof(etaforjec[0])-1;

  static constexpr float halfeta[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0};
  static constexpr unsigned int nhalfeta = sizeof(halfeta)/sizeof(halfeta[0])-1;

  // 0.000, 0.087, 0.174, 0.261,0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.870, 0.957, 1.044, 1.131, 1.218,1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5,2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013, 4.191, 4.363,4.538, 4.716, 4.889, 5.191
  
  static constexpr double etaforjer[] = {0.0, 0.522, 0.783, 1.044, 1.305, 1.566, 2.043, 2.322, 2.65, 2.853, 3.139, 3.485, 5.191}; // Above is okayish, this reflects detector plus adds HF
  static constexpr unsigned int netaforjer = sizeof(etaforjer)/sizeof(etaforjer[0])-1;

    //   static constexpr double ptforjec[] = {40, 60, 80, 100, 120, 140, 180, 220, 300, 500, 700, 5000};
  //  static constexpr double ptforjec[] = {40, 55, 80, 120, 170, 1000};
  //  static constexpr double ptforjec[] = {15 , 25, 55, 80, 120, 170, 1000};   // Low pT as Nick
  static constexpr double ptforjec[] = {15, 30, 40, 80, 92, 120, 1000};   // instable?
  //static constexpr double ptforjec[] = {15, 30, 80, 120, 1000};
  
  // static constexpr double ptforjec[] = {15 , 25, 40, 60, 80, 120, 1000};      // More bins in ZB 
  static constexpr unsigned int nptforjec = sizeof(ptforjec)/sizeof(ptforjec[0])-1;

  //  static constexpr double ptforJER[] = {15, 21, 28, 37, 49, 64, 84, 114, 153, 196, 245, 300, 362, 430, 507, 592, 686, 790, 905, 1032, 2238};
  static constexpr double ptforJER[] = {15, 21, 28, 37, 49, 64, 84, 114, 153, 196, 245, 300, 362, 430, 592, 790, 1032, 2238};
  static constexpr unsigned int nptforJER = sizeof(ptforJER)/sizeof(ptforJER[0])-1;

  static constexpr double alphavalues[] = {0.0, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 1.}; 
  //static constexpr double alphavalues[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
  static constexpr unsigned int nalphavalues = sizeof(alphavalues)/sizeof(alphavalues[0])-1;

  static constexpr double alphavaluesgraph[] = {0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5}; 
  //static constexpr double alphavalues[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
  static constexpr unsigned int nalphavaluesgraph = sizeof(alphavaluesgraph)/sizeof(alphavaluesgraph[0])-1;

  static constexpr double alphavaluesgraphjer[] = {0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45}; 
  //static constexpr double alphavalues[] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
  static constexpr unsigned int nalphavaluesgraphjer = sizeof(alphavaluesgraphjer)/sizeof(alphavaluesgraphjer[0])-1;

 
  histograms(TDirectory *dir, float ptmin, float ptmax, float hibinmin, float hibinmax, bool ismc);
  //   ~histograms();
  
 void Write();

  private:


};


#endif
