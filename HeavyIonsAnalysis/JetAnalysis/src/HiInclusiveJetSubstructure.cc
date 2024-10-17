/*
  Based on the jet response analyzer
  Modified by Matt Nguyen, November 2010
  Modified by Leticia Cunqueiro, September 2021
*/
#include "HeavyIonsAnalysis/JetAnalysis/interface/HiInclusiveJetSubstructure.h"
#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
//#include "RecoBTag/SecondaryVertex/interface/TrackKinematics.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/HiGenData/interface/GenHIEvent.h"
// #include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "TLorentzVector.h"
#include <random>
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"
#include <math.h>

using namespace std;
using namespace edm;
using namespace reco;

// float delta_phi(float phi1, float phi2){
//     float result = phi1 - phi2;
//     while ( result < -M_PI )
//       {
//         result += 2.*M_PI;
//       }
//     while ( result > M_PI )
//       {
//         result -= 2.*M_PI;
//       }
//     return result;
// }

HiInclusiveJetSubstructure::HiInclusiveJetSubstructure(const edm::ParameterSet& iConfig)
{
  doMatch_ = iConfig.getUntrackedParameter<bool>("matchJets",false);
  jetTag_ = consumes<pat::JetCollection> (iConfig.getParameter<InputTag>("jetTag"));
  matchTag_ = consumes<pat::JetCollection> (iConfig.getUntrackedParameter<InputTag>("matchTag"));

  vtxTag_ = consumes<vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vtxTag"));
  //vtxTag_ = consumes<vector<reco::Vertex>>(iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("offlinePrimaryVertices")));
  trackTag_ = consumes<reco::TrackCollection> (iConfig.getParameter<InputTag>("trackTag"));
  useQuality_ = iConfig.getUntrackedParameter<bool>("useQuality",1);
  trackQuality_ = iConfig.getUntrackedParameter<string>("trackQuality","highPurity");

  jetName_ = iConfig.getUntrackedParameter<string>("jetName");
  doGenTaus_ = iConfig.getUntrackedParameter<bool>("doGenTaus",0);
  doGenSym_ = iConfig.getUntrackedParameter<bool>("doGenSym",0);
  doSubJets_ = iConfig.getUntrackedParameter<bool>("doSubJets",0);
  doJetConstituents_ = iConfig.getUntrackedParameter<bool>("doJetConstituents", false);
  doGenSubJets_ = iConfig.getUntrackedParameter<bool>("doGenSubJets", false);
  if (doGenSubJets_)
    subjetGenTag_ = consumes<reco::JetView> (iConfig.getUntrackedParameter<InputTag>("subjetGenTag"));
  // subjetGenTag_ = consumes<reco::JetView> (iConfig.getUntrackedParameter<InputTag>("subjetGenTag"));

  //reWTA reclustering
  doWTARecluster_ = iConfig.getUntrackedParameter<bool>("doWTARecluster", false);
/*
  if(doGenTaus_){
    tokenGenTau1_ = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genTau1"));
    tokenGenTau2_ = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genTau2"));
    tokenGenTau3_ = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genTau3"));
  }
*/
  if (doGenSym_){
    tokenGenSym_ = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("genSym"));
    tokenGenDroppedBranches_ = consumes<edm::ValueMap<int> >(iConfig.getParameter<edm::InputTag>("genDroppedBranches"));
  }

  isMC_ = iConfig.getUntrackedParameter<bool>("isMC",false);
  useHepMC_ = iConfig.getUntrackedParameter<bool> ("useHepMC",false);
  fillGenJets_ = iConfig.getUntrackedParameter<bool>("fillGenJets",false);

  doHiJetID_ = iConfig.getUntrackedParameter<bool>("doHiJetID",false);
  doStandardJetID_ = iConfig.getUntrackedParameter<bool>("doStandardJetID",false);

  rParam = iConfig.getParameter<double>("rParam");
  hardPtMin_ = iConfig.getUntrackedParameter<double>("hardPtMin",4);
  jetPtMin_ = iConfig.getParameter<double>("jetPtMin");
  mysdcut1 = iConfig.getParameter<double>("mysdcut1");
  mysdcut2 = iConfig.getParameter<double>("mysdcut2");
  mydynktcut = iConfig.getParameter<double>("mydynktcut");
  groom_type = iConfig.getParameter<double>("groom_type");
  groom_combine = iConfig.getParameter<double>("groom_combine");
  jetAbsEtaMax_ = iConfig.getUntrackedParameter<double>("jetAbsEtaMax", 2.5);

  if(isMC_){
    genjetTag_ = consumes<edm::View<reco::GenJet>>(iConfig.getParameter<InputTag>("genjetTag"));
    if(useHepMC_) eventInfoTag_ = consumes<HepMCProduct> (iConfig.getParameter<InputTag>("eventInfoTag"));
    eventGenInfoTag_ = consumes<GenEventInfoProduct> (iConfig.getParameter<InputTag>("eventInfoTag"));
  }
  verbose_ = iConfig.getUntrackedParameter<bool>("verbose",false);
  useVtx_ = iConfig.getUntrackedParameter<bool>("useVtx",false);
  useRawPt_ = iConfig.getUntrackedParameter<bool>("useRawPt",true);

  doLifeTimeTagging_ = iConfig.getUntrackedParameter<bool>("doLifeTimeTagging",false);
  doLifeTimeCandidateTagging_ = iConfig.getUntrackedParameter<bool>("doLifeTimeCandidateTagging",false);
  doLifeTimeTaggingExtras_ = iConfig.getUntrackedParameter<bool>("doLifeTimeTaggingExtras",true);
  saveBfragments_  = iConfig.getUntrackedParameter<bool>("saveBfragments",false);

  // pfCandidateLabel_ = consumes<reco::PFCandidateCollection>(iConfig.getUntrackedParameter<edm::InputTag>("pfCandidateLabel")); 
  // pfCandidateToken_ = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCandSource"));
  doTower = iConfig.getUntrackedParameter<bool>("doTower",false);
  if(doTower){
    TowerSrc_ = consumes<CaloTowerCollection>(iConfig.getParameter<edm::InputTag>("towersSrc"));
  }

  doExtraCTagging_ = iConfig.getUntrackedParameter<bool>("doExtraCTagging",false);

  pfCandidateToken_ = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCandSource"));


  if(isMC_){
    genParticleSrc_ = consumes<reco::GenParticleCollection>(iConfig.getUntrackedParameter<edm::InputTag>("genParticles"));
  }

  doSubEvent_ = 0;
  doChargedConstOnly_ = iConfig.getUntrackedParameter<bool>("doChargedConstOnly",0);
  TrackVariation_ = -1;
  pfChargedCandidateEnergyScale_ = -1;
  pfNeutralCandidateEnergyScale_ = -1;
  pfGammaCandidateEnergyScale_ = -1;
  pfNeutralSmear_ = false;
  doNaiveNeuPFScaling_ = false;
  doRatioNeuPFScaling_ = false;

  doPrimaryLJPReco_ = iConfig.getUntrackedParameter<bool>("doPrimaryLJPReco", false);
  if(isMC_){
    doPrimaryLJPTruth_ = iConfig.getUntrackedParameter<bool>("doPrimaryLJPTruth", false);
    pfChargedCandidateEnergyScale_ = iConfig.getUntrackedParameter<double>("pfChargedEnergyScaleVar",1.);
    pfNeutralCandidateEnergyScale_ = iConfig.getUntrackedParameter<double>("pfNeutralEnergyScaleVar",1.);
    pfGammaCandidateEnergyScale_ = iConfig.getUntrackedParameter<double>("pfGammaEnergyScaleVar",1.);
    pfNeutralSmear_ = iConfig.getUntrackedParameter<bool>("pfNeutralSmear", false);
    doNaiveNeuPFScaling_ = iConfig.getUntrackedParameter<bool>("doNaiveNeuPFScaling", false);
    doRatioNeuPFScaling_ = iConfig.getUntrackedParameter<bool>("doRatioNeuPFScaling", false);

    TrackVariation_ = iConfig.getUntrackedParameter<double>("TrackVariation",0.);

    genPtMin_ = iConfig.getUntrackedParameter<double>("genPtMin",10);
    doSubEvent_ = iConfig.getUntrackedParameter<bool>("doSubEvent",0);
    doSubjetPurity = iConfig.getUntrackedParameter<bool>("doSubjetPurity",0);
    dopthatcut = iConfig.getUntrackedParameter<bool>("dopthatcut",0);
    doHardestSplitMatching_ = iConfig.getUntrackedParameter<bool>("doHardestSplitMatching",0);
  }
}

HiInclusiveJetSubstructure::~HiInclusiveJetSubstructure() { 
}

void HiInclusiveJetSubstructure::beginRun(const edm::Run& run, const edm::EventSetup & es) {
}
void HiInclusiveJetSubstructure::endRun(const edm::Run& run, const edm::EventSetup & es) {
}

