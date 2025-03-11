### HiForest Configuration
# Input: miniAOD
# Type: mc

import FWCore.ParameterSet.Config as cms
from Configuration.Eras.Era_Run3_pp_on_PbPb_2023_cff import Run3_pp_on_PbPb_2023
process = cms.Process('HiForest', Run3_pp_on_PbPb_2023)

###############################################################################

# HiForest info
process.load("HeavyIonsAnalysis.EventAnalysis.HiForestInfo_cfi")
process.HiForestInfo.info = cms.vstring("HiForest, miniAOD, 132X, mc")

###############################################################################

# input files
process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
    fileNames = cms.untracked.vstring(),
)

# number of events to process, set to -1 to process all events
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
    )

###############################################################################

# load Global Tag, geometry, etc.
process.load('Configuration.Geometry.GeometryDB_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')


from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '132X_mcRun3_2023_realistic_HI_v10', '')
process.HiForestInfo.GlobalTagLabel = process.GlobalTag.globaltag
process.GlobalTag.snapshotTime = cms.string("9999-12-31 23:59:59.000")
process.GlobalTag.toGet.extend([
    cms.PSet(record = cms.string("BTagTrackProbability3DRcd"),
             tag = cms.string("JPcalib_MC103X_2018PbPb_v4"),
             connect = cms.string("frontier://FrontierProd/CMS_CONDITIONS")
         )
])


###############################################################################

# Define centrality binning
process.load("RecoHI.HiCentralityAlgos.CentralityBin_cfi")
process.centralityBin.Centrality = cms.InputTag("hiCentrality")
process.centralityBin.centralityVariable = cms.string("HFtowers")

###############################################################################

# root output
process.TFileService = cms.Service("TFileService",
    fileName = cms.string("HiForestMiniAOD_MC.root"))

# # edm output for debugging purposes
# process.output = cms.OutputModule(
#     "PoolOutputModule",
#     fileName = cms.untracked.string('HiForestEDM.root'),
#     outputCommands = cms.untracked.vstring(
#         'keep *',
#         )
#     )

# process.output_path = cms.EndPath(process.output)

###############################################################################

#############################
# Gen Analyzer
#############################
process.load('HeavyIonsAnalysis.EventAnalysis.HiGenAnalyzer_cfi')

# event analysis
process.load('HeavyIonsAnalysis.EventAnalysis.hltanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.particleFlowAnalyser_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hievtanalyzer_mc_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.skimanalysis_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.hltobject_cfi')
process.load('HeavyIonsAnalysis.EventAnalysis.l1object_cfi')
process.metFilters = process.skimanalysis.clone(hltresults = "TriggerResults::PAT")

#from HeavyIonsAnalysis.EventAnalysis.hltobject_cfi import trigger_list_mc
#process.hltobject.triggerNames = trigger_list_mc

################################
# electrons, photons, muons
process.load('HeavyIonsAnalysis.EGMAnalysis.ggHiNtuplizer_cfi')
process.ggHiNtuplizer.doGenParticles = cms.bool(True)
process.ggHiNtuplizer.doMuons = cms.bool(False)
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
################################
# jet reco sequence
process.load('HeavyIonsAnalysis.JetAnalysis.akCs4PFJetSequence_pponPbPb_mc_cff')
################################
# tracks
process.load("HeavyIonsAnalysis.TrackAnalysis.TrackAnalyzers_cff")
#muons
process.load("HeavyIonsAnalysis.MuonAnalysis.unpackedMuons_cfi")
process.load("HeavyIonsAnalysis.MuonAnalysis.muonAnalyzer_cfi")
process.muonAnalyzer.doGen = cms.bool(True)

###############################################################################

# ZDC analyzer
process.load('HeavyIonsAnalysis.ZDCAnalysis.QWZDC2018Producer_cfi')
process.load('HeavyIonsAnalysis.ZDCAnalysis.QWZDC2018RecHit_cfi')
process.load('HeavyIonsAnalysis.ZDCAnalysis.zdcanalyzer_cfi')

process.zdcanalyzer.doZDCRecHit = False
process.zdcanalyzer.doZDCDigi = True
process.zdcanalyzer.zdcRecHitSrc = cms.InputTag("QWzdcreco")
process.zdcanalyzer.zdcDigiSrc = cms.InputTag("hcalDigis", "ZDC")
process.zdcanalyzer.calZDCDigi = False
process.zdcanalyzer.verbose = False

###############################################################################
# main forest sequence
process.forest = cms.Path(
    process.HiForestInfo +
    process.centralityBin +
    process.hltanalysis +
#    process.hltobject +
#    process.l1object +
    process.trackSequencePbPb +
#    process.particleFlowAnalyser +
    process.hiEvtAnalyzer +
    process.HiGenParticleAna +
    process.ggHiNtuplizer +
    process.metFilters
#    process.zdcdigi +
#    process.QWzdcreco +
#    process.zdcanalyzer +
#    process.unpackedMuons +
#    process.muonAnalyzer
    )

#customisation

