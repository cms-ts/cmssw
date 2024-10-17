#ifndef MNguyen_HiInclusiveJetSubstructure_inclusiveJetSubstructure_
#define MNguyen_HiInclusiveJetSubstructure_inclusiveJetSubstructure_

// system include files
#include <memory>
#include <string>
#include <iostream>

// ROOT headers
#include "TTree.h"
#include "TH2.h"
// user include files
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "fastjet/contrib/Njettiness.hh"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/Candidate.h"


//

/**\class HiInclusiveJetAnalyzer

   \author Matt Nguyen
   \date   November 2010
*/



float delta_phi(float phi1, float phi2);
//class HiInclusiveJetSubstructure : public edm::EDAnalyzer {
class HiInclusiveJetSubstructure : public edm::one::EDAnalyzer<edm::one::WatchRuns> {
public:

  explicit HiInclusiveJetSubstructure(const edm::ParameterSet&);

  ~HiInclusiveJetSubstructure() override;

  void analyze(const edm::Event&, const edm::EventSetup&) override;

  void beginRun(const edm::Run & r, const edm::EventSetup & c) override;

  void endRun(const edm::Run & r, const edm::EventSetup & c) override;

  void beginJob();

private:
  // for reWTA reclustering-----------------------
  bool doWTARecluster_ = false;
  fastjet::JetDefinition WTAjtDef = fastjet::JetDefinition(fastjet::JetAlgorithm::antikt_algorithm, 2, fastjet::WTA_pt_scheme);
  //--------------------------------------------
  void RandomConePtSum(float &cone_eta, float &cone_phi, float &cone_pt, edm::Handle<std::vector<reco::PFCandidate> > &pfCandidateColl);

  void IterativeDeclusteringRec(double groom_type, double groom_combine, const reco::Jet& jet,    fastjet::PseudoJet *sub1, fastjet::PseudoJet *sub2);
  void IterativeDeclusteringGen(double groom_type, double groom_combine, const reco::GenJet& jet, fastjet::PseudoJet *sub1, fastjet::PseudoJet *sub2);
  float ReadJetAsymmMap(float eta, float phi, TH2F Asymm_map);
  void RecoTruthSplitMatching(std::vector<fastjet::PseudoJet> &constituents_level1, fastjet::PseudoJet &hardest_level2, bool *bool_array, int *hardest_level1_split);
  void TruthRecoRecoTruthMatching();
  int getPFJetMuon(const pat::Jet& pfJet, const reco::PFCandidateCollection *pfCandidateColl);
  void LookThroughJetSplits(fastjet::PseudoJet jj, int i);
  double getPtRel(const reco::PFCandidate& lep, const pat::Jet& jet );

  void saveDaughters( const reco::GenParticle & gen);
  void saveDaughters( const reco::Candidate & gen);
  void analyzeSubjets(const reco::Jet& jet);
  int  getGroomedGenJetIndex(const reco::GenJet& jet) const;
  void analyzeRefSubjets(const reco::GenJet& jet);
  void analyzeGenSubjets(const reco::GenJet& jet);
  // float getAboveCharmThresh(reco::TrackRefVector& selTracks, const reco::TrackIPTagInfo& ipData, int sigOrVal);


  edm::InputTag   jetTagLabel_;
  edm::EDGetTokenT<std::vector<reco::Vertex> >       vtxTag_;
  edm::EDGetTokenT<pat::JetCollection>               jetTag_;
  edm::EDGetTokenT<pat::JetCollection>               matchTag_;
  edm::EDGetTokenT<pat::PackedCandidateCollection> pfCandidateToken_;
  // edm::EDGetTokenT<reco::PFCandidateCollection>      pfCandidateLabel_;
  edm::EDGetTokenT<reco::TrackCollection>            trackTag_;
  edm::EDGetTokenT<reco::GenParticleCollection>      genParticleSrc_;
  edm::EDGetTokenT<edm::View<reco::GenJet>>          genjetTag_;
  edm::EDGetTokenT<edm::HepMCProduct>                eventInfoTag_;
  edm::EDGetTokenT<GenEventInfoProduct>              eventGenInfoTag_;
  
