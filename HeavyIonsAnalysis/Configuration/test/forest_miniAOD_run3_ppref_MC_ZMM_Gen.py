### HiForest Configuration
# Collisions: pp
# Type: MC
# Input: miniAOD

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_2024_ppRef_cff import Run3_2024_ppRef
process = cms.Process('HiForest', Run3_2024_ppRef)
process.options = cms.untracked.PSet()

#####################################################################################
# HiForest labelling info
#####################################################################################

process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 141X, mc")

#####################################################################################
# Input source
#####################################################################################

process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(
'/store/mc/RunIIIpp5p36Winter24MiniAOD/DYto2L-2Jets_MLL-50_TuneCP5_5p36TeV_amcatnloFXFX-pythia8/MINIAODSIM/141X_mcRun3_2024_realistic_ppRef5TeV_v7-v1/2810000/01137b1a-5ae8-4b09-9ace-73592ce0ac72.root'
#        '/store/user/bharikri/Run3MC_pp/MINIAOD/2024_Mar_21_Pythia8_ppRef_QCDPhoton30_PU10_TuneCP5_14_0_0_LLR/Pythia8_ppRef_QCDPhoton30_TuneCP5/2024_Mar_21_step3_RAW2DIGI_MINIAODSIM_Pythia8_ppRef_QCDPhoton30_PU10_TuneCP5_14_0_0/240326_082338/0000/step3_pp_673.root'
)
)

# Number of events we want to process, -1 = all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1000)
)

#####################################################################################
# Load Global Tag, Geometry, etc.
#####################################################################################

process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')

# TODO: Global tag complete guess from the list. Probably wrong. But does not crash
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '141X_mcRun3_2024_realistic_ppRef5TeV_v7', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag

# TODO: Old calibration here, might need to update
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC94X_2017pp_v2"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")

         )
      ])

#####################################################################################
# Define tree output
#####################################################################################

process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD.root"))

#####################################################################################
# Additional Reconstruction and Analysis: Main Body
#####################################################################################

#############################
# Jets
#############################
process.load("HeavyIonsAnalysis.JetAnalysis.ak4PFJetSequence_ppref_mc_cff")
#####################################################################################

############################
# Event Analysis
############################

# Load rho analyzer
process.load('HeavyIonsAnalysis.JetAnalysis.hiFJRhoAnalyzer_cff')
process.load('RecoHI.HiJetAlgos.hiFJGridEmptyAreaCalculator_cff')

# use data version to avoid PbPb MC
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_data_cfi')
process.hiEvtAnalyzer.Vertex = cms.InputTag("offlineSlimmedPrimaryVertices")
# Disable centrality for ppRef (no centrality in pp collisions)
process.hiEvtAnalyzer.doCentrality = cms.bool(False)
process.hiFJGridEmptyAreaCalculatorFinerBins.doCentrality = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlane = cms.bool(False)
process.hiEvtAnalyzer.doEvtPlaneFlat = cms.bool(False)
process.hiEvtAnalyzer.doMET = cms.bool(False)   ## Missing Et
process.hiEvtAnalyzer.doMC = cms.bool(True) # general MC info
process.hiEvtAnalyzer.doHiMC = cms.bool(False) # HI specific MC info
process.hiEvtAnalyzer.doHFfilters = cms.bool(False) # Disable HF filters for ppRef

process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')

process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.metFilters = process.skimanalysis.clone(hltresults = "TriggerResults::RECO")

# TODO: Many of these triggers are not available in the test file
from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_mc
process.hltobject.triggerNames = trigger_list_mc

# Gen particles
process.load('HeavyIonsAnalysis.EventAnalysis.HiGenAnalyzer_cfi')

#####################################################################################

#########################
# Track Analyzer
#########################
process.load('HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff')

#####################################################################################

#####################
# photons
######################
process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.doGenParticles = cms.bool(True)
process.ggHiNtuplizer.doMuons = cms.bool(False) # unpackedMuons collection not found from file
process.ggHiNtuplizer.useValMapIso = cms.bool(False) # True here causes seg fault
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")

####################################################################################
# muons
process.load("HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi")
process.unpackedMuons.muonSelectors = cms.vstring()
process.load("HeavyIonsAnalysis.MuonAnalysis.muonAnalyzer_cfi")
process.muonAnalyzer.doGen = cms.bool(True)

#########################
# Main analysis list
#########################

process.forest = cms.Path(
    process.HiForestInfo +
    process.hltanalysis *
    process.hiEvtAnalyzer *
    #process.hltobject +
    #process.l1object +
    process.HiGenParticleAna +
    #process.ggHiNtuplizer +
    process.trackSequencePP +
    process.unpackedMuons +
    process.muonAnalyzer +
    process.rhoSequence +
    process.metFilters
)

# Schedule definition
process.pAna = cms.EndPath(process.skimanalysis)

process.primaryVertexFilter = cms.EDFilter("VertexSelector",
    src = cms.InputTag("offlineSlimmedPrimaryVertices"),
    cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2"), #in miniADO trackSize()==0, however there is no influence.
    filter = cms.bool(True), # otherwise it won't filter the event
)