addR2Jets = True
addR2FlowJets = False
addR4Jets = False
addR4FlowJets = False
addR2JetsSubstructure = True
addR2FlowJetsSubstructure = False
matchJets = True             # Enables q/g and heavy flavor jet identification in MC
addCandidateTagging = False
doHIJetID = True             # Fill jet ID and composition information branches
doWTARecluster = True        # Add jet phi and eta for WTA axis

if addR2Jets or addR2FlowJets or addR4Jets or addR4FlowJets or addR2JetsSubstructure or addR2FlowJetsSubstructure:
    process.load("HeavyIonsAnalysis.JetAnalysis.extraJets_cff")
    from HeavyIonsAnalysis.JetAnalysis.clusterJetsFromMiniAOD_cff import setupHeavyIonJets
    process.load("HeavyIonsAnalysis.JetAnalysis.candidateBtaggingMiniAOD_cff")

    if addR2Jets :
        process.jetsR2 = cms.Sequence()
        jetName = 'akCs2PF'
        setupHeavyIonJets(jetName, process.jetsR2, process, isMC = 1, radius = 0.20, JECTag = 'AK2PF', doFlow = False, matchJets = matchJets)
        process.akCs2PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akCs2PFJetAnalyzer = process.akCs4PFJetAnalyzer.clone(jetTag = jetName + "patJets", jetName = jetName, genjetTag = "ak2GenJetsNoNu", matchJets = matchJets, matchTag = "ak2PFMatchingFor" + jetName + "patJets", doHiJetID = doHIJetID, doWTARecluster = doWTARecluster)
        process.forest += process.extraJetsMC * process.jetsR2 * process.akCs2PFJetAnalyzer

    if addR2FlowJets :
        process.jetsR2flow = cms.Sequence()
        jetName = 'akCs2PFFlow'
        setupHeavyIonJets(jetName, process.jetsR2flow, process, isMC = 1, radius = 0.20, JECTag = 'AK2PF', doFlow = True, matchJets = matchJets)
        process.akCs2PFFlowpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akFlowPuCs2PFJetAnalyzer = process.akCs4PFJetAnalyzer.clone(jetTag = jetName + "patJets", jetName = jetName, genjetTag = "ak2GenJetsNoNu", matchJets = matchJets, matchTag = "ak2PFMatchingFor" + jetName + "patJets", doHiJetID = doHIJetID, doWTARecluster = doWTARecluster)
        process.forest += process.extraFlowJetsMC * process.jetsR2flow * process.akFlowPuCs2PFJetAnalyzer

    if addR4Jets :
        # Recluster using an alias "0" in order not to get mixed up with the default AK4 collections
        process.jetsR4 = cms.Sequence()
        jetName = 'akCs0PF'
        setupHeavyIonJets(jetName, process.jetsR4, process, isMC = 1, radius = 0.40, JECTag = 'AK4PF', doFlow = False, matchJets = matchJets)
        process.akCs0PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akCs4PFJetAnalyzer.jetTag = jetName + 'patJets'
        process.akCs4PFJetAnalyzer.jetName = jetName
        process.akCs4PFJetAnalyzer.matchJets = matchJets
        process.akCs4PFJetAnalyzer.matchTag = 'ak4PFMatchingFor' + jetName + 'patJets'
        process.akCs4PFJetAnalyzer.doHiJetID = doHIJetID
        process.akCs4PFJetAnalyzer.doWTARecluster = doWTARecluster
        process.forest += process.extraJetsMC * process.jetsR4 * process.akCs4PFJetAnalyzer

    if addR4FlowJets :
        process.jetsR4flow = cms.Sequence()
        jetName = 'akCs4PFFlow'
        setupHeavyIonJets(jetName, process.jetsR4flow, process, isMC = 1, radius = 0.40, JECTag = 'AK4PF', doFlow = True, matchJets = matchJets)
        process.akCs4PFFlowpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akFlowPuCs4PFJetAnalyzer.jetTag = jetName + 'patJets'
        process.akFlowPuCs4PFJetAnalyzer.jetName = jetName
        process.akFlowPuCs4PFJetAnalyzer.matchJets = matchJets
        process.akFlowPuCs4PFJetAnalyzer.matchTag = 'ak4PFMatchingFor' + jetName + 'patJets'
        process.akFlowPuCs4PFJetAnalyzer.doHiJetID = doHIJetID
        process.akFlowPuCs4PFJetAnalyzer.doWTARecluster = doWTARecluster
        process.forest += process.extraFlowJetsMC * process.jetsR4flow * process.akFlowPuCs4PFJetAnalyzer

    if addR2JetsSubstructure :
        process.jetsR2Substructure = cms.Sequence()
        jetName = 'akCs2PFSubstructure'
        setupHeavyIonJets(jetName, process.jetsR2Substructure, process, isMC = 1, radius = 0.20, JECTag = 'AK2PF', doFlow = False, matchJets = matchJets)
        process.akCs2PFpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akCs2PFJetAnalyzerSubstructure = process.akCs4PFJetAnalyzerSubstructure.clone(jetTag = jetName + "patJets", jetName = jetName, genjetTag = "ak2GenJetsNoNu", matchJets = matchJets, matchTag = "ak2PFMatchingFor" + jetName + "patJets", doHiJetID = doHIJetID, doWTARecluster = doWTARecluster)
        process.forest += process.extraJetsMC * process.jetsR2Substructure * process.akCs2PFJetAnalyzerSubstructure

    if addR2FlowJetsSubstructure :
        process.jetsR2flowSubstructure = cms.Sequence()
        jetName = 'akCs2PFFlowSubstructure'
        setupHeavyIonJets(jetName, process.jetsR2flowSubstructure, process, isMC = 1, radius = 0.20, JECTag = 'AK2PF', doFlow = True, matchJets = matchJets)
        process.akCs2PFFlowpatJetCorrFactors.levels = ['L2Relative', 'L3Absolute']
        process.akFlowPuCs2PFJetAnalyzerSubstructure = process.akCs4PFJetAnalyzerSubstructure.clone(jetTag = jetName + "patJets", jetName = jetName, genjetTag = "ak2GenJetsNoNu", matchJets = matchJets, matchTag = "ak2PFMatchingFor" + jetName + "patJets", doHiJetID = doHIJetID, doWTARecluster = doWTARecluster)
        process.forest += process.extraFlowJetsMC * process.jetsR2flowSubstructure * process.akFlowPuCs2PFJetAnalyzerSubstructure