  std::string                              jetName_; //used as prefix for jet structures
  edm::EDGetTokenT<edm::View<reco::Jet>>   subjetGenTag_;
  edm::Handle<reco::JetView>               gensubjets_;
  edm::EDGetTokenT< edm::ValueMap<float> > tokenGenTau1_;
  edm::EDGetTokenT< edm::ValueMap<float> > tokenGenTau2_;
  edm::EDGetTokenT< edm::ValueMap<float> > tokenGenTau3_;
  edm::EDGetTokenT< edm::ValueMap<float> > tokenGenSym_;
  edm::Handle<edm::ValueMap<float> >       genSymVM_;
  edm::EDGetTokenT< edm::ValueMap<int> >   tokenGenDroppedBranches_;
  edm::Handle<edm::ValueMap<int> >         genDroppedBranchesVM_;
  
  // towers
  edm::EDGetTokenT<CaloTowerCollection> TowerSrc_;

  std::vector<float> usedStringPts;

  TH2F *Asymm_map_;

  /// verbose ?
  bool verbose_;
  bool doMatch_;
  bool useVtx_;
  bool useRawPt_;
  bool doTower;
  bool isMC_;
  bool useHepMC_;
  bool fillGenJets_;
  bool useQuality_;
  std::string trackQuality_;

  bool doPrimaryLJPReco_;
  bool doPrimaryLJPTruth_;

  bool doChargedConstOnly_;
  bool doHardestSplitMatching_;
  bool doSubEvent_;
  bool doSubjetPurity;
  bool dopthatcut;
  double genPtMin_;
  bool doLifeTimeTagging_;
  bool doLifeTimeCandidateTagging_;
  bool doLifeTimeTaggingExtras_;
  bool saveBfragments_;
  bool doExtraCTagging_;

  bool doHiJetID_;
  bool doStandardJetID_;

  double rParam;
  double hardPtMin_;
  double jetPtMin_;
  double mysdcut1;
  double mysdcut2;
  double mydynktcut;
  double groom_type;
  double groom_combine;
  double jetAbsEtaMax_;
  bool doGenTaus_;
  bool doGenSym_;
  bool doSubJets_;
  bool doJetConstituents_;
  bool doGenSubJets_;


  //Systematics variables
  bool doNaiveNeuPFScaling_;
  bool doRatioNeuPFScaling_;
  double pfChargedCandidateEnergyScale_;
  double pfGammaCandidateEnergyScale_;
  double pfNeutralCandidateEnergyScale_;
  double TrackVariation_;
  bool pfNeutralSmear_;


  TTree *t;
  edm::Service<TFileService> fs1;

  std::string bTagJetName_;
  std::string ipTagInfos_;
  std::string svTagInfos_;
  std::string trackCHEBJetTags_;
  std::string trackCHPBJetTags_;
  std::string jetPBJetTags_;
  std::string jetBPBJetTags_;
  std::string simpleSVHighEffBJetTags_;
  std::string simpleSVHighPurBJetTags_;
  std::string combinedSVV1BJetTags_;
  std::string combinedSVV2BJetTags_;
  std::string deepCSVBJetTags_;
  
  static const int MAXJETS = 1000;
  static const int MAXTRACKS = 5000;
  static const int MAXBFRAG = 500;

  struct JRA{

    int nref;
    int run;
    int evt;
    int lumi;
    float vx, vy, vz;

    float rawpt[MAXJETS];
    float jtrawE[MAXJETS];
    float jtpt[MAXJETS];
    float jteta[MAXJETS];
    float jtphi[MAXJETS];

    //reWTA reclusted jet axis
    float WTAeta[MAXJETS];
    float WTAphi[MAXJETS];
    float WTAgeneta[MAXJETS];
    float WTAgenphi[MAXJETS];

    float jty[MAXJETS];
    float jtpu[MAXJETS];
    float jtm[MAXJETS];
    float jtarea[MAXJETS];


    
    float jttau1[MAXJETS];
    float jttau2[MAXJETS];
    float jttau3[MAXJETS];

    // float jtsym[MAXJETS];
    // float jtrg[MAXJETS];
    // float jtdyn_pt1[MAXJETS];
    // float jtangu[MAXJETS];
    float jtdyn_var[MAXJETS];
    int jtdyn_split[MAXJETS];
    // float jtdyn_theta[MAXJETS];
    float jtdyn_deltaR[MAXJETS];
    float jtdyn_kt[MAXJETS];
    float jtdyn_eta[MAXJETS];
    float jtdyn_phi[MAXJETS];
    float jtdyn_z[MAXJETS];
    int jt_intjet_multi[MAXJETS];
    float jt_girth[MAXJETS];
    float jt_girth_new[MAXJETS];
    float jt_thrust[MAXJETS];
    float jt_LHA[MAXJETS];
    float jt_pTD[MAXJETS];
    std::vector<std::vector<float>> jt_PLJPkT;
    std::vector<std::vector<float>> jt_PLJPdR;
    std::vector<std::vector<float>> jt_PLJPeta;
    std::vector<std::vector<float>> jt_PLJPphi;