void HiInclusiveJetSubstructure::beginJob() {
  std::cout << "Running job with systematics" << std::endl;
  std::cout << "Doing Charged only: " << doChargedConstOnly_ << std::endl;
  std::cout << "Track efficiency var: " << TrackVariation_ << std::endl;
  std::cout << "Charged pfCand 4-mom var: " << pfChargedCandidateEnergyScale_ << std::endl;
  std::cout << "Neutral pfCand 4-mom var: " << pfNeutralCandidateEnergyScale_ << std::endl;
  std::cout << "Photon pfCand 4-mom var: " << pfGammaCandidateEnergyScale_ << std::endl;
  std::cout << "Neutral phi and rapidity smearing: " << pfNeutralSmear_ << std::endl;
  string jetTagTitle = jetTagLabel_.label()+" Jet Analysis Tree";
  t = fs1->make<TTree>("t",jetTagTitle.c_str());
  t->Branch("run",&jets_.run,"run/I");
  t->Branch("evt",&jets_.evt,"evt/I");
  t->Branch("lumi",&jets_.lumi,"lumi/I");
  if (useVtx_) {
    t->Branch("vx",&jets_.vx,"vx/F");
    t->Branch("vy",&jets_.vy,"vy/F");
    t->Branch("vz",&jets_.vz,"vz/F");
  }

  t->Branch("nref",&jets_.nref,"nref/I");
  t->Branch("jtptUncorrected",jets_.rawpt,"jtptUncorrected[nref]/F");
  t->Branch("jtEUncorrected",jets_.jtrawE,"jtEUncorrected[nref]/F");
  t->Branch("jtpt",jets_.jtpt,"jtpt[nref]/F");
  t->Branch("jteta",jets_.jteta,"jteta[nref]/F");
  t->Branch("jtphi",jets_.jtphi,"jtphi[nref]/F");
  if (doHiJetID_) {
    t->Branch("trackMax", jets_.trackMax, "trackMax[nref]/F");
    t->Branch("trackSum", jets_.trackSum, "trackSum[nref]/F");
    t->Branch("trackN", jets_.trackN, "trackN[nref]/I");
    t->Branch("trackHardSum", jets_.trackHardSum, "trackHardSum[nref]/F");
    t->Branch("trackHardN", jets_.trackHardN, "trackHardN[nref]/I");

    t->Branch("chargedMax", jets_.chargedMax, "chargedMax[nref]/F");
    t->Branch("chargedSum", jets_.chargedSum, "chargedSum[nref]/F");
    t->Branch("chargedN", jets_.chargedN, "chargedN[nref]/I");
    t->Branch("chargedHardSum", jets_.chargedHardSum, "chargedHardSum[nref]/F");
    t->Branch("chargedHardN", jets_.chargedHardN, "chargedHardN[nref]/I");

    t->Branch("photonMax", jets_.photonMax, "photonMax[nref]/F");
    t->Branch("photonSum", jets_.photonSum, "photonSum[nref]/F");
    t->Branch("photonN", jets_.photonN, "photonN[nref]/I");
    t->Branch("photonHardSum", jets_.photonHardSum, "photonHardSum[nref]/F");
    t->Branch("photonHardN", jets_.photonHardN, "photonHardN[nref]/I");

    t->Branch("neutralMax", jets_.neutralMax, "neutralMax[nref]/F");
    t->Branch("neutralSum", jets_.neutralSum, "neutralSum[nref]/F");
    t->Branch("neutralN", jets_.neutralN, "neutralN[nref]/I");

    t->Branch("eMax", jets_.eMax, "eMax[nref]/F");
    t->Branch("eSum", jets_.eSum, "eSum[nref]/F");
    t->Branch("eN", jets_.eN, "eN[nref]/I");

    t->Branch("eg_HFMax", jets_.eg_HFMax, "eg_HFMax[nref]/F");
    t->Branch("eg_HFSum", jets_.eg_HFSum, "eg_HFSum[nref]/F");
    t->Branch("eg_HFN", jets_.eg_HFN, "eg_HFN[nref]/I");

    t->Branch("h_HFMax", jets_.h_HFMax, "h_HFMax[nref]/F");
    t->Branch("h_HFSum", jets_.h_HFSum, "h_HFSum[nref]/F");
    t->Branch("h_HFN", jets_.h_HFN, "h_HFN[nref]/I");

    t->Branch("muMax", jets_.muMax, "muMax[nref]/F");
    t->Branch("muSum", jets_.muSum, "muSum[nref]/F");
    t->Branch("muN", jets_.muN, "muN[nref]/I");
  }
  if (doStandardJetID_) {
    t->Branch("fHPD", jets_.fHPD, "fHPD[nref]/F");
    t->Branch("fRBX", jets_.fRBX, "fRBX[nref]/F");
    t->Branch("n90", jets_.n90, "n90[nref]/I");
    t->Branch("fSubDet1", jets_.fSubDet1, "fSubDet1[nref]/F");
    t->Branch("fSubDet2", jets_.fSubDet2, "fSubDet2[nref]/F");
    t->Branch("fSubDet3", jets_.fSubDet3, "fSubDet3[nref]/F");
    t->Branch("fSubDet4", jets_.fSubDet4, "fSubDet4[nref]/F");
    t->Branch("restrictedEMF", jets_.restrictedEMF, "restrictedEMF[nref]/F");
    t->Branch("nHCAL", jets_.nHCAL, "nHCAL[nref]/I");
    t->Branch("nECAL", jets_.nECAL, "nECAL[nref]/I");
    t->Branch("apprHPD", jets_.apprHPD, "apprHPD[nref]/F");
    t->Branch("apprRBX", jets_.apprRBX, "apprRBX[nref]/F");
    t->Branch("n2RPC", jets_.n2RPC, "n2RPC[nref]/I");
    t->Branch("n3RPC", jets_.n3RPC, "n3RPC[nref]/I");
    t->Branch("nRPC", jets_.nRPC, "nRPC[nref]/I");

    t->Branch("fEB", jets_.fEB, "fEB[nref]/F");
    t->Branch("fEE", jets_.fEE, "fEE[nref]/F");
    t->Branch("fHB", jets_.fHB, "fHB[nref]/F");
    t->Branch("fHE", jets_.fHE, "fHE[nref]/F");
    t->Branch("fHO", jets_.fHO, "fHO[nref]/F");
    t->Branch("fLong", jets_.fLong, "fLong[nref]/F");
    t->Branch("fShort", jets_.fShort, "fShort[nref]/F");
    t->Branch("fLS", jets_.fLS, "fLS[nref]/F");
    t->Branch("fHFOOT", jets_.fHFOOT, "fHFOOT[nref]/F");
  }



  // t->Branch("jtsym",jets_.jtsym,"jtsym[nref]/F");
  // t->Branch("jtrg",jets_.jtrg,"jtrg[nref]/F");
  // t->Branch("jtdynkt",jets_.jtdynkt,"jtdynkt[nref]/F");
  // t->Branch("jtdyn_pt1",jets_.jtdyn_pt1,"jtdyn_pt1[nref]/F");
  // t->Branch("jtdyn_var",jets_.jtdyn_var,"jtdyn_var[nref]/F");
  t->Branch("jtdyn_split",jets_.jtdyn_split,"jtdyn_split[nref]/I");
  t->Branch("jtdyn_eta",jets_.jtdyn_eta,"jtdyn_eta[nref]/F");
  t->Branch("jtdyn_phi",jets_.jtdyn_phi,"jtdyn_phi[nref]/F");
  // t->Branch("jtdyn_theta",jets_.jtdyn_theta,"jtdyn_theta[nref]/F");
  t->Branch("jtdyn_deltaR",jets_.jtdyn_deltaR,"jtdyn_deltaR[nref]/F");
  t->Branch("jtdyn_kt",jets_.jtdyn_kt,"jtdyn_kt[nref]/F");
  t->Branch("jtdyn_z",jets_.jtdyn_z,"jtdyn_z[nref]/F");

  t->Branch("jt_intjet_multi", jets_.jt_intjet_multi,"jt_intjet_multi[nref]/I");
  t->Branch("jt_girth", jets_.jt_girth, "jt_girth[nref]/F");
  t->Branch("jt_girth_new", jets_.jt_girth_new, "jt_girth_new[nref]/F");
  t->Branch("jt_thrust", jets_.jt_thrust,"jt_thrust[nref]/I");
  t->Branch("jt_LHA", jets_.jt_LHA,"jt_LHA[nref]/I");
  t->Branch("jt_pTD", jets_.jt_pTD,"jt_pTD[nref]/I");
  if(doPrimaryLJPReco_){
    t->Branch("jt_PLJPkT",&jets_.jt_PLJPkT);
    t->Branch("jt_PLJPdR",&jets_.jt_PLJPdR);
    t->Branch("jt_PLJPeta",&jets_.jt_PLJPeta);
    t->Branch("jt_PLJPphi",&jets_.jt_PLJPphi);
  }
  t->Branch("triggerJetInAcceptance", &jets_.triggerJetInAcceptance, "triggerJetInAcceptance/O");
  // t->Branch("jtangu",jets_.jtangu,"jtangu[nref]/F");

  if(isMC_){
    if (useHepMC_) {
      t->Branch("beamId1",&jets_.beamId1,"beamId1/I");
      t->Branch("beamId2",&jets_.beamId2,"beamId2/I");
    }
    // Only matched gen jets
    t->Branch("refpt",jets_.refpt,"refpt[nref]/F");
    t->Branch("refeta",jets_.refeta,"refeta[nref]/F");
    t->Branch("refphi",jets_.refphi,"refphi[nref]/F");
    // t->Branch("refsym",jets_.refsym,"refsym[nref]/F");
    // t->Branch("refrg",jets_.refrg,"rg[nref]/F");
    // t->Branch("refdynkt",jets_.refdynkt,"refdynkt[nref]/F");
    // t->Branch("refangu",jets_.refangu,"refangu[nref]/F");
    // t->Branch("refdyn_pt1",jets_.refdyn_pt1,"refdyn_pt1[nref]/F");
    // t->Branch("refdyn_var",jets_.refdyn_var,"refdyn_var[nref]/F");
    t->Branch("refdyn_split",jets_.refdyn_split,"refdyn_split[nref]/I");
    t->Branch("refdyn_eta",jets_.refdyn_eta,"refdyn_eta[nref]/F");
    t->Branch("refdyn_phi",jets_.refdyn_phi,"refdyn_phi[nref]/F");
    // t->Branch("refdyn_theta",jets_.refdyn_theta,"refdyn_theta[nref]/F");
    t->Branch("refdyn_deltaR",jets_.refdyn_deltaR,"refdyn_deltaR[nref]/F");
    t->Branch("refdyn_kt",jets_.refdyn_kt,"refdyn_kt[nref]/F");
    t->Branch("refdyn_z",jets_.refdyn_z,"refdyn_z[nref]/F");
    
    t->Branch("jtdyn_isClosestToTruth", jets_.jtdyn_isClosestToTruth,"jtdyn_isClosestToTruth[nref]/O");
    t->Branch("refdyn_isClosestToReco", jets_.refdyn_isClosestToReco,"refdyn_isClosestToReco[nref]/O");
    t->Branch("jtdyn_refdyn_dR", jets_.jtdyn_refdyn_dR,"jtdyn_refdyn_dR[nref]/F");

    t->Branch("ref_intjet_multi", jets_.ref_intjet_multi,"ref_intjet_multi[nref]/I");
    t->Branch("ref_girth", jets_.ref_girth, "ref_girth[nref]/F");
    t->Branch("ref_girth_new", jets_.ref_girth_new, "ref_girth_new[nref]/F");
    t->Branch("ref_thrust", jets_.ref_thrust,"ref_thrust[nref]/I");
    t->Branch("ref_LHA", jets_.ref_LHA,"ref_LHA[nref]/I");
    t->Branch("ref_pTD", jets_.ref_pTD,"ref_pTD[nref]/I");
    if(doPrimaryLJPTruth_){
      t->Branch("ref_PLJPkT",&jets_.ref_PLJPkT);
      t->Branch("ref_PLJPdR",&jets_.ref_PLJPdR);
      t->Branch("ref_PLJPeta",&jets_.ref_PLJPeta);
      t->Branch("ref_PLJPphi",&jets_.ref_PLJPphi);
    }
    

    if(doSubjetPurity){
      t->Branch("refsub11",jets_.refsub11,"sub11[nref]/F");
      t->Branch("refsub12",jets_.refsub12,"sub12[nref]/F");
      t->Branch("refsub21",jets_.refsub21,"sub21[nref]/F");
      t->Branch("refsub22",jets_.refsub22,"sub22[nref]/F");
    }
    t->Branch("refparton_pt",jets_.refparton_pt,"refparton_pt[nref]/F");
    t->Branch("refparton_flavor",jets_.refparton_flavor,"refparton_flavor[nref]/I");
  }    

  if(doSubEvent_){
    t->Branch("subid",jets_.subid,"subid[nref]/I");
  }
  float x[83] = {-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664, -3.489, -3.314, -3.139, -2.964, -2.853, -2.65, -2.5, -2.322, -2.172, -2.043, -1.93, -1.83, -1.74, -1.653, -1.566, -1.479, -1.392, -1.305, -1.218, -1.131, -1.044, -0.957, -0.879, -0.783, -0.696, -0.609, -0.522, -0.435, -0.348, -0.261, -0.174, -0.087, 0, 0.087, 0.174, 0.261, 0.348, 0.435, 0.522, 0.609, 0.696, 0.783, 0.879, 0.957, 1.044, 1.131, 1.218, 1.305, 1.392, 1.479, 1.566, 1.653, 1.74, 1.83, 1.93, 2.043, 2.172, 2.322, 2.5, 2.65, 2.853, 2.964, 3.139, 3.314, 3.489, 3.664, 3.839, 4.013, 4.191, 4.363, 4.538, 4.716, 4.889, 5.191};
  float y[73] = {-3.14159, -3.05433, -2.96706, -2.87979, -2.79253, -2.70526, -2.61799, -2.53073, -2.44346, -2.35619, -2.26893, -2.18166, -2.0944, -2.00713, -1.91986, -1.8326, -1.74533, -1.65806, -1.5708, -1.48353, -1.39626, -1.309, -1.22173, -1.13446, -1.0472, -0.959931, -0.872665, -0.785398, -0.698132, -0.610865, -0.523599, -0.436332, -0.349066, -0.261799, -0.174533, -0.0872665, 0, 0.0872665, 0.174533, 0.261799, 0.349066, 0.436332, 0.523599, 0.610865, 0.698132, 0.785398, 0.872665, 0.959931, 1.0472, 1.13446, 1.22173, 1.309, 1.39626, 1.48353, 1.5708, 1.65806, 1.74533, 1.8326, 1.91986, 2.00713, 2.0944, 2.18166, 2.26893, 2.35619, 2.44346, 2.53073, 2.61799, 2.70526, 2.79253, 2.87979, 2.96706, 3.05433, 3.14159};
  std::vector<float> contents = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.507465, 0.0796012, 0.183024, 0.212938, 0.730266, 0.131144, 0.232972, 0.231465, -0.0574707, 0.133946, -0.844814, -0.0787219, -0.0152037, -0.0163551, 0.027989, 0.0308469, 0.0019693, -0.0149127, 0.0013724, -0.0195291, -0.0107795, -0.00870356, -0.0143934, 0.000882332, -0.136103, 0.00433799, -0.00352195, -0.0217966, -0.00320519, -0.0108043, -0.00943697, -0.00156682, -0.0122157, -0.0118559, 0.0103557, -0.00153315, -0.000658004, 0.00636435, -0.00746871, -0.00851731, -0.00256419, -0.0101753, 0.00703516, -0.00249335, 0.0021446, 0.000466199, -0.00481683, 0.00902269, 0.00208355, -0.00196148, 0.00127837, -0.0105149, -0.0234657, -0.100749, -0.028911, -0.0148835, -0.0168364, -0.000106435, -0.0111054, -0.0323909, -0.0118812, 0.0119817, 0.00458793, -0.0136839, -0.0176551, -0.0317254, -0.00311071, 0.0082967, -0.0537145, -0.940951, 0.173691, 0.0271618, 0.118512, 0.110611, -1.12147, -0.700733, -0.231008, -0.848315, 0.220186, 0.458185, -0.265471, -0.360494, 0, 0, 0.404848, -0.0104684, 0.684818, 0.23301, 0.150628, -0.0574562, 0.231239, 0.217279, 0.13974, -0.717883, 0.0779257, 0.109014, 0.230435, 0.00597394, 0.0151825, -0.00798514, -0.00523494, -0.00859097, -0.000101764, -0.00119164, -0.00539382, 0.0156037, 0.0209037, 0.0115576, -0.0263665, -0.0181522, -0.00317165, -0.0162885, -0.0149336, -0.024433, -0.0134773, -0.00662291, 0.0129602, -0.00818216, -0.000674648, 0.00541831, -0.00750815, -0.00468904, -0.00638552, -0.00747796, 0.00800984, 0.00706084, 0.013477, 0.00655266, -0.0162287, -0.00729501, 0.0015935, -0.00481111, -0.00979848, 0.00530281, -0.00681717, -0.020569, -0.00352835, -0.0429322, -0.0133893, -0.0124006, -0.0175662, -0.00868263, 0.000598477, -0.0146299, 0.00167058, 0.0192479, 0.005642, -0.00253209, -0.00339242, -0.0207422, -0.00348661, -0.0230884, -0.17085, -0.203951, -0.367511, -0.0109471, 0.0611342, 0.0597771, -0.638761, 0.405634, 0.127306, 0.477882, 0.0722736, 0.169819, -0.164424, -0.144408, 0, 0, -0.193409, 0.0704684, -0.905197, 0.0316707, -0.268832, 0.0887115, 0.523357, -0.0403159, 0.167245, 0.103278, 0.37897, 0.0876719, 0.0115831, 0.058714, 0.00258173, -0.0243898, 0.00191969, -0.011517, -0.0374223, -0.0169825, -0.00612449, -0.000925643, -0.00575197, -0.0337002, -0.00419126, -0.010567, 0.00601481, -0.0171573, -0.00961165, -0.0015392, -0.00126578, -0.00669872, 0.00470078, -0.00334335, -0.00989288, -0.00277547, -0.013744, -0.00626298, 0.00149204, 0.00993829, -0.00414957, -0.0238606, -0.0109437, -0.00469799, -0.0146127, -0.011803, -0.0103703, -0.00296007, 0.0115326, -0.00746594, -0.00389898, -0.0122945, -0.0138343, -0.0211843, -0.0127381, -0.0143903, -0.0179773, -0.00125161, 0.00654168, -0.00795082, -0.0143928, -0.00428543, 0.0213084, 0.00229586, 0.0137565, 0.0376298, 0.00897149, 0.0461807, -0.00340644, 0.0946458, -0.396783, 0.253587, 0.0853267, 0.30113, -0.515035, 0.487289, 0.227264, -0.0751977, -0.315966, 0.152529, -0.0480716, -0.138713, 0, 0, -0.2688, -0.654623, -0.23011, -0.0726812, 0.0934335, -0.368647, 0.232625, -0.320506, 0.206524, 0.120227, 0.0893648, -0.10617, -0.177776, 0.00797075, -0.0442123, -0.00599339, 0.00603135, -0.0176438, -0.0213556, -0.0132834, -0.0108779, -0.00391507, -0.0185094, -0.0414886, -0.0160773, 0.00678968, -0.00173394, 0.00165083, -0.0122033, -0.0108461, -0.00135812, 0.00193834, 0.0161502, 0.00706841, -0.00541672, 0.00226648, 0.000662838, -0.00387916, 0.00623334, 0.00895252, 0.0103183, 0.0128436, -0.00211187, -0.0047729, -0.00919391, -0.0182098, 0.00910894, -0.00247426, -0.000151817, -0.00980482, -0.017452, -0.0148396, -0.00732512, -0.00419328, -0.0230181, -0.00817394, -0.000553289, -0.0207177, -0.0216196, -0.039174, -0.000592323, -0.0167229, -0.0035481, 0.00287897, -0.0220521, 0.0157952, -0.0372135, -0.0218068, -0.00484392, -0.722917, -0.438572, 0.12319, 0.119938, -0.100626, 0.120016, -0.747964, 0.666395, -0.255783, 0.211562, -0.291922, -0.274441, -0.0240356, 0, 0, 0.105532, 0.388309, 0.0527036, -0.00572825, 0.372368, 0.0310079, -0.142079, -0.683721, -0.0129152, 0.131062, 0.0194483, -0.638714, 0.108353, 0.0217421, -0.00133042, 0.000229962, -0.00493446, 0.00862471, -0.00108092, -0.0162092, 0.000923624, -0.00421173, 0.0121794, -0.0231656, -0.0242077, -0.00506742, -0.0110009, -0.0013527, -5.16256e-05, -0.022936, -0.00201295, -0.00434465, 0.00145365, -0.0063126, -0.0142526, 0.000960517, -0.00537796, -0.0142161, -0.00150329, 0.00709749, 0.00366862, -0.000641899, -0.00349017, -0.00438784, -0.0114969, -0.0192256, 0.00674492, -0.00603516, -0.00824241, -0.0111463, -0.0241455, 0.0093198, -0.0130724, -0.000724664, -0.00479745, -0.0178786, 0.00249408, -0.0110747, -0.0596441, -0.000994173, -0.0053209, -0.00144051, -0.00937248, -0.0127653, 0.00279908, 0.0186407, -0.0102118, -0.00757754, -0.0642962, -0.152892, -0.0458228, 0.605385, 0.402215, 0.591594, 0.00759949, 0.0294184, 0.311682, 0.0249182, 0.255878, -0.402368, -0.240697, 0.465084, 0, 0, 0.456733, 0.371257, 0.181129, -0.348751, -0.492062, 0.279185, -0.310194, 0.245196, -0.00971937, -0.38973, -0.664234, 0.313976, 0.210575, 0.0463579, -0.00230211, -0.00815445, -0.00401876, -1.66109e-05, -0.00542834, -0.0108393, 0.00363981, 0.00618873, 0.00420081, 0.0191333, 8.70024e-05, -0.0489405, -0.0673538, -0.0136006, -0.0217874, -0.0341552, -0.00656403, -0.0118893, -0.0174082, -0.0145179, -0.0204429, -0.012476, -0.013048, -0.00779579, -0.0151059, -0.0217071, 0.00190326, -0.005365, -0.00145606, -0.00809699, -0.00720109, -0.0250863, -0.0372309, -0.0104194, -0.0157014, -0.0117378, -0.00677177, -0.0212321, -0.00845211, -0.0225443, -0.0168747, -0.00845916, -0.0170339, -0.0192602, 0.00722784, -0.0133231, -0.0105022, 0.00269707, 0.00522469, -0.025989, -0.00211084, 0.010628, -0.0179059, -0.0120644, 0.00980931, 0.325011, -0.0939149, 0.400783, -0.14431, -0.00894576, 0.103275, -0.190297, 0.203432, 0.281942, 0.0402529, 0.735564, -0.151162, -0.0750299, 0, 0, 0.0901673, 0.0726722, 0.19501, 0.172631, 0.50351, -0.205109, -0.00816477, 0.165634, -1.10114, 0.103787, 0.0917308, 0.182363, 0.169248, 0.0340761, -0.0117205, 0.0232691, 0.0132761, -0.0757986, -0.058835, -0.0180756, 0.00251094, -0.00613381, 0.00941034, -0.000863279, -0.00181801, -0.00504352, -0.00887719, -0.0167313, -0.00507549, -0.0119106, -0.0042828, -0.0131565, -0.00536848, -0.0180556, -0.00629445, -0.0023061, 0.00230139, 0.0015366, -0.0175102, -0.118522, -0.0244602, -0.00165338, -0.00783065, -0.0200815, -0.0114842, -0.00795278, -0.0241526, -0.0266009, -0.0187046, -0.0238406, -0.0292679, -0.0169372, -0.0293133, -0.0259649, 0.00302882, 0.0101774, -0.0413945, -0.072486, -0.0106455, -0.00211014, 0.00743852, 0.0229691, 0.016491, 0.0252052, -0.00356182, -0.0120923, 0.00221182, -0.0288034, 0.00386962, 0.14774, -0.22319, 0.0231582, 0.0921232, 0.0284628, -0.37662, 0.0747781, -0.777846, -0.0824901, 0.0339186, -0.000999132, 0.328868, -1.07322, 0, 0, 0.248837, -0.898874, -0.113178, -0.288112, -0.352743, 0.118058, -0.250011, 0.116239, -0.446066, 0.119006, 0.339942, 0.104172, 0.125118, 0.0134225, 0.0514595, 0.0216726, 0.0181226, -0.217771, -0.16766, -0.00426379, -0.00678534, -0.000138611, 0.00758755, 0.00188911, 0.00166715, -0.00561095, 0.00064784, 0.00839696, 0.0110279, 0.00276051, 0.00598958, -0.00159488, -0.0116647, -0.0247525, -0.00219241, 0.00204039, 0.0183151, 0.00926324, 0.00805677, 0.00737878, 0.00542505, 0.00415139, -0.00246142, -0.00248533, -0.0178295, -0.0147452, 6.72561e-05, -0.00385706, -0.00471262, -0.00613036, -0.00556882, 0.00159633, -0.013359, -0.00175565, -0.0114332, -0.0128465, 0.000846742, -0.0089984, 0.028267, -0.00227565, -0.00723246, -0.00549154, 0.0224517, 0.00741744, -0.00131762, -0.00379517, -0.00574153, 0.0310587, -0.0450525, 0.00102435, -0.0800447, 0.115267, 0.12083, 0.0712979, -0.023491, 0.525223, 0.191792, 0.259961, -0.79798, -0.136291, -0.174271, 0.269844, 0, 0, 0.584099, -0.0893128, 0.0170604, 0.286254, 0.569247, 0.146777, 0.119617, -0.491715, 0.189969, -0.448681, -0.145837, 0.165747, -0.112999, 0.0182769, 0.0153422, 0.0417382, 0.00120913, -0.0832189, -0.0888126, -0.00786404, 0.0258338, -0.00384909, -0.0124028, -0.0138164, 0.00411737, 0.0102018, 0.000700018, 0.000326264, 0.00203166, 0.014328, 0.00686572, -0.00300505, -0.0149385, -0.00606302, 0.00122414, 0.00533491, -0.00201323, 0.0145133, -0.00873432, 0.00464461, -0.00448346, 0.00701487, 0.000911633, -0.0247622, -0.0126293, -0.000462322, -0.00890812, -0.0122403, -0.0104576, -0.0101473, -0.000680515, -0.00863839, -0.00812902, -0.0246704, -0.00144535, -0.00887119, -0.00393555, 0.010759, 0.00728176, 0.00744885, 0.016402, 0.0314985, -0.0011376, -0.00446241, 0.00713654, 0.031985, -0.00153117, 0.0375063, -0.0541968, 0.0387885, 0.590376, 0.0611124, 0.056837, -0.0854827, 0.146747, 0.0615332, -0.0904183, 0.652132, 0.0905602, 0.120625, -0.253781, -0.242811, 0, 0, -0.423029, -0.398683, 0.182991, 0.32952, -0.01693, 0.778755, 0.27361, 0.148696, 0.16082, 0.0934281, 0.0631339, 0.0747339, 0.0678742, -0.0187473, 0.0154728, 0.0106189, 0.00830067, -0.0010942, -0.00857716, 0.00345042, -0.0281243, -0.0251142, 0.0212891, 0.00939189, 0.00577745, -0.00450644, 0.000662273, -0.0140219, -0.0181085, -0.00774129, -0.00315378, -0.0136244, -0.0380917, -0.0174782, -0.00330299, -0.00526791, -0.00610472, 0.00181315, -0.0118601, 0.00123035, -0.00609716, -0.0151161, -0.00962135, -0.0216655, -0.0135237, -0.0175273, -0.0282853, -0.0201313, -0.0251359, -0.031572, -0.0114983, -0.00450421, -0.0182685, -0.020007, -0.0134262, -0.00362053, -0.0120905, -0.00618415, -0.00199895, 0.00228962, -0.0072358, 0.0104823, 0.00394639, 0.00398799, 0.0198361, 0.0120439, -0.00746682, 0.0106609, -0.0107519, 0.18979, -0.385552, 0.00486197, 0.0955249, -0.240778, 0.37038, 0.530576, -0.0902613, -0.505238, -0.813943, 0.0672219, -0.153569, -0.155037, 0, 0, 0.452523, -0.00458618, 0.0152003, -0.0153162, -0.0796731, 0.349414, 0.307178, 0.176006, 0.171279, 0.127342, -0.625605, -0.264915, 0.0625971, 0.0168788, 0.00946198, 0.00349269, 0.0161956, -0.0391673, -0.0135792, 0.000959731, -0.0227746, -0.00635603, 0.000199195, -0.00708142, 0.0190234, -0.0290701, -0.00471766, -0.0152848, -0.00400813, -0.00512425, -0.00942244, -0.0266074, -0.0995338, -0.0474388, -0.0331773, -0.0108453, -0.00732286, -0.00302823, -0.00859253, -0.00307311, -0.00939811, -0.0256612, -0.00310166, -0.0166486, -0.00876744, -0.0281526, -0.0297663, -0.0325851, -0.0250883, -0.0368855, -0.024156, -0.00886939, -0.0138615, -0.0251246, -0.0153763, -0.0145473, -0.025285, -0.0156286, 0.0191443, 0.0103784, -0.0139047, 0.00588848, 0.0136644, -0.00990056, -0.00339406, 0.0244269, 0.0024923, -0.0184435, -0.0285005, -0.120123, 0.296093, -0.199939, -0.217366, 0.0859074, 0.150356, 0.0809544, 0.119162, 0.144831, 0.00439509, -0.106714, 0.0101154, -0.0372904, 0, 0, -0.0830443, 0.69578, 0.406326, -0.0338487, -1.11473, 0.0312861, 0.0605072, -0.671109, 0.202698, 0.148029, 0.12789, -0.0802132, 0.0556198, 0.0123526, 0.0336623, -0.0327231, -0.00485251, 0.00625317, 0.00152891, 8.52052e-05, -0.00758534, -0.0167253, -0.00863777, -0.0198229, 0.000216189, 0.0102661, -0.00257356, -0.0181715, -0.00408095, -0.00244318, 0.0117464, -0.0127218, -0.0370041, 0.001999, -0.0120475, -0.00391904, -0.00200466, -0.00136742, 0.00455679, 0.0120843, 0.0056123, -0.00811707, -0.0072579, -0.0127966, 0.00268827, -0.00778032, -0.00548835, -0.0172081, -0.0261136, -0.0128478, -0.0216188, -0.00489259, -0.00468361, -0.013713, -0.00803653, -0.0177185, 0.00221131, -0.00804633, -0.0355752, -0.0476015, -0.0127049, -0.0794839, -0.0215982, -0.00917764, -0.0037352, 0.00189165, -0.0130382, -0.0234516, -0.00674752, -0.0252974, 0.0598054, 0.140011, -0.579897, -0.412036, 0.997212, -0.493679, 0.239045, 0.00983235, -0.13999, 0.000266088, -0.109387, 0.310899, 0, 0, 0.112511, 0.972695, 0.12371, 0.01389, -0.380999, -0.729759, 0.170332, 0.240079, 0.200682, 0.437696, 0.115552, 0.346171, 0.422237, 0.0540096, 0.00373391, 0.00192172, 0.00209588, -0.00194842, -0.0139833, 0.00874938, 0.0192085, 0.00719754, -0.00667794, 0.0130758, -0.0123364, -0.00754074, -0.0259373, -0.0319638, -0.00329259, 0.00486902, 0.00885602, -0.00228109, -0.0120428, -0.0162009, -0.00183495, -0.0138153, 0.00479197, -0.00988451, -0.0042201, -0.00586009, -0.00373122, -0.00522287, -0.00551452, -0.00127634, -0.00757426, 0.00296333, -0.0102404, -0.0134089, -0.000949749, -0.0141249, -0.00613948, 0.00510292, -0.0118895, -0.0218675, -0.0166773, 0.00219765, -0.00225429, -0.000738212, -0.0296897, -0.0405418, -0.0324582, -0.199749, -0.0794256, 0.0122776, 0.00863389, -0.0103936, -0.0232436, -0.0282257, -0.057769, 0.0410363, -0.137771, 0.529227, -0.36285, 0.137257, 0.184641, -0.996701, -0.311512, 0.390202, 0.0228104, 0.019066, -0.0050971, 0.0546847, 0, 0, -0.456187, -0.156135, -0.840614, -0.256079, -0.315965, -0.470229, 0.0836215, -0.873065, 0.208779, 0.103943, 0.0649832, 0.309658, 0.065571, -0.0767791, 0.0161487, 0.0113887, -0.026364, -0.000928436, 0.0165488, -0.00704153, 0.00702222, 0.0105503, -0.00863292, -0.018571, -0.0228166, -0.0570282, -0.0389174, -0.0238494, -0.050862, -0.0316999, -0.0120556, -0.0217039, -0.0203752, -0.0210811, -0.0214721, -0.0434535, -0.0254311, -0.00474111, -0.0129031, -0.00145137, -0.0121821, -0.0111736, -0.02201, -0.017269, -0.0156415, -0.0189303, -0.00782446, -0.0176109, -0.012501, -0.0367858, -0.0302841, -0.0330414, -0.0184113, -0.0155653, -0.0191292, -0.0298249, -0.0277198, -0.0306485, -0.0291278, -0.0118204, -0.00280048, -0.0315281, -0.021078, -0.0120957, 0.00271056, -0.0101209, -0.0144147, -0.0126156, -0.0540657, 0.27779, -0.0924864, -0.402997, 0.0953008, -0.109196, 0.139463, -0.556564, -0.0462593, -0.460418, -0.150012, 0.262676, -0.0755589, -0.0286506, 0, 0, 0.141723, -0.464516, 0.217748, 0.462801, 0.804173, 0.135939, 0.388447, -0.600569, 0.234436, -1.12881, -0.257329, 0.143756, -0.633982, -0.0930873, -0.0370146, -0.00551938, -0.0281607, -0.0106104, -0.0281944, -0.0161915, -0.0328599, -0.0152007, -0.0105469, -0.0237199, -0.0244804, -0.0394805, -0.0287565, -0.0160339, -0.0339419, -0.0316736, -0.0137835, -0.0180637, -0.032522, -0.0289584, -0.0365962, -0.0133692, -0.00542766, -0.00836985, -0.0311413, -0.0141715, -0.0120714, -0.020151, -0.00818119, 0.00177177, -0.0144357, -0.00534632, -0.0174252, -0.0221479, -0.0210638, -0.0213739, -0.0102684, -0.0191013, -0.0146387, -0.0128986, -0.0213729, -0.0348688, -0.0545937, 0.0026122, -0.0212884, 0.0161595, 0.00358823, 0.0161107, 0.0183385, -0.00211863, 0.0258166, 0.00776422, -0.0224813, -0.022392, -0.0228126, -0.48118, -0.560569, 0.0334464, 0.0961975, 0.0874295, -0.996084, -0.651801, -1.00416, -0.336764, 0.180023, 0.0936661, -0.0248707, -0.163696, 0, 0, 0.158773, -0.961879, -0.0390384, 0.122011, 0.268303, 0.0882348, 0.0255704, -0.846111, -0.34745, 0.110535, -0.895851, 0.0572245, -0.332221, -0.0296099, -0.0517357, 0.000909608, -0.00989064, -0.0105084, -0.0159132, -0.0246403, -0.0113524, -0.0139094, -0.0181662, -0.026228, -0.0135233, -0.00785019, -0.00748394, -0.0098986, -0.035096, -0.0311628, -0.0434805, -0.0339281, -0.0140187, -0.0250465, -0.0287851, -0.0159931, -0.0119046, -0.00744726, -0.0216514, -0.00854652, 0.00681203, 9.61479e-05, 0.010665, -0.0035811, 0.00315475, -0.00034733, -0.0143868, -0.00728838, -0.0134464, -0.0115685, -0.0103948, 0.0145376, 0.00587499, 0.00445154, -0.00820417, -0.0026358, -0.00659971, 0.00286424, -0.0115629, 0.00855968, 0.00521098, 0.0057157, 0.017182, 0.00529393, 0.0115685, 0.00603839, 0.000267604, -0.0413702, -0.0556007, 0.0724915, 0.0268458, 0.0745947, 0.0952326, 0.0857576, 0.186836, 0.162012, -0.420824, 0.0684674, 0.0161761, -0.741894, 0.622015, -0.36239, 0, 0, -0.146219, -0.166172, -0.354392, 0.303608, 0.189742, 0.0466855, 0.180402, 0.202292, 0.176355, 0.0970095, 0.0555272, -0.111476, -0.311692, 0.00161617, -0.00512263, -0.0180595, 0.00860495, -0.0300931, -0.0318901, -0.0218078, 0.000550667, 0.00891109, 0.0105518, -0.00284521, -0.00954916, -0.0171936, -0.0174284, 0.00228638, -0.0282348, -0.0445389, -0.0543819, -0.121257, -0.0225991, -0.0199847, -0.0309234, -0.0280404, -0.0181595, -0.0194736, -0.0193032, -0.0133765, -0.0136791, 0.00969771, -0.00995556, 0.0185805, -0.0146668, -0.0050731, -0.00600675, -0.00370777, -0.00499578, -0.00643066, -0.0015086, 0.00909296, 0.0079026, -0.00842592, -0.00219962, -0.0108772, -0.00205748, 0.0256072, 0.0188153, -9.25175e-05, 0.00702072, 0.000367276, 0.0059808, 0.00174413, 0.0151386, -0.0178747, 0.000287906, -0.0112974, 0.0129834, -0.100854, 0.126291, 0.0131759, -0.17108, 0.0932396, 0.273501, -0.107578, 0.247053, -0.00510846, 0.289184, 0.327354, -0.0366151, 0.507169, 0, 0, 0.0803317, -0.00696958, -0.174965, -0.476357, -0.214226, 0.307341, 0.200282, 0.787724, 0.200016, 0.12031, 0.0795716, -0.703017, 0.356421, -0.00240331, -0.0375966, -0.0506823, -0.0396719, -0.0204859, 0.0142038, -0.0042861, -0.0162062, 0.000436767, 0.0307259, -0.00121523, -0.0365486, -0.0403762, -0.0327446, -0.0161005, -0.047031, -0.0455954, -0.0528378, -0.0507513, -0.0294126, -0.0374937, -0.0348449, -0.0109731, -0.0360245, -0.0307376, -0.0227681, -0.0148103, -0.00129274, -0.0166163, -0.00883225, -0.0157151, -0.0164311, -0.00621051, -0.013413, -0.00052852, 0.00956173, -0.00362234, -0.00882557, 0.00165093, 0.00171899, 0.00886903, -0.00916197, 0.00130197, -0.0129174, 0.013793, -0.00759866, -0.00911192, 0.001167, -0.0126347, -0.00122097, -0.0115093, -0.0100729, -0.00932753, 0.00518416, 0.000670483, 0.0107682, 0.084265, -0.634217, -0.438818, 0.245696, 0.110569, 0.633186, 0.135934, -0.829022, 0.513644, -0.289247, 0.0954459, 0.281259, -0.168951, 0, 0, 0.00514551, 0.132503, 0.659284, 0.171604, -0.136055, -0.00108891, 0.0818014, -0.620278, -0.406899, 0.347107, -0.255872, -0.456446, 0.052281, -0.0580267, 0.0124119, -0.0967266, -0.15061, -0.0324344, -0.00378576, -0.00197461, -0.0133632, -0.0332094, 0.0148319, 0.0193165, -0.0229573, -0.0416496, -0.0140691, -0.0109171, -0.0253488, -0.0284546, -0.0289514, -0.030758, -0.0100773, -0.0167162, -0.0173844, -0.00210656, -0.0236542, -0.0195648, -0.00405922, -0.00923267, -0.0047556, 0.00740323, -0.0295933, -0.0222147, 0.00253694, -0.00651687, -0.019383, -0.016825, -0.0124285, -0.00802827, 0.00292087, -0.00578489, -0.00872968, -0.00435918, -0.000906928, -0.00292248, 0.000897713, 0.0101938, -0.116783, -0.0826465, 0.0116447, -0.00507013, 0.00605234, -0.0157752, -0.0143706, -0.0101288, -0.0290676, -0.0272558, 0.020096, 0.113961, -0.00417635, -0.582818, 0.0685556, -0.624735, 0.271504, -0.333928, -0.0809417, 0.0914115, -0.067816, -0.419235, -0.167667, -0.198642, 0, 0, 0.370917, 0.0785737, 0.836152, -0.64647, -0.0109127, 0.0930428, 0.0732249, 0.264996, 0.273525, -0.391337, 0.338851, 0.337389, 0.109534, -0.0497563, -0.0415624, -0.0800899, -0.310753, -0.028612, -0.0106946, -0.0155469, -0.0234962, -0.000398707, 0.0142278, 0.02106, 0.00276023, -0.00837157, -0.0128841, -0.00558828, -0.0146592, -0.0114248, -0.0101688, -0.0122567, -0.00335733, -0.0139159, 0.00414563, -0.00354778, -0.00350952, -0.0123862, -0.00596071, -0.00904975, 0.017458, -0.00725792, 0.00670205, -0.0153595, -0.00467792, 0.0110967, 0.00419651, -0.000811158, -0.000342912, -0.00836901, 0.0128247, 0.0197976, 0.0123326, -0.00999616, -0.00306644, 0.011134, 0.0309249, 0.00203927, -0.027495, -0.0153754, -0.00104086, -0.0125684, -0.0624704, -0.037823, -0.0285824, 0.0153869, -0.0188078, -0.0492256, 0.0304512, -0.146889, 0.392693, 0.161104, -0.182165, 0.0805022, -0.918224, 0.0902025, 0.119236, 0.0628835, -0.375976, 0.0251463, 0.0537184, 0.285198, 0, 0, -0.193305, 0.250744, 0.182265, 0.459792, -0.126849, 0.289716, -0.68677, 0.240833, -0.799374, 0.145542, 0.312691, -0.801933, 0.272632, -0.0319654, -0.0150972, -0.0439594, -0.113475, 0.00697547, 0.00418733, 0.0128178, 0.0124139, -0.00298136, 0.0182205, 0.00138163, -0.00171794, -0.00171108, -0.0245267, -0.0200505, -0.0271908, -0.0127301, -0.017538, -0.0143791, -0.00705473, -0.0053046, -0.0110087, -0.00794837, -0.0177693, -0.00674731, -0.00848869, -0.00334325, 0.00669846, 0.000242402, 0.00683234, 0.00637166, -0.00251393, -0.00217307, 0.0123043, -0.0172421, -0.0017401, 0.0161409, 0.014109, 0.00921768, 0.0162485, 0.00412842, -0.00129061, -4.09592e-06, 0.00380778, -0.0287031, -0.00712416, 0.0198351, 0.0164041, -0.0325345, -0.190726, -0.0615292, -0.0340709, -0.034332, -0.000388609, 0.0318014, -0.0342544, -0.474295, 0.432683, -0.0927452, -0.463627, -0.215474, -0.0171606, -1.10262, 0.318428, -0.290793, 0.0993222, 0.238555, 0.0243645, -0.118378, 0, 0, 0.0582126, -0.677283, -0.0401264, 0.224911, 0.185033, 0.0645809, 0.153178, 0.674526, 0.00360232, 0.14202, 0.107262, 0.352631, -0.624968, -0.0937306, -0.0104478, -0.00260989, 0.00895039, 0.0166393, 0.025115, 0.0110771, -0.00747793, 0.0100261, 0.0106881, -0.0187887, -0.0231109, -0.0391486, -0.0401616, -0.0520729, -0.0542986, -0.0136061, -0.0339828, -0.0163564, -0.0209171, -0.0137007, -0.0111122, -0.0168658, -0.0151461, -0.0177419, -0.0194895, -0.0109267, -0.00645687, -0.0271427, -0.0110006, -0.0139873, 0.0103592, -0.00472768, -0.0121756, -0.0200751, -0.0233813, -0.0254527, -0.026002, -0.0032364, -0.0170319, -0.0109456, -0.00512843, -0.0134821, -0.0242582, -0.0824398, -0.0437587, 0.00314152, 0.0221427, 0.00181748, -0.0858541, -0.0319657, -0.0267471, -0.0111452, 0.00193025, -0.0301443, -0.0476312, -0.21421, -0.237852, 0.0538262, -0.217905, 0.115745, 0.197977, 0.41908, 0.109282, 0.0450541, -0.0801659, -0.151869, 0.810727, -0.148143, 0, 0, -0.149005, 0.0629851, -0.397976, 0.254674, 0.0623006, 0.0637515, 0.128198, -0.0435821, -0.852342, 0.109341, 0.439576, -0.0463116, -0.0912317, -0.368807, 0.0034688, 0.0127783, 0.0195323, 0.0179932, 0.016437, -0.0018744, 0.00864789, 0.0193761, -1.40438e-05, -0.011347, -0.00322869, -0.0232683, -0.0411421, -0.102459, -0.0240361, -0.00742991, 0.00345193, -0.00736629, 0.00013136, -0.00147853, 0.00550115, 0.00316263, -0.0219478, -0.0227429, -0.0261261, -0.0188284, -0.00128404, -0.00519411, -0.00653749, -0.0131733, -0.00296641, -0.0210122, -0.0117311, -0.0352784, -0.0245956, -0.0219934, -0.0306295, -0.0931533, -0.0184347, -0.0157466, -0.0333768, -0.023154, -0.0345488, -0.0352428, 0.00657606, 0.011029, 0.0163309, 0.0159192, 0.00633834, 0.0125775, 0.0173415, 0.0070476, -0.0117764, -0.0233222, -0.0419659, 0.284021, 0.372604, 0.0354918, 0.111692, 0.111916, 0.170642, 0.0434002, -1.16837, -1.43394, 0.0760999, 0.0352572, -0.238201, 0.201932, 0, 0, 0.349381, 0.548167, -0.103777, 0.128282, 0.0258535, 0.311805, -0.772536, 0.34863, 0.234626, 0.584404, -0.00086918, -0.509356, -0.169556, -0.0212644, -0.0293681, 0.0101518, 0.00587397, 0.00484255, 0.00228245, -0.00255161, -0.020548, 0.00783727, 0.00514817, -0.000889175, -0.0215392, 0.00397434, 9.68743e-05, -0.0147105, 0.00876086, 0.0111152, -0.00127382, 0.0128368, 0.00777634, 0.0185357, -0.0115278, -0.00946887, -0.0031127, 0.00565104, 0.00366178, 0.0216641, 0.00155357, 0.010362, 0.00323724, -0.000550742, -0.0175364, -0.0145084, 0.00355708, -0.0153386, -0.0102002, -0.00289498, -0.0402318, -0.120294, -0.0149911, -0.00358008, -0.01566, -0.018867, 0.0203342, -0.020639, 0.0189313, 0.0246426, 0.00607482, 0.0370291, 0.0167633, -0.0104402, 0.0125157, 0.020922, 0.00199972, 0.0307752, -0.0287277, 0.141036, 0.0904644, 0.0608674, -0.288506, 0.112161, 0.241235, -0.268708, -0.754536, -0.0551649, -0.0571079, 0.411868, 0.101936, 0.317265, 0, 0, 0.324524, 0.0512216, -0.869054, 0.26189, -0.0221858, -0.664671, 0.129146, 0.304984, 0.492825, -0.961726, 0.148178, 0.240189, 0.139214, -0.000599905, 0.0045413, 0.00243284, 0.00212942, 0.00959453, 0.0237846, -0.0137227, 0.0051206, 0.00756527, 0.000585311, 0.00649624, -0.009288, 0.0103225, 0.00764184, 0.0243625, 0.0141825, 0.0180862, 0.0207928, 0.00140987, -0.000722324, 0.00904813, 0.00461299, -0.0117866, 0.000642397, 0.0132322, -0.00217985, 0.00149148, 0.0041618, 0.00022058, -0.00986597, 0.00894904, -0.00215176, 0.000863158, -0.0082965, 0.00318768, -0.0251415, -0.00907946, -0.00195626, -0.0309595, -0.016943, -0.0210426, -0.00690423, -0.0228826, -0.00383571, -0.00295506, -0.0245962, -0.000888695, -0.00871698, 0.00164479, 0.000584116, -0.0166484, 0.0159204, -0.0434675, -0.0439792, 0.0106312, 0.000553152, -0.235167, 0.0592218, -0.107785, 0.646729, 0.196413, 0.110825, 0.286591, -0.291138, 0.274011, 0.820889, -0.915517, 0.0161361, 0.220222, 0, 0, -0.527789, -0.375963, -0.597302, 0.341062, 0.4012, 0.0933097, 0.618139, 0.405443, 0.172557, 0.112695, 0.300922, -0.00229678, -0.534867, -0.0340594, -0.0109741, 0.0114472, 0.00748194, 0.0386859, 0.01062, 0.0132854, 0.0400761, -0.0125574, 0.00344284, -0.00427309, -0.0108575, -0.0166886, 0.0128046, 0.0243269, 0.00656951, 0.00957826, -0.00283338, -0.0149134, -0.00174615, 0.000658239, -0.00895074, -0.00980118, -0.00937855, 0.00025507, 0.000187167, -0.00263607, 0.000998072, -0.00313664, 0.0066013, -0.00662865, 1.32108e-05, -0.00413439, -0.00117306, -0.0136484, -0.00287215, -0.00695207, -0.013067, 0.002096, -0.0127748, -0.00911067, -0.00359717, 0.0048019, -0.0209428, 0.00377006, 0.0103172, 0.0143653, 0.0171553, 0.0206745, 0.00430476, -0.00803953, 0.00559794, -0.0229309, 0.00166087, 0.0105747, 0.0449984, -0.825739, 0.0397767, 0.0644896, 0.489336, 0.0972857, 0.0320448, 0.342261, -0.0269036, 0.407357, -0.207035, 0.264746, -0.00989524, -0.171035, 0, 0, -0.252243, -0.174639, 0.209388, -0.436956, 0.0872885, -0.0128231, 0.547446, 0.0577526, 0.259665, 0.125562, 0.0752567, 0.746471, 0.0640558, 0.0195796, -0.0140465, -0.00389458, -0.0255465, 0.0164654, 0.0185107, 0.00246608, -0.0236871, -0.00602808, -0.0100688, -0.0184988, 0.00102929, -0.00940506, -0.0128097, 0.0180555, 0.00637732, -0.0130332, -0.00496961, 0.000218873, -0.0478884, -0.00830851, 0.000420491, -0.00295763, -0.0132305, -0.00841929, -0.0106486, -0.00893697, -0.000514895, 0.0185993, -0.000708002, 0.00230009, 0.006782, 0.0104328, -0.0115878, -0.0189136, -0.00492618, -0.00975611, -0.00149994, 0.0122467, -0.00432573, -0.0154742, 0.000789921, 0.0106098, -0.00951343, 0.0104115, 0.0133421, -0.000252981, 0.0118284, 0.0209793, 0.00616454, 0.0208508, 0.00889434, 0.0118881, 0.0231635, 0.0456518, 0.00278998, 0.117641, 0.437943, 0.0580519, 0.113652, 0.0756894, 0.135036, 0.477097, 0.817835, -0.280069, 0.151146, -0.300452, -0.33053, 0.458084, 0, 0, 0.258049, -0.10996, 0.38686, -0.0903657, -0.251228, 0.0159384, 0.198088, 0.229269, 0.190125, -0.109232, 0.121182, 0.0921239, -0.639756, 0.00831158, 0.00638323, -0.0239496, 0.0187472, 0.0228243, 0.0169053, 0.0266967, 0.0226034, 0.00997046, -0.0031701, -0.0142202, -0.0138747, -0.00277689, -0.0375318, -0.01098, -0.00136105, -0.00317379, -0.0204811, -0.035712, -0.126027, -0.0172293, -0.0148488, -0.00279888, 0.00432752, -0.00589616, 7.86803e-05, 0.00331925, 0.00950947, -0.000226324, -0.00256936, -0.00796588, 0.00130853, 0.000577324, 0.00359349, -0.00237721, -0.00901151, 0.0151549, 0.0100259, 0.021811, 0.0099344, 0.0203133, 0.0136045, 0.0161937, 0.0265552, 0.0280519, 0.0126925, -0.00725133, 0.0143286, 0.011465, 0.0284734, 0.0161123, 0.0195033, 0.0154779, 0.0281941, -4.69488e-05, -0.00793454, -0.20898, -0.289245, 0.0286111, 0.724074, 0.134825, 0.164264, 0.107051, 0.269104, -0.0281937, 0.0977092, -0.083442, -0.0595257, 0.308331, 0, 0, 0.327923, 0.0019805, -0.720545, -0.414863, 0.283291, 0.100402, 0.0112374, -0.279413, 0.454741, 0.441493, 0.300302, 0.143606, 0.00878702, 0.0198656, -0.00206299, 0.0312613, 0.0355557, 0.00307648, 0.0240953, 0.0427668, -0.00514442, 0.0223993, 0.00669511, 0.0220188, 0.00692376, 0.00555753, -0.00677271, -0.00396415, -0.0126264, -0.0113563, -0.00650729, -0.0231885, -0.0398175, -0.0206522, -0.0153313, -0.00686825, -0.00603441, -0.010141, -0.0138921, 0.00518041, -0.00367169, 0.0126457, -0.00202759, 0.0156966, -0.000479483, 0.00186913, -0.00309882, 0.00839385, 0.011362, 0.00925659, 0.019363, 0.0302874, 0.0183295, -0.000148583, 0.00832763, 0.00131283, 0.0154791, 0.0364143, 0.0242529, -0.0168736, -0.090992, -0.0502458, -0.000628408, 0.00255429, 0.0132123, 0.0298362, 0.0264994, 0.00237816, -0.0247489, 0.0261069, 0.136411, 0.0591942, 0.149884, 0.14917, 0.838332, 0.108652, -0.139244, 0.22347, 0.237504, 0.359542, -0.262469, 0.184822, 0, 0, -0.179198, -0.242927, 0.0332168, -0.705803, -0.140064, -0.0325254, 0.0423449, 0.198226, 0.180125, 0.124127, -0.772541, 0.0578202, 0.122396, -0.0107833, -0.00764908, 0.0148617, 0.03629, 0.0157593, 0.0326257, 0.00257147, 0.0219593, 0.00358422, -0.000342333, 0.0253596, 0.0213976, -0.0201152, -0.0116306, -0.0151375, -0.00436474, -0.0200572, -0.0181772, -0.0146341, -0.0274535, -0.0182532, -0.0338307, -0.0202529, -0.0158803, -0.0255223, -0.021012, -0.015451, -0.000681831, -0.0070789, 0.00638249, -0.0168642, -0.0010748, 0.00687209, -0.0132873, -0.0126722, 0.00695684, 0.0034193, -0.0114533, -0.00508856, -0.00202927, -0.0160157, -0.0135136, 0.00111996, -0.0147816, 0.0412073, 0.0185534, -0.00645421, -0.141498, -0.09473, 0.00770746, 0.0133175, 0.0246089, -0.0253598, 0.0178559, 0.0350896, 0.00675551, 0.0495666, -0.126618, 0.238742, 0.128471, -0.151196, 0.187027, 0.116789, 0.816726, 0.423063, 0.150325, 0.329924, 0.0686176, -0.12871, 0, 0, -0.790043, -0.0436484, -0.0231587, -0.0751625, 0.0669419, 0.32547, -0.568876, 0.102374, 0.506941, 0.099192, -0.224013, 0.110274, 0.272492, -0.00691804, 0.0171648, 0.0320725, 0.0023312, -0.00637585, 0.0107958, 0.00165103, -0.0135735, -0.0146549, -0.0232639, 0.0293095, 0.0158174, 0.00371131, 0.00534691, -0.00469093, 0.00956895, -0.0314848, -0.0263641, -0.00714211, -0.0082658, -0.00376917, -0.0140983, -0.0101231, -0.0178753, -0.0098988, -0.014898, -0.00312451, 0.00497636, 0.0107372, -0.00983192, -0.0114541, -0.00120772, -0.0108194, -0.00145058, -0.0129609, -0.00246547, -0.00817049, -0.00847834, -0.00345153, -0.00828269, 0.000182285, -0.0228914, -0.1553, -0.0555437, 0.0146054, 0.0371486, -0.00753543, -0.015263, 0.0104745, 0.0130016, 0.00193484, -0.00600557, -0.0187217, 0.00961597, 0.0246148, 0.00485413, 0.0838111, 0.261641, 0.0512976, 0.284092, -0.0515052, 0.16092, 0.221995, 0.122022, 0.50957, 0.354616, 0.278882, 0.00911812, 0.174113, 0, 0, -0.142608, -0.473973, 0.260076, 0.577445, 0.158002, -0.789751, 0.0630608, -0.907179, 0.180801, 0.185225, -0.0395476, -0.255478, 0.116641, 0.0482321, 0.0216924, -0.0179895, 0.00789975, 0.0233117, 0.00569581, -0.010834, -0.00225424, -0.0381713, -0.00165343, -0.00979405, -0.0107035, 0.015717, 0.012068, 0.00107162, -0.0189066, -0.0441, 0.00493853, 0.00711677, 0.00497068, 0.0100147, 0.000421968, -0.00488272, 0.0114384, -0.00550405, -0.0187456, -0.00613836, 0.0071046, 0.0186116, 0.0100106, 0.00972027, 0.0105869, 0.0104593, 0.00653103, 0.0182948, 0.0089735, -0.00334512, 0.00338925, 0.0177427, 0.00371169, 0.0143907, -0.00365643, -0.0471015, 0.00765304, 0.00612988, -0.00318734, 0.016099, 0.0219401, 0.0299003, 0.0372284, 0.00327138, 0.000739901, -0.0139928, -0.00336626, -0.00904173, -0.0113904, 0.111034, 0.34717, 0.073593, -0.0837113, 0.127281, 0.211062, -0.540709, -0.953802, -0.922821, 0.231688, -0.0659497, -0.0616399, 0.216807, 0, 0, -0.231899, 0.120287, 0.342809, 0.184051, 0.216721, 0.115876, 0.280319, 0.0585534, -0.25716, 0.133406, 0.0898762, 0.133465, -0.164925, -0.0260477, 0.0319221, -0.00924896, 0.0327439, 0.0081259, 0.00812514, 0.00906624, 0.0130866, -0.00820219, -0.00254268, -0.00953427, 0.0379791, 0.0121408, 0.00636334, 0.00891732, 0.00574043, -0.00391695, -0.00292488, 0.00310478, 0.000651205, -0.00833666, -0.015844, -0.00842067, 0.00728017, -0.0079611, -0.00973909, 0.00680231, 0.0178134, 0.0121381, 0.0220592, 0.012776, 0.00615364, 0.0100241, 0.017079, 0.0347446, 0.0111052, 0.00151809, 0.0304804, 0.0183354, 0.0177766, 0.0309215, -0.00688228, 0.011111, 0.0183224, -0.00361279, -0.0269019, -0.0174767, 0.0183237, 0.0167074, 0.010809, -0.0105129, 0.0101515, -0.00078698, -0.0134195, -0.0142643, 0.00937275, 0.202146, -0.109196, 0.184368, 0.113088, -0.917951, 0.16163, 0.141977, -0.282488, 0.415931, 0.117747, -0.133912, 0.00653495, 0.284463, 0, 0, -0.423232, -0.0355822, -0.832336, -0.0898858, -0.303356, -0.637845, 0.0824857, 0.206523, 0.305548, 0.090308, -0.281191, 0.582686, 0.127617, -0.0376906, 0.0141936, 0.0259807, 0.0181768, 0.0302547, 0.0249619, 0.026655, 0.0306426, 0.00058798, 0.0240652, 0.0112282, 0.0400513, -0.00307203, 0.0172581, 0.00122126, -0.00833604, -0.0183584, -0.00655043, -0.00969775, 0.00142729, -0.000596852, -0.0149771, 0.00778961, 0.00291816, -0.00413354, -0.00684945, -0.00239971, 0.0116719, -0.00677096, 0.0120588, 0.00265392, 0.0015896, 0.00417345, 0.0123648, -4.7221e-05, 0.00175273, -0.00732251, 0.0121317, 0.0219949, 0.00346021, -0.00578448, -0.000940132, 0.0181176, -0.0270212, -0.10629, -0.0111753, 0.0118945, 0.0108137, 0.0330215, 0.0175839, 0.00878999, 0.0060732, 0.0160803, 0.0129071, 0.0397111, 0.0391812, 0.103442, 0.294124, -0.383127, -0.16153, 0.106085, 0.171934, -0.343576, -0.0565759, 0.270827, 0.099639, 0.0419931, -0.120815, -0.139356, 0, 0, -0.166841, 0.119768, 0.127523, 0.0320555, 0.0690333, -0.130769, -0.662898, -0.684058, 0.261299, 0.0836482, -0.119971, -0.497337, -0.203136, -0.0408619, -0.043792, 0.00510939, 0.0106793, -0.00106937, -0.011294, 0.0089281, 0.00542222, -0.0102662, 0.0136485, 0.0316403, 0.0317528, -0.0130228, 0.00212269, 0.00568606, 0.00209619, -0.00507779, -0.00377003, -0.00192216, 0.0111339, -0.00749373, 0.00941502, 0.00321006, 0.0150422, 0.00575449, 0.0148411, 0.0198468, 0.00303947, 0.00410847, -0.000899428, -0.00325632, -0.00107206, -0.00042739, -0.00643614, 0.000664354, 0.0146403, 0.000602607, 0.00645572, -0.00208926, 0.0170201, 0.00759848, -0.00457699, -0.0206398, -0.0197201, -0.00925335, 0.0537666, 0.0246116, 0.0246569, 0.0198384, 0.0190228, 0.00608674, 0.0042647, 0.00113791, -0.0171354, -0.000500687, 0.0178424, 0.341586, 0.035708, 0.510584, -0.164521, 0.0788744, 0.0975233, 0.586211, 0.182747, -1.28246, -0.295163, 0.440089, -0.126817, -0.184434, 0, 0, 0.396314, 0.352882, -0.804874, -0.216864, 0.0393822, 0.381717, 0.358005, -0.236743, 0.328749, -0.0985118, 0.12298, 0.240271, 0.0765664, -0.144965, -0.0140264, 0.00418598, -0.00517303, -0.0200829, 0.0051615, 0.0123298, -0.00586269, 0.0107822, 0.00751573, 0.0350194, 0.0352082, 0.042063, 0.00963596, 0.0145619, 0.00887474, 0.0166, 0.0214076, 0.021857, 0.0083676, 0.0277618, 0.0127435, 0.0178593, 0.0249379, 0.0223563, 0.0132848, 0.0190327, 0.00609476, 0.00383254, 0.00787664, -0.00724325, -0.000649987, -0.00420661, -0.00151822, 0.0188173, 0.0231722, 0.0282127, 0.0177031, 0.0104196, 0.0349742, 0.0168366, 0.00645567, 0.0187349, 0.0251009, 0.0233726, 0.0575355, 0.0268531, 0.0216276, -0.00510103, -0.00207521, 0.00664184, -0.0176575, -0.00446639, 0.00662781, -0.0169965, 0.0275475, 0.0815498, 0.342711, 0.0975815, -0.334484, 0.502057, 0.183947, 0.06581, 0.287413, -1.10843, -0.26191, 0.125567, -0.0632572, 0.132364, 0, 0, 0.312927, 0.552231, 0.457025, 0.331797, -0.282747, 0.166433, 0.128903, 0.243763, 0.208316, -0.181764, 0.235701, 0.144413, 0.0985668, -0.000253293, 0.0337894, -0.00558321, 0.0137007, -0.02045, 0.00214084, 0.00277311, 0.0202637, 0.0089942, 0.00752768, 0.0204006, 0.0330107, 0.0067269, 0.0217456, 0.0236499, 0.0120134, 0.00640936, 0.0167892, 0.0109196, 0.0132521, 0.011487, 0.0116916, 0.0098066, 0.0152824, 0.00426325, 0.00217087, 0.0071026, 0.00248377, 0.00218696, 0.00237551, 0.0163653, 0.0145026, 0.0192084, 0.00987485, 0.0191698, 0.00375276, 0.017005, 0.0300613, 0.045237, 0.0215641, 0.0128345, 0.0116372, 0.0226387, 0.0276963, 0.0456544, 0.0288245, 0.0342047, 0.0266019, 0.0102897, 0.00621582, 0.0223135, -0.0303064, 0.0194755, 0.0635817, -0.0198292, -0.00442743, -0.0677945, 0.141827, -0.519744, -0.50258, 0.126315, 0.206849, 0.163779, 0.265126, 0.189183, 0.0804783, -0.440753, 0.447448, 0.43253, 0, 0, 0.308296, -0.261571, 0.808024, 0.537938, 0.135697, 0.0494239, 0.0622563, 0.211061, 0.393671, 0.064731, -0.418943, 0.164936, -0.0858327, -0.0147029, 0.0223045, 0.0420405, 0.0122554, 0.0018962, 0.00233892, -0.00149495, 0.00471752, -0.00582272, 0.0114326, 0.0444885, 0.025622, 0.000346738, 0.0124722, 0.00615882, 0.0107076, -0.00202907, 0.00824047, 0.00477563, 0.00366422, 0.00677242, 0.004476, 0.0143418, -0.00350194, -0.0550737, -0.0976658, -0.0108059, -0.00752023, -0.0101487, 0.0226558, -0.000961889, 0.00810752, -0.00335478, 0.0209075, 0.00709618, 0.00189043, 0.00766768, 0.00494761, 0.0138593, 0.0124195, 0.0324555, 0.0146457, 0.00660776, 0.00543295, 0.0344669, 0.0368166, 0.0143329, -0.00603963, 0.00905754, 0.011312, -0.0137743, 0.00662024, -0.0138763, -0.000915638, 0.0138412, -0.0583015, -0.233798, 0.0795112, -0.0291741, 0.101157, -0.590659, 0.127197, 0.121784, -0.743093, 0.541947, -0.12673, 0.0641946, -0.105551, -0.0729321, 0, 0, 0.119332, 0.0258488, 0.205647, -0.193384, -0.0373294, 0.191259, -0.282976, 0.614752, -0.0677257, 0.133102, 0.575796, 0.0409288, -0.147464, 0.0134517, 0.00544352, 0.00880358, -0.0039089, 0.0254666, 0.0155534, 0.00510006, 0.00725125, -0.0198478, 0.000562712, -0.0151066, 0.000600463, 0.0112459, 0.00278443, 0.0129349, 0.0339065, 0.0114972, 0.00482203, 0.0132393, 0.0115146, 0.00262103, 0.00263984, -0.00580972, -0.0412845, -0.136654, -0.0263737, -0.00493225, 0.00885548, -0.00654245, 0.007128, -0.000564064, 0.0129321, -0.0112862, 0.00647276, 0.0116801, 0.003182, -0.0195492, -0.00837143, 0.0336604, 0.0159754, 0.0165559, 0.000509921, 0.0203692, -0.0151184, -0.00131489, 0.00643072, 0.011508, 0.0224052, 0.00234579, 0.016934, 0.0160728, 0.00801424, 0.0141489, -0.00322525, 0.0335973, 0.0227905, 0.0249154, 0.307633, -0.164854, 0.125346, 0.0877266, 0.908868, 0.316647, 0.147388, 0.244137, 0.308932, 0.421985, 0.419304, 0.361663, 0, 0, 0.145022, 0.0843777, 0.143698, 0.380051, 0.0460275, -0.866631, -0.816386, 0.257609, 0.220702, -0.732604, -0.749459, -0.931204, 0.0723077, 0.17377, -0.0112917, 0.00466412, 0.00201869, -0.0148108, -0.00995847, 0.00995571, 0.00244292, -0.00131839, -0.0121378, -0.0530479, 0.0164675, 0.0349169, 0.0366642, 0.0200382, 0.0205194, 0.0237291, 0.0410059, 0.0108858, 0.0102547, 0.0261026, 0.0154796, 0.01906, 0.00560915, -0.010388, 0.0167971, 0.022731, 0.00381355, -0.0107504, 0.00635017, 0.0113218, 0.0158179, 0.00293774, 0.00224373, 0.0118725, 0.0182059, 0.0160465, 0.00318743, 0.00647251, 0.010512, 0.00015965, 0.00271909, 0.00529789, 0.0141591, 0.0240249, -0.00722703, -0.00197782, 0.0224268, 0.0151718, 0.0159411, 0.0155714, 0.00591535, 0.0120488, 0.00585294, -0.00692558, 0.0314885, 0.303762, 0.288759, 0.0510943, 0.0359337, 0.137601, 0.530536, 0.758209, -0.888046, -0.380721, 0.0419405, 0.295098, -0.11401, 0.0312169, 0, 0, -0.195574, 0.170903, -0.489333, -0.0007724, -0.627435, 0.137721, 0.0874363, 0.269864, -0.608853, 0.1595, 0.00891245, 0.248017, 0.0136354, -0.0642345, -0.000432224, 0.0183073, 0.0180969, -0.00869057, 0.0155822, 0.0199923, 0.00962349, 0.000605561, 0.00337559, -0.00907364, -0.00128699, 0.0306051, 0.0158025, 0.0262591, 0.0262151, 0.0186505, 0.0183444, 0.0177188, 0.0260244, 0.0325355, 0.0143112, 0.0343368, 0.0106025, 0.0245445, 0.0307265, 0.0110393, 0.00397828, 0.010466, 0.00790279, 0.0185927, 0.00755156, 0.0186703, 0.0100034, 0.00764798, 0.0174677, 0.0193069, 0.00900468, 0.0135195, 0.0243092, 0.00592569, 0.0145046, 0.0104082, 0.039877, 0.00155149, -0.0202454, -0.00378919, 0.000499213, 0.00655144, 0.0113942, 0.0148824, -0.0100762, 0.00952784, -0.00360318, 0.013145, 0.0596471, 0.119787, -0.103966, -0.112682, 0.564031, 0.371698, -0.776339, 0.146128, 0.891279, 0.261403, -0.0556854, 0.0229187, -0.564731, -0.793658, 0, 0, 0.0805303, 0.474524, -0.514368, -0.914677, 0.377261, -0.137858, -0.107688, 0.0557272, 0.227152, 0.153803, 0.145261, -0.129111, 0.108924, 0.0346285, 0.00342663, -0.000274127, 0.0104689, 0.0320726, 0.0350468, 0.0186923, 0.00646141, 0.0170711, 0.017801, 0.00342161, 0.000615368, 6.90743e-05, 0.0213554, 0.0160914, 0.0132644, 0.00871429, 0.0181833, 0.0180404, 0.0274605, -0.00144676, 0.0205441, 0.0145818, 0.0121387, 0.0305136, 0.0106599, -0.000524844, 0.0102672, -0.000650982, 0.0037732, 0.00545955, 0.0109494, 0.0106008, -0.0100528, 0.00171471, 0.00288972, 0.00969334, 0.0190599, 0.0173427, 0.00950468, -0.00230879, 0.0122167, 0.0277691, 0.00784667, 0.028366, 0.000978042, 0.0132093, -0.00595368, 0.0204037, -0.000308575, 0.0072004, 0.00773115, 0.0218236, -0.000774972, -0.00381911, 0.0449146, 0.301945, -0.1146, 0.0561036, 0.360099, 0.141615, 0.122107, -0.308441, 0.229457, -0.429957, 0.250635, -0.446469, -0.105886, 0.045012, 0, 0, -0.702147, -0.552685, -0.0476603, -0.247463, 0.385857, -0.258473, 0.492528, 0.236909, -1.01504, 0.341306, 0.451742, 0.135897, 0.355751, 0.0208248, 0.0225391, -0.00631026, 0.0442304, -0.010116, 0.0129288, -0.023825, -0.00911755, -0.0121819, -0.0732196, -0.057863, 0.00942084, 0.0151085, 0.00809672, 0.0176158, 0.0175752, 0.0340834, 0.0244438, 0.0154995, 0.0301292, 0.0191577, 0.00624873, 0.0265766, 0.00427185, 0.0214081, 0.0211791, 0.00356188, -0.00812415, -0.0148526, -0.0101725, 0.00766837, -0.00289704, -0.000248814, 0.00501333, 0.00233655, 0.00362184, -0.00445276, 0.00687201, -0.0149696, -0.0271231, 0.0188288, -0.0106079, 0.0142266, -0.0048516, 0.0172048, 0.0280456, 0.0101162, 0.0150063, 0.0257783, 0.0196345, 0.00566553, 0.0214078, 0.000758709, 0.0291914, 0.0277476, 0.0269809, 0.332162, -0.447149, 0.0351684, 0.12037, 0.0587313, 0.231384, -0.550643, 0.380497, -0.213039, -0.294405, -0.221807, -0.462884, -0.160887, 0, 0, 0.352861, -0.0177214, 0.826166, -0.209085, -0.320636, 0.207346, -0.113187, 0.178333, -0.30614, 0.148509, 0.167544, -0.122687, 0.0664346, 0.00830336, -0.000177063, 0.00878489, 0.00485298, 0.00351259, 0.0111435, -0.0132331, -0.00700551, -0.0191985, -0.123005, -0.0831379, 0.00345694, 0.0157416, 0.0233914, 0.0266269, 0.0327848, 0.0208024, 0.0273495, 0.0259684, 0.0265568, 0.0300798, 0.0175194, 0.0223294, 0.0295316, 0.0230221, 0.00114046, -0.023591, 0.0128242, 0.0106171, 0.00810288, 0.00894293, 0.0034591, -0.000348431, -0.00625996, -0.00544833, -0.0130855, 0.0148401, 0.00891006, 0.00250449, -0.132251, -0.0174491, 0.0199907, 0.0241792, 0.0388134, 0.0217316, 0.016451, 0.02707, -0.00136912, 0.000407884, -0.00155862, 0.0212819, 0.0161212, -0.00700427, -0.0175543, 0.00712348, -0.0651838, -0.0157805, -0.290724, 0.136797, 0.138653, 0.131845, -0.0112648, 0.141139, -0.0355231, 0.360894, 0.471183, 0.201921, 0.369957, 0.0159194, 0, 0, 0.14227, -0.542195, 0.529014, -0.0150997, -0.0266598, 0.408821, 0.24345, -0.501852, 0.23043, 0.233686, 0.0087078, 0.60591, -0.33832, 0.0194164, -0.0482456, 0.00683775, 0.0335082, 0.0317978, 0.00944204, 0.00806831, 0.0144777, 0.0426317, -0.00669975, -0.000745386, 0.00485706, 0.0298259, 0.00286774, 0.032206, 0.0138541, 0.0286037, 0.0289788, 0.0246753, 0.0322342, 0.00858591, 0.0230759, 0.0190396, 0.0164627, 0.0346309, -0.020969, -0.108986, -0.0115992, -0.00216174, 0.0187257, -0.00367979, 0.00645574, 0.02059, 0.0137362, 0.0114531, -0.000338723, -9.06518e-05, 0.0107523, 0.0260175, -0.0136344, 0.0125533, 0.013751, 0.00488135, 0.0188861, 0.018631, 0.00677977, -0.00786771, -0.0661716, -0.118971, -0.00981952, -0.00771333, 0.00671516, 0.00962752, 0.00170629, 0.0433712, 0.0097494, 0.752341, 0.537573, -0.539717, -0.155735, -0.674348, 0.20786, 0.233364, 0.252056, -0.0274965, 0.344996, -0.0659735, 0.513134, 0.580377, 0, 0, -0.239654, -0.0758966, 0.266596, 0.289596, -1.24218, -0.5426, 0.278001, 0.193215, 0.216115, 0.119506, 0.108203, 0.119015, 0.165553, 0.0131031, 0.0239489, 0.0178622, 0.0354085, 0.0246704, 0.0225036, 0.00666471, 0.0113176, 0.0073618, 0.0147784, 0.0134891, 0.000146234, 0.0154446, 0.0236002, 0.0305504, 0.0342673, 0.0348545, 0.0385092, 0.0373301, 0.0201641, 0.0125271, 0.0295687, 0.00918832, 0.0125794, 0.0075003, -0.00162665, -0.0122077, -0.0173036, -0.00188784, -0.00997207, -0.00400357, -0.00537845, 0.0116782, 0.0124694, 0.000789181, 0.0139885, 0.0167844, 0.00911192, 0.0396211, 0.0052568, -0.00263833, 0.00162728, 0.0249571, 0.00617718, 0.00298785, 0.0169685, 0.00632317, -0.064717, -0.0864975, -0.00827016, 0.000964355, 0.00614817, -0.00862281, 0.0229462, 0.0228515, 0.0195085, -0.00516469, 0.27211, 0.409192, 0.219127, 0.146134, -0.338684, 0.244516, 0.0311012, 0.100021, 0.451678, 0.0883936, -0.15353, -0.173334, 0, 0, -0.290443, 0.116101, 0.138695, 0.208643, -0.0378117, 0.101792, 0.072878, 0.197939, -0.0715397, 0.124935, 0.111316, -0.140974, 0.156958, 0.033123, -0.00766518, 0.0283144, 0.00480889, 0.0229772, 0.00747709, 0.00654937, -0.000868612, 0.0141089, 0.000659792, 0.013989, 0.0158918, 0.00903451, 0.0252594, 0.0190212, 0.0241154, 0.0291279, 0.0260783, 0.0142613, 0.030153, 0.025105, 0.0129642, 0.00352343, -0.00228727, 0.0145036, 0.0119527, -0.0165307, -0.0779897, -0.0235052, -0.00964113, -0.00284949, -0.00268456, 0.0171599, 0.024492, 0.00808088, -0.00854303, -0.00409035, -0.0102674, 0.0210914, 0.0153817, 0.00787349, 0.0125609, 0.00400798, 0.0104452, 0.0262228, 0.0381991, 0.00445668, 0.00258841, -0.00697198, 0.00657253, 0.028033, -0.0083342, -0.00360611, 0.0203619, 0.00138239, 0.0490661, 0.109895, 0.196019, -0.636403, 0.121568, 0.0796361, 0.137003, -0.524521, 0.178005, -0.0261073, -0.0734116, -0.280662, 0.339382, -0.333702, 0, 0, 0.110043, -0.0799681, -0.994239, 0.103738, -0.133187, 0.835193, 0.75731, 0.246066, 0.23398, -0.655469, 0.0322699, 0.0752283, 0.00613013, -0.0602498, 0.0322961, 0.0128974, 0.00668863, 0.042636, 0.00137754, -0.00560987, 0.0122685, 0.00887011, -0.0259053, 0.00473034, 0.0149468, 0.0156961, -0.0231823, 0.0131505, 0.0294266, 0.0497114, 0.0551901, 0.040342, 0.0594725, 0.0354651, 0.0451912, 0.00074545, 0.00357702, 0.0271149, 0.0431104, -0.0219182, -0.0470347, 0.00403028, 0.00934395, -0.019174, 0.0184895, 0.00707531, 0.0119705, 0.0133702, 0.015473, 0.0149602, 0.0191502, 0.00329596, 0.0208222, 0.0139958, 0.0167524, 0.0204249, 0.0304201, 0.0355056, 0.0133922, 0.00399478, 0.0057699, 0.0295299, 0.00786322, 0.0163553, 0.0142437, -0.016668, 0.0339098, -0.0226997, 0.00145475, -0.235698, 0.494363, 0.109546, -0.224065, 0.129013, 0.218321, -0.870067, 0.268142, 0.0784171, -0.146728, -0.449583, -0.104838, 0.27386, 0, 0, -0.106002, 0.27531, 0.148556, -0.702765, -0.891822, -0.4761, -0.219924, 0.523815, -0.339917, -0.088642, 0.143906, 0.465898, 0.212268, -0.0541829, 0.0278194, 0.0176617, -0.0120979, 0.0194753, -0.00672207, 0.00995056, -0.0025048, 0.0236662, 0.0170649, 0.0400912, 0.00998179, 0.0269631, 0.00746627, 0.0130785, 0.0286343, 0.0468336, 0.0412146, 0.0220454, 0.0216783, 0.0403545, 0.0398768, -0.00136286, 0.00940393, 0.00260998, 0.0270998, 0.0149207, 0.00927306, -0.000526352, 0.0233053, 0.0167807, 0.0079517, -0.0082127, 0.0229947, 0.0132546, 0.0266178, 0.0292488, 0.0216173, 0.00122105, 0.0210155, 0.0209969, 0.0112666, 0.0181056, 0.0519767, 0.0297803, 0.0287513, 0.0195414, 0.0245209, 0.00167551, 0.0089548, -0.00725823, 0.0118813, -0.0234004, 0.00763914, -0.0138497, 0.00190379, -0.13938, 0.369499, 0.120837, 0.117554, 0.034209, -0.143511, 0.140117, 0.292607, 0.372597, -0.146702, 0.225791, 0.0314292, 0.0915597, 0, 0, -0.170366, 0.0929373, -0.305982, -0.421973, 0.155267, -0.731054, -0.401238, 0.246787, 0.185659, 0.0770779, 0.00357011, 0.130148, 0.117085, -0.0213994, 0.0151246, 0.0151559, 0.0270413, 0.0245415, -0.00242166, -0.016132, 0.0060456, 0.0163159, 0.00813071, 0.0231943, -0.000632065, -0.00302836, 0.0232412, 0.00426581, -0.00117107, 0.0153372, 0.0155899, 0.0202449, 0.00827389, 0.00691851, 0.0245794, 0.00912181, 0.00631397, 0.0228423, 0.00881941, 0.0151588, 0.00396746, -0.0116824, -0.00380213, 0.0334631, 0.0147629, 0.0135369, 0.00292757, 0.0130498, 0.0096923, 0.0293863, 0.0103856, 0.0243964, 0.0167266, 0.00903693, 0.0119849, 0.0273706, -0.0153759, 0.00524803, -0.00845425, 0.0199563, 0.0166447, 0.00614369, 0.013759, -0.00369641, -0.0207142, 0.00244241, 0.00704693, 0.0165003, 0.0263125, -0.126921, 0.33525, 0.275101, 0.0964794, -0.0279062, 0.276295, -0.0898866, -0.114476, -0.154717, -1.16924, 0.16427, 0.140561, -0.0656118, 0, 0, -1.01671, 0.579686, 0.0587478, 0.228033, 0.0845411, 0.158347, 0.14762, 0.245729, 0.307352, 0.098835, -0.245373, 0.110077, 0.0135818, -0.0193209, -0.0265162, 0.00135194, 0.022458, -0.00182074, 0.00402217, -0.0036545, -0.00551382, 0.00614064, -0.000955415, 0.0060087, -0.0237173, -0.0217515, 0.00999508, 0.0240216, 0.0214199, 0.0267233, 0.00446582, 0.00710324, 0.0174225, 0.0163336, 0.00596344, 0.0145639, 0.00268764, 0.0124111, 0.0172115, 0.0141331, -0.00358624, 0.000915042, 0.00704083, 0.00366454, 0.00677087, 0.0053697, 0.00191335, 0.00876987, 0.0062013, 0.0158351, 0.0140922, 0.0230853, 0.0188745, 0.0100694, 0.00398891, 0.0113014, 0.00948254, -0.00808068, -0.007938, 0.0164367, 0.0262808, 0.0371095, 0.02598, 0.0162768, -0.0122608, 0.00445659, 0.0238588, 0.00989636, 0.0105785, 0.0754624, 0.0608152, 0.0686849, -1.0486, -0.781631, 0.140351, -0.0715477, -0.587318, -0.0288828, 0.452986, 0.330053, -0.101335, -0.105717, 0, 0, 0.127108, -0.436722, -0.0953791, 0.287091, -0.081557, 0.159085, 0.159271, 0.252939, 0.203426, -0.0311945, 0.139957, 0.220478, 0.218088, -0.00630489, -0.0241939, -0.0302855, 0.00243636, 0.010349, -0.0242364, 5.05415e-05, -0.00111845, 0.0017867, 0.0177028, 0.00517896, 0.0255265, -0.0179121, 0.00248077, 0.014764, 0.0149373, 0.0192855, 0.0342892, 0.0297574, 0.0304321, 0.0218236, 0.0132966, 0.00998794, 0.00419806, 0.0257195, 0.0283988, 0.0164601, 0.025985, 0.0136651, 0.0294683, 0.0112426, -0.00114719, 0.00771533, 0.0102623, 0.0211178, 0.0245389, 0.0206099, 0.013759, 0.0170459, 0.0348332, 0.0317464, 0.0224309, 0.0215132, 0.0292723, 0.0087347, 0.0165729, 0.0069524, 0.0256648, 0.0288632, 0.0219788, 0.0053357, -0.0304864, 0.00155183, 0.0225444, -0.0085163, 0.0111772, 0.170346, 0.155889, 0.193488, -0.119053, 0.354333, 0.163508, 0.178113, -0.443607, 0.0281412, -0.375436, -0.0713834, -0.153396, -0.0730397, 0, 0, -0.144317, 0.215238, 0.361094, 1.21106, -0.48724, 0.0198489, -0.0417697, -0.871553, 0.110659, 0.141952, -0.628183, -0.607704, -0.10308, -0.0531648, 0.00441932, 0.0182038, -0.000532968, 0.0111387, -0.00869416, 0.00301953, 0.00599966, 0.00474786, 0.0169018, 0.0375952, -0.00757433, -0.00596454, 0.00494013, 0.0174411, 0.0334728, 0.0209036, 0.0248724, 0.0257831, 0.00735379, 0.00914221, 0.0190425, 0.007977, 0.000629332, 0.018455, 0.0250534, 0.0240776, 0.0144833, 0.0239961, 0.0147148, 0.026668, 0.00472217, 0.0244913, 0.0180529, 0.0246393, 0.0258784, 0.0253173, 0.0277045, 0.0242894, 0.0303319, 0.0375995, 0.0228655, 0.0216174, 0.0392676, 0.00971793, 0.0218066, 0.0266002, -0.00778094, 0.00787932, -0.00559273, -0.0144086, -0.0308442, -0.000968875, -0.026834, -0.0392371, 0.043311, -0.0270759, 0.346282, -0.497044, -0.136597, 0.100009, 0.132499, 0.280627, 0.243155, 0.0788073, 0.189903, -0.627269, 0.294971, -0.00785697, 0, 0, -0.197727, -0.0658932, 0.240466, 0.398735, 0.178563, 0.0398199, -0.217998, -0.34179, -0.614596, 0.0832511, 0.284929, -0.529445, -0.478151, 0.0252301, -0.035981, -0.0171645, 0.00498494, 0.00206238, 0.0102514, 0.00105775, -0.0112382, 0.00537105, 0.0213306, 0.0246392, 0.0110787, -0.000293954, 0.0121807, 0.0239736, 0.00747329, -0.000619344, 0.0218011, -0.00096476, 0.019401, 0.00604514, 0.0053252, -0.00111951, 0.00211318, 0.00586173, 0.0194137, 0.0176817, 0.00222861, -0.00139633, 0.0179067, 0.00507238, 0.0198391, 0.0112214, 0.025489, 0.0153514, 0.0211008, 0.00986471, 0.0173096, 0.0289893, 0.0237234, 0.0186775, 0.0159621, 0.0260032, 0.00969434, 0.0128414, 0.008913, 0.00303948, 0.00266832, 0.0129525, 0.0068498, -0.00382883, -0.0133436, -0.018195, -0.0273098, -0.00419936, -0.0381104, 0.0285236, -0.165354, -0.295351, 0.0833925, -0.00476403, 0.0720168, -0.250793, 0.217007, 0.195691, -0.186944, -0.453708, 0.35081, -0.0026836, 0, 0, -0.32808, -0.121352, 0.141308, 0.215901, -0.245852, -0.597758, -1.05336, 0.139165, 0.127865, -0.446391, 0.379135, -0.255498, -0.277629, 0.00813613, -0.0321147, -0.0260419, -0.0160939, -0.0177183, -0.0256895, -0.0286939, -0.0245268, -0.00291399, 0.0020037, 0.0261188, 0.0398927, 0.0164519, 0.035611, 0.0146476, 0.0149345, 0.0140955, 0.00141547, 0.010967, 0.0211361, 0.0155247, 0.00907935, 0.0239046, 0.0217193, 0.00930912, 0.0136236, 0.0149103, -0.00703879, 0.00344991, 0.0151991, 0.0118686, -4.01236e-05, -0.00350515, 0.00193806, -0.00313859, 0.00225791, 0.00232425, 0.0064871, 0.00759805, -0.0010561, 0.0182402, 0.0101571, -0.000441796, -0.0281388, 0.0168195, 0.0217755, 0.020193, 0.0150357, 0.0115244, 0.00942411, 0.00634666, -0.010433, 0.0170893, -0.0140055, 0.0253638, -0.0451425, -0.926435, 0.106855, 0.0497039, 0.0846448, 0.0383932, -0.625952, 0.281354, 0.0154588, -0.0217047, -0.0623516, 0.335349, 0.0970995, -0.163928, 0, 0, 0.324669, 0.380451, -0.366386, -0.193811, 0.0686272, 0.56081, 0.00954701, 0.205276, 0.150335, -0.203452, 0.0248191, 0.119685, -0.507679, 0.0046985, -0.0151448, -0.00250645, -0.0139632, -0.0253649, -0.0169595, -0.0332232, -0.0169742, -0.016523, 0.0252805, 0.0289095, 0.00451079, 0.0283578, 0.0164318, 0.0115766, 0.0161212, 0.0219283, 0.0258644, 0.0211903, 0.0233834, 0.0190075, 0.016035, 0.0249204, 0.015406, 0.0166733, 0.0321911, 0.0199894, 0.00889824, 0.0043297, -0.00797965, 0.0227313, 0.00966878, 0.0129838, -0.00176649, 0.00743944, 0.0222619, 0.0104927, 0.0191704, -0.0564721, 0.0117035, 0.0394497, 0.00377718, -0.00923991, 0.0139627, 0.0208916, 0.0187249, 0.00831879, -0.00687143, -0.00856069, 0.0173107, 0.00617303, 0.0129127, 0.00746416, 0.000879039, -0.000822699, 0.0200773, 0.081818, -0.122273, 0.0716159, 0.138905, 0.0819157, 0.155405, 0.414759, 0.937334, 0.338747, 0.167474, -0.375023, -0.274467, 0.165663, 0, 0, 0.51277, -0.0263845, 0.0907723, 0.00669375, -0.341688, -0.890349, 0.112485, -0.938655, -0.729866, 0.104192, 0.109093, -0.388052, 0.0861015, 0.0503686, -0.0428389, -0.0382358, -0.0076547, 0.00549689, 0.0118468, -0.0150376, -0.00826924, -0.0174529, 0.0103739, 0.0123119, 0.00297875, 0.021266, 0.00541264, 0.0199713, 0.0240477, 0.0168818, 0.0175718, 0.0250809, 0.00849503, 0.0110588, 0.00374775, 0.00942059, 0.009642, 0.0195695, 0.0261342, 0.0239312, 0.00292506, -0.0165568, -0.103226, -0.00124631, 0.0170844, -0.00786558, 0.0185933, 0.0166685, 0.0262211, 0.0217342, 0.0094534, -0.00208232, 0.0198119, 0.028506, 0.0343607, 0.0293222, 0.0258421, 0.0131981, -0.0352405, -0.0142973, -0.0241897, 0.00816537, 0.00375098, -0.0140577, 0.00502896, -0.00867037, -0.0185554, -0.0271369, 0.0324208, 0.0565613, -0.284287, -0.373908, 0.239431, 0.0804595, 0.120016, -0.897884, 0.288939, 0.146763, 0.103277, 0.110671, 0.161437, -0.111308, 0, 0, 0.469973, 0.0411889, -0.319425, -0.0876676, 0.26614, 0.139103, -0.00192692, 0.192285, -0.891133, 0.507099, 0.083909, -0.185286, 0.11711, 0.0503114, -0.012426, 0.00990384, 0.0158234, 0.0192235, 0.0177932, -0.00912072, 0.0197953, 0.0122192, 0.00798533, -0.0224509, 0.0104648, 0.00136809, 0.00971481, 0.0276621, 0.0166133, 0.0155641, 0.00896272, 0.0127167, 0.00756988, -0.0109924, -0.00299699, 0.00131092, 0.0196466, 0.0197113, 0.0132246, 0.0132148, -0.000436025, 0.00893798, -0.0020373, -0.00227778, 0.00996218, 0.00978594, -0.000986138, 0.00481682, 0.00919609, 0.0206084, 0.00831915, 0.0147308, -0.00721529, 0.0142272, 0.0139011, 0.0232286, 0.00417044, 0.010024, -0.021757, 0.00166067, 0.00951437, 0.0180115, 0.00944031, 0.0028884, -0.0275052, -0.00175132, -0.0102279, 0.0133575, -0.0185617, -0.102743, -0.335385, -0.575325, 0.121674, -0.155245, 0.103348, 0.0820822, 0.203062, 0.421896, -0.481253, -0.0509974, -0.0446212, -0.380843, 0, 0, -0.197725, -0.0514788, 0.141, -0.872965, 0.12821, 0.0886758, 0.186512, -0.281986, -0.51154, 0.0901854, -0.020594, 0.188758, 0.0849871, 0.018696, -0.00194144, -0.0197107, 0.00472987, 0.0125614, 0.00544391, -0.0101274, 0.00257488, -0.0124534, 0.00339615, -0.00759121, 0.00983669, 0.0189914, 0.0208413, 0.0390345, 0.0218295, -0.0112956, -0.016594, 0.0154262, 0.00762849, 0.00183015, 0.0123372, 0.00346603, 0.029095, 0.014305, 0.00298933, 0.000640444, 0.00550988, 0.00365077, 0.00315869, 0.00969303, -0.0104143, -0.00773419, -0.00437164, -0.000617444, 0.000855367, 0.00345216, 0.0145777, 0.00172399, 0.0169964, -0.00167891, -0.00203361, -0.0178602, -0.0197299, -0.0069926, -0.0215155, -0.00379904, -0.00156684, 0.0170956, 0.0285584, 0.0082879, -0.011176, -0.00169873, -0.00992598, 0.00193298, -0.0169969, 0.0907866, 0.102591, -0.496603, 0.0877421, 0.0405157, -0.942751, 0.26103, 0.111736, 0.147865, 0.176875, -0.595774, -0.105785, -0.12666, 0, 0, 0.11601, 0.109602, -0.42211, 0.175653, 1.00726, 0.105072, -0.534053, 0.187353, -0.364317, -0.278088, 0.0968671, 0.588157, -0.10674, 0.028425, 0.00736128, -0.0236981, 0.0115482, 0.0179697, 0.00199823, 0.0240861, 0.00171201, 0.0102051, 0.00085894, -0.0148487, 0.0196696, 0.0283232, 0.0226183, 0.014223, 0.0155412, 0.00853353, -0.0714738, 0.0134216, 0.0198948, 0.0223352, 0.0131046, 0.031782, 0.00463946, 0.0117998, 0.0306972, 0.0206826, 0.0132561, 0.00772545, -0.0023007, -0.0015318, 0.00910988, 0.0089012, 0.0025118, 0.016342, 0.0142151, 0.0158211, 0.012691, 0.0201111, 0.0168949, 0.0295184, 0.0140407, -0.00153951, 0.00869321, 0.0107625, -0.000491018, -0.00533711, 0.0299114, 0.0292826, 0.0028075, 0.0137695, -0.012011, -0.013709, -0.0123369, -0.0933073, -0.0280147, 0.00602922, 0.12924, 0.0322805, -0.231119, 0.0461764, 0.14912, 0.553964, -0.610229, -0.288836, -0.108988, -0.137392, -0.164148, 0.734696, 0, 0, 0.0218754, -0.271875, -0.0860625, -0.422906, 0.162246, 0.0737254, -1.13734, 0.161283, 0.45452, 0.104248, -0.516352, 0.114697, 0.102705, 0.000572892, 0.0220716, 0.0111478, 0.00744765, 0.0146806, 0.00736366, 0.00482508, -0.0154954, -0.0120425, -0.0129775, 0.00854091, 0.0216637, 0.0186535, 0.0193571, 0.0256516, -0.00194073, 0.025848, 0.0104914, 0.0290404, 0.0321083, 0.0145593, 0.00301465, 0.00457932, -0.00151504, 0.0116777, -0.00087613, 0.0171626, 0.0135994, 0.016101, 0.010647, 0.00636316, 0.0039803, -0.00242884, 0.0123492, 0.0167015, 0.0161929, 0.00747398, 0.0212887, 0.0256959, 0.00240396, 0.0172532, 0.00504944, 0.0193268, 0.00475044, 0.00197857, -0.0138601, -0.0101902, -0.00774043, -0.0120108, 0.015203, 0.0077325, 0.00285876, 0.0193486, 0.0128449, -0.00220699, 0.0321726, 0.358973, 0.0543295, 0.0733502, 0.0760461, 0.11416, 0.21404, 0.375543, -0.538835, -0.395812, 0.103893, 0.0845051, 0.0132767, 0.62944, 0, 0, 0.220672, -0.0827594, -0.477505, 0.201075, 0.0777501, 0.205226, 0.340954, 0.164338, 0.167706, -0.324423, 0.0696063, 0.0812903, 0.440959, 0.0292436, 0.0146105, 0.016951, 0.0155639, 0.0476891, 0.00317159, 0.00618074, 0.00025761, -0.00561604, -0.0027293, 0.0024575, 0.00747287, 0.00672896, 0.0208431, 0.00641529, -0.00910318, 0.000264495, 0.015908, 0.00970544, 0.00886638, -0.00447934, 0.00724256, -0.00134461, -0.00254015, -0.00396706, 0.00404893, 0.00240462, 0.00402241, 0.0129706, -0.00915147, 0.0085597, -0.00923241, 0.00491906, -0.00822143, -0.0039401, 0.0054932, -0.00621948, -0.0116093, 0.018482, -0.0049147, 0.00711381, 0.00181194, 0.0103533, 0.0029757, 0.00817914, 0.00695958, -0.000119002, -0.0300762, 0.0124755, 0.00306697, -0.00788878, -0.0179338, 0.017025, 0.00730556, 0.0181585, 0.0509637, 0.177705, -0.686899, 0.0462767, 0.101501, 0.0670618, -0.498621, 0.178531, 0.28862, 0.105774, 0.0765734, 0.10906, 0.100205, -0.175543, 0, 0, 0.343159, 0.194466, 0.126743, 0.299826, -0.0897381, -0.00381269, 0.32035, -0.434158, -0.317112, -0.378094, 0.113387, -0.731132, 0.151337, -0.0128055, 0.0476507, 0.0385859, 0.0210153, 0.0214244, 0.0127526, 0.022218, 0.0188005, 0.01374, 0.00586975, 0.00728893, 0.0120589, -0.00045664, -0.00343222, 0.0105607, 0.0014497, -0.0181072, 0.00761841, -0.0146827, 0.00293054, 0.00334077, 0.0102801, -0.00176466, 0.00321095, 0.000810621, -0.00627883, -0.00954531, 0.0135883, 0.019819, -0.00742899, 0.00141036, -0.000453509, -0.00168244, -0.0081388, -0.00787068, -0.00212273, -0.00164617, -0.010528, 0.0010865, 0.00394147, 0.00346885, 0.0132204, 0.00892171, -0.0118617, 0.00645227, 0.016221, 0.0117577, 0.00929914, -0.00914847, -0.00108177, 0.00070527, -0.00849699, 0.0185456, 0.00770226, 0.00241712, -0.0261796, -0.438404, -0.419013, 0.0329568, -0.0648615, -0.825614, 0.191214, -0.413647, 0.16382, -0.245602, 0.256498, 0.481031, -0.230335, 0.226724, 0, 0, 0.160492, 0.379021, 0.125051, -0.394023, 0.453105, 0.135272, -0.691447, 0.173982, 0.20958, 0.133557, -0.401771, 0.144058, -0.0989494, 0.0103445, 0.027696, 0.0128143, 0.0285259, -0.018086, 0.0381917, 0.00620039, 0.00108761, 0.0128052, 0.0132693, -0.000815501, -0.0010254, 0.0258138, 0.0027362, 0.00993157, 0.00989159, 0.0203851, 0.0131995, 0.0145528, 0.0289504, -0.000174347, 0.00327849, 0.011665, 0.0216385, 0.00460951, 0.0190338, 0.0118726, 0.00462418, 0.00898782, 0.0125754, 0.000703935, -0.00342692, 0.00904587, -0.00878324, 0.00139004, -0.00197914, -0.00271453, 0.00433298, 0.0177375, 0.0130398, 0.0177348, 0.00116581, 0.0130944, 0.0115954, 0.000445021, -0.00761442, 0.0145831, 0.0194089, 0.00666908, -0.0151925, -0.0112687, 0.0231923, 0.00101864, -0.00277426, 0.00530248, -0.0280954, 0.0153765, 0.048734, -0.237454, 0.135381, 0.0856597, -0.656745, 0.471874, 0.168841, 0.652966, -0.213696, -0.145167, -0.0602996, -0.278303, 0, 0, 0.0224515, -0.2094, 0.372314, 0.834031, 0.0864222, 0.373337, -0.0614263, 0.125215, 0.207479, 0.270901, 0.083413, -0.084681, 0.144969, 0.0919517, 0.0268666, 0.00647108, 0.0268192, 0.0457779, 0.0320291, 0.00878686, 0.0192623, 0.0163947, 0.0151807, 0.0309426, 0.0084386, 0.0232073, 0.00770925, 0.022282, 0.00195581, 0.023307, 0.00862047, 0.00277666, 0.0117729, -0.0014108, -0.00584406, -7.99608e-05, 0.0073374, 0.00690555, -0.0101672, 0.0207443, 0.00982768, 0.00515935, 0.012673, -0.0051611, 0.00771916, 0.00671512, 0.0136044, 0.0119948, 0.00713987, 0.0231161, 0.0236856, 0.0176995, 0.0123142, 0.0211874, 0.00980152, 0.00980353, 0.0301264, 0.0274986, 0.00214791, 0.000749363, -0.000144481, 0.0211065, -0.00286758, 0.000623105, -0.00135806, -0.0319775, -0.0153421, 0.0223426, 0.0165537, 0.0864622, -0.504239, 0.0454867, -0.179949, 0.11304, 0.168487, -0.287551, 0.113776, 0.430614, -0.509781, 0.0111774, 0.0799922, 0.228464, 0, 0, -0.104959, -0.0807807, 0.376288, -0.142937, -0.127214, 0.271179, 0.327116, -0.109658, 0.27267, 0.0640966, 0.582092, 0.0455356, -0.202553, 0.0577131, 0.000839399, 0.0304346, 0.0426289, 0.020633, 0.0345184, 0.0248412, 0.0201712, 0.0112769, 0.0168296, 0.0271008, -0.000552233, -0.00765773, 0.0141092, 0.00756734, -0.00891695, 0.0102058, 0.00285531, 0.0101275, -0.00126422, -0.00235571, 0.00187726, -0.00190243, 0.000804765, 0.00643872, 0.00630239, 0.0182188, 0.000977798, 0.003668, -0.000759258, -0.013215, 0.00714472, 0.0128989, 0.0183075, 0.00209541, 0.00395886, 0.00544589, -0.0129467, 0.0226776, 0.0182191, -0.0120165, 0.00209588, 0.00230601, -0.00346487, 0.0234807, 0.0312905, -0.0103803, 0.0189289, 0.0100253, 0.00445346, 0.00746137, 0.00113026, 0.0127197, -0.0288126, 0.00822805, 0.0343931, 0.397321, 0.528501, 0.171375, 0.0947913, -0.0755955, -0.380315, 0.331751, 0.326954, -0.21674, 0.0178053, 0.111503, -0.114913, -0.685483, 0, 0, 0.073433, 0.505702, 0.0229995, -0.89758, 0.10656, 0.251101, 0.0861931, 0.176386, 0.140761, -0.243758, 0.145292, -0.0495936, 0.291125, 0.190122, 0.00337411, 0.0155446, -0.0174318, 0.00527125, 0.0094637, 0.0177896, 0.00597736, 0.00455204, 0.00203451, -6.58653e-05, 0.00847471, -0.00887262, 0.0135048, 0.00308286, 0.00356851, 0.00737411, 0.00643255, 0.00216593, -0.00734983, -0.000931123, 0.00291638, -0.0199057, 0.00341565, 0.0101332, 0.0194678, 0.0270493, 0.00995485, -0.00326814, 0.00308607, 0.00516288, 0.000813756, 0.00616111, 0.0012403, -0.00265278, -0.00727078, 0.00208868, 0.0108703, -0.00117744, 0.00444171, 0.00341918, 0.0221239, 0.000513972, -0.0206063, -0.023282, 0.00679237, -0.00429768, 0.0117833, 0.0338622, 0.00338659, -0.011251, 0.0125534, -0.0169482, 0.00063622, 0.0328315, 0.0140078, 0.160725, -0.168424, 0.0780409, -0.906393, -0.111844, -0.875514, 0.312649, 0.0430418, -0.0283411, 0.258995, 0.0519853, 0.262125, -0.818378, 0, 0, -0.164267, -0.0609866, -0.260096, 0.0558968, 0.458712, 0.0735322, -0.735341, 0.210442, 0.222445, 0.123322, 0.0847887, -0.159716, -0.590239, -0.00630243, -0.000272007, -0.0556336, 0.0196293, 0.00963864, -0.00185404, 0.0100667, -0.00227032, 0.00249417, -0.0212947, -0.0312975, 0.000845762, 0.0157883, -0.00241539, -0.0275214, 0.00259345, 0.0237026, 0.0270353, 0.018024, 0.028604, 0.016141, 0.00947002, -0.0550163, 0.0151148, 0.0170707, 0.0207178, 0.0302973, 0.0141577, 0.0117242, -0.000149357, -0.000947857, 0.000701661, 0.00893654, 0.00893102, 0.00711359, -0.00754681, -0.023661, -0.0189301, -0.013904, -0.0108278, 0.00679914, 0.0228444, 0.0024207, -0.000571654, -0.158785, -0.0365786, 0.019051, 0.0210119, 0.0220753, -0.00334971, -0.0165962, -0.00349948, 0.0109838, 0.0210903, -0.0330201, 0.0448968, 0.392194, -0.0835511, 0.0509066, 0.13672, -0.376382, -0.142133, 0.263579, 0.564148, 0.41448, 0.144795, -0.546305, -0.272911, 0.031192, 0, 0, 0.118088, -0.0489305, -0.131705, -0.212339, 0.0750245, -0.849964, 0.17661, -0.651461, 0.17185, 0.10683, -0.0729062, 0.118462, 0.107465, 0.0484597, 0.000558717, 0.0117413, 0.0452489, 0.00656822, 0.0169244, 0.00676164, -0.0159824, 0.000433659, -0.00936603, -0.0308871, -0.00225009, 0.0025264, -0.0302476, -0.145979, -0.012044, 0.000833357, 0.00533716, 0.00998125, 0.0232534, 0.00986184, 0.00292847, -0.00514859, 0.0210219, 0.00935195, 0.0143981, 0.0216698, 0.00704815, 0.00142274, 0.0104183, 0.00585931, 0.0115554, 0.00320029, 0.0146599, 0.0152716, -0.0010417, 0.0163638, -0.00292275, -0.148811, -0.0341117, -0.00820037, 0.00690723, 0.00580386, 0.0102776, -0.0287342, -0.0426742, -0.00346314, -0.014082, 0.0164168, -0.0151897, -0.0133228, -0.00402774, 0.0294466, 0.0167652, -0.0360319, 0.0711642, 0.083808, -0.184709, -0.0980398, -0.399685, 0.0804186, -0.782502, 0.271326, -0.613257, 0.14975, 0.180464, 0.0670264, 0.416608, 0.102782, 0, 0, -0.192021, 0.492874, 0.207674, -0.0196691, -0.294052, 0.197247, -0.0735843, -0.109156, 0.0555986, 0.168678, 0.0641907, -0.775262, 0.0994696, 0.0202677, 0.0309279, -0.0142657, 0.0301083, 0.00962297, 0.0210668, 3.58343e-06, 0.00281656, 0.00364458, -0.0223304, 0.0018766, -0.0110041, -0.0175352, -0.0073413, -0.0303934, -0.0216453, -0.0200569, -0.0275308, -0.00626038, -0.00309842, 0.00603172, -0.00668402, 0.00443937, -0.00862822, -0.00330077, 0.000224463, 6.62862e-05, -0.0195622, -0.000130437, -0.00770041, 0.00533275, -0.00391618, -4.66355e-05, -0.00771137, 0.0042634, -0.0167789, 0.000385853, -0.0228836, -0.0208459, -0.00342089, 0.00206059, 0.00132507, 0.0124075, -0.0238468, 0.0219538, -0.0301166, -0.0163151, 0.0055811, 0.0122646, -0.0191304, 0.0100754, 0.00806302, -0.0232621, 0.0211904, 0.00515403, 0.0764653, 0.0269575, 0.290038, 0.0375496, 0.070808, 0.0471113, -0.746099, -1.10055, 0.107236, -0.577217, -0.186472, -0.247479, -0.246867, -0.167438, 0, 0, -0.920212, -0.124836, 0.18137, -0.167062, 0.0663951, 0.0413478, 0.241166, 0.0485479, 0.299229, -0.291361, -0.0577546, 0.439095, 0.439336, -0.00685212, -0.018186, 0.0191875, 0.00134158, 0.0186317, -0.00180959, -0.000343055, -0.00525811, -0.0173829, -0.0120612, 0.0111842, -0.00162653, -0.0204498, 0.00168794, 0.0142601, -0.00473493, -0.0388172, -0.141292, -0.0100236, -0.00879179, -0.00842432, -0.00892723, -0.00195914, -0.0199702, -0.0159568, -0.00544021, -0.00495973, -0.0194822, -0.0116988, -0.007264, 0.00779178, -0.0165814, -0.00229094, -0.0110314, -0.00737096, -0.020545, -0.0168759, -0.0220858, 0.000520981, -0.00104014, -0.0331411, -0.0289936, -0.00340396, -0.0158464, -0.00914367, 0.0286717, -0.0155749, 0.0147171, 0.00936366, -0.00521418, 0.0139895, 0.0176361, -0.0226626, 0.00714659, 0.0373704, 0.0538818, 0.0928518, -0.293312, 0.473294, -0.698679, 0.0904287, 0.12754, 0.270273, 0.374576, -0.162374, 0.0202955, 0.0462198, -0.00906006, -0.0636285, 0, 0, 0.0810608, -0.00272496, 0.290353, -0.280434, -0.102417, 0.0764063, -0.135846, 0.157132, -0.992854, -0.125817, 0.123486, 0.41063, 0.030256, 0.143689, -0.0183929, 0.00164764, 0.00286223, 0.0127256, -0.00837642, -0.0179849, -0.0125724, -0.0120477, -0.00901109, -0.0259241, -0.0467739, 0.0352959, -0.00391901, -0.00286235, -0.0361554, -0.0297949, -0.0166319, -0.0121744, -0.00873186, -0.033098, -0.000429523, -0.000700627, -0.0187568, -0.0106145, 0.00145481, 0.00319789, -0.00822775, 0.00273214, -0.00301346, -0.00105057, 0.00484948, -0.00192215, 0.00767673, 0.0126274, -0.010013, -0.00737091, -0.0143242, 0.00279192, -0.00893212, -0.0140216, -0.0139853, -0.00715914, 0.0219237, 0.0131066, 0.0120845, -0.0171834, 0.0112012, 0.00481712, -0.0129329, 0.0207899, 0.0131395, 0.000301797, 0.00591426, 0.00601964, 0.110523, -0.00255375, -0.934664, 0.0938624, 0.153472, 0.106667, 0.174197, 0.106787, -0.349591, 0.361262, 0.0165563, 0.62917, 0.436654, 0.481975, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  Asymm_map_ = new TH2F("Asymm_map","Asymm_map", 82, x, 72, y);
  for(size_t i{0}; i < (1+sizeof(x)/sizeof(x[0]))*(1+sizeof(y)/sizeof(y[0])); ++i){ 
    Asymm_map_->SetBinContent(i, contents.at(i));  
  }

}