process.noscraping = cms.EDFilter("FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25),
    src = cms.untracked.InputTag("unpackedTracksAndVertices") # generalTracks collection not found
)

process.goodvertex = cms.Path(process.primaryVertexFilter+process.noscraping)

#####################################################################################
# Select the types of jets filled 
matchJets = True             # Enables q/g and heavy flavor jet identification in MC
jetPtMin = 15
jetAbsEtaMax = 2.5

# Choose which additional information is added to jet trees
doHIJetID = True             # Fill jet ID and composition information branches
doWTARecluster = False        # Add jet phi and eta for WTA axis
doBtagging  =  False         # Note that setting to True increases computing time a lot

# 0 means use original mini-AOD jets, otherwise use R value, e.g., 3,4,8
# Generator level jets in original miniAOD jets contain neutrinos
# You will need to do reclustering with R-value to get generator level jets without neutrinos
# Add all the values you want to process to the list
jetLabels = ["2", "3"]

# add candidate tagging for all selected jet radii
from HeavyIonsAnalysis.JetAnalysis.setupJets_ppRef_cff import candidateBtaggingMiniAOD

for jetLabel in jetLabels:
    candidateBtaggingMiniAOD(process, isMC = True, jetPtMin = jetPtMin, jetCorrLevels = ['L2Relative', 'L3Absolute'], doBtagging = doBtagging, labelR = jetLabel)

    # setup jet analyzer
    setattr(process,"ak"+jetLabel+"PFJetAnalyzer",process.ak4PFJetAnalyzer.clone())
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetTag = "selectedUpdatedPatJetsAK"+jetLabel+"PFCHSBtag"
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetName = 'ak'+jetLabel+'PF'
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").matchJets = matchJets
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").matchTag = 'patJetsAK'+jetLabel+'PFUnsubJets'
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").doBtagging = doBtagging
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").doHiJetID = doHIJetID
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").doWTARecluster = doWTARecluster
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetPtMin = jetPtMin
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetAbsEtaMax = cms.untracked.double(jetAbsEtaMax)
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").rParam = 0.4 if jetLabel=="0" else float(jetLabel)*0.1
    getattr(process,"ak"+jetLabel+"PFJetAnalyzer").jetFlavourInfos = "ak"+jetLabel+"PFFlavourInfos"
    if jetLabel != "0": getattr(process,"ak"+jetLabel+"PFJetAnalyzer").genjetTag = "ak"+jetLabel+"GenJetsReclusterNoNu"
    if doBtagging:
        getattr(process,"ak"+jetLabel+"PFJetAnalyzer").pfJetProbabilityBJetTag = cms.untracked.string("pfJetProbabilityBJetTagsAK"+jetLabel+"PFCHSBtag")
        getattr(process,"ak"+jetLabel+"PFJetAnalyzer").pfUnifiedParticleTransformerAK4JetTags = cms.untracked.string("pfUnifiedParticleTransformerAK4JetTagsAK"+jetLabel+"PFCHSBtag")
    process.forest += getattr(process,"ak"+jetLabel+"PFJetAnalyzer")

#from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel
#process.hltfilter = hltHighLevel.clone(
#    HLTPaths = ["HLT_PPRefL2SingleMu7_v6"
#    ]
#)
#
## selection of dimuons with mass in Z range
#process.muonSelectorForZMM = cms.EDFilter("MuonSelector",
#    src = cms.InputTag("slimmedMuons"),
#    cut = cms.string("(isPFMuon && isGlobalMuon) && pt > 15."),
#    filter = cms.bool(True)
#    )
#
#process.muonFilterForZMM = cms.EDFilter("MuonCountFilter",
#    src = cms.InputTag("muonSelectorForZMM"),
#    minNumber = cms.uint32(1)
#    )
#
#process.dimuonMassCutForZMM = cms.EDProducer("CandViewShallowCloneCombiner",
#    checkCharge = cms.bool(True),
#    cut = cms.string('50 < mass < 130'),
#    decay = cms.string("muonSelectorForZMM@+ muonSelectorForZMM@-")
#    )
#
#process.dimuonMassCutFilterForZMM = cms.EDFilter("CandViewCountFilter",
#    src = cms.InputTag("dimuonMassCutForZMM"),
#    minNumber = cms.uint32(1)
#    )
#
## Z->mumu skim sequence
#process.zMMSkimSequence = cms.Sequence(
#    process.hltfilter *
#    process.muonSelectorForZMM *
#    process.muonFilterForZMM *
#    process.dimuonMassCutForZMM *
#    process.dimuonMassCutFilterForZMM
#    )
#
#process.zMMSkimPath = cms.Path(process.zMMSkimSequence)
#process.skimanalysis.superFilters = cms.vstring("zMMSkimPath")
#
#for path in process.paths:
#    getattr(process, path)._seq = process.zMMSkimSequence * getattr(process,path)._seq