    std::vector<std::vector<float>> jtSubJetPt;
    std::vector<std::vector<float>> jtSubJetEta;
    std::vector<std::vector<float>> jtSubJetPhi;
    std::vector<std::vector<float>> jtSubJetM;

    std::vector<fastjet::PseudoJet> jtJetConstituent;
    // std::vector<std::vector<float>> jtJetConstituentPhi;
    // std::vector<int> jtJetConstituentHardestSplitN;
    std::vector<fastjet::PseudoJet> refJetConstituent;
    // std::vector<std::vector<float>> refJetConstituentPhi;
    // std::vector<int> refJetConstituentHardestSplitN;

    bool triggerJetInAcceptance;

    bool jtdyn_isClosestToTruth[MAXJETS];
    bool refdyn_isClosestToReco[MAXJETS];
    float jtdyn_refdyn_dR[MAXJETS];

    std::vector<std::vector<int>> jtConstituentsId;
    std::vector<std::vector<float>> jtConstituentsE;
    std::vector<std::vector<float>> jtConstituentsPt;
    std::vector<std::vector<float>> jtConstituentsEta;
    std::vector<std::vector<float>> jtConstituentsPhi;
    std::vector<std::vector<float>> jtConstituentsM;
    std::vector<std::vector<int>> jtSDConstituentsId;
    std::vector<std::vector<float>> jtSDConstituentsE;
    std::vector<std::vector<float>> jtSDConstituentsPt;
    std::vector<std::vector<float>> jtSDConstituentsEta;
    std::vector<std::vector<float>> jtSDConstituentsPhi;
    std::vector<std::vector<float>> jtSDConstituentsM;


    float trackMax[MAXJETS] = {0};
    float trackSum[MAXJETS] = {0};
    int trackN[MAXJETS] = {0};

    float chargedMax[MAXJETS] = {0};
    float chargedSum[MAXJETS] = {0};
    int chargedN[MAXJETS] = {0};

    float h_HFMax[MAXJETS] = {0};
    float h_HFSum[MAXJETS] = {0};
    int h_HFN[MAXJETS] = {0};

    float eg_HFMax[MAXJETS] = {0};
    float eg_HFSum[MAXJETS] = {0};
    int eg_HFN[MAXJETS] = {0};

    float photonMax[MAXJETS] = {0};
    float photonSum[MAXJETS] = {0};
    int photonN[MAXJETS] = {0};

    float trackHardSum[MAXJETS] = {0};
    float chargedHardSum[MAXJETS] = {0};
    float photonHardSum[MAXJETS] = {0};

    int trackHardN[MAXJETS] = {0};
    int chargedHardN[MAXJETS] = {0};
    int photonHardN[MAXJETS] = {0};

    float neutralMax[MAXJETS] = {0};
    float neutralSum[MAXJETS] = {0};
    int neutralN[MAXJETS] = {0};

    float eMax[MAXJETS] = {0};
    float eSum[MAXJETS] = {0};
    int eN[MAXJETS] = {0};

    float muMax[MAXJETS] = {0};
    float muSum[MAXJETS] = {0};
    int muN[MAXJETS] = {0};

    float genChargedSum[MAXJETS] = {0};
    float genHardSum[MAXJETS] = {0};
    float signalChargedSum[MAXJETS] = {0};
    float signalHardSum[MAXJETS] = {0};
    // Update by Raghav, modified to take it from the towers
    float hcalSum[MAXJETS];
    float ecalSum[MAXJETS];

    float fHPD[MAXJETS];
    float fRBX[MAXJETS];
    int n90[MAXJETS];
    float fSubDet1[MAXJETS];
    float fSubDet2[MAXJETS];
    float fSubDet3[MAXJETS];
    float fSubDet4[MAXJETS];
    float restrictedEMF[MAXJETS];
    int nHCAL[MAXJETS];
    int nECAL[MAXJETS];
    float apprHPD[MAXJETS];
    float apprRBX[MAXJETS];

    //    int n90[MAXJETS];
    int n2RPC[MAXJETS];
    int n3RPC[MAXJETS];
    int nRPC[MAXJETS];

    float fEB[MAXJETS];
    float fEE[MAXJETS];
    float fHB[MAXJETS];
    float fHE[MAXJETS];
    float fHO[MAXJETS];
    float fLong[MAXJETS];
    float fShort[MAXJETS];
    float fLS[MAXJETS];
    float fHFOOT[MAXJETS];