if addCandidateTagging:
    process.load("HeavyIonsAnalysis.JetAnalysis.candidateBtaggingMiniAOD_cff")

    from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
    updateJetCollection(
        process,
        jetSource = cms.InputTag('slimmedJets'),
        jetCorrections = ('AK4PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute']), 'None'),
        btagDiscriminators = ['pfCombinedSecondaryVertexV2BJetTags', 'pfDeepCSVDiscriminatorsJetTags:BvsAll', 'pfDeepCSVDiscriminatorsJetTags:CvsB', 'pfDeepCSVDiscriminatorsJetTags:CvsL'], ## to add discriminators,
        btagPrefix = 'TEST',
    )

    process.updatedPatJets.addJetCorrFactors = False
    process.updatedPatJets.discriminatorSources = cms.VInputTag(
        cms.InputTag('pfDeepCSVJetTags:probb'),
        cms.InputTag('pfDeepCSVJetTags:probc'),
        cms.InputTag('pfDeepCSVJetTags:probudsg'),
        cms.InputTag('pfDeepCSVJetTags:probbb'),
    )

    process.akCs4PFJetAnalyzer.jetTag = "updatedPatJets"

    process.forest.insert(1,process.candidateBtagging*process.updatedPatJets)


#########################
# Event Selection -> add the needed filters here
#########################

process.load('HeavyIonsAnalysis.EventAnalysis.collisionEventSelection_cff')
process.pclusterCompatibilityFilter = cms.Path(process.clusterCompatibilityFilter)
process.pprimaryVertexFilter = cms.Path(process.primaryVertexFilter)
process.load('HeavyIonsAnalysis.EventAnalysis.hffilter_cfi')
process.pphfCoincFilter2Th4 = cms.Path(process.phfCoincFilter2Th4)
process.pAna = cms.EndPath(process.skimanalysis)

# HLT trigger
import HLTrigger.HLTfilters.hltHighLevel_cfi
process.hltZEEHI = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone()
process.hltZEEHI.HLTPaths = ["HLT_HIMinimumBiasHF1AND_v3"]

# selection of valid vertex
process.primaryVertexFilterForZEE = cms.EDFilter("VertexSelector",
                                         src = cms.InputTag("offlineSlimmedPrimaryVertices"),
                                         cut = cms.string("!isFake && abs(z) <= 25 && position.Rho <= 2"), 
                                         filter = cms.bool(True),   # otherwise it won't filter the events
                                         )
# single lepton selector
process.goodElectronsForZEE = cms.EDFilter("PATElectronSelector",
                                   src = cms.InputTag("slimmedElectrons"),
                                   cut = cms.string("pt > 20")
                                   )

# dilepton selectors
process.diElectronsForZEE = cms.EDProducer("CandViewShallowCloneCombiner",
                                   checkCharge = cms.bool(True),
                                   #checkCharge = cms.bool(False),
                                   cut = cms.string("50 < mass < 130"),
                                   decay = cms.string("goodElectronsForZEE@+ goodElectronsForZEE@-"),
                                   #decay = cms.string("goodElectronsForZEE goodElectronsForZEE"),
                                   )

# dilepton counter
process.diElectronsFilterForZEE = cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag("diElectronsForZEE"),
                                       minNumber = cms.uint32(1)
                                       )

# Z->ee skim sequence
process.zEESkimSequence = cms.Sequence(
    process.hltZEEHI *
    process.primaryVertexFilterForZEE *
    process.goodElectronsForZEE * 
    process.diElectronsForZEE *
    process.diElectronsFilterForZEE
)

process.zEESkimPath = cms.Path(process.zEESkimSequence)

for path in process.paths:
    getattr(process, path)._seq = process.zEESkimSequence * getattr(process,path)._seq