void HiInclusiveJetSubstructure::analyze(const Event& iEvent, const EventSetup& iSetup) {
  int event = iEvent.id().event();
  int run = iEvent.id().run();
  int lumi = iEvent.id().luminosityBlock();

  jets_.run = run;
  jets_.evt = event;
  jets_.lumi = lumi;

  LogDebug("HiInclusiveJetSubstructure")<<"START event: "<<event<<" in run "<<run<<endl;

    // loop the events
  reco::Vertex::Point vtx(0,0,0);
  if (useVtx_) {
    edm::Handle<vector<reco::Vertex> >vertex;
    iEvent.getByToken(vtxTag_, vertex);
    if(vertex->size()>0) {
      jets_.vx = vertex->begin()->x();
      jets_.vy = vertex->begin()->y();
      jets_.vz = vertex->begin()->z();
      vtx = vertex->begin()->position();
    }
  }

  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jetTag_, jets);

  edm::Handle<pat::JetCollection> matchedjets;
  iEvent.getByToken(matchTag_, matchedjets);

  auto const& pfCandidates = iEvent.get(pfCandidateToken_);
    // edm::Handle<reco::PFCandidateCollection> pfCandidates;
    // iEvent.getByToken(pfCandidateLabel_,pfCandidates);

  edm::Handle<reco::TrackCollection> tracks;
  iEvent.getByToken(trackTag_,tracks);
    // 
  // edm::Handle<reco::GenParticleCollection> genparts;
  // if(isMC_){
  //   iEvent.getByToken(genParticleSrc_,genparts);
  // }

  edm::Handle<CaloTowerCollection> towers;
  if(doTower){
    iEvent.getByToken(TowerSrc_,towers);
  }

  double pthat = 0;
  if(isMC_){
    edm::Handle<GenEventInfoProduct> hEventInfo;
    iEvent.getByToken(eventGenInfoTag_,hEventInfo);
    pthat = hEventInfo->qScale();
  }

  jets_.nref = 0;
  // std::cout << "Checking jets inside the range " << jetAbsEtaMax_-rParam << " for radius parameter " << rParam << std::endl;
  // if(doChargedConstOnly_) std::cout << "Doing only charged jet constituents!" << std::endl;
  // else std::cout << "Doing ALL jet constituents!" << std::endl;
  jets_.triggerJetInAcceptance = false;
    // std::cout << "Number of jets: " << jets->size() << std::endl;
  for(unsigned int j = 0; j < jets->size(); ++j){
    const pat::Jet& jet = (*jets)[j];
    auto pt = useRawPt_ ? jet.correctedJet("Uncorrected").pt() : jet.pt();
    if(pt < jetPtMin_) continue;
    // if(std::abs(jet.eta()) > jetAbsEtaMax_-rParam) continue;
    // std::cout << "Raw pt: " << jet.correctedJet("Uncorrected").pt() << " corrected: " << jet.pt() << " eta: " << jet.eta() << std::endl;
    //assume highest jet in event is also the trigger object, check if it's within the eta acceptance above
    if(j==0){ 
      jets_.triggerJetInAcceptance = true;
    }
   
    jets_.rawpt[jets_.nref] = jet.correctedJet("Uncorrected").pt();
    jets_.jtrawE[jets_.nref] = jet.correctedJet("Uncorrected").energy();
    jets_.jtpt[jets_.nref] = jet.pt();
    jets_.jteta[jets_.nref] = jet.eta();
    jets_.jtphi[jets_.nref] = jet.phi();
        // jets_.jtsym[jets_.nref] = 0;
        // jets_.jtrg[jets_.nref] = 0;
        // jets_.jtdynkt[jets_.nref] = 0;
    // jets_.jtangu[jets_.nref] = 0;

        // jets_.jtdyn_pt1[jets_.nref] = 0;
    // jets_.jtdyn_var[jets_.nref] = 0;
    jets_.jtdyn_split[jets_.nref] = 0;
    // jets_.jtdyn_theta[jets_.nref] = 0;
    jets_.jtdyn_kt[jets_.nref] = 0;
    jets_.jtdyn_z[jets_.nref] = 0;

    fastjet::PseudoJet *sub1Gen = new fastjet::PseudoJet();
    fastjet::PseudoJet *sub2Gen = new fastjet::PseudoJet();
    fastjet::PseudoJet *sub1Hyb = new fastjet::PseudoJet();
    fastjet::PseudoJet *sub2Hyb = new fastjet::PseudoJet();

    IterativeDeclusteringRec(groom_type, groom_combine, jet, sub1Hyb, sub2Hyb);

    jets_.refpt[jets_.nref] = 0;
    jets_.refeta[jets_.nref] = 0;
    jets_.refphi[jets_.nref] = 0;
    jets_.refsym[jets_.nref] = 0.;

        // jets_.refrg[jets_.nref] = 0;
        // jets_.refdynkt[jets_.nref] = 0;
    // jets_.refangu[jets_.nref] = 0; 
        // jets_.refdyn_pt1[jets_.nref] = 0;
    // jets_.refdyn_var[jets_.nref] = 0;
    jets_.refdyn_split[jets_.nref] = 0;
    // jets_.refdyn_theta[jets_.nref] = 0;
    jets_.refdyn_kt[jets_.nref] = 0;
    jets_.refdyn_z[jets_.nref] = 0;
    jets_.jtdyn_isClosestToTruth[jets_.nref] = 0;
    jets_.refdyn_isClosestToReco[jets_.nref] = 0;
    jets_.jtdyn_refdyn_dR[jets_.nref] = 0;


    jets_.refsub11[jets_.nref] = 0;
    jets_.refsub12[jets_.nref] = 0;
    jets_.refsub21[jets_.nref] = 0;
    jets_.refsub22[jets_.nref] = 0;
    // std::cout << jets_.jtJetConstituent.size() << " " << jets_.refJetConstituent.size() << " sizes of consts" << std::endl;
    if(isMC_){
      const reco::GenJet * genjet = jet.genJet();
      if(!genjet) continue;

      if(jet.genParton()){
        const reco::GenParticle & parton = *jet.genParton();
        jets_.refparton_pt[jets_.nref] = parton.pt();
        jets_.refparton_flavor[jets_.nref] = parton.pdgId();
      }
      else {
        jets_.refparton_pt[jets_.nref] = -999;
        jets_.refparton_flavor[jets_.nref] = -999;
      }

      jets_.refpt[jets_.nref] = genjet->pt();
      jets_.refeta[jets_.nref] = genjet->eta();
      jets_.refphi[jets_.nref] = genjet->phi();
            //cout<<"jet daughters gen"<<genjet->numberOfDaughters()<<endl;
      if(dopthatcut) if(pthat<0.35*genjet->pt()) continue;

      IterativeDeclusteringGen(groom_type, groom_combine, *genjet, sub1Gen, sub2Gen);
      if(doHardestSplitMatching_){
        TruthRecoRecoTruthMatching();
        // if(!(jets_.jtdyn_isClosestToTruth[jets_.nref] && jets_.refdyn_isClosestToReco[jets_.nref])){
        //   std::cout << "Failed matching " << jets_.jtdyn_isClosestToTruth[jets_.nref] << " " <<  jets_.refdyn_isClosestToReco[jets_.nref] << std::endl;
        //   std::cout << "dR=" << jets_.jtdyn_deltaR[jets_.nref] << " kt=" << jets_.jtdyn_kt[jets_.nref] << " truth jtpt=" << jets_.refpt[jets_.nref] << std::endl;
          
        //   if(jets_.jtdyn_deltaR[jets_.nref] > 0.175 && jets_.jtdyn_deltaR[jets_.nref] < 0.2 && jets_.jtdyn_kt[jets_.nref]>2 && jets_.jtdyn_kt[jets_.nref]<30 && jets_.jtpt[jets_.nref]>150 ){
        //     std::cout << "The above jet satisfies reco selection for purity" << std::endl;
        //     std::cout << "dR=" << jets_.jtdyn_deltaR[jets_.nref] << " kt=" << jets_.jtdyn_kt[jets_.nref] << " truth jtpt=" << jets_.refpt[jets_.nref] << std::endl;
        //   }
        // }
        // std::cout << "Constituent vector lengths " << jets_.jtJetConstituent.size() << " " << jets_.refJetConstituent.size() << std::endl;
        jets_.jtJetConstituent = {};
        jets_.refJetConstituent = {};
         // std::cout << "Constituent vector lengths after clear " << jets_.jtJetConstituent.size() << " " << jets_.refJetConstituent.size() << std::endl;
        
      }
      if(doSubjetPurity){
        jets_.refsub11[jets_.nref] = sqrt(pow((sub1Gen->rap()-sub1Hyb->rap()),2)+pow((sub1Gen->phi()-sub1Hyb->phi()),2));
        jets_.refsub12[jets_.nref] = sqrt(pow((sub1Gen->rap()-sub2Hyb->rap()),2)+pow((sub1Gen->phi()-sub2Hyb->phi()),2));
        jets_.refsub21[jets_.nref] = sqrt(pow((sub2Gen->rap()-sub1Hyb->rap()),2)+pow((sub2Gen->phi()-sub1Hyb->phi()),2));
        jets_.refsub22[jets_.nref] = sqrt(pow((sub2Gen->rap()-sub2Hyb->rap()),2)+pow((sub2Gen->phi()-sub2Hyb->phi()),2));
      }
    }

    delete sub1Gen;
    delete sub2Gen;
    delete sub1Hyb;
    delete sub2Hyb;       

    jets_.nref++;
  } 
  // jets_.triggerJetInAcceptance = trigger_jet_in_acceptance;
  t->Fill();
  memset(&jets_,0,sizeof jets_);

}
//reco::Jet& jet and const reco::GenJet& jet - can replace the two IterDec with one using a template? 
void HiInclusiveJetSubstructure::IterativeDeclusteringRec(double groom_type, double groom_combine, const reco::Jet& jet, fastjet::PseudoJet *sub1, fastjet::PseudoJet *sub2)
{
  if(doHiJetID_){
        jets_.muMax[jets_.nref] = 0;
        jets_.muSum[jets_.nref] = 0;
        jets_.muN[jets_.nref] = 0;

        jets_.eg_HFMax[jets_.nref] = 0;
        jets_.eg_HFSum[jets_.nref] = 0;
        jets_.eg_HFN[jets_.nref] = 0;

        jets_.h_HFMax[jets_.nref] = 0;
        jets_.h_HFSum[jets_.nref] = 0;
        jets_.h_HFN[jets_.nref] = 0;

        jets_.eMax[jets_.nref] = 0;
        jets_.eSum[jets_.nref] = 0;
        jets_.eN[jets_.nref] = 0;

        jets_.neutralMax[jets_.nref] = 0;
        jets_.neutralSum[jets_.nref] = 0;
        jets_.neutralN[jets_.nref] = 0;


        jets_.photonMax[jets_.nref] = 0;
        jets_.photonSum[jets_.nref] = 0;
        jets_.photonN[jets_.nref] = 0;
        jets_.photonHardSum[jets_.nref] = 0;
        jets_.photonHardN[jets_.nref] = 0;

        jets_.chargedMax[jets_.nref] = 0;
        jets_.chargedSum[jets_.nref] = 0;
        jets_.chargedN[jets_.nref] = 0;
        jets_.chargedHardSum[jets_.nref] = 0;
        jets_.chargedHardN[jets_.nref] = 0;

        jets_.trackMax[jets_.nref] = 0;
        jets_.trackSum[jets_.nref] = 0;
        jets_.trackN[jets_.nref] = 0;
        jets_.trackHardSum[jets_.nref] = 0;
        jets_.trackHardN[jets_.nref] = 0;

        jets_.genChargedSum[jets_.nref] = 0;
        jets_.genHardSum[jets_.nref] = 0;

        jets_.signalChargedSum[jets_.nref] = 0;
        jets_.signalHardSum[jets_.nref] = 0;
      }
  TRandom *rand_track_sel = new TRandom3(0);
  TRandom *rand_charge_smear = new TRandom3(0);
  TRandom *rand_hcal_y_smear = new TRandom3(0);
  TRandom *rand_hcal_phi_smear = new TRandom3(0);
  std::vector<Int_t> posDonor = {};
  std::vector<Int_t> posAcceptor = {};
  Int_t intjet_multi = 0;
  float jet_girth = 0;
  float jet_girth_new = 0;
  float jet_thrust = 0;
  float jet_LHA = 0;
  float jet_pTD = 0;
  std::vector<float> jet_PLJPkT = {};
  std::vector<float> jet_PLJPdR = {};
  std::vector<float> jet_PLJPeta = {};
  std::vector<float> jet_PLJPphi = {};
	Int_t nsplit = 0;
  double dyn_kt = std::numeric_limits<double>::min();
  Int_t dyn_split = 0;
  double z = 0;
  double dyn_eta = 0;
  double dyn_phi = 0;
  double dyn_deltaR = 0;
  // double dyn_var = std::numeric_limits<double>::min();
  double dyn_z = 0;
  double jet_radius_ca = 1.0;
  fastjet::JetDefinition jet_def(fastjet::genkt_algorithm,jet_radius_ca,0,static_cast<fastjet::RecombinationScheme>(0), fastjet::Best);
  fastjet::PseudoJet myjet;
  fastjet::PseudoJet mypart;

  // fastjet::PseudoJet testpart;
  // testpart.reset_PtYPhiM(15, 1.7, 2.31, 0.00005);
  // std::cout << testpart.e() << " energy of test particle, pz=" << testpart.pz() << " pT=" << testpart.perp() << std::endl;
  // testpart.reset_PtYPhiM(15, 1.7+0.10, 2.31+0.09, 0.00005);
  // std::cout << testpart.e() << " energy of test particle correction, pz=" << testpart.pz() << " pT=" << testpart.perp() << std::endl;
  // testpart.reset_PtYPhiM(15, 1.7+0.30, 2.31+0.09, 0.00005);
  // std::cout << testpart.e() << " energy of test particle correction, pz=" << testpart.pz() << " pT=" << testpart.perp() << std::endl;
  // std::cout << testpart.phi() << " should be pi/2" << std::endl;

  myjet.reset(jet.p4().px(),jet.p4().py(),jet.p4().pz(),jet.p4().e());
  // Reclustering jet constituents with new algorithm
  try{
    std::vector<fastjet::PseudoJet> particles = {};
    auto daughters = jet.getJetConstituents();
        // std::cout << "Number of pfCand " << pfCandidates.size() << std::endl;
        // Geometrical PF Candidate x Jet Constituent Matching - Added by Bharadwaj - Apr 2023
        // poor man's matching, someone fix please
    // std::vector<int> vec_jet_consituent_charge;
    Int_t count_it = 0;
    double JetNeutralEnergy = 0.;
    //scaling of neutral candidates using an (eta,phi) map from Mikko 
    if(isMC_ && doRatioNeuPFScaling_){
      for(auto it = daughters.begin(); it!=daughters.end(); ++it){
        Int_t abspdgId = abs((**it).pdgId());
        if(abspdgId==130)
          JetNeutralEnergy += (**it).energy();
      }
    }
    double JetNeutralEnergyFraction = JetNeutralEnergy/jets_.jtrawE[jets_.nref];
    // std::cout << jet.pt() << " " << jets_.jtpt[jets_.nref] << " jet pT " << jets_.jtrawE[jets_.nref] << " raw jet E" << std::endl;
    // std::cout << " ratio " << JetNeutralEnergyFraction << std::endl; 
    for(auto it = daughters.begin(); it!=daughters.end(); ++it){
      //if we want only charged constituents and the daughter charge is 0, skip it
      //hide all this jetID stuff in a function 
      if(doHiJetID_){
        if((**it).charge()!=0){
          // reco::Track const& track = (**it).pseudoTrack();
          TrackRef trk = (**it).get<TrackRef>();
          if(trk.isNonnull()){
          // if(!useQuality_ || (useQuality_ && reco::TrackBase::qualityByName(trackQuality_))){
            double ptcand = trk->pt();
            jets_.trackSum[jets_.nref] += ptcand;
            jets_.trackN[jets_.nref] += 1;

            if (ptcand > hardPtMin_) {
              jets_.trackHardSum[jets_.nref] += ptcand;
              jets_.trackHardN[jets_.nref] += 1;
            }
            if (ptcand > jets_.trackMax[jets_.nref])
              jets_.trackMax[jets_.nref] = ptcand;
          }
        }
        Double_t ecand = (**it).energy();
        // std::cout << (**it).pt() << " " << (**it).energy() << " pt and E" << std::endl;
        Int_t abspdgId = abs((**it).pdgId());
        if(abspdgId < 0) std::cout << "Negative pdg ID, what do?? " << (**it).pdgId() << std::endl;
        else if(abspdgId==22){
          jets_.photonSum[jets_.nref] += ecand;
          jets_.photonN[jets_.nref] += 1;
          if (ecand > hardPtMin_) {
            jets_.photonHardSum[jets_.nref] += ecand;
            jets_.photonHardN[jets_.nref] += 1;
          }
          if (ecand > jets_.photonMax[jets_.nref])
            jets_.photonMax[jets_.nref] = ecand;
        }
        else if(abspdgId==130){
          jets_.neutralSum[jets_.nref] += ecand;
          jets_.neutralN[jets_.nref] += 1;
          if (ecand > jets_.neutralMax[jets_.nref])
            jets_.neutralMax[jets_.nref] = ecand;
        }
        else if(abspdgId==211){
          jets_.chargedSum[jets_.nref] += ecand;
          jets_.chargedN[jets_.nref] += 1;
          if (ecand > hardPtMin_) {
            jets_.chargedHardSum[jets_.nref] += ecand;
            jets_.chargedHardN[jets_.nref] += 1;
          }
          if (ecand > jets_.chargedMax[jets_.nref])
            jets_.chargedMax[jets_.nref] = ecand;
        }
        else if(abspdgId==11){
          jets_.eSum[jets_.nref] += ecand;
          jets_.eN[jets_.nref] += 1;
          if (ecand > jets_.eMax[jets_.nref])
            jets_.eMax[jets_.nref] = ecand;
        }
        else if(abspdgId==13){
          jets_.muSum[jets_.nref] += ecand;
          jets_.muN[jets_.nref] += 1;
          if (ecand > jets_.muMax[jets_.nref])
            jets_.muMax[jets_.nref] = ecand;
        }
        else if(abspdgId==1){
          jets_.h_HFSum[jets_.nref] += ecand;
          jets_.h_HFN[jets_.nref] += 1;
          if (ecand > jets_.h_HFMax[jets_.nref])
            jets_.h_HFMax[jets_.nref] = ecand;
        }
        else if(abspdgId==2){
          jets_.eg_HFSum[jets_.nref] += ecand;
          jets_.eg_HFN[jets_.nref] += 1;
          if (ecand > jets_.eg_HFMax[jets_.nref])
            jets_.eg_HFMax[jets_.nref] = ecand;
        }
        else{
          std::cout << " something else???" << abspdgId << std::endl;
        }
      }

      if(doChargedConstOnly_ && (**it).charge()==0) continue;
      double PFE_scale = 1.;
      double charge_track_smear = 1.;
      double Hcal_y_smear = 1.;
      double Hcal_phi_smear = 1.;
      //if it is MC, rescale the 4-momentum of the charged particles (we accept only them above) by pfCCES(+-1%)
      if(isMC_){
        if((**it).charge()!=0)
          PFE_scale = pfChargedCandidateEnergyScale_;
        else if((**it).pdgId()==22){
          PFE_scale = pfGammaCandidateEnergyScale_;
          // std::cout << "Photon found! " << (**it).mass() << std::endl;
        }
        else if((**it).pdgId()==130){
          PFE_scale = pfNeutralCandidateEnergyScale_;
          if(doRatioNeuPFScaling_){
            double map_value = ReadJetAsymmMap((**it).eta(), (**it).phi(), *Asymm_map_);
            // std::cout << map_value << " map value for neutral at eta-phi = " << (**it).eta() << " " << (**it).phi() << std::endl;
            PFE_scale = 1 + map_value/JetNeutralEnergyFraction;
          }
          else if(doNaiveNeuPFScaling_){
            double map_value = ReadJetAsymmMap((**it).eta(), (**it).phi(), *Asymm_map_);
            PFE_scale = 1 + map_value;
          }
          // std::cout << PFE_scale << " Changed to " << std::endl;
          if(pfNeutralSmear_){
            while(abs(Hcal_y_smear) > 0.087){
              Hcal_y_smear = rand_hcal_y_smear->Gaus(0, 0.087/2);
            }
            while(abs(Hcal_phi_smear) > 0.087){
              Hcal_phi_smear = rand_hcal_phi_smear->Gaus(0, 0.087/2);
            }
          }
          // std::cout << " Smearing amount y: " << Hcal_y_smear << " phi: " << Hcal_phi_smear << std::endl;
          // std::cout << "Neutral found with rapidity: " << (**it).rap() << " and pseudorap: " << (**it).eta() << std::endl;
        }
        else{
          std::cout << "Not supposed to be here!!!!! What is this particle: " << (**it).pdgId() << std::endl;
        }
      }
      //vary tracking efficiency - drop TrackVariation_% of particles within the jet if using only charged particles
      if(isMC_ && TrackVariation_ != 0. && doChargedConstOnly_){
        // std::cout << "doing the track variation" << std::endl;
        if(rand_track_sel->Uniform(0,1) < TrackVariation_) continue;
      }
      //vary tracking efficiency - smear by 10% TrackVariation_% of charged particles within the jet if using inclusive
      else if(isMC_ && TrackVariation_ != 0. && !doChargedConstOnly_ && (**it).charge()!=0 && rand_track_sel->Uniform(0,1) < TrackVariation_ ){
        double closest_to_particle = 0.087;
        fastjet::PseudoJet temp_part;
        temp_part.reset((**it).px(), (**it).py(), (**it).pz(), (**it).energy());;
        Bool_t acceptorCharge = false;
        // int acceptorPos = 0;
        Int_t  count_it2 = 0;
        Int_t selectedAcceptor = 0;
        for(auto it2 = daughters.begin(); it2!=daughters.end(); ++it2){
          fastjet::PseudoJet temp_part_acceptor;
          temp_part_acceptor.reset((**it2).px(), (**it2).py(), (**it2).pz(), (**it2).energy());
          Double_t dR_DonorAcceptor = temp_part_acceptor.delta_R(temp_part);
          // std::cout << dR_DonorAcceptor << " distance between charged marked for removal and acceptors " << count_it << " and " << count_it2 << " particle numbers" << std::endl;
          // if( dR_DonorAcceptor < 0.1 && count_it2!=count_it && (**it2).charge()==0 && (**it).charge()==0 && (**it2).pdgId()!=22 && (**it).pdgId()!=22  ){
            // std::cout << "Neutrals within distance " << dR_DonorAcceptor << " " << count_it2 << " " << count_it << std::endl;
          // }
          if( dR_DonorAcceptor < closest_to_particle && count_it2!=count_it && (**it2).pdgId()!=22 ){
            selectedAcceptor = count_it2;
            closest_to_particle = dR_DonorAcceptor;
            // std::cout << " Acceptor particle ID = " << (**it2).pdgId() << std::endl;
          }
          count_it2++;
        }
        if(closest_to_particle < 0.087){
          posDonor.push_back(count_it);
          posAcceptor.push_back(selectedAcceptor);
          // std::cout << "Will move particle " << count_it << " to particle " << selectedAcceptor << " due to dR =" << closest_to_particle << std::endl; 
        }
        double charge_track_shift = rand_charge_smear->Gaus(0, (**it).energy()*0.1);
        charge_track_smear = ((**it).energy()-charge_track_shift)/(**it).energy();
        // std::cout << charge_track_smear << " smear factor for particle with energy " << (**it).energy() << std::endl;
      }
      // std::cout << "Rescaling charged pfCand energy by " << PFE_scale << std::endl;
      mypart.reset((**it).px()*PFE_scale*charge_track_smear, (**it).py()*PFE_scale*charge_track_smear, (**it).pz()*PFE_scale*charge_track_smear, (**it).energy()*PFE_scale*charge_track_smear);
      if(isMC_ && pfNeutralSmear_ && (**it).pdgId()==130){
        // std::cout << "Old y = " << mypart.rap() << " and eta =" << mypart.eta() << std::endl;
        mypart.reset_PtYPhiM(mypart.perp(), mypart.rap()+Hcal_y_smear, mypart.phi()+Hcal_phi_smear, mypart.m());
        // mypart.reset_PtYPhiM(p, y, phi, m);
        // std::cout << "new y and phi " << mypart.rap() << " and " << mypart.phi() << " perp and m " << mypart.perp() << " " << mypart.m() << std::endl;

      }
      particles.push_back(mypart);
      double frac_dR = mypart.delta_R(myjet)/rParam;
      // std::cout << rParam << " rParam" << std::endl;
      double frac_pt = mypart.perp()/myjet.perp();
      intjet_multi++;
      jet_girth     += mypart.perp()*mypart.delta_R(myjet)/myjet.perp();
      jet_girth_new += frac_pt*frac_dR;
      jet_thrust    += frac_pt*frac_dR*frac_dR;
      jet_LHA       += frac_pt*sqrt(frac_dR);
      jet_pTD       += frac_pt*frac_pt;
      count_it++;
    }
   // std::cout << " N/CH/C/P = " << jets_.neutralN[jets_.nref] << "/" << jets_.chargedN[jets_.nref] << "/" << jets_.eN[jets_.nref]+jets_.muN[jets_.nref] << "/" <<  jets_.photonN[jets_.nref] << std::endl;
    // std::cout << " total particles " << particles.size() << std::endl;
    // std::cout << "Particle container has " << particles.size() << " reco particles" << std::endl;
    // std::cout << "acceptors" << std::endl;
    // for(size_t acc{0}; acc<posAcceptor.size(); ++acc){
    //   std::cout << posAcceptor.at(acc) << " ";
    // }
    // std::cout << std::endl;
    // std::cout << "donors" << std::endl;
    // for(size_t don{0}; don<posDonor.size(); ++don){
    //   std::cout << posDonor.at(don) << " ";
    // }
    // std::cout << std::endl;
    // merge the particle donors/acceptors, zero and remove donors from list
    //if two charged happen to be close and both get dropped, they swap possitions and one still remains 0 vector
    if(isMC_ && TrackVariation_ != 0. && !doChargedConstOnly_){
      for(size_t pos{0}; pos < posDonor.size(); ++pos){
        // std::cout << "before merger pt=" << particles.at(posAcceptor.at(pos)).perp() << std::endl;
        particles.at(posAcceptor.at(pos)) = particles.at(posAcceptor.at(pos)) + particles.at(posDonor.at(pos));
        // std::cout << "after merger pt=" << particles.at(posAcceptor.at(pos)).perp() << std::endl;
        // std::cout << "donor before merger pt=" << particles.at(posDonor.at(pos)).perp() << std::endl;
        particles.at(posDonor.at(pos)).reset(0., 0., 0., 0.);
        // std::cout << "donor after merger pt=" << particles.at(posDonor.at(pos)).perp() << std::endl;
      }
    }
    // remove the particles just in case
    // for(size_t p{0}; p < particles.size(); ++p){
    //   std::cout << particles.at(p).perp() << " ";
    // }
    // std::cout << std::endl;
    Int_t itp_idx = 0;
    // std::cout << jets_.eN[jets_.nref] + jets_.photonN[jets_.nref] + jets_.muN[jets_.nref] + jets_.chargedN[jets_.nref] + jets_.neutralN[jets_.nref] << " constituents recorded out of " << particles.size() << std::endl;
    // std::cout << (jets_.eg_HFSum[jets_.nref] + jets_.h_HFSum[jets_.nref] + jets_.eSum[jets_.nref] + jets_.muSum[jets_.nref] + jets_.photonSum[jets_.nref] + jets_.chargedSum[jets_.nref] + jets_.neutralSum[jets_.nref])/jets_.jtrawE[jets_.nref] << " fractional energy" << std::endl;
    // std::cout << "something else above? jet eta " << jets_.jteta[jets_.nref] << std::endl;
    for(auto itp = particles.begin(); itp!=particles.end(); ++itp){
      // std::cout << itp_idx << " " << itp->perp() << " in checks" << std::endl;
      if(itp->perp()==0 and itp!=particles.end()){
        // std::cout << "removing " << itp_idx << " particle" << std::endl;
        particles.erase(itp);
        itp--;
      }
      // itp_idx++;
    }
    // std::cout << "Final length of particle vector " << particles.size() << std::endl;
    // for(size_t p{0}; p < particles.size(); ++p){
    //   std::cout << particles.at(p).perp() << " ";
    // }
    // std::cout << std::endl;
    if(particles.empty()){
      // jets_.jtdyn_var[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_split[jets_.nref] = std::numeric_limits<int>::min();
      jets_.jtdyn_eta[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_phi[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_deltaR[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_kt[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_z[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_intjet_multi[jets_.nref] = std::numeric_limits<int>::min();
      jets_.jt_girth[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_girth_new[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_thrust[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_LHA[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_pTD[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_PLJPkT.push_back(jet_PLJPkT);
      jets_.jt_PLJPdR.push_back(jet_PLJPdR);
      jets_.jt_PLJPeta.push_back(jet_PLJPeta);
      jets_.jt_PLJPphi.push_back(jet_PLJPphi);
      throw(123);
    }
    // std::cout << "Clustering " << particles.size() << " number of reco particles" << std::endl;
    fastjet::ClusterSequence csiter(particles, jet_def);
    std::vector<fastjet::PseudoJet> output_jets = csiter.inclusive_jets(0);
    // std::cout << output_jets.size() << " size of output jets" << std::endl;

    // for(size_t h{0}; h < output_jets.size(); ++h){
      // std::cout << output_jets.at(h).perp() << " pT of output jet " << h << std::endl;
    // }
    output_jets = sorted_by_pt(output_jets);
    fastjet::PseudoJet jj = output_jets[0];
    // LookThroughJetSplits(jj,1);
    fastjet::PseudoJet j1;
    fastjet::PseudoJet j2;
    fastjet::PseudoJet highest_splitting;
    if(!jj.has_parents(j1,j2)){
      jets_.jtdyn_split[jets_.nref] = std::numeric_limits<int>::min();
      jets_.jtdyn_eta[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_phi[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_deltaR[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_kt[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jtdyn_z[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_intjet_multi[jets_.nref] = std::numeric_limits<int>::min();
      jets_.jt_girth[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_girth_new[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_thrust[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_LHA[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_pTD[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.jt_PLJPkT.push_back(jet_PLJPkT);
      jets_.jt_PLJPdR.push_back(jet_PLJPdR);
      jets_.jt_PLJPeta.push_back(jet_PLJPeta);
      jets_.jt_PLJPphi.push_back(jet_PLJPphi);
      throw(124);
    }
    while(jj.has_parents(j1,j2)){
      if(j1.perp() < j2.perp()) std::swap(j1,j2);
      double delta_R = j1.delta_R(j2);
      if(doHardestSplitMatching_ && isMC_) jets_.jtJetConstituent.push_back(j2);
      double k_t = j2.perp()*delta_R;
      z = j2.perp()/(j1.perp()+j2.perp());
      // double dyn = z*(1-z)*j2.perp()*pow(delta_R/rParam,mydynktcut);
      // double dyn = 1./output_jets[0].perp()*z*(1-z)*jj.perp()*pow(delta_R/rParam,mydynktcut);
      // std::cout << "Reco split " << nsplit << " with k_T=" << k_t << " z=" << z << " eta " << j2.eta() << " phi " << j2.phi() <<  std::endl;
      if(doPrimaryLJPReco_){
        jet_PLJPkT.push_back(k_t);
        jet_PLJPdR.push_back(delta_R);
        jet_PLJPeta.push_back(j2.eta());
        jet_PLJPphi.push_back(j2.phi());
      }
      if(k_t > dyn_kt){
        // dyn_var = dyn;
        highest_splitting = j2;
        dyn_kt = k_t;
        dyn_split = nsplit;
        dyn_deltaR = delta_R;
        dyn_z = z;
        dyn_eta = j2.eta();
        dyn_phi = j2.phi();
      }
      jj = j1;
      nsplit = nsplit+1;
    }
    // std::cout << eSum/jet.pt() << " vs given " << jet.chargedEmEnergyFraction() << std::endl; 
    // std::cout << highest_splitting.eta() << " " << highest_splitting.phi() << " highest reco splitting eta phi at " << dyn_split << std::endl;
    // jets_.jtdyn_var[jets_.nref] = dyn_var;
    jets_.jtdyn_split[jets_.nref] = dyn_split;
    jets_.jtdyn_eta[jets_.nref] = dyn_eta;
    jets_.jtdyn_phi[jets_.nref] = dyn_phi;
    jets_.jtdyn_deltaR[jets_.nref] = dyn_deltaR;
    jets_.jtdyn_kt[jets_.nref] = dyn_kt;
    jets_.jtdyn_z[jets_.nref] = dyn_z;
    jets_.jt_intjet_multi[jets_.nref] = intjet_multi;
    jets_.jt_girth[jets_.nref] = jet_girth;
    jets_.jt_girth_new[jets_.nref] = jet_girth_new;
    jets_.jt_thrust[jets_.nref] = jet_thrust;
    jets_.jt_LHA[jets_.nref] = jet_LHA;
    jets_.jt_pTD[jets_.nref] = jet_pTD;
    jets_.jt_PLJPkT.push_back(jet_PLJPkT);
    jets_.jt_PLJPdR.push_back(jet_PLJPdR);
    jets_.jt_PLJPeta.push_back(jet_PLJPeta);
    jets_.jt_PLJPphi.push_back(jet_PLJPphi);
  } 
  catch (fastjet::Error) { /*return -1;*/ }
  catch (Int_t MyNum){
    if(MyNum == 123)
      std::cout << "Whoops, seems the number of charged jet constituents is 0! Setting all reco jet split variables to numeric min." << std::endl;
    if(MyNum == 124)
      std::cout << "Jet does not have any parents, out of the loop!" << std::endl;
  }
}

void HiInclusiveJetSubstructure::IterativeDeclusteringGen(double groom_type, double groom_combine, const reco::GenJet& jet,fastjet::PseudoJet *sub1,fastjet::PseudoJet *sub2)
{
  Int_t intjet_multi = 0;
  float jet_girth = 0;
  float jet_girth_new = 0;
  float jet_thrust = 0;
  float jet_LHA = 0;
  float jet_pTD = 0;
  std::vector<float> jet_PLJPkT = {};
  std::vector<float> jet_PLJPdR = {};
  std::vector<float> jet_PLJPeta = {};
  std::vector<float> jet_PLJPphi = {};
  double nsplit = 0;
  // double dyn_theta = 0;
  double dyn_kt = std::numeric_limits<double>::min();
  double dyn_eta = 0;
  double dyn_phi = 0;

  Int_t dyn_split = 0;
  double dyn_deltaR = 0;
  double z = 0;
  double dyn_z = 0;
  double jet_radius_ca = 1.0;
  fastjet::JetDefinition jet_def(fastjet::genkt_algorithm,jet_radius_ca,0,static_cast<fastjet::RecombinationScheme>(0), fastjet::Best);
  fastjet::PseudoJet myjet;
  fastjet::PseudoJet mypart;
  myjet.reset(jet.p4().px(),jet.p4().py(),jet.p4().pz(),jet.p4().e());  
    // Reclustering jet constituents with new algorithm
  try{
    std::vector<fastjet::PseudoJet> particles = {};                         
    auto daughters = jet.getJetConstituents();
    for(auto it = daughters.begin(); it!=daughters.end(); ++it){
      //if we want only charged constituents and the daughter charge is 0, skip it
      if(doChargedConstOnly_ && (**it).charge()==0) continue;
      particles.push_back(fastjet::PseudoJet((**it).px(), (**it).py(), (**it).pz(), (**it).energy()));
      mypart.reset((**it).px(), (**it).py(), (**it).pz(), (**it).energy());
      double frac_dR = mypart.delta_R(myjet)/rParam;
      double frac_pt = mypart.perp()/myjet.perp();
      intjet_multi++;
      jet_girth     += mypart.perp()*mypart.delta_R(myjet)/myjet.perp();
      jet_girth_new += frac_pt*frac_dR;
      jet_thrust    += frac_pt*frac_dR*frac_dR;
      jet_LHA       += frac_pt*sqrt(frac_dR);
      jet_pTD       += frac_pt*frac_pt;
    }
    // std::cout << "Particle container has " << particles.size() << " reco particles" << std::endl;
    if(particles.empty()){
      // jets_.jtdyn_var[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.refdyn_split[jets_.nref] = std::numeric_limits<int>::min();
      jets_.refdyn_eta[jets_.nref] = -std::numeric_limits<double>::max();
      jets_.refdyn_phi[jets_.nref] = -std::numeric_limits<double>::max();
      jets_.refdyn_deltaR[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.refdyn_kt[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.refdyn_z[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_intjet_multi[jets_.nref] = std::numeric_limits<int>::min();
      jets_.ref_girth[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_girth_new[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_thrust[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_LHA[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_pTD[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_PLJPkT.push_back(jet_PLJPkT);
      jets_.ref_PLJPdR.push_back(jet_PLJPdR);
      jets_.ref_PLJPeta.push_back(jet_PLJPeta);
      jets_.ref_PLJPphi.push_back(jet_PLJPphi);
      throw(123);
    }
    // std::cout << "Clustering " << particles.size() << " number of truth particles" << std::endl;
    fastjet::ClusterSequence csiter(particles, jet_def);
    std::vector<fastjet::PseudoJet> output_jets = csiter.inclusive_jets(0);
    output_jets = sorted_by_pt(output_jets);

    fastjet::PseudoJet jj = output_jets[0];
    fastjet::PseudoJet j1;
    fastjet::PseudoJet j2;
    fastjet::PseudoJet highest_splitting;
    if(!jj.has_parents(j1,j2)){
      jets_.refdyn_split[jets_.nref] = std::numeric_limits<int>::min();
      jets_.refdyn_eta[jets_.nref] = -std::numeric_limits<double>::max();
      jets_.refdyn_phi[jets_.nref] = -std::numeric_limits<double>::max();
      jets_.refdyn_deltaR[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.refdyn_kt[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.refdyn_z[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_intjet_multi[jets_.nref] = std::numeric_limits<int>::min();
      jets_.ref_girth[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_girth_new[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_thrust[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_LHA[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_pTD[jets_.nref] = - std::numeric_limits<double>::max();
      jets_.ref_PLJPkT.push_back(jet_PLJPkT);
      jets_.ref_PLJPdR.push_back(jet_PLJPdR);
      jets_.ref_PLJPeta.push_back(jet_PLJPeta);
      jets_.ref_PLJPphi.push_back(jet_PLJPphi);
      throw(124);
    }
    while(jj.has_parents(j1,j2)){
      if(j1.perp() < j2.perp()) std::swap(j1,j2);
      double delta_R = j1.delta_R(j2);
      if(doHardestSplitMatching_ && isMC_) jets_.refJetConstituent.push_back(j2);
      double k_t = j2.perp()*delta_R;
      z = j2.perp()/(j1.perp()+j2.perp());
      if(doPrimaryLJPTruth_){
        jet_PLJPkT.push_back(k_t);
        jet_PLJPdR.push_back(delta_R);
        jet_PLJPeta.push_back(j2.eta());
        jet_PLJPphi.push_back(j2.phi());
      }
      
      // std::cout << "Truth split " << nsplit << " with k_T=" << k_t << " z=" << z <<  " eta " << j2.eta() << " phi " << j2.phi() <<  std::endl;
      if(k_t > dyn_kt){
        highest_splitting = j2;
        dyn_kt = k_t;
        dyn_split = nsplit;
        dyn_deltaR = delta_R;
        dyn_z = z;
        dyn_eta = j2.eta();
        dyn_phi = j2.phi();
      }
      jj = j1;
      nsplit = nsplit+1;
    }
    // std::cout << highest_splitting.eta() << " " << highest_splitting.phi() << " highest truth splitting eta phi at " << dyn_split << std::endl;
    jets_.refdyn_split[jets_.nref] = dyn_split;
    jets_.refdyn_eta[jets_.nref] = dyn_eta;
    jets_.refdyn_phi[jets_.nref] = dyn_phi;
    jets_.refdyn_deltaR[jets_.nref] = dyn_deltaR;
    jets_.refdyn_kt[jets_.nref] = dyn_kt;
    jets_.refdyn_z[jets_.nref] = dyn_z;
    jets_.ref_intjet_multi[jets_.nref] = intjet_multi;
    jets_.ref_girth[jets_.nref] = jet_girth;
    jets_.ref_girth_new[jets_.nref] = jet_girth_new;
    jets_.ref_thrust[jets_.nref] = jet_thrust;
    jets_.ref_LHA[jets_.nref] = jet_LHA;
    jets_.ref_pTD[jets_.nref] = jet_pTD;
    jets_.ref_PLJPkT.push_back(jet_PLJPkT);
    jets_.ref_PLJPdR.push_back(jet_PLJPdR);
      jets_.ref_PLJPeta.push_back(jet_PLJPeta);
      jets_.ref_PLJPphi.push_back(jet_PLJPphi);
  } 
  catch (fastjet::Error) { /*return -1;*/ }
  catch (Int_t MyNum){
    if(MyNum == 123)
      std::cout << "Whoops, seems the number of charged jet constituents is 0! Setting all gen jet split variables to numeric min." << std::endl;
    if(MyNum == 124)
      std::cout << "Jet does not have any parents, out of the loop!" << std::endl;
  }
}
// My dream is indefinitely on hold :(
// template<typename T>
// void HiInclusiveJetSubstructure::TemplateDeclustering(Bool_t recoLevel, double groom_type, T& jet)
// {
//   TRandom *rand_track_sel = new TRandom3(0);
//   Int_t intjet_multi = 0;
//   float jet_girth = 0;
//   Int_t nsplit = 0;
//   double dyn_kt = std::numeric_limits<double>::min();
//   Int_t dyn_split = 0;
//   double z = 0;
//   double dyn_deltaR = 0;
//   // double dyn_var = std::numeric_limits<double>::min();
//   double dyn_z = 0;
//   fastjet::PseudoJet myjet;
//   fastjet::PseudoJet mypart;
//   myjet.reset(jet.p4().px(),jet.p4().py(),jet.p4().pz(),jet.p4().e());
//   // Reclustering jet constituents with new algorithm
//   try{
//     std::vector<fastjet::PseudoJet> particles = {};                         
//     auto daughters = jet.getJetConstituents();
//         // std::cout << "Number of pfCand " << pfCandidates.size() << std::endl;
//     // std::vector<int> vec_jet_consituent_charge;
//     for(auto it = daughters.begin(); it!=daughters.end(); ++it){
//       //if we want only charged constituents and the daughter charge is 0, skip it
//       if(doChargedConstOnly_ && (**it).charge()==0) continue;
//       double PFE_scale = 1.;
//       //if it is MC, rescale the 4-momentum of the charged particles (we accept only them above) by pfCCES(+-1%)
//       if(isMC_ && recoLevel && (**it).charge()!=0) PFE_scale = pfChargedCandidateEnergyScale_;
//       //shouldn't go into else in charged MC case 
//       else if(isMC_ && recoLevel && (**it).charge()==0) PFE_scale = pfNeutralCandidateEnergyScale_;
//       //vary tracking efficiency - drop ~4% of particles within the jet
//       if(isMC_ && recoLevel && doTrackVariation_){
//         // std::cout << "doing the track variation" << std::endl;
//         if(rand_track_sel->Uniform(0,1)<0.05) continue;
//       }
//       // std::cout << "Rescaling charged pfCand energy by " << PFE_scale << std::endl;
//       particles.push_back(fastjet::PseudoJet((**it).px()*PFE_scale, (**it).py()*PFE_scale, (**it).pz()*PFE_scale, (**it).energy()*PFE_scale));
//       mypart.reset((**it).px()*PFE_scale, (**it).py()*PFE_scale, (**it).pz()*PFE_scale, (**it).energy()*PFE_scale);
//       intjet_multi++;
//       jet_girth += mypart.perp()*mypart.delta_R(myjet)/myjet.perp();

//     }
//     //Call substructure loop function here

//     // std::cout << "Particle container has " << particles.size() << " reco particles" << std::endl;
//     if(particles.empty()){
//       // jets_.jtdyn_var[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jtdyn_split[jets_.nref] = std::numeric_limits<int>::min();
//       jets_.jtdyn_deltaR[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jtdyn_kt[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jtdyn_z[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jt_intjet_multi[jets_.nref] = std::numeric_limits<int>::min();
//       jets_.jt_girth[jets_.nref] = - std::numeric_limits<double>::max();
//       throw(123);
//     }
//     // std::cout << "Clustering " << particles.size() << " number of reco particles" << std::endl;
//     fastjet::ClusterSequence csiter(particles, jet_def);
//     std::vector<fastjet::PseudoJet> output_jets = csiter.inclusive_jets(0);
//     output_jets = sorted_by_pt(output_jets);
//     fastjet::PseudoJet jj = output_jets[0];
//     fastjet::PseudoJet j1;
//     fastjet::PseudoJet j2;
//     fastjet::PseudoJet highest_splitting;
//     if(!jj.has_parents(j1,j2)){
//       jets_.jtdyn_split[jets_.nref] = std::numeric_limits<int>::min();
//       jets_.jtdyn_deltaR[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jtdyn_kt[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jtdyn_z[jets_.nref] = - std::numeric_limits<double>::max();
//       jets_.jt_intjet_multi[jets_.nref] = std::numeric_limits<int>::min();
//       jets_.jt_girth[jets_.nref] = - std::numeric_limits<double>::max();
//       throw(124);
//     }
//     while(jj.has_parents(j1,j2)){
//       if(j1.perp() < j2.perp()) std::swap(j1,j2);
//       double delta_R = j1.delta_R(j2);
//       if(doHardestSplitMatching_ && isMC_) jets_.jtJetConstituent.push_back(j2);
//       double k_t = j2.perp()*delta_R;
//       z = j2.perp()/(j1.perp()+j2.perp());
//       // double dyn = z*(1-z)*j2.perp()*pow(delta_R/rParam,mydynktcut);
//       // double dyn = 1./output_jets[0].perp()*z*(1-z)*jj.perp()*pow(delta_R/rParam,mydynktcut);
//       // std::cout << "Reco split " << nsplit << " with k_T=" << k_t << " z=" << z << " eta " << j2.eta() << " phi " << j2.phi() <<  std::endl;
//       if(k_t > dyn_kt){
//         // dyn_var = dyn;
//         highest_splitting = j2;
//         dyn_kt = k_t;
//         dyn_split = nsplit;
//         dyn_deltaR = delta_R;
//         dyn_z = z;
//       }
//       jj = j1;
//       nsplit = nsplit+1;
//     }

//     // std::cout << highest_splitting.eta() << " " << highest_splitting.phi() << " highest reco splitting eta phi at " << dyn_split << std::endl;
//     // jets_.jtdyn_var[jets_.nref] = dyn_var;
//     jets_.jtdyn_split[jets_.nref] = dyn_split;
//     jets_.jtdyn_deltaR[jets_.nref] = dyn_deltaR;
//     jets_.jtdyn_kt[jets_.nref] = dyn_kt;
//     jets_.jtdyn_z[jets_.nref] = dyn_z;
//     jets_.jt_intjet_multi[jets_.nref] = intjet_multi;
//     jets_.jt_girth[jets_.nref] = jet_girth;
//   } 
//   catch (fastjet::Error) { /*return -1;*/ }
//   catch (Int_t MyNum){
//     if(MyNum == 123)
//       std::cout << "Whoops, seems the number of charged jet constituents is 0! Setting all reco jet split variables to numeric min." << std::endl;
//     if(MyNum == 124)
//       std::cout << "Jet does not have any parents, out of the loop!" << std::endl;
//   }
// }

// void HiInclusiveJetSubstructure::ClusterConstituents(std::vector<fastjet::PseudoJet> particles, Bool_t recoLevel){
//   double jet_radius_ca = 1.0;
//   fastjet::JetDefinition jet_def(fastjet::genkt_algorithm,jet_radius_ca,0,static_cast<fastjet::RecombinationScheme>(0), fastjet::Best);
  
// }

//maybe there is a more elegant way than the one below for matching...
void HiInclusiveJetSubstructure::RecoTruthSplitMatching(std::vector<fastjet::PseudoJet> &constituents_level1, fastjet::PseudoJet &hardest_level2, bool *bool_array, int *hardest_level1_split){
    //for now only include geometric matching, maybe consider pt/z, Lund plane location, etc...
  float min_dR = std::numeric_limits<float>::max();
  size_t closest_level1 = 0;
  // std::cout << "Starting loop over " << constituents_level1.size() << " particles" << std::endl;
  for(size_t i{0};i<constituents_level1.size();++i){
    float dR = constituents_level1.at(i).delta_R(hardest_level2);
    if(min_dR > dR){
      closest_level1 = i;
      min_dR = dR;
    }
  }
  // std::cout << "Compare particle " << static_cast<int>(closest_level1) << " with hardest split " << hardest_level1_split[jets_.nref] << std::endl;
  if(static_cast<int>(closest_level1) == hardest_level1_split[jets_.nref] ){
    bool_array[jets_.nref] = true;
  }
  else{
    // std::cout << "Sorry, closest pair is " << min_dR << " away with index " << static_cast<int>(closest_level1) << " as opposed to " << hardest_level1_split[jets_.nref] << std::endl;
    bool_array[jets_.nref] = false;
  }
}

float HiInclusiveJetSubstructure::ReadJetAsymmMap(float eta, float phi, TH2F Asymm_map){
  float result =  Asymm_map.GetBinContent(Asymm_map.GetXaxis()->FindBin(eta),Asymm_map.GetYaxis()->FindBin(phi));
  if( result < -0.15 ){
    result = -0.15;
  }
  if( result > 0.15 ){
    result = 0.15;
  }
  return result;
}

void HiInclusiveJetSubstructure::TruthRecoRecoTruthMatching(){
  // std::cout << jets_.jtdyn_split[jets_.nref] << " " << jets_.refdyn_split[jets_.nref] << " numbers of highest splits" << std::endl;
  if( jets_.jtdyn_split[jets_.nref] == std::numeric_limits<int>::min() || jets_.refdyn_split[jets_.nref] == std::numeric_limits<int>::min() || jets_.jtJetConstituent.size() == 0 || jets_.refJetConstituent.size() == 0 ){
    jets_.refdyn_isClosestToReco[jets_.nref] = false;
    jets_.jtdyn_isClosestToTruth[jets_.nref] = false;
    jets_.jtdyn_refdyn_dR[jets_.nref] = std::numeric_limits<float>::max();
    return;
  }
  //mind how the split number is defined in the reclustering
  fastjet::PseudoJet hardest_R_split = jets_.jtJetConstituent.at(jets_.jtdyn_split[jets_.nref]);
  fastjet::PseudoJet hardest_T_split = jets_.refJetConstituent.at(jets_.refdyn_split[jets_.nref]);
  // std::cout << hardest_R_split.eta() << " " << hardest_R_split.phi() << " hardest reco  splitting in matching" << std::endl;
  // std::cout << hardest_T_split.eta() << " " << hardest_T_split.phi() << " hardest truth splitting in matching" << std::endl;
  // std::cout << "Angle between hardest splits is dR = " << hardest_R_split.delta_R(hardest_T_split) << std::endl;
  jets_.jtdyn_refdyn_dR[jets_.nref] = hardest_R_split.delta_R(hardest_T_split);
  // std::cout << "truth loop" << std::endl;
  RecoTruthSplitMatching(jets_.refJetConstituent, hardest_R_split, jets_.refdyn_isClosestToReco, jets_.refdyn_split);
  // std::cout << "reco loop" << std::endl;
  RecoTruthSplitMatching(jets_.jtJetConstituent,  hardest_T_split, jets_.jtdyn_isClosestToTruth, jets_.jtdyn_split);
}

int HiInclusiveJetSubstructure::getPFJetMuon(const pat::Jet& pfJet, const reco::PFCandidateCollection *pfCandidateColl)
{

  int pfMuonIndex = -1;
  float ptMax = 0.;

  for(unsigned icand=0;icand<pfCandidateColl->size(); icand++) {
    const reco::PFCandidate& pfCandidate = pfCandidateColl->at(icand);
    int id = pfCandidate.particleId();
    if(abs(id) != 3) continue;
    if(reco::deltaR(pfJet,pfCandidate)>0.5) continue;

    double pt =  pfCandidate.pt();
    if(pt>ptMax){
      ptMax = pt;
      pfMuonIndex = (int) icand;
    }
  }

  return pfMuonIndex;
}

void HiInclusiveJetSubstructure::LookThroughJetSplits(fastjet::PseudoJet jj, int i=1){
  fastjet::PseudoJet j1;
  fastjet::PseudoJet j2;
  if(i==1)
    std::cout << "primary" << std::endl;
  else
    std::cout << "secondary" << std::endl;

  if(jj.has_parents(j1,j2)){
    // jj.has_parents(j1,j2);
    // std::cout << "here" << std::endl;
    
    LookThroughJetSplits(j2, 2);
    LookThroughJetSplits(j1, 1);
  }
}


double HiInclusiveJetSubstructure::getPtRel(const reco::PFCandidate& lep, const pat::Jet& jet )
{
  float lj_x = jet.p4().px();
  float lj_y = jet.p4().py();
  float lj_z = jet.p4().pz();

    // absolute values squared
  float lj2  = lj_x*lj_x+lj_y*lj_y+lj_z*lj_z;
  float lep2 = lep.px()*lep.px()+lep.py()*lep.py()+lep.pz()*lep.pz();

    // projection vec(mu) to lepjet axis
  float lepXlj = lep.px()*lj_x+lep.py()*lj_y+lep.pz()*lj_z;

    // absolute value squared and normalized
  float pLrel2 = lepXlj*lepXlj/lj2;

    // lep2 = pTrel2 + pLrel2
  float pTrel2 = lep2-pLrel2;

  return (pTrel2 > 0) ? std::sqrt(pTrel2) : 0.0;
}

// Recursive function, but this version gets called only the first time

void HiInclusiveJetSubstructure::saveDaughters(const reco::GenParticle &gen){

  for(unsigned i = 0; i<gen.numberOfDaughters(); i++){
    const reco::Candidate & daughter = *gen.daughter(i);
    double daughterPt = daughter.pt();
    if(daughterPt<1.) continue;
    double daughterEta = daughter.eta();
    if(fabs(daughterEta)>3.) continue;
    int daughterPdgId = daughter.pdgId();
    int daughterStatus = daughter.status();
        // Special case when b->b+string, both b and string contain all daughters, so only take the string
    if(gen.pdgId()==daughterPdgId && gen.status()==3 && daughterStatus==2) continue;

        // cheesy way of finding strings which were already used
    if(daughter.pdgId()==92){
      for(unsigned ist = 0;ist<usedStringPts.size();ist++){
       if(fabs(daughter.pt() - usedStringPts[ist]) < 0.0001) return;
     }
     usedStringPts.push_back(daughter.pt());
   }
   jets_.bJetIndex[jets_.bMult] = jets_.nref;
   jets_.bStatus[jets_.bMult] = daughterStatus;
   jets_.bVx[jets_.bMult] = daughter.vx();
   jets_.bVy[jets_.bMult] = daughter.vy();
   jets_.bVz[jets_.bMult] = daughter.vz();
   jets_.bPt[jets_.bMult] = daughterPt;
   jets_.bEta[jets_.bMult] = daughterEta;
   jets_.bPhi[jets_.bMult] = daughter.phi();
   jets_.bPdg[jets_.bMult] = daughterPdgId;
   jets_.bChg[jets_.bMult] = daughter.charge();
   jets_.bMult++;
   saveDaughters(daughter);
 }
}

// This version called for all subsequent calls
void HiInclusiveJetSubstructure::saveDaughters(const reco::Candidate &gen){

  for(unsigned i = 0; i<gen.numberOfDaughters(); i++){
    const reco::Candidate & daughter = *gen.daughter(i);
    double daughterPt = daughter.pt();
    if(daughterPt<1.) continue;
    double daughterEta = daughter.eta();
    if(fabs(daughterEta)>3.) continue;
    int daughterPdgId = daughter.pdgId();
    int daughterStatus = daughter.status();
        // Special case when b->b+string, both b and string contain all daughters, so only take the string
    if(gen.pdgId()==daughterPdgId && gen.status()==3 && daughterStatus==2) continue;

        // cheesy way of finding strings which were already used
    if(daughter.pdgId()==92){
      for(unsigned ist=0;ist<usedStringPts.size();ist++){
        if(fabs(daughter.pt() - usedStringPts[ist]) < 0.0001) return;
      }
      usedStringPts.push_back(daughter.pt());
    }

    jets_.bJetIndex[jets_.bMult] = jets_.nref;
    jets_.bStatus[jets_.bMult] = daughterStatus;
    jets_.bVx[jets_.bMult] = daughter.vx();
    jets_.bVy[jets_.bMult] = daughter.vy();
    jets_.bVz[jets_.bMult] = daughter.vz();
    jets_.bPt[jets_.bMult] = daughterPt;
    jets_.bEta[jets_.bMult] = daughterEta;
    jets_.bPhi[jets_.bMult] = daughter.phi();
    jets_.bPdg[jets_.bMult] = daughterPdgId;
    jets_.bChg[jets_.bMult] = daughter.charge();
    jets_.bMult++;
    saveDaughters(daughter);
  }
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetSubstructure::analyzeSubjets(const reco::Jet& jet) {

  std::vector<float> sjpt;
  std::vector<float> sjeta;
  std::vector<float> sjphi;
  std::vector<float> sjm;
  if(jet.numberOfDaughters()>0) {
    for (unsigned k = 0; k < jet.numberOfDaughters(); ++k) {
      const reco::Candidate & dp = *jet.daughter(k);
      sjpt.push_back(dp.pt());
      sjeta.push_back(dp.eta());
      sjphi.push_back(dp.phi());
      sjm.push_back(dp.mass());
    }
  } 
  else {
    sjpt.push_back(-999.);
    sjeta.push_back(-999.);
    sjphi.push_back(-999.);
    sjm.push_back(-999.);
  }
  jets_.jtSubJetPt.push_back(sjpt);
  jets_.jtSubJetEta.push_back(sjeta);
  jets_.jtSubJetPhi.push_back(sjphi);
  jets_.jtSubJetM.push_back(sjm);  
}

//--------------------------------------------------------------------------------------------------
int HiInclusiveJetSubstructure::getGroomedGenJetIndex(const reco::GenJet& jet) const {

    //Find closest soft-dropped gen jet
  double drMin = 100;
  int imatch = -1;
  for(unsigned int i = 0 ; i < gensubjets_->size(); ++i) {
    const reco::Jet& mjet = (*gensubjets_)[i];
    double dr = deltaR(jet,mjet);
    if(dr < drMin){
      imatch = i;
      drMin = dr;
    }
  }
  return imatch;
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetSubstructure::analyzeRefSubjets(const reco::GenJet& jet) {

    //Find closest soft-dropped gen jet
  int imatch = getGroomedGenJetIndex(jet);

}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetSubstructure::analyzeGenSubjets(const reco::GenJet& jet) {
  //Find closest soft-dropped gen jet
  int imatch = getGroomedGenJetIndex(jet);
  double dr = 999.;

}

//--------------------------------------------------------------------------------------------------
// float HiInclusiveJetSubstructure::getAboveCharmThresh(TrackRefVector& selTracks, const TrackIPTagInfo& ipData, int sigOrVal)
// {

//     const double pdgCharmMass = 1.290;
//     btag::SortCriteria sc;
//     switch(sigOrVal){
//         case 1: //2d significance 
//         sc = reco::btag::IP2DSig;
//         case 2: //3d significance
//         sc = reco::btag::IP3DSig;
//         case 3:
//         sc = reco::btag::IP2DSig;  //values are not sortable!
//         case 4:
//         sc = reco::btag::IP3DSig;
// 	}
//     std::vector<std::size_t> indices = ipData.sortedIndexes(sc);
//     reco::TrackKinematics kin;
//     for(unsigned int i=0; i<indices.size(); i++){
//         size_t idx = indices[i];
//         const Track track = *(selTracks[idx]);
//         const btag::TrackIPData &data = ipData.impactParameterData()[idx];
//         kin.add(track);
//         if(kin.vectorSum().M() > pdgCharmMass){
//             switch(sigOrVal){
//                 case 1:
//                 return data.ip2d.significance();
//                 case 2:
//                 return data.ip3d.significance();
//                 case 3:
//                 return data.ip2d.value();
//                 case 4:
//                 return data.ip3d.value();
//             }
//         }
//     }
// 	return 0;
// }

DEFINE_FWK_MODULE(HiInclusiveJetSubstructure);