    int subid[MAXJETS];

    float matchedPt[MAXJETS];
    float matchedRawPt[MAXJETS];
    float matchedR[MAXJETS];
    float matchedPu[MAXJETS];
    int matchedHadronFlavor[MAXJETS];
    int matchedPartonFlavor[MAXJETS];

    float discr_csvV1[MAXJETS];
    float discr_csvV2[MAXJETS];
    float discr_deepCSV[MAXJETS];
    float discr_muByIp3[MAXJETS];
    float discr_muByPt[MAXJETS];
    float discr_prob[MAXJETS];
    float discr_probb[MAXJETS];
    float discr_tcHighEff[MAXJETS];
    float discr_tcHighPur[MAXJETS];
    float discr_ssvHighEff[MAXJETS];
    float discr_ssvHighPur[MAXJETS];

    float ndiscr_ssvHighEff[MAXJETS];
    float ndiscr_ssvHighPur[MAXJETS];
    float ndiscr_csvV1[MAXJETS];
    float ndiscr_csvV2[MAXJETS];
    float ndiscr_muByPt[MAXJETS];

    float pdiscr_csvV1[MAXJETS];
    float pdiscr_csvV2[MAXJETS];

    int nsvtx[MAXJETS];
    int svtxntrk[MAXJETS];
    float svtxdl[MAXJETS];
    float svtxdls[MAXJETS];
    float svtxdl2d[MAXJETS];
    float svtxdls2d[MAXJETS];
    float svtxm[MAXJETS];
    float svtxpt[MAXJETS];
    float svtxmcorr[MAXJETS];
    float svtxnormchi2[MAXJETS];
    float svJetDeltaR[MAXJETS];
    float svtxTrkSumChi2[MAXJETS];
    int svtxTrkNetCharge[MAXJETS];
    int svtxNtrkInCone[MAXJETS];

    int nIPtrk[MAXJETS];
    int nselIPtrk[MAXJETS];

    int nIP;
    int ipJetIndex[MAXTRACKS];
    float ipPt[MAXTRACKS];
    float ipEta[MAXTRACKS];
    float ipDxy[MAXTRACKS];
    float ipDz[MAXTRACKS];
    float ipChi2[MAXTRACKS];
    int ipNHit[MAXTRACKS];
    int ipNHitPixel[MAXTRACKS];
    int ipNHitStrip[MAXTRACKS];
    bool ipIsHitL1[MAXTRACKS];
    float ipProb0[MAXTRACKS];
    float ipProb1[MAXTRACKS];
    float ip2d[MAXTRACKS];
    float ip2dSig[MAXTRACKS];
    float ip3d[MAXTRACKS];
    float ip3dSig[MAXTRACKS];
    float ipDist2Jet[MAXTRACKS];
    float ipDist2JetSig[MAXTRACKS];
    float ipClosest2Jet[MAXTRACKS];
  
    float trackPtRel[MAXTRACKS];
    float trackPtRatio[MAXTRACKS];
    float trackPPar[MAXTRACKS];
    float trackPParRatio[MAXTRACKS];
    float trackDeltaR[MAXTRACKS];

    float trackSip2dSigAboveCharm[MAXJETS];
    float trackSip2dValAboveCharm[MAXJETS];
    float trackSip3dValAboveCharm[MAXJETS];
    float trackSip3dSigAboveCharm[MAXJETS];
    float trackSumJetDeltaR[MAXJETS];

    float mue[MAXJETS];
    float mupt[MAXJETS];
    float mueta[MAXJETS];
    float muphi[MAXJETS];
    float mudr[MAXJETS];
    float muptrel[MAXJETS];
    int muchg[MAXJETS];

    float refpt[MAXJETS];
    float refeta[MAXJETS];
    float refphi[MAXJETS];
    float refm[MAXJETS];
    float refarea[MAXJETS];
    float refy[MAXJETS];
    float reftau1[MAXJETS];
    float reftau2[MAXJETS];
    float reftau3[MAXJETS];
    float refsym[MAXJETS];
    // float refrg[MAXJETS];
    // float refdyn_pt1[MAXJETS];
    // float refangu[MAXJETS];
    
    float refdyn_var[MAXJETS];
    int refdyn_split[MAXJETS];
    // float refdyn_theta[MAXJETS];
    float refdyn_deltaR[MAXJETS];
    float refdyn_kt[MAXJETS];
    float refdyn_eta[MAXJETS];
    float refdyn_phi[MAXJETS];
    float refdyn_z[MAXJETS];
    int ref_intjet_multi[MAXJETS];
    float ref_girth[MAXJETS];
    float ref_girth_new[MAXJETS];
    float ref_thrust[MAXJETS];
    float ref_LHA[MAXJETS];
    float ref_pTD[MAXJETS];
    std::vector<std::vector<float>> ref_PLJPkT = {};
    std::vector<std::vector<float>> ref_PLJPdR = {};
    std::vector<std::vector<float>> ref_PLJPeta = {};
    std::vector<std::vector<float>> ref_PLJPphi = {};
    std::vector<float> ref_test_vec = {};

    float refsub11[MAXJETS];
    float refsub12[MAXJETS];
    float refsub21[MAXJETS];
    float refsub22[MAXJETS];
    float refdphijt[MAXJETS];
    float refdrjt[MAXJETS];
    float refparton_pt[MAXJETS];
    int refparton_flavor[MAXJETS];
    int refparton_flavorForB[MAXJETS];

    float refptG[MAXJETS];
    float refetaG[MAXJETS];
    float refphiG[MAXJETS];
    float refmG[MAXJETS];
    std::vector<std::vector<float>> refSubJetPt;
    std::vector<std::vector<float>> refSubJetEta;
    std::vector<std::vector<float>> refSubJetPhi;
    std::vector<std::vector<float>> refSubJetM;
    
    std::vector<std::vector<int>> refConstituentsId;
    std::vector<std::vector<float>> refConstituentsE;
    std::vector<std::vector<float>> refConstituentsPt;
    std::vector<std::vector<float>> refConstituentsEta;
    std::vector<std::vector<float>> refConstituentsPhi;
    std::vector<std::vector<float>> refConstituentsM;
    std::vector<std::vector<int>> refSDConstituentsId;
    std::vector<std::vector<float>> refSDConstituentsE;
    std::vector<std::vector<float>> refSDConstituentsPt;
    std::vector<std::vector<float>> refSDConstituentsEta;
    std::vector<std::vector<float>> refSDConstituentsPhi;
    std::vector<std::vector<float>> refSDConstituentsM;

    float pthat;
    int beamId1, beamId2;
    int ngen;
    int genmatchindex[MAXJETS];
    float genpt[MAXJETS];
    float geneta[MAXJETS];
    float genphi[MAXJETS];
    float genm[MAXJETS];
    float geny[MAXJETS];
    float gentau1[MAXJETS];
    float gentau2[MAXJETS];
    float gentau3[MAXJETS];
    float gendphijt[MAXJETS];
    float gendrjt[MAXJETS];
    int gensubid[MAXJETS];

    float genptG[MAXJETS];
    float genetaG[MAXJETS];
    float genphiG[MAXJETS];
    float genmG[MAXJETS];
    std::vector<std::vector<float>> genSubJetPt;
    std::vector<std::vector<float>> genSubJetEta;
    std::vector<std::vector<float>> genSubJetPhi;
    std::vector<std::vector<float>> genSubJetM;
    std::vector<std::vector<float>> genSubJetArea;
    float gensym[MAXJETS];
    int   gendroppedBranches[MAXJETS];
    
    std::vector<std::vector<int>> genConstituentsId;
    std::vector<std::vector<float>> genConstituentsE;
    std::vector<std::vector<float>> genConstituentsPt;
    std::vector<std::vector<float>> genConstituentsEta;
    std::vector<std::vector<float>> genConstituentsPhi;
    std::vector<std::vector<float>> genConstituentsM;
    std::vector<std::vector<int>> genSDConstituentsId;
    std::vector<std::vector<float>> genSDConstituentsE;
    std::vector<std::vector<float>> genSDConstituentsPt;
    std::vector<std::vector<float>> genSDConstituentsEta;
    std::vector<std::vector<float>> genSDConstituentsPhi;
    std::vector<std::vector<float>> genSDConstituentsM;

    int bMult;
    int bJetIndex[MAXBFRAG];
    int bStatus[MAXBFRAG];
    int bPdg[MAXBFRAG];
    int bChg[MAXBFRAG];
    float bVx[MAXBFRAG];
    float bVy[MAXBFRAG];
    float bVz[MAXBFRAG];
    float bPt[MAXBFRAG];
    float bEta[MAXBFRAG];
    float bPhi[MAXBFRAG];
  };

  JRA jets_;

};

#endif
