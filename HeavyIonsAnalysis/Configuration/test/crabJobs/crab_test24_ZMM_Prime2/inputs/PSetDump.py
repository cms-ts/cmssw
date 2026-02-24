import FWCore.ParameterSet.Config as cms
from HeterogeneousCore.CUDACore.SwitchProducerCUDA import SwitchProducerCUDA

process = cms.Process("HiForest")

process.source = cms.Source("PoolSource",
    duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
    fileNames = cms.untracked.vstring()
)
process.CondDB = cms.PSet(
    DBParameters = cms.PSet(
        authenticationPath = cms.untracked.string(''),
        authenticationSystem = cms.untracked.int32(0),
        connectionTimeout = cms.untracked.int32(0),
        messageLevel = cms.untracked.int32(0),
        security = cms.untracked.string('')
    ),
    connect = cms.string('')
)

process.HFRecalParameterBlock = cms.PSet(
    HFdepthOneParameterA = cms.vdouble(
        0.004123, 0.00602, 0.008201, 0.010489, 0.013379,
        0.016997, 0.021464, 0.027371, 0.034195, 0.044807,
        0.058939, 0.125497
    ),
    HFdepthOneParameterB = cms.vdouble(
        -4e-06, -2e-06, 0.0, 4e-06, 1.5e-05,
        2.6e-05, 6.3e-05, 8.4e-05, 0.00016, 0.000107,
        0.000425, 0.000209
    ),
    HFdepthTwoParameterA = cms.vdouble(
        0.002861, 0.004168, 0.0064, 0.008388, 0.011601,
        0.014425, 0.018633, 0.023232, 0.028274, 0.035447,
        0.051579, 0.086593
    ),
    HFdepthTwoParameterB = cms.vdouble(
        -2e-06, -0.0, -7e-06, -6e-06, -2e-06,
        1e-06, 1.9e-05, 3.1e-05, 6.7e-05, 1.2e-05,
        0.000157, -3e-06
    )
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
    output = cms.optional.untracked.allowed(cms.int32,cms.PSet)
)

process.maxLuminosityBlocks = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.nanoDQMIO_perLSoutput = cms.PSet(
    MEsToSave = cms.untracked.vstring(
        'Hcal/DigiTask/Occupancy/depth/depth1',
        'Hcal/DigiTask/Occupancy/depth/depth2',
        'Hcal/DigiTask/Occupancy/depth/depth3',
        'Hcal/DigiTask/Occupancy/depth/depth4',
        'Hcal/DigiTask/Occupancy/depth/depth5',
        'Hcal/DigiTask/Occupancy/depth/depth6',
        'Hcal/DigiTask/Occupancy/depth/depth7',
        'Hcal/DigiTask/Occupancy/depth/depthHO',
        'Hcal/DigiTask/OccupancyCut/depth/depth1',
        'Hcal/DigiTask/OccupancyCut/depth/depth2',
        'Hcal/DigiTask/OccupancyCut/depth/depth3',
        'Hcal/DigiTask/OccupancyCut/depth/depth4',
        'Hcal/DigiTask/OccupancyCut/depth/depth5',
        'Hcal/DigiTask/OccupancyCut/depth/depth6',
        'Hcal/DigiTask/OccupancyCut/depth/depth7',
        'Hcal/DigiTask/OccupancyCut/depth/depthHO',
        'EcalBarrel/EBOccupancyTask/EBOT digi occupancy',
        'EcalEndcap/EEOccupancyTask/EEOT digi occupancy EE -',
        'EcalEndcap/EEOccupancyTask/EEOT digi occupancy EE +',
        'EcalBarrel/EBOccupancyTask/EBOT DCC entries',
        'EcalEndcap/EEOccupancyTask/EEOT DCC entries',
        'Ecal/EventInfo/processedEvents',
        'PixelPhase1/Tracks/charge_PXBarrel',
        'PixelPhase1/Tracks/charge_PXForward',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_1',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_2',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_3',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_4',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_+1',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_+2',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_+3',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_-1',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_-2',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_-3',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_1',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_2',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_3',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_4',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_+1',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_+2',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_+3',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_-1',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_-2',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_-3',
        'HLT/Vertexing/hltPixelVertices/hltPixelVertices/goodvtxNbr',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_pt',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_pt',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_pt',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_pt',
        'HLT/Tracking/pixelTracks/GeneralProperties/Chi2Prob_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/Chi2oNDFVsEta_ImpactPoint_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/DeltaZToPVZoom_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/DistanceOfClosestApproachToPVVsPhi_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/DistanceOfClosestApproachToPVZoom_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/NumberOfTracks_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/Chi2Prob_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/Chi2oNDFVsEta_ImpactPoint_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/DeltaZToPVZoom_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/DistanceOfClosestApproachToPVVsPhi_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/DistanceOfClosestApproachToPVZoom_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/NumberOfTracks_GenTk',
        'HLT/Tracking/tracks/LUMIanalysis/NumberEventsVsLUMI',
        'HLT/Tracking/tracks/PUmonitoring/NumberEventsVsGoodPVtx',
        'PixelPhase1/Tracks/num_clusters_ontrack_PXBarrel',
        'PixelPhase1/Tracks/num_clusters_ontrack_PXForward',
        'PixelPhase1/Tracks/clusterposition_zphi_ontrack',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_1',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_2',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_3',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_4',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_+1',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_+2',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_+3',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_-1',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_-2',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_-3',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_1/NormalizedHitResiduals_TEC__wheel__1',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_2/NormalizedHitResiduals_TEC__wheel__2',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_3/NormalizedHitResiduals_TEC__wheel__3',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_4/NormalizedHitResiduals_TEC__wheel__4',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_5/NormalizedHitResiduals_TEC__wheel__5',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_6/NormalizedHitResiduals_TEC__wheel__6',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_7/NormalizedHitResiduals_TEC__wheel__7',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_8/NormalizedHitResiduals_TEC__wheel__8',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_9/NormalizedHitResiduals_TEC__wheel__9',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_1/NormalizedHitResiduals_TEC__wheel__1',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_2/NormalizedHitResiduals_TEC__wheel__2',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_3/NormalizedHitResiduals_TEC__wheel__3',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_4/NormalizedHitResiduals_TEC__wheel__4',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_5/NormalizedHitResiduals_TEC__wheel__5',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_6/NormalizedHitResiduals_TEC__wheel__6',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_7/NormalizedHitResiduals_TEC__wheel__7',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_8/NormalizedHitResiduals_TEC__wheel__8',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_9/NormalizedHitResiduals_TEC__wheel__9',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__1',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__2',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__3',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_4/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__4',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_5/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__5',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_6/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__6',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_7/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__7',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_8/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__8',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_9/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__9',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__1',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__2',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__3',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_4/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__4',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_5/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__5',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_6/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__6',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_7/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__7',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_8/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__8',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_9/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__9',
        'SiStrip/MechanicalView/TIB/layer_1/NormalizedHitResiduals_TIB__Layer__1',
        'SiStrip/MechanicalView/TIB/layer_2/NormalizedHitResiduals_TIB__Layer__2',
        'SiStrip/MechanicalView/TIB/layer_3/NormalizedHitResiduals_TIB__Layer__3',
        'SiStrip/MechanicalView/TIB/layer_4/NormalizedHitResiduals_TIB__Layer__4',
        'SiStrip/MechanicalView/TIB/layer_1/Summary_ClusterStoNCorr__OnTrack__TIB__layer__1',
        'SiStrip/MechanicalView/TIB/layer_2/Summary_ClusterStoNCorr__OnTrack__TIB__layer__2',
        'SiStrip/MechanicalView/TIB/layer_3/Summary_ClusterStoNCorr__OnTrack__TIB__layer__3',
        'SiStrip/MechanicalView/TIB/layer_4/Summary_ClusterStoNCorr__OnTrack__TIB__layer__4',
        'SiStrip/MechanicalView/TID/PLUS/wheel_1/NormalizedHitResiduals_TID__wheel__1',
        'SiStrip/MechanicalView/TID/PLUS/wheel_2/NormalizedHitResiduals_TID__wheel__2',
        'SiStrip/MechanicalView/TID/PLUS/wheel_3/NormalizedHitResiduals_TID__wheel__3',
        'SiStrip/MechanicalView/TID/MINUS/wheel_1/NormalizedHitResiduals_TID__wheel__1',
        'SiStrip/MechanicalView/TID/MINUS/wheel_2/NormalizedHitResiduals_TID__wheel__2',
        'SiStrip/MechanicalView/TID/MINUS/wheel_3/NormalizedHitResiduals_TID__wheel__3',
        'SiStrip/MechanicalView/TID/PLUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TID__PLUS__wheel__1',
        'SiStrip/MechanicalView/TID/PLUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TID__PLUS__wheel__2',
        'SiStrip/MechanicalView/TID/PLUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TID__PLUS__wheel__3',
        'SiStrip/MechanicalView/TID/MINUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TID__MINUS__wheel__1',
        'SiStrip/MechanicalView/TID/MINUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TID__MINUS__wheel__2',
        'SiStrip/MechanicalView/TID/MINUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TID__MINUS__wheel__3',
        'SiStrip/MechanicalView/TOB/layer_1/NormalizedHitResiduals_TOB__Layer__1',
        'SiStrip/MechanicalView/TOB/layer_2/NormalizedHitResiduals_TOB__Layer__2',
        'SiStrip/MechanicalView/TOB/layer_3/NormalizedHitResiduals_TOB__Layer__3',
        'SiStrip/MechanicalView/TOB/layer_4/NormalizedHitResiduals_TOB__Layer__4',
        'SiStrip/MechanicalView/TOB/layer_5/NormalizedHitResiduals_TOB__Layer__5',
        'SiStrip/MechanicalView/TOB/layer_6/NormalizedHitResiduals_TOB__Layer__6',
        'SiStrip/MechanicalView/TOB/layer_1/Summary_ClusterStoNCorr__OnTrack__TOB__layer__1',
        'SiStrip/MechanicalView/TOB/layer_2/Summary_ClusterStoNCorr__OnTrack__TOB__layer__2',
        'SiStrip/MechanicalView/TOB/layer_3/Summary_ClusterStoNCorr__OnTrack__TOB__layer__3',
        'SiStrip/MechanicalView/TOB/layer_4/Summary_ClusterStoNCorr__OnTrack__TOB__layer__4',
        'SiStrip/MechanicalView/TOB/layer_5/Summary_ClusterStoNCorr__OnTrack__TOB__layer__5',
        'SiStrip/MechanicalView/TOB/layer_6/Summary_ClusterStoNCorr__OnTrack__TOB__layer__6',
        'SiStrip/MechanicalView/MainDiagonal Position',
        'SiStrip/MechanicalView/NumberOfClustersInPixel',
        'SiStrip/MechanicalView/NumberOfClustersInStrip',
        'Tracking/TrackParameters/generalTracks/LSanalysis/Chi2oNDF_lumiFlag_GenTk',
        'Tracking/TrackParameters/generalTracks/LSanalysis/NumberOfRecHitsPerTrack_lumiFlag_GenTk',
        'Tracking/TrackParameters/generalTracks/LSanalysis/NumberOfTracks_lumiFlag_GenTk',
        'Tracking/TrackParameters/highPurityTracks/pt_1/GeneralProperties/SIPDxyToPV_GenTk',
        'Tracking/TrackParameters/highPurityTracks/pt_1/GeneralProperties/SIPDzToPV_GenTk',
        'Tracking/TrackParameters/highPurityTracks/pt_1/GeneralProperties/SIP3DToPV_GenTk',
        'Tracking/TrackParameters/generalTracks/HitProperties/NumberOfMissingOuterRecHitsPerTrack_GenTk',
        'Tracking/TrackParameters/generalTracks/HitProperties/NumberMORecHitsPerTrackVsPt_GenTk',
        'OfflinePV/offlinePrimaryVertices/tagVtxProb',
        'OfflinePV/offlinePrimaryVertices/tagType',
        'OfflinePV/Resolution/PV/pull_x',
        'OfflinePV/Resolution/PV/pull_y',
        'OfflinePV/Resolution/PV/pull_z',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_highPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_highPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_mediumPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_mediumPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_lowPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_lowPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_highPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_highPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_mediumPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_mediumPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_lowPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_lowPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Constituents',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Eta',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Eta_uncor',
        'JetMET/Jet/Cleanedak4PFJetsCHS/JetEnergyCorr',
        'JetMET/Jet/Cleanedak4PFJetsCHS/NJets',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Phi',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Phi_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Phi_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Pt',
        'JetMET/MET/pfMETT1/Cleaned/METSig',
        'JetMET/vertices',
        'JetMET/HIJetValidation/akCs4PFJets/SumPFPt',
        'JetMET/HIJetValidation/akPu4CaloJets/SumPFPt'
    )
)

process.options = cms.untracked.PSet(
    FailPath = cms.untracked.vstring(),
    IgnoreCompletely = cms.untracked.vstring(),
    Rethrow = cms.untracked.vstring(),
    SkipEvent = cms.untracked.vstring(),
    accelerators = cms.untracked.vstring('*'),
    allowUnscheduled = cms.obsolete.untracked.bool,
    canDeleteEarly = cms.untracked.vstring(),
    deleteNonConsumedUnscheduledModules = cms.untracked.bool(True),
    dumpOptions = cms.untracked.bool(False),
    emptyRunLumiMode = cms.obsolete.untracked.string,
    eventSetup = cms.untracked.PSet(
        forceNumberOfConcurrentIOVs = cms.untracked.PSet(
            allowAnyLabel_=cms.required.untracked.uint32
        ),
        numberOfConcurrentIOVs = cms.untracked.uint32(0)
    ),
    fileMode = cms.untracked.string('FULLMERGE'),
    forceEventSetupCacheClearOnNewRun = cms.untracked.bool(False),
    holdsReferencesToDeleteEarly = cms.untracked.VPSet(),
    makeTriggerResults = cms.obsolete.untracked.bool,
    modulesToIgnoreForDeleteEarly = cms.untracked.vstring(),
    numberOfConcurrentLuminosityBlocks = cms.untracked.uint32(0),
    numberOfConcurrentRuns = cms.untracked.uint32(1),
    numberOfStreams = cms.untracked.uint32(0),
    numberOfThreads = cms.untracked.uint32(1),
    printDependencies = cms.untracked.bool(False),
    sizeOfStackForThreadsInKB = cms.optional.untracked.uint32,
    throwIfIllegalParameter = cms.untracked.bool(True),
    wantSummary = cms.untracked.bool(False)
)

process.ZDC2018Pedestal_0 = cms.VPSet(
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM0'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM1'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM2'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM3'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM4'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM5'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM6'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM7'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM8'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM9'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM10'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM11'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM12'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM13'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM14'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_EM15'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD0'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD1'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD2'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD3'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD4'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD5'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD6'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD7'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD8'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD9'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD10'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD11'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD12'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD13'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD14'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_HAD15'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM0'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM1'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM2'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM3'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM4'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM5'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM6'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM7'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM8'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM9'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM10'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM11'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM12'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM13'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM14'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_EM15'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD0'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD1'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD2'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD3'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD4'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD5'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD6'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD7'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD8'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD9'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD10'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD11'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD12'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD13'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD14'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_HAD15'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD0'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD1'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD2'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD3'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD4'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD5'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD6'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD7'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD8'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD9'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD10'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD11'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD12'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD13'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD14'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCM_RPD15'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD0'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD1'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD2'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD3'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD4'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD5'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD6'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD7'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD8'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD9'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD10'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD11'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD12'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD13'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD14'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    ),
    cms.PSet(
        object = cms.untracked.string('hZDCP_RPD15'),
        ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
    )
)

process.gainFill7435 = cms.VPSet(
    cms.PSet(
        calib = cms.untracked.double(0.11),
        object = cms.untracked.string('hZDCP_EM1')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.11),
        object = cms.untracked.string('hZDCP_EM2')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.08),
        object = cms.untracked.string('hZDCP_EM3')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.1),
        object = cms.untracked.string('hZDCP_EM4')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.06),
        object = cms.untracked.string('hZDCP_EM5')
    ),
    cms.PSet(
        calib = cms.untracked.double(1.0),
        object = cms.untracked.string('hZDCP_HAD1')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.61),
        object = cms.untracked.string('hZDCP_HAD2')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.46),
        object = cms.untracked.string('hZDCP_HAD3')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.52),
        object = cms.untracked.string('hZDCP_HAD4')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.12),
        object = cms.untracked.string('hZDCM_EM1')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.12),
        object = cms.untracked.string('hZDCM_EM2')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.08),
        object = cms.untracked.string('hZDCM_EM3')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.1),
        object = cms.untracked.string('hZDCM_EM4')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.11),
        object = cms.untracked.string('hZDCM_EM5')
    ),
    cms.PSet(
        calib = cms.untracked.double(1.0),
        object = cms.untracked.string('hZDCM_HAD1')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.81),
        object = cms.untracked.string('hZDCM_HAD2')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.46),
        object = cms.untracked.string('hZDCM_HAD3')
    ),
    cms.PSet(
        calib = cms.untracked.double(0.58),
        object = cms.untracked.string('hZDCM_HAD4')
    ),
    cms.PSet(
        calib = cms.untracked.double(2.67),
        object = cms.untracked.string('Pscale')
    ),
    cms.PSet(
        calib = cms.untracked.double(4.45),
        object = cms.untracked.string('Mscale')
    )
)

process.PackedPFTowers = cms.EDProducer("PackedPFTowers",
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("packedPFCandidates"),
    useHF = cms.bool(True)
)


process.QWzdcreco = cms.EDProducer("QWZDC2018RecHit",
    ZDCCalib = cms.VPSet(
        cms.PSet(
            calib = cms.untracked.double(0.11),
            object = cms.untracked.string('hZDCP_EM1')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.11),
            object = cms.untracked.string('hZDCP_EM2')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.08),
            object = cms.untracked.string('hZDCP_EM3')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.1),
            object = cms.untracked.string('hZDCP_EM4')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.06),
            object = cms.untracked.string('hZDCP_EM5')
        ),
        cms.PSet(
            calib = cms.untracked.double(1.0),
            object = cms.untracked.string('hZDCP_HAD1')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.61),
            object = cms.untracked.string('hZDCP_HAD2')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.46),
            object = cms.untracked.string('hZDCP_HAD3')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.52),
            object = cms.untracked.string('hZDCP_HAD4')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.12),
            object = cms.untracked.string('hZDCM_EM1')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.12),
            object = cms.untracked.string('hZDCM_EM2')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.08),
            object = cms.untracked.string('hZDCM_EM3')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.1),
            object = cms.untracked.string('hZDCM_EM4')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.11),
            object = cms.untracked.string('hZDCM_EM5')
        ),
        cms.PSet(
            calib = cms.untracked.double(1.0),
            object = cms.untracked.string('hZDCM_HAD1')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.81),
            object = cms.untracked.string('hZDCM_HAD2')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.46),
            object = cms.untracked.string('hZDCM_HAD3')
        ),
        cms.PSet(
            calib = cms.untracked.double(0.58),
            object = cms.untracked.string('hZDCM_HAD4')
        ),
        cms.PSet(
            calib = cms.untracked.double(2.67),
            object = cms.untracked.string('Pscale')
        ),
        cms.PSet(
            calib = cms.untracked.double(4.45),
            object = cms.untracked.string('Mscale')
        )
    ),
    srcDetId = cms.untracked.InputTag("zdcdigi","DetId"),
    srcfC = cms.untracked.InputTag("zdcdigi","regularfC")
)


process.ak2PFUnsubJets = cms.EDProducer("FastjetJetProducer",
    Active_Area_Repeats = cms.int32(1),
    GhostArea = cms.double(0.01),
    Ghost_EtaMax = cms.double(5.0),
    Rho_EtaMax = cms.double(4.4),
    applyWeight = cms.bool(False),
    doAreaDiskApprox = cms.bool(False),
    doAreaFastjet = cms.bool(True),
    doPUOffsetCorr = cms.bool(False),
    doPVCorrection = cms.bool(False),
    doRhoFastjet = cms.bool(False),
    inputEMin = cms.double(0.0),
    inputEtMin = cms.double(0.0),
    jetAlgorithm = cms.string('AntiKt'),
    jetPtMin = cms.double(5.0),
    jetType = cms.string('PFJet'),
    maxBadEcalCells = cms.uint32(9999999),
    maxBadHcalCells = cms.uint32(9999999),
    maxProblematicEcalCells = cms.uint32(9999999),
    maxProblematicHcalCells = cms.uint32(9999999),
    maxRecoveredEcalCells = cms.uint32(9999999),
    maxRecoveredHcalCells = cms.uint32(9999999),
    minSeed = cms.uint32(14327),
    nSigmaPU = cms.double(1.0),
    rParam = cms.double(0.2),
    radiusPU = cms.double(0.5),
    src = cms.InputTag("packedPFCandidates"),
    srcPVs = cms.InputTag(""),
    useDeterministicSeed = cms.bool(True),
    voronoiRfact = cms.double(-0.9)
)


process.ak3PFUnsubJets = cms.EDProducer("FastjetJetProducer",
    Active_Area_Repeats = cms.int32(1),
    GhostArea = cms.double(0.01),
    Ghost_EtaMax = cms.double(5.0),
    Rho_EtaMax = cms.double(4.4),
    applyWeight = cms.bool(False),
    doAreaDiskApprox = cms.bool(False),
    doAreaFastjet = cms.bool(True),
    doPUOffsetCorr = cms.bool(False),
    doPVCorrection = cms.bool(False),
    doRhoFastjet = cms.bool(False),
    inputEMin = cms.double(0.0),
    inputEtMin = cms.double(0.0),
    jetAlgorithm = cms.string('AntiKt'),
    jetPtMin = cms.double(5.0),
    jetType = cms.string('PFJet'),
    maxBadEcalCells = cms.uint32(9999999),
    maxBadHcalCells = cms.uint32(9999999),
    maxProblematicEcalCells = cms.uint32(9999999),
    maxProblematicHcalCells = cms.uint32(9999999),
    maxRecoveredEcalCells = cms.uint32(9999999),
    maxRecoveredHcalCells = cms.uint32(9999999),
    minSeed = cms.uint32(14327),
    nSigmaPU = cms.double(1.0),
    rParam = cms.double(0.30000000000000004),
    radiusPU = cms.double(0.5),
    src = cms.InputTag("packedPFCandidates"),
    srcPVs = cms.InputTag(""),
    useDeterministicSeed = cms.bool(True),
    voronoiRfact = cms.double(-0.9)
)


process.akCs2PFJets = cms.EDProducer("CSJetProducer",
    Active_Area_Repeats = cms.int32(1),
    GhostArea = cms.double(0.005),
    Ghost_EtaMax = cms.double(6.5),
    Rho_EtaMax = cms.double(4.5),
    applyWeight = cms.bool(False),
    csAlpha = cms.double(2.0),
    csRParam = cms.double(-1.0),
    doAreaDiskApprox = cms.bool(False),
    doAreaFastjet = cms.bool(True),
    doFastJetNonUniform = cms.bool(True),
    doPUOffsetCorr = cms.bool(False),
    doPVCorrection = cms.bool(False),
    doRhoFastjet = cms.bool(True),
    etaMap = cms.InputTag("hiPuRho","mapEtaEdges"),
    inputEMin = cms.double(0.0),
    inputEtMin = cms.double(0.0),
    jetAlgorithm = cms.string('AntiKt'),
    jetCollInstanceName = cms.string('pfParticlesCs'),
    jetPtMin = cms.double(15),
    jetType = cms.string('PFJet'),
    maxBadEcalCells = cms.uint32(9999999),
    maxBadHcalCells = cms.uint32(9999999),
    maxProblematicEcalCells = cms.uint32(9999999),
    maxProblematicHcalCells = cms.uint32(9999999),
    maxRecoveredEcalCells = cms.uint32(9999999),
    maxRecoveredHcalCells = cms.uint32(9999999),
    minSeed = cms.uint32(14327),
    nExclude = cms.uint32(2),
    nSigmaPU = cms.double(1.0),
    puCenters = cms.vdouble(
        -5, -4, -3, -2, -1,
        0, 1, 2, 3, 4,
        5
    ),
    puWidth = cms.double(0.8),
    rParam = cms.double(0.2),
    radiusPU = cms.double(0.5),
    rho = cms.InputTag("hiPuRho","mapToRho"),
    rhoFlowFitParams = cms.InputTag("rhoModulationAkCs2PFJets","rhoFlowFitParams"),
    rhom = cms.InputTag("hiPuRho","mapToRhoM"),
    src = cms.InputTag("packedPFCandidates"),
    srcPVs = cms.InputTag(""),
    useDeterministicSeed = cms.bool(True),
    useExplicitGhosts = cms.bool(True),
    useModulatedRho = cms.bool(False),
    voronoiRfact = cms.double(-0.9),
    writeJetsWithConst = cms.bool(True)
)


process.akCs3PFJets = cms.EDProducer("CSJetProducer",
    Active_Area_Repeats = cms.int32(1),
    GhostArea = cms.double(0.005),
    Ghost_EtaMax = cms.double(6.5),
    Rho_EtaMax = cms.double(4.5),
    applyWeight = cms.bool(False),
    csAlpha = cms.double(2.0),
    csRParam = cms.double(-1.0),
    doAreaDiskApprox = cms.bool(False),
    doAreaFastjet = cms.bool(True),
    doFastJetNonUniform = cms.bool(True),
    doPUOffsetCorr = cms.bool(False),
    doPVCorrection = cms.bool(False),
    doRhoFastjet = cms.bool(True),
    etaMap = cms.InputTag("hiPuRho","mapEtaEdges"),
    inputEMin = cms.double(0.0),
    inputEtMin = cms.double(0.0),
    jetAlgorithm = cms.string('AntiKt'),
    jetCollInstanceName = cms.string('pfParticlesCs'),
    jetPtMin = cms.double(15),
    jetType = cms.string('PFJet'),
    maxBadEcalCells = cms.uint32(9999999),
    maxBadHcalCells = cms.uint32(9999999),
    maxProblematicEcalCells = cms.uint32(9999999),
    maxProblematicHcalCells = cms.uint32(9999999),
    maxRecoveredEcalCells = cms.uint32(9999999),
    maxRecoveredHcalCells = cms.uint32(9999999),
    minSeed = cms.uint32(14327),
    nExclude = cms.uint32(2),
    nSigmaPU = cms.double(1.0),
    puCenters = cms.vdouble(
        -5, -4, -3, -2, -1,
        0, 1, 2, 3, 4,
        5
    ),
    puWidth = cms.double(0.8),
    rParam = cms.double(0.30000000000000004),
    radiusPU = cms.double(0.5),
    rho = cms.InputTag("hiPuRho","mapToRho"),
    rhoFlowFitParams = cms.InputTag("rhoModulationAkCs3PFJets","rhoFlowFitParams"),
    rhom = cms.InputTag("hiPuRho","mapToRhoM"),
    src = cms.InputTag("packedPFCandidates"),
    srcPVs = cms.InputTag(""),
    useDeterministicSeed = cms.bool(True),
    useExplicitGhosts = cms.bool(True),
    useModulatedRho = cms.bool(False),
    voronoiRfact = cms.double(-0.9),
    writeJetsWithConst = cms.bool(True)
)


process.candidateVertexArbitrator = cms.EDProducer("CandidateVertexArbitrator",
    beamSpot = cms.InputTag("offlineBeamSpot"),
    dLenFraction = cms.double(0.333),
    dRCut = cms.double(0.4),
    distCut = cms.double(0.04),
    fitterRatio = cms.double(0.25),
    fitterSigmacut = cms.double(3),
    fitterTini = cms.double(256),
    maxTimeSignificance = cms.double(3.5),
    mightGet = cms.optional.untracked.vstring,
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    secondaryVertices = cms.InputTag("candidateVertexMerger"),
    sigCut = cms.double(5),
    trackMinLayers = cms.int32(4),
    trackMinPixels = cms.int32(1),
    trackMinPt = cms.double(0.4),
    tracks = cms.InputTag("packedPFCandidates")
)


process.candidateVertexMerger = cms.EDProducer("CandidateVertexMerger",
    maxFraction = cms.double(0.7),
    minSignificance = cms.double(2),
    secondaryVertices = cms.InputTag("inclusiveCandidateVertexFinder")
)


process.centralityBin = cms.EDProducer("CentralityBinProducer",
    Centrality = cms.InputTag("hiCentrality"),
    centralityVariable = cms.string('HFtowers'),
    nonDefaultGlauberModel = cms.string(''),
    pPbRunFlip = cms.uint32(99999999)
)


process.correctedElectrons = cms.EDProducer("CorrectedPatElectronProducer",
    centrality = cms.InputTag("centralityBin","HFtowers"),
    correctionFile = cms.string('HeavyIonsAnalysis/EGMAnalysis/data/SSHIRun2023A.dat'),
    epCombConfig = cms.PSet(
        ecalTrkRegressionConfig = cms.PSet(
            ebHighEtForestName = cms.ESInputTag("","electron_eb_ECALTRK"),
            ebLowEtForestName = cms.ESInputTag("","electron_eb_ECALTRK_lowpt"),
            eeHighEtForestName = cms.ESInputTag("","electron_ee_ECALTRK"),
            eeLowEtForestName = cms.ESInputTag("","electron_ee_ECALTRK_lowpt"),
            forceHighEnergyTrainingIfSaturated = cms.bool(False),
            lowEtHighEtBoundary = cms.double(50),
            rangeMaxHighEt = cms.double(3),
            rangeMaxLowEt = cms.double(3),
            rangeMinHighEt = cms.double(-1),
            rangeMinLowEt = cms.double(-1)
        ),
        ecalTrkRegressionUncertConfig = cms.PSet(
            ebHighEtForestName = cms.ESInputTag("","electron_eb_ECALTRK"),
            ebLowEtForestName = cms.ESInputTag("","electron_eb_ECALTRK_lowpt"),
            eeHighEtForestName = cms.ESInputTag("","electron_ee_ECALTRK"),
            eeLowEtForestName = cms.ESInputTag("","electron_ee_ECALTRK_lowpt"),
            forceHighEnergyTrainingIfSaturated = cms.bool(False),
            lowEtHighEtBoundary = cms.double(50),
            rangeMaxHighEt = cms.double(3),
            rangeMaxLowEt = cms.double(3),
            rangeMinHighEt = cms.double(-1),
            rangeMinLowEt = cms.double(-1)
        ),
        maxEPDiffInSigmaForComb = cms.double(15),
        maxEcalEnergyForComb = cms.double(200),
        maxRelTrkMomErrForComb = cms.double(10),
        minEOverPForComb = cms.double(0.025)
    ),
    mightGet = cms.optional.untracked.vstring,
    minPt = cms.double(20),
    semiDeterministic = cms.bool(True),
    src = cms.InputTag("slimmedElectrons")
)


process.correctedPatElectronProducer = cms.EDProducer("CorrectedPatElectronProducer",
    centrality = cms.InputTag("centralityBin"),
    correctionFile = cms.string(''),
    epCombConfig = cms.PSet(
        ecalTrkRegressionConfig = cms.PSet(
            ebHighEtForestName = cms.ESInputTag("","electron_eb_ECALTRK"),
            ebLowEtForestName = cms.ESInputTag("","electron_eb_ECALTRK_lowpt"),
            eeHighEtForestName = cms.ESInputTag("","electron_ee_ECALTRK"),
            eeLowEtForestName = cms.ESInputTag("","electron_ee_ECALTRK_lowpt"),
            forceHighEnergyTrainingIfSaturated = cms.bool(False),
            lowEtHighEtBoundary = cms.double(50),
            rangeMaxHighEt = cms.double(3),
            rangeMaxLowEt = cms.double(3),
            rangeMinHighEt = cms.double(-1),
            rangeMinLowEt = cms.double(-1)
        ),
        ecalTrkRegressionUncertConfig = cms.PSet(
            ebHighEtForestName = cms.ESInputTag("","electron_eb_ECALTRK"),
            ebLowEtForestName = cms.ESInputTag("","electron_eb_ECALTRK_lowpt"),
            eeHighEtForestName = cms.ESInputTag("","electron_ee_ECALTRK"),
            eeLowEtForestName = cms.ESInputTag("","electron_ee_ECALTRK_lowpt"),
            forceHighEnergyTrainingIfSaturated = cms.bool(False),
            lowEtHighEtBoundary = cms.double(50),
            rangeMaxHighEt = cms.double(3),
            rangeMaxLowEt = cms.double(3),
            rangeMinHighEt = cms.double(-1),
            rangeMinLowEt = cms.double(-1)
        ),
        maxEPDiffInSigmaForComb = cms.double(15),
        maxEcalEnergyForComb = cms.double(200),
        maxRelTrkMomErrForComb = cms.double(10),
        minEOverPForComb = cms.double(0.025)
    ),
    mightGet = cms.optional.untracked.vstring,
    minPt = cms.double(20),
    semiDeterministic = cms.bool(True),
    src = cms.InputTag("gedGsfElectrons")
)


process.dimuonMassCutForZMM = cms.EDProducer("CandViewShallowCloneCombiner",
    checkCharge = cms.bool(True),
    cut = cms.string('50 < mass < 130'),
    decay = cms.string('muonSelectorForZMM@+ muonSelectorForZMM@-')
)


process.hiElectrons = cms.EDProducer("HIElectronInfoProducer",
    centrality = cms.InputTag("centralityBin","HFtowers"),
    electron_minPt = cms.double(0),
    electrons = cms.InputTag("correctedElectrons"),
    etaMap = cms.InputTag("hiFJRhoProducerFinerBins","mapEtaEdges"),
    file_corr = cms.FileInPath('HeavyIonsAnalysis/EGMAnalysis/data/lepton_spectra_train_weights.json.gz'),
    file_idModel = cms.FileInPath('HeavyIonsAnalysis/EGMAnalysis/data/eleid_BDT.root'),
    file_isoModel = cms.FileInPath('HeavyIonsAnalysis/EGMAnalysis/data/eleiso_BDT.root'),
    iso_rCone = cms.double(0.3),
    iso_rVeto = cms.double(0.026),
    mightGet = cms.optional.untracked.vstring,
    pfCandidates = cms.InputTag("packedPFCandidates"),
    pf_maxAbsEta = cms.double(2.8),
    rhoMap = cms.InputTag("hiFJRhoProducerFinerBins","mapToRho"),
    sk_radius = cms.double(0.4)
)


process.hiFJGridEmptyAreaCalculator = cms.EDProducer("HiFJGridEmptyAreaCalculator",
    CentralityBinSrc = cms.InputTag("centralityBin","HFtowers"),
    bandWidth = cms.double(0.2),
    doCentrality = cms.bool(True),
    gridWidth = cms.double(0.05),
    hiBinCut = cms.int32(100),
    jetSource = cms.InputTag("kt4PFJetsForRho"),
    keepGridInfo = cms.bool(False),
    mapEtaEdges = cms.InputTag("hiFJRhoProducer","mapEtaEdges"),
    mapToRho = cms.InputTag("hiFJRhoProducer","mapToRho"),
    mapToRhoM = cms.InputTag("hiFJRhoProducer","mapToRhoM"),
    pfCandSource = cms.InputTag("particleFlow")
)


process.hiFJGridEmptyAreaCalculatorFinerBins = cms.EDProducer("HiFJGridEmptyAreaCalculator",
    CentralityBinSrc = cms.InputTag("centralityBin","HFtowers"),
    bandWidth = cms.double(0.2),
    doCentrality = cms.bool(True),
    gridWidth = cms.double(0.05),
    hiBinCut = cms.int32(100),
    jetSource = cms.InputTag("kt4PFJetsForRho"),
    keepGridInfo = cms.bool(False),
    mapEtaEdges = cms.InputTag("hiFJRhoProducerFinerBins","mapEtaEdges"),
    mapToRho = cms.InputTag("hiFJRhoProducerFinerBins","mapToRho"),
    mapToRhoM = cms.InputTag("hiFJRhoProducerFinerBins","mapToRhoM"),
    pfCandSource = cms.InputTag("packedPFCandidates")
)


process.hiFJRhoProducer = cms.EDProducer("HiFJRhoProducer",
    etaMaxExcl = cms.double(2.0),
    etaMaxExcl2 = cms.double(3.0),
    etaRanges = cms.vdouble(
        -5.0, -3.0, -2.1, -1.3, 1.3,
        2.1, 3.0, 5.0
    ),
    jetSource = cms.InputTag("kt4PFJetsForRho"),
    nExcl = cms.int32(2),
    nExcl2 = cms.int32(1),
    ptMinExcl = cms.double(20.0),
    ptMinExcl2 = cms.double(20.0)
)


process.hiFJRhoProducerFinerBins = cms.EDProducer("HiFJRhoProducer",
    etaMaxExcl = cms.double(2.0),
    etaMaxExcl2 = cms.double(3.0),
    etaRanges = cms.vdouble(
        -5.0, -4.0, -3, -2.5, -2.0,
        -0.8, 0.8, 2.0, 2.5, 3.0,
        4.0, 5.0
    ),
    jetSource = cms.InputTag("kt4PFJetsForRho"),
    nExcl = cms.int32(2),
    nExcl2 = cms.int32(1),
    ptMinExcl = cms.double(20.0),
    ptMinExcl2 = cms.double(20.0)
)


process.hiMuons = cms.EDProducer("HIMuonMVAProducer",
    centrality = cms.InputTag("centralityBin","HFtowers"),
    etaMap = cms.InputTag("hiFJRhoProducerFinerBins","mapEtaEdges"),
    file_isoCorr = cms.FileInPath('HeavyIonsAnalysis/MuonAnalysis/data/lepton_spectra_train_weights.json.gz'),
    file_isoModel = cms.FileInPath('HeavyIonsAnalysis/MuonAnalysis/data/muiso_BDT.root'),
    iso_rCone = cms.double(0.3),
    iso_rVeto = cms.double(0.001),
    mightGet = cms.optional.untracked.vstring,
    muon_minPt = cms.double(0),
    muons = cms.InputTag("slimmedMuons"),
    pfCandidates = cms.InputTag("packedPFCandidates"),
    pf_maxAbsEta = cms.double(2.8),
    rhoMap = cms.InputTag("hiFJRhoProducerFinerBins","mapToRho"),
    sk_radius = cms.double(0.4)
)


process.hiPuRho = cms.EDProducer("HiPuRhoProducer",
    dropZeroTowers = cms.bool(True),
    medianWindowWidth = cms.int32(2),
    mightGet = cms.optional.untracked.vstring,
    minimumTowersFraction = cms.double(0.7),
    nSigmaPU = cms.double(1),
    puPtMin = cms.double(15),
    rParam = cms.double(0.3),
    radiusPU = cms.double(0.5),
    src = cms.InputTag("PackedPFTowers"),
    towSigmaCut = cms.double(5)
)


process.inclusiveCandidateSecondaryVertices = cms.EDProducer("CandidateVertexMerger",
    maxFraction = cms.double(0.2),
    minSignificance = cms.double(10.0),
    secondaryVertices = cms.InputTag("candidateVertexArbitrator")
)


process.inclusiveCandidateVertexFinder = cms.EDProducer("InclusiveCandidateVertexFinder",
    beamSpot = cms.InputTag("offlineBeamSpot"),
    clusterizer = cms.PSet(
        clusterMaxDistance = cms.double(0.05),
        clusterMaxSignificance = cms.double(4.5),
        clusterMinAngleCosine = cms.double(0.5),
        distanceRatio = cms.double(20),
        maxTimeSignificance = cms.double(3.5),
        seedMax3DIPSignificance = cms.double(9999),
        seedMax3DIPValue = cms.double(9999),
        seedMin3DIPSignificance = cms.double(1.2),
        seedMin3DIPValue = cms.double(0.005)
    ),
    fitterRatio = cms.double(0.25),
    fitterSigmacut = cms.double(3),
    fitterTini = cms.double(256),
    maxNTracks = cms.uint32(30),
    maximumLongitudinalImpactParameter = cms.double(0.3),
    maximumTimeSignificance = cms.double(3),
    mightGet = cms.optional.untracked.vstring,
    minHits = cms.uint32(0),
    minPt = cms.double(0.8),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    tracks = cms.InputTag("packedPFCandidates"),
    useDirectVertexFitter = cms.bool(True),
    useVertexReco = cms.bool(True),
    vertexMinAngleCosine = cms.double(0.95),
    vertexMinDLen2DSig = cms.double(2.5),
    vertexMinDLenSig = cms.double(0.5),
    vertexReco = cms.PSet(
        finder = cms.string('avr'),
        primcut = cms.double(1),
        seccut = cms.double(3),
        smoothing = cms.bool(True)
    )
)


process.kt4PFJetsForRho = cms.EDProducer("FastjetJetProducer",
    Active_Area_Repeats = cms.int32(1),
    GhostArea = cms.double(0.005),
    Ghost_EtaMax = cms.double(6.5),
    Rho_EtaMax = cms.double(4.5),
    applyWeight = cms.bool(False),
    doAreaDiskApprox = cms.bool(False),
    doAreaFastjet = cms.bool(True),
    doFastJetNonUniform = cms.bool(True),
    doPUOffsetCorr = cms.bool(False),
    doPVCorrection = cms.bool(False),
    doRhoFastjet = cms.bool(True),
    inputEMin = cms.double(0.0),
    inputEtMin = cms.double(0.0),
    jetAlgorithm = cms.string('Kt'),
    jetPtMin = cms.double(0.0),
    jetType = cms.string('PFJet'),
    maxBadEcalCells = cms.uint32(9999999),
    maxBadHcalCells = cms.uint32(9999999),
    maxProblematicEcalCells = cms.uint32(9999999),
    maxProblematicHcalCells = cms.uint32(9999999),
    maxRecoveredEcalCells = cms.uint32(9999999),
    maxRecoveredHcalCells = cms.uint32(9999999),
    minSeed = cms.uint32(14327),
    nExclude = cms.uint32(2),
    nSigmaPU = cms.double(1.0),
    puCenters = cms.vdouble(
        -5, -4, -3, -2, -1,
        0, 1, 2, 3, 4,
        5
    ),
    puWidth = cms.double(0.8),
    rParam = cms.double(0.4),
    radiusPU = cms.double(0.5),
    src = cms.InputTag("packedPFCandidates"),
    srcPVs = cms.InputTag(""),
    useDeterministicSeed = cms.bool(True),
    voronoiRfact = cms.double(-0.9)
)


process.patJetCorrFactorsAK2PFBtag = cms.EDProducer("JetCorrFactorsProducer",
    emf = cms.bool(False),
    extraJPTOffset = cms.string('L1FastJet'),
    flavorType = cms.string('J'),
    levels = cms.vstring(
        'L2Relative',
        'L2L3Residual'
    ),
    mightGet = cms.optional.untracked.vstring,
    payload = cms.string('AK4PF'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    src = cms.InputTag("patJetsAKCs2PF"),
    useNPV = cms.bool(False),
    useRho = cms.bool(False)
)


process.patJetCorrFactorsAK2PFUnsubJets = cms.EDProducer("JetCorrFactorsProducer",
    emf = cms.bool(False),
    extraJPTOffset = cms.string('L1FastJet'),
    flavorType = cms.string('J'),
    levels = cms.vstring(
        'L2Relative',
        'L2L3Residual'
    ),
    mightGet = cms.optional.untracked.vstring,
    payload = cms.string('AK4PF'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    src = cms.InputTag("ak2PFUnsubJets"),
    useNPV = cms.bool(False),
    useRho = cms.bool(False)
)


process.patJetCorrFactorsAK3PFBtag = cms.EDProducer("JetCorrFactorsProducer",
    emf = cms.bool(False),
    extraJPTOffset = cms.string('L1FastJet'),
    flavorType = cms.string('J'),
    levels = cms.vstring(
        'L2Relative',
        'L2L3Residual'
    ),
    mightGet = cms.optional.untracked.vstring,
    payload = cms.string('AK4PF'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    src = cms.InputTag("patJetsAKCs3PF"),
    useNPV = cms.bool(False),
    useRho = cms.bool(False)
)


process.patJetCorrFactorsAK3PFUnsubJets = cms.EDProducer("JetCorrFactorsProducer",
    emf = cms.bool(False),
    extraJPTOffset = cms.string('L1FastJet'),
    flavorType = cms.string('J'),
    levels = cms.vstring(
        'L2Relative',
        'L2L3Residual'
    ),
    mightGet = cms.optional.untracked.vstring,
    payload = cms.string('AK4PF'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    src = cms.InputTag("ak3PFUnsubJets"),
    useNPV = cms.bool(False),
    useRho = cms.bool(False)
)


process.patJetCorrFactorsAKCs2PF = cms.EDProducer("JetCorrFactorsProducer",
    emf = cms.bool(False),
    extraJPTOffset = cms.string('L1FastJet'),
    flavorType = cms.string('J'),
    levels = cms.vstring(
        'L2Relative',
        'L2L3Residual'
    ),
    mightGet = cms.optional.untracked.vstring,
    payload = cms.string('AK4PF'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    src = cms.InputTag("akCs2PFJets"),
    useNPV = cms.bool(False),
    useRho = cms.bool(False)
)


process.patJetCorrFactorsAKCs3PF = cms.EDProducer("JetCorrFactorsProducer",
    emf = cms.bool(False),
    extraJPTOffset = cms.string('L1FastJet'),
    flavorType = cms.string('J'),
    levels = cms.vstring(
        'L2Relative',
        'L2L3Residual'
    ),
    mightGet = cms.optional.untracked.vstring,
    payload = cms.string('AK4PF'),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    src = cms.InputTag("akCs3PFJets"),
    useNPV = cms.bool(False),
    useRho = cms.bool(False)
)


process.patJetGenJetMatchAK2PFUnsubJets = cms.EDProducer("GenJetMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDeltaR = cms.double(0.2),
    mcPdgId = cms.vint32(),
    mcStatus = cms.vint32(),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(True),
    src = cms.InputTag("ak2PFUnsubJets")
)


process.patJetGenJetMatchAK3PFUnsubJets = cms.EDProducer("GenJetMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDeltaR = cms.double(0.30000000000000004),
    mcPdgId = cms.vint32(),
    mcStatus = cms.vint32(),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(True),
    src = cms.InputTag("ak3PFUnsubJets")
)


process.patJetGenJetMatchAKCs2PF = cms.EDProducer("GenJetMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDeltaR = cms.double(0.2),
    mcPdgId = cms.vint32(),
    mcStatus = cms.vint32(),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(True),
    src = cms.InputTag("akCs2PFJets")
)


process.patJetGenJetMatchAKCs3PF = cms.EDProducer("GenJetMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDeltaR = cms.double(0.30000000000000004),
    mcPdgId = cms.vint32(),
    mcStatus = cms.vint32(),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(True),
    src = cms.InputTag("akCs3PFJets")
)


process.patJetPartonMatchAK2PFUnsubJets = cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.4),
    mcPdgId = cms.vint32(
        1, 2, 3, 4, 5,
        21
    ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("ak2PFUnsubJets")
)


process.patJetPartonMatchAK3PFUnsubJets = cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.4),
    mcPdgId = cms.vint32(
        1, 2, 3, 4, 5,
        21
    ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("ak3PFUnsubJets")
)


process.patJetPartonMatchAKCs2PF = cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.4),
    mcPdgId = cms.vint32(
        1, 2, 3, 4, 5,
        21
    ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("akCs2PFJets")
)


process.patJetPartonMatchAKCs3PF = cms.EDProducer("MCMatcher",
    checkCharge = cms.bool(False),
    matched = cms.InputTag(""),
    maxDPtRel = cms.double(3.0),
    maxDeltaR = cms.double(0.4),
    mcPdgId = cms.vint32(
        1, 2, 3, 4, 5,
        21
    ),
    mcStatus = cms.vint32(3, 23),
    resolveAmbiguities = cms.bool(True),
    resolveByMatchQuality = cms.bool(False),
    src = cms.InputTag("akCs3PFJets")
)


process.patJetsAK2PFUnsubJets = cms.EDProducer("PATJetProducer",
    JetFlavourInfoSource = cms.InputTag("patJetFlavourAssociation"),
    JetPartonMapSource = cms.InputTag("patJetFlavourAssociationLegacy"),
    addAssociatedTracks = cms.bool(False),
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(False),
    addEfficiencies = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(True),
    addJetFlavourInfo = cms.bool(False),
    addJetID = cms.bool(False),
    addPartonJetMatch = cms.bool(False),
    addResolutions = cms.bool(False),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(),
    efficiencies = cms.PSet(

    ),
    embedCaloTowers = cms.bool(False),
    embedGenJetMatch = cms.bool(False),
    embedGenPartonMatch = cms.bool(False),
    embedPFCandidates = cms.bool(False),
    genJetMatch = cms.InputTag(""),
    genPartonMatch = cms.InputTag(""),
    getJetMCFlavour = cms.bool(False),
    jetChargeSource = cms.InputTag(""),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsAK2PFUnsubJets")),
    jetIDMap = cms.InputTag("ak4JetID"),
    jetSource = cms.InputTag("ak2PFUnsubJets"),
    mightGet = cms.optional.untracked.vstring,
    partonJetSource = cms.InputTag("NOT_IMPLEMENTED"),
    resolutions = cms.PSet(

    ),
    tagInfoSources = cms.VInputTag(),
    trackAssociationSource = cms.InputTag(""),
    useLegacyJetMCFlavour = cms.bool(False),
    userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
        ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFloats = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
        )
    )
)


process.patJetsAK3PFUnsubJets = cms.EDProducer("PATJetProducer",
    JetFlavourInfoSource = cms.InputTag("patJetFlavourAssociation"),
    JetPartonMapSource = cms.InputTag("patJetFlavourAssociationLegacy"),
    addAssociatedTracks = cms.bool(False),
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(False),
    addEfficiencies = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(True),
    addJetFlavourInfo = cms.bool(False),
    addJetID = cms.bool(False),
    addPartonJetMatch = cms.bool(False),
    addResolutions = cms.bool(False),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(),
    efficiencies = cms.PSet(

    ),
    embedCaloTowers = cms.bool(False),
    embedGenJetMatch = cms.bool(False),
    embedGenPartonMatch = cms.bool(False),
    embedPFCandidates = cms.bool(False),
    genJetMatch = cms.InputTag(""),
    genPartonMatch = cms.InputTag(""),
    getJetMCFlavour = cms.bool(False),
    jetChargeSource = cms.InputTag(""),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsAK3PFUnsubJets")),
    jetIDMap = cms.InputTag("ak4JetID"),
    jetSource = cms.InputTag("ak3PFUnsubJets"),
    mightGet = cms.optional.untracked.vstring,
    partonJetSource = cms.InputTag("NOT_IMPLEMENTED"),
    resolutions = cms.PSet(

    ),
    tagInfoSources = cms.VInputTag(),
    trackAssociationSource = cms.InputTag(""),
    useLegacyJetMCFlavour = cms.bool(False),
    userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
        ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFloats = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
        )
    )
)


process.patJetsAKCs2PF = cms.EDProducer("PATJetProducer",
    JetFlavourInfoSource = cms.InputTag("patJetFlavourAssociation"),
    JetPartonMapSource = cms.InputTag("patJetFlavourAssociationLegacy"),
    addAssociatedTracks = cms.bool(False),
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(False),
    addEfficiencies = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(True),
    addJetFlavourInfo = cms.bool(False),
    addJetID = cms.bool(False),
    addPartonJetMatch = cms.bool(False),
    addResolutions = cms.bool(False),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(),
    efficiencies = cms.PSet(

    ),
    embedCaloTowers = cms.bool(False),
    embedGenJetMatch = cms.bool(False),
    embedGenPartonMatch = cms.bool(False),
    embedPFCandidates = cms.bool(True),
    genJetMatch = cms.InputTag(""),
    genPartonMatch = cms.InputTag(""),
    getJetMCFlavour = cms.bool(False),
    jetChargeSource = cms.InputTag(""),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsAKCs2PF")),
    jetIDMap = cms.InputTag("ak4JetID"),
    jetSource = cms.InputTag("akCs2PFJets"),
    mightGet = cms.optional.untracked.vstring,
    partonJetSource = cms.InputTag("NOT_IMPLEMENTED"),
    resolutions = cms.PSet(

    ),
    tagInfoSources = cms.VInputTag(),
    trackAssociationSource = cms.InputTag(""),
    useLegacyJetMCFlavour = cms.bool(True),
    userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
        ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFloats = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
        )
    )
)


process.patJetsAKCs3PF = cms.EDProducer("PATJetProducer",
    JetFlavourInfoSource = cms.InputTag("patJetFlavourAssociation"),
    JetPartonMapSource = cms.InputTag("patJetFlavourAssociationLegacy"),
    addAssociatedTracks = cms.bool(False),
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(False),
    addEfficiencies = cms.bool(False),
    addGenJetMatch = cms.bool(False),
    addGenPartonMatch = cms.bool(False),
    addJetCharge = cms.bool(False),
    addJetCorrFactors = cms.bool(True),
    addJetFlavourInfo = cms.bool(False),
    addJetID = cms.bool(False),
    addPartonJetMatch = cms.bool(False),
    addResolutions = cms.bool(False),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(),
    efficiencies = cms.PSet(

    ),
    embedCaloTowers = cms.bool(False),
    embedGenJetMatch = cms.bool(False),
    embedGenPartonMatch = cms.bool(False),
    embedPFCandidates = cms.bool(True),
    genJetMatch = cms.InputTag(""),
    genPartonMatch = cms.InputTag(""),
    getJetMCFlavour = cms.bool(False),
    jetChargeSource = cms.InputTag(""),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsAKCs3PF")),
    jetIDMap = cms.InputTag("ak4JetID"),
    jetSource = cms.InputTag("akCs3PFJets"),
    mightGet = cms.optional.untracked.vstring,
    partonJetSource = cms.InputTag("NOT_IMPLEMENTED"),
    resolutions = cms.PSet(

    ),
    tagInfoSources = cms.VInputTag(),
    trackAssociationSource = cms.InputTag(""),
    useLegacyJetMCFlavour = cms.bool(True),
    userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
        ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFloats = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
        )
    )
)


process.pfJetProbabilityBJetTagsAK2PFBtag = cms.EDProducer("JetTagProducer",
    jetTagComputer = cms.string('candidateJetProbabilityComputer'),
    tagInfos = cms.VInputTag("pfImpactParameterTagInfosAK2PFBtag")
)


process.pfJetProbabilityBJetTagsAK3PFBtag = cms.EDProducer("JetTagProducer",
    jetTagComputer = cms.string('candidateJetProbabilityComputer'),
    tagInfos = cms.VInputTag("pfImpactParameterTagInfosAK3PFBtag")
)


process.randomEngineStateProducer = cms.EDProducer("RandomEngineStateProducer")


process.siPixelRecHits = cms.EDProducer("SiPixelRecHitConverter",
    CPE = cms.string('PixelCPEGeneric'),
    VerboseLevel = cms.untracked.int32(0),
    src = cms.InputTag("siPixelClusters")
)


process.siPixelRecHitsPreSplittingCPU = cms.EDProducer("SiPixelRecHitSoAFromLegacyHIonPhase1",
    CPE = cms.string('PixelCPEFastHIonPhase1'),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    convertToLegacy = cms.bool(True),
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelClustersPreSplitting")
)


process.siPixelRecHitsPreSplittingCUDA = cms.EDProducer("SiPixelRecHitCUDAHIonPhase1",
    CPE = cms.string('PixelCPEFastHIonPhase1'),
    beamSpot = cms.InputTag("offlineBeamSpotToCUDA"),
    mightGet = cms.optional.untracked.vstring,
    src = cms.InputTag("siPixelClustersPreSplittingCUDA")
)


process.unpackedMuons = cms.EDProducer("MuonUnpacker",
    addPropToMuonSt = cms.bool(False),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    cosmicPropagationHypothesis = cms.bool(False),
    fallbackToME1 = cms.bool(True),
    mightGet = cms.optional.untracked.vstring,
    muonSelectors = cms.vstring(
        'AllTrackerMuons',
        'TMOneStationTight'
    ),
    muons = cms.InputTag("hiMuons"),
    primaryVertices = cms.InputTag("unpackedTracksAndVertices"),
    propagatorAlong = cms.ESInputTag("","hltESPSteppingHelixPropagatorAlong"),
    propagatorAny = cms.ESInputTag("","SteppingHelixPropagatorAny"),
    propagatorOpposite = cms.ESInputTag("","hltESPSteppingHelixPropagatorOpposite"),
    tracks = cms.InputTag("unpackedTracksAndVertices"),
    useMB2InOverlap = cms.bool(True),
    useSimpleGeometry = cms.bool(True),
    useState = cms.string('atVertex'),
    useStation2 = cms.bool(True),
    useTrack = cms.string('tracker')
)


process.unpackedTracksAndVertices = cms.EDProducer("TrackAndVertexUnpacker",
    mightGet = cms.optional.untracked.vstring,
    packedCandidateNormChi2Map = cms.VInputTag("packedPFCandidateTrackChi2", "lostTrackChi2", ""),
    packedCandidates = cms.VInputTag("packedPFCandidates", "lostTracks", "lostTracks:eleTracks"),
    primaryVertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    recoverTracks = cms.bool(True),
    secondaryVertices = cms.InputTag("slimmedSecondaryVertices")
)


process.unsubAK2JetMap = cms.EDProducer("JetMatcherDR",
    matched = cms.InputTag("patJetsAK2PFUnsubJets"),
    source = cms.InputTag("selectedUpdatedPatJetsAK2PF")
)


process.unsubAK3JetMap = cms.EDProducer("JetMatcherDR",
    matched = cms.InputTag("patJetsAK3PFUnsubJets"),
    source = cms.InputTag("selectedUpdatedPatJetsAK3PF")
)


process.unsubUpdatedPatJetsAK2PF = cms.EDProducer("JetMatcherDR",
    matched = cms.InputTag("patJetsAK2PFUnsubJets"),
    source = cms.InputTag("updatedPatJetsAK2PFBtag")
)


process.unsubUpdatedPatJetsAK3PF = cms.EDProducer("JetMatcherDR",
    matched = cms.InputTag("patJetsAK3PFUnsubJets"),
    source = cms.InputTag("updatedPatJetsAK3PFBtag")
)


process.updatedPatJetsAK2PFBtag = cms.EDProducer("PATJetUpdater",
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(True),
    addJetCorrFactors = cms.bool(True),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsAK2PFBtag")),
    jetSource = cms.InputTag("patJetsAKCs2PF"),
    mightGet = cms.optional.untracked.vstring,
    printWarning = cms.bool(True),
    sort = cms.bool(True),
    tagInfoSources = cms.VInputTag(),
    userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
        ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFloats = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
        )
    )
)


process.updatedPatJetsAK3PFBtag = cms.EDProducer("PATJetUpdater",
    addBTagInfo = cms.bool(False),
    addDiscriminators = cms.bool(True),
    addJetCorrFactors = cms.bool(True),
    addTagInfos = cms.bool(False),
    discriminatorSources = cms.VInputTag(),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsAK3PFBtag")),
    jetSource = cms.InputTag("patJetsAKCs3PF"),
    mightGet = cms.optional.untracked.vstring,
    printWarning = cms.bool(True),
    sort = cms.bool(True),
    tagInfoSources = cms.VInputTag(),
    userData = cms.PSet(
        userCands = cms.PSet(
            src = cms.VInputTag("")
        ),
        userClasses = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFloats = cms.PSet(
            src = cms.VInputTag("")
        ),
        userFunctionLabels = cms.vstring(),
        userFunctions = cms.vstring(),
        userInts = cms.PSet(
            src = cms.VInputTag("")
        )
    )
)


process.zdcdigi = cms.EDProducer("QWZDC2018Producer2",
    Debug = cms.untracked.bool(False),
    HardCode = cms.untracked.bool(True),
    Pedestal = cms.VPSet(
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM0'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM1'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM2'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM3'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM4'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM5'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM6'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM7'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM8'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM9'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM10'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM11'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM12'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM13'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM14'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_EM15'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD0'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD1'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD2'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD3'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD4'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD5'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD6'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD7'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD8'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD9'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD10'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD11'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD12'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD13'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD14'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_HAD15'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM0'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM1'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM2'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM3'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM4'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM5'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM6'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM7'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM8'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM9'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM10'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM11'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM12'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM13'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM14'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_EM15'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD0'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD1'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD2'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD3'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD4'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD5'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD6'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD7'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD8'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD9'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD10'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD11'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD12'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD13'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD14'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_HAD15'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD0'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD1'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD2'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD3'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD4'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD5'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD6'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD7'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD8'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD9'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD10'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD11'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD12'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD13'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD14'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCM_RPD15'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD0'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD1'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD2'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD3'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD4'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD5'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD6'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD7'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD8'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD9'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD10'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD11'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD12'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD13'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD14'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        ),
        cms.PSet(
            object = cms.untracked.string('hZDCP_RPD15'),
            ped = cms.untracked.vdouble(0.0, 0.0, 0.0, 0.0)
        )
    ),
    SOI = cms.untracked.int32(2),
    Src = cms.untracked.InputTag("hcalDigis","ZDC")
)


process.siPixelRecHitsPreSplitting = SwitchProducerCUDA(
    cpu = cms.EDProducer("SiPixelRecHitConverter",
        CPE = cms.string('PixelCPEGeneric'),
        VerboseLevel = cms.untracked.int32(0),
        src = cms.InputTag("siPixelClustersPreSplitting")
    )
)


process.siPixelRecHitsPreSplittingSoA = SwitchProducerCUDA(
    cpu = cms.EDAlias(
        siPixelRecHitsPreSplittingCPU = cms.VPSet(
            cms.PSet(
                type = cms.string('pixelTopologyHIonPhase1TrackingRecHitSoAHost')
            ),
            cms.PSet(
                type = cms.string('uintAsHostProduct')
            )
        )
    )
)


process.clusterCompatibilityFilter = cms.EDFilter("HIClusterCompatibilityFilter",
    cluscomSrc = cms.InputTag("hiClusterCompatibility"),
    clusterPars = cms.vdouble(0.0, 0.0045),
    clusterTrunc = cms.double(2.0),
    maxZ = cms.double(20.05),
    minZ = cms.double(-20.0),
    nhitsTrunc = cms.int32(150)
)


process.dimuonMassCutFilterForZMM = cms.EDFilter("CandViewCountFilter",
    minNumber = cms.uint32(1),
    src = cms.InputTag("dimuonMassCutForZMM")
)


process.hltPixelClusterShapeFilter = cms.EDFilter("HLTPixelClusterShapeFilter",
    clusterPars = cms.vdouble(0, 0.0045),
    clusterTrunc = cms.double(2),
    inputTag = cms.InputTag("siPixelRecHits"),
    maxZ = cms.double(20.05),
    mightGet = cms.optional.untracked.vstring,
    minZ = cms.double(-20),
    nhitsTrunc = cms.int32(150),
    saveTags = cms.bool(True),
    zStep = cms.double(0.2)
)


process.hltfilter = cms.EDFilter("HLTHighLevel",
    HLTPaths = cms.vstring('HLT_HIL2SingleMu7_v3'),
    TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
    andOr = cms.bool(True),
    eventSetupPathsKey = cms.string(''),
    mightGet = cms.optional.untracked.vstring,
    throw = cms.bool(True)
)


process.muonFilterForZMM = cms.EDFilter("MuonCountFilter",
    minNumber = cms.uint32(1),
    src = cms.InputTag("muonSelectorForZMM")
)


process.muonSelectorForZMM = cms.EDFilter("MuonSelector",
    cut = cms.string('(isPFMuon && isGlobalMuon) && pt > 15.'),
    filter = cms.bool(True),
    src = cms.InputTag("slimmedMuons")
)


process.phfCoincFilter1Th3 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(1),
    threshold = cms.int32(3)
)


process.phfCoincFilter1Th4 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(1),
    threshold = cms.int32(4)
)


process.phfCoincFilter1Th5 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(1),
    threshold = cms.int32(5)
)


process.phfCoincFilter1Th6 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(1),
    threshold = cms.int32(6)
)


process.phfCoincFilter2Th3 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(2),
    threshold = cms.int32(3)
)


process.phfCoincFilter2Th4 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(2),
    threshold = cms.int32(4)
)


process.phfCoincFilter2Th5 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(2),
    threshold = cms.int32(5)
)


process.phfCoincFilter2Th6 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(2),
    threshold = cms.int32(6)
)


process.phfCoincFilter3Th3 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(3),
    threshold = cms.int32(3)
)


process.phfCoincFilter3Th4 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(3),
    threshold = cms.int32(4)
)


process.phfCoincFilter3Th5 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(3),
    threshold = cms.int32(5)
)


process.phfCoincFilter3Th6 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(3),
    threshold = cms.int32(6)
)


process.phfCoincFilter4Th2 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(4),
    threshold = cms.int32(2)
)


process.phfCoincFilter4Th3 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(4),
    threshold = cms.int32(3)
)


process.phfCoincFilter4Th4 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(4),
    threshold = cms.int32(4)
)


process.phfCoincFilter4Th5 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(4),
    threshold = cms.int32(5)
)


process.phfCoincFilter4Th6 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(4),
    threshold = cms.int32(6)
)


process.phfCoincFilter5Th3 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(5),
    threshold = cms.int32(3)
)


process.phfCoincFilter5Th4 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(5),
    threshold = cms.int32(4)
)


process.phfCoincFilter5Th5 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(5),
    threshold = cms.int32(5)
)


process.phfCoincFilter5Th6 = cms.EDFilter("HiHFFilter",
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    minnumtowers = cms.int32(5),
    threshold = cms.int32(6)
)


process.primaryVertexFilter = cms.EDFilter("VertexSelector",
    cut = cms.string('!isFake && abs(z) <= 25 && position.Rho <= 2'),
    filter = cms.bool(True),
    src = cms.InputTag("offlineSlimmedPrimaryVertices")
)


process.primaryVertexFilterForZMM = cms.EDFilter("VertexSelector",
    cut = cms.string('!isFake && abs(z) <= 25 && position.Rho <= 2'),
    filter = cms.bool(True),
    src = cms.InputTag("offlineSlimmedPrimaryVertices")
)


process.selectedPatJetsAK2PFUnsubJets = cms.EDFilter("PATJetSelector",
    cut = cms.string(''),
    cutLoose = cms.string(''),
    nLoose = cms.uint32(0),
    src = cms.InputTag("patJetsAK2PFUnsubJets")
)


process.selectedPatJetsAK3PFUnsubJets = cms.EDFilter("PATJetSelector",
    cut = cms.string(''),
    cutLoose = cms.string(''),
    nLoose = cms.uint32(0),
    src = cms.InputTag("patJetsAK3PFUnsubJets")
)


process.selectedPatJetsAKCs2PF = cms.EDFilter("PATJetSelector",
    cut = cms.string(''),
    cutLoose = cms.string(''),
    nLoose = cms.uint32(0),
    src = cms.InputTag("patJetsAKCs2PF")
)


process.selectedPatJetsAKCs3PF = cms.EDFilter("PATJetSelector",
    cut = cms.string(''),
    cutLoose = cms.string(''),
    nLoose = cms.uint32(0),
    src = cms.InputTag("patJetsAKCs3PF")
)


process.selectedUpdatedPatJetsAK2PFBtag = cms.EDFilter("PATJetSelector",
    cut = cms.string(''),
    cutLoose = cms.string(''),
    nLoose = cms.uint32(0),
    src = cms.InputTag("updatedPatJetsAK2PFBtag")
)


process.selectedUpdatedPatJetsAK3PFBtag = cms.EDFilter("PATJetSelector",
    cut = cms.string(''),
    cutLoose = cms.string(''),
    nLoose = cms.uint32(0),
    src = cms.InputTag("updatedPatJetsAK3PFBtag")
)


process.HiForestInfo = cms.EDAnalyzer("HiForestInfo",
    GlobalTagLabel = cms.string('132X_dataRun3_Prompt_v7'),
    HiForestVersion = cms.string(''),
    info = cms.vstring('HiForest, miniAOD, 132X, data')
)


process.PbPbTracks = cms.EDAnalyzer("TrackAnalyzer",
    beamSpotSrc = cms.untracked.InputTag("offlineBeamSpot"),
    doTrack = cms.untracked.bool(True),
    trackPtMin = cms.untracked.double(0.01),
    trackSrc = cms.InputTag("unpackedTracksAndVertices"),
    vertexSrc = cms.InputTag("unpackedTracksAndVertices")
)


process.akCs2PFJetAnalyzer = cms.EDAnalyzer("HiInclusiveJetAnalyzer",
    caloJetTag = cms.InputTag("slimmedCaloJets"),
    doCandidateBtagging = cms.untracked.bool(True),
    doGenSym = cms.untracked.bool(False),
    doGenTaus = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(True),
    doJetConstituents = cms.untracked.bool(False),
    doLegacyBtagging = cms.untracked.bool(False),
    doStandardJetID = cms.untracked.bool(False),
    doSubEvent = cms.untracked.bool(False),
    doSubJets = cms.untracked.bool(False),
    doWTARecluster = cms.untracked.bool(True),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    hltTrgResults = cms.untracked.string('TriggerResults::HISIGNAL'),
    isMC = cms.untracked.bool(False),
    jetAbsEtaMax = cms.untracked.double(2.5),
    jetName = cms.untracked.string('akCs2PF'),
    jetPtMin = cms.double(15),
    jetTag = cms.InputTag("selectedUpdatedPatJetsAK2PFBtag"),
    matchJets = cms.untracked.bool(False),
    matchTag = cms.untracked.InputTag("akPu4PFpatJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.2),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useNewBtaggers = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True),
    useRawPt = cms.untracked.bool(False),
    vtxTag = cms.InputTag("hiSelectedVertex")
)


process.akCs3PFJetAnalyzer = cms.EDAnalyzer("HiInclusiveJetAnalyzer",
    caloJetTag = cms.InputTag("slimmedCaloJets"),
    doCandidateBtagging = cms.untracked.bool(True),
    doGenSym = cms.untracked.bool(False),
    doGenTaus = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(True),
    doJetConstituents = cms.untracked.bool(False),
    doLegacyBtagging = cms.untracked.bool(False),
    doStandardJetID = cms.untracked.bool(False),
    doSubEvent = cms.untracked.bool(False),
    doSubJets = cms.untracked.bool(False),
    doWTARecluster = cms.untracked.bool(True),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    hltTrgResults = cms.untracked.string('TriggerResults::HISIGNAL'),
    isMC = cms.untracked.bool(False),
    jetAbsEtaMax = cms.untracked.double(2.5),
    jetName = cms.untracked.string('akCs3PF'),
    jetPtMin = cms.double(15),
    jetTag = cms.InputTag("selectedUpdatedPatJetsAK3PFBtag"),
    matchJets = cms.untracked.bool(False),
    matchTag = cms.untracked.InputTag("akPu4PFpatJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.30000000000000004),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useNewBtaggers = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True),
    useRawPt = cms.untracked.bool(False),
    vtxTag = cms.InputTag("hiSelectedVertex")
)


process.akCs4PFJetAnalyzer = cms.EDAnalyzer("HiInclusiveJetAnalyzer",
    caloJetTag = cms.InputTag("slimmedCaloJets"),
    doCandidateBtagging = cms.untracked.bool(True),
    doGenSym = cms.untracked.bool(False),
    doGenTaus = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(False),
    doJetConstituents = cms.untracked.bool(False),
    doLegacyBtagging = cms.untracked.bool(False),
    doStandardJetID = cms.untracked.bool(False),
    doSubEvent = cms.untracked.bool(False),
    doSubJets = cms.untracked.bool(False),
    doWTARecluster = cms.untracked.bool(False),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    hltTrgResults = cms.untracked.string('TriggerResults::HISIGNAL'),
    isMC = cms.untracked.bool(False),
    jetName = cms.untracked.string('akCs4PF'),
    jetPtMin = cms.double(5.0),
    jetTag = cms.InputTag("slimmedJets"),
    matchJets = cms.untracked.bool(False),
    matchTag = cms.untracked.InputTag("akPu4PFpatJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.4),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useNewBtaggers = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True),
    useRawPt = cms.untracked.bool(False),
    vtxTag = cms.InputTag("hiSelectedVertex")
)


process.akFlowPuCs4PFJetAnalyzer = cms.EDAnalyzer("HiInclusiveJetAnalyzer",
    caloJetTag = cms.InputTag("slimmedCaloJets"),
    doCandidateBtagging = cms.untracked.bool(True),
    doGenSym = cms.untracked.bool(False),
    doGenTaus = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(False),
    doJetConstituents = cms.untracked.bool(False),
    doLegacyBtagging = cms.untracked.bool(False),
    doStandardJetID = cms.untracked.bool(False),
    doSubEvent = cms.untracked.bool(False),
    doSubJets = cms.untracked.bool(False),
    doWTARecluster = cms.untracked.bool(False),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    hltTrgResults = cms.untracked.string('TriggerResults::HISIGNAL'),
    isMC = cms.untracked.bool(False),
    jetName = cms.untracked.string('akCs4PF'),
    jetPtMin = cms.double(5.0),
    jetTag = cms.InputTag("slimmedJets"),
    matchJets = cms.untracked.bool(False),
    matchTag = cms.untracked.InputTag("akPu4PFpatJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.4),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useNewBtaggers = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True),
    useRawPt = cms.untracked.bool(False),
    vtxTag = cms.InputTag("hiSelectedVertex")
)


process.akPu4CaloJetAnalyzer = cms.EDAnalyzer("HiCaloJetAnalyzer",
    doCaloEnergyFractions = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(True),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    hltTrgResults = cms.untracked.string('TriggerResults::HISIGNAL'),
    isMC = cms.untracked.bool(False),
    jetName = cms.untracked.string('akPu4Calo'),
    jetPtMin = cms.double(5.0),
    jetTag = cms.InputTag("slimmedCaloJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.4),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True)
)


process.caloJetAnalyzer = cms.EDAnalyzer("HiCaloJetAnalyzer",
    doCaloEnergyFractions = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(False),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    isMC = cms.untracked.bool(False),
    jetName = cms.untracked.string('akPu4Calo'),
    jetPtMin = cms.double(5.0),
    jetTag = cms.InputTag("slimmedCaloJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.4),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True)
)


process.ggHiNtuplizer = cms.EDAnalyzer("ggHiNtuplizer",
    beamSpotSrc = cms.InputTag("offlineBeamSpot"),
    conversionsSrc = cms.InputTag("reducedEgamma","reducedConversions"),
    doEffectiveAreas = cms.bool(True),
    doElectrons = cms.bool(True),
    doGenParticles = cms.bool(False),
    doMuons = cms.bool(False),
    doPackedGenParticle = cms.bool(True),
    doPfIso = cms.bool(True),
    doPhoERegression = cms.bool(True),
    doPhotons = cms.bool(True),
    doRecHitsEB = cms.bool(False),
    doRecHitsEE = cms.bool(False),
    doSuperClusters = cms.bool(False),
    effAreasConfigFile = cms.FileInPath('HeavyIonsAnalysis/EGMAnalysis/data/EffectiveAreas_94X_v0'),
    electronSrc = cms.InputTag("hiElectrons"),
    genParticleSrc = cms.InputTag("packedGenParticles"),
    isPackedPFCandidate = cms.bool(True),
    isParticleGun = cms.bool(False),
    muonPtMin = cms.double(0.0),
    muonSrc = cms.InputTag("unpackedMuons"),
    particleFlowCollection = cms.InputTag("packedPFCandidates"),
    photonSrc = cms.InputTag("slimmedPhotons"),
    pileupSrc = cms.InputTag("slimmedAddPileupInfo"),
    recHitsEB = cms.untracked.InputTag("reducedEgamma","reducedEBRecHits"),
    recHitsEE = cms.untracked.InputTag("reducedEgamma","reducedEERecHits"),
    rhoSrc = cms.InputTag("fixedGridRhoFastjetAll"),
    signalGenParticleSrc = cms.InputTag("packedGenParticlesSignal"),
    superClusters = cms.InputTag("reducedEgamma","reducedSuperClusters"),
    useValMapIso = cms.bool(True),
    vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices")
)


process.hiEvtAnalyzer = cms.EDAnalyzer("HiEvtAnalyzer",
    CentralityBinSrc = cms.InputTag("centralityBin","HFtowers"),
    CentralitySrc = cms.InputTag("hiCentrality"),
    EvtPlane = cms.InputTag("hiEvtPlane"),
    EvtPlaneFlat = cms.InputTag("hiEvtPlaneFlat"),
    HFfilters = cms.InputTag("hiHFfilters","hiHFfilters"),
    HiMC = cms.InputTag("heavyIon"),
    Vertex = cms.InputTag("offlineSlimmedPrimaryVertices"),
    doCentrality = cms.bool(True),
    doEvtPlane = cms.bool(True),
    doEvtPlaneFlat = cms.bool(True),
    doHFfilters = cms.bool(True),
    doHiMC = cms.bool(False),
    doMC = cms.bool(False),
    doVertex = cms.bool(True),
    evtPlaneLevel = cms.int32(0),
    pfCandidateSrc = cms.InputTag("packedPFCandidates"),
    useHepMC = cms.bool(False)
)


process.hiFJRhoAnalyzer = cms.EDAnalyzer("HiFJRhoAnalyzer",
    areaJets = cms.InputTag("hiFJRhoProducer","areaJets"),
    etaJets = cms.InputTag("hiFJRhoProducer","etaJets"),
    etaMap = cms.InputTag("hiFJRhoProducer","mapEtaEdges","HiForest"),
    etaMaxRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapEtaMaxGrid"),
    etaMinRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapEtaMinGrid"),
    meanRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapMeanRhoVsEtaGrid"),
    ptJets = cms.InputTag("hiFJRhoProducer","ptJets"),
    rho = cms.InputTag("hiFJRhoProducer","mapToRho"),
    rhoCorr = cms.InputTag("hiFJGridEmptyAreaCalculator","mapToRhoCorr"),
    rhoCorr1Bin = cms.InputTag("hiFJGridEmptyAreaCalculator","mapToRhoCorr1Bin"),
    rhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapRhoVsEtaGrid"),
    rhom = cms.InputTag("hiFJRhoProducer","mapToRhoM"),
    rhomCorr = cms.InputTag("hiFJGridEmptyAreaCalculator","mapToRhoMCorr"),
    rhomCorr1Bin = cms.InputTag("hiFJGridEmptyAreaCalculator","mapToRhoMCorr1Bin"),
    useModulatedRho = cms.bool(False)
)


process.hiFJRhoAnalyzerFinerBins = cms.EDAnalyzer("HiFJRhoAnalyzer",
    areaJets = cms.InputTag("hiFJRhoProducerFinerBins","areaJets"),
    etaJets = cms.InputTag("hiFJRhoProducerFinerBins","etaJets"),
    etaMap = cms.InputTag("hiFJRhoProducerFinerBins","mapEtaEdges","HiForest"),
    etaMaxRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapEtaMaxGrid"),
    etaMinRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapEtaMinGrid"),
    meanRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapMeanRhoVsEtaGrid"),
    ptJets = cms.InputTag("hiFJRhoProducerFinerBins","ptJets"),
    rho = cms.InputTag("hiFJRhoProducerFinerBins","mapToRho"),
    rhoCorr = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapToRhoCorr"),
    rhoCorr1Bin = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapToRhoCorr1Bin"),
    rhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapRhoVsEtaGrid"),
    rhom = cms.InputTag("hiFJRhoProducerFinerBins","mapToRhoM"),
    rhomCorr = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapToRhoMCorr"),
    rhomCorr1Bin = cms.InputTag("hiFJGridEmptyAreaCalculatorFinerBins","mapToRhoMCorr1Bin"),
    useModulatedRho = cms.bool(False)
)


process.hiPuRhoR3Analyzer = cms.EDAnalyzer("HiFJRhoAnalyzer",
    areaJets = cms.InputTag("hiPuRhoR3Producer","areaJets"),
    etaJets = cms.InputTag("hiPuRhoR3Producer","etaJets"),
    etaMap = cms.InputTag("hiPuRhoR3Producer","mapEtaEdges","HiForest"),
    etaMaxRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapEtaMaxGrid"),
    etaMinRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapEtaMinGrid"),
    meanRhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapMeanRhoVsEtaGrid"),
    nTow = cms.InputTag("hiPuRhoR3Producer","mapToNTow"),
    ptJets = cms.InputTag("hiPuRhoR3Producer","ptJets"),
    rho = cms.InputTag("hiPuRhoR3Producer","mapToRho"),
    rhoCorr = cms.InputTag("hiPuRhoR3Producer","mapToRhoMedian"),
    rhoCorr1Bin = cms.InputTag("hiPuRhoR3Producer","mapToRho"),
    rhoExtra = cms.InputTag("hiPuRhoR3Producer","mapToRhoExtra"),
    rhoFlowFitParams = cms.InputTag("hiFJRhoFlowModulationProducer","rhoFlowFitParams"),
    rhoGrid = cms.InputTag("hiFJGridEmptyAreaCalculator","mapRhoVsEtaGrid"),
    rhom = cms.InputTag("hiPuRhoR3Producer","mapToRhoM"),
    rhomCorr = cms.InputTag("hiPuRhoR3Producer","mapToRhoM"),
    rhomCorr1Bin = cms.InputTag("hiPuRhoR3Producer","mapToRhoM"),
    towExcludeEta = cms.InputTag("hiPuRhoR3Producer","mapToTowExcludeEta"),
    towExcludePhi = cms.InputTag("hiPuRhoR3Producer","mapToTowExcludePhi"),
    towExcludePt = cms.InputTag("hiPuRhoR3Producer","mapToTowExcludePt"),
    useModulatedRho = cms.bool(True)
)


process.hltanalysis = cms.EDAnalyzer("TriggerAnalyzer",
    HLTProcessName = cms.string('HLT'),
    hltPSProvCfg = cms.PSet(
        stageL1Trigger = cms.uint32(2)
    ),
    hltdummybranches = cms.vstring(),
    hltresults = cms.InputTag("TriggerResults","","HLT"),
    l1dummybranches = cms.vstring(),
    l1results = cms.InputTag("gtStage2Digis")
)


process.hltobject = cms.EDAnalyzer("TriggerObjectAnalyzer",
    processName = cms.string('HLT'),
    treeName = cms.string('JetTriggers'),
    triggerNames = cms.vstring(
        'HLT_HIZeroBias_v',
        'HLT_HIMinimumBiasHF1AND_v',
        'HLT_HIMinimumBiasHF1ANDZDC2nOR_v',
        'HLT_HIPuAK4CaloJet80Eta5p1_v',
        'HLT_HIPuAK4CaloJet100Eta5p1_v',
        'HLT_HIPuAK4CaloJet120Eta2p1_v',
        'HLT_HIPuAK4CaloJet120Eta5p1_v',
        'HLT_HIPuAK4CaloJet40Fwd_v',
        'HLT_HIPuAK4CaloJet60Fwd_v',
        'HLT_HIPuAK4CaloJet80Fwd_v',
        'HLT_HIPuAK4CaloJet100Fwd_v',
        'HLT_HIPuAK4CaloJet120Fwd_v',
        'HLT_HICsAK4PFJet60Eta1p5_v',
        'HLT_HICsAK4PFJet60Eta2p1_v',
        'HLT_HICsAK4PFJet80Eta1p5_v',
        'HLT_HICsAK4PFJet80Eta2p1_v',
        'HLT_HICsAK4PFJet100Eta1p5_v',
        'HLT_HICsAK4PFJet100Eta1p5_Beamspot_v',
        'HLT_HICsAK4PFJet100Eta2p1_v',
        'HLT_HICsAK4PFJet120Eta1p5_v',
        'HLT_HICsAK4PFJet120Eta2p1_v',
        'HLT_HIGEDPhoton10_v',
        'HLT_HIGEDPhoton10_EB_v',
        'HLT_HIGEDPhoton20_v',
        'HLT_HIGEDPhoton20_EB_v',
        'HLT_HIGEDPhoton30_v',
        'HLT_HIGEDPhoton30_EB_v',
        'HLT_HIGEDPhoton40_v',
        'HLT_HIGEDPhoton40_EB_v',
        'HLT_HIGEDPhoton50_v',
        'HLT_HIGEDPhoton50_EB_v',
        'HLT_HIGEDPhoton60_v',
        'HLT_HIGEDPhoton60_EB_v',
        'HLT_HIDoubleGEDPhoton20_v',
        'HLT_HIEle10Gsf_v',
        'HLT_HIEle15Gsf_v',
        'HLT_HIEle20Gsf_v',
        'HLT_HIEle30Gsf_v',
        'HLT_HIEle40Gsf_v',
        'HLT_HIEle50Gsf_v',
        'HLT_HIEle15Ele10Gsf_v',
        'HLT_HIEle15Ele10GsfMass50_v',
        'HLT_HIDoubleEle10Gsf_v',
        'HLT_HIDoubleEle10GsfMass50_v',
        'HLT_HIDoubleEle15Gsf_v',
        'HLT_HIDoubleEle15GsfMass50_v',
        'HLT_HIL1SingleMu0_v',
        'HLT_HIL1SingleMu0_Open_v',
        'HLT_HIL1SingleMu0_Cosmic_v',
        'HLT_HIL1SingleMu5_SingleEG20Gsf_v',
        'HLT_HIL1SingleMu5_SingleGEDPhoton20_v',
        'HLT_HIL1SingleMu7_SingleEG20Gsf_v',
        'HLT_HIL1SingleMu7_SingleGEDPhoton10_v',
        'HLT_HIL1DoubleMu0_v',
        'HLT_HIL1DoubleMu0_MaxDr3p5_Open_v',
        'HLT_HIL1DoubleMu0_SQ_v',
        'HLT_HIL2SingleMu3_Open_v',
        'HLT_HIL2SingleMu5_v',
        'HLT_HIL2SingleMu7_v',
        'HLT_HIL2DoubleMu0_Open_v',
        'HLT_HIL2DoubleMu0_M1p5to6_Open_v',
        'HLT_HIL2DoubleMu0_M7to15_Open_v',
        'HLT_HIL2DoubleMu2p8_M1p5to6_Open_v',
        'HLT_HIL2DoubleMu3_M7to15_Open_v',
        'HLT_HIL3SingleMu3_Open_v',
        'HLT_HIL3SingleMu5_v',
        'HLT_HIL3SingleMu7_v',
        'HLT_HIL3SingleMu12_v',
        'HLT_HIL3DoubleMu0_M0toInf_Open_v',
        'HLT_HIL3DoubleMu0_M2to4p5_Open_v',
        'HLT_HIL3DoubleMu0_M7to15_Open_v',
        'HLT_HIL3DoubleMu0_Quarkonia_Open_v',
        'HLT_HIL3DoubleMu2_M2to4p5_Open_v',
        'HLT_HIL3DoubleMu2_M7to15_Open_v',
        'HLT_HIL3DoubleMu2_Quarkonia_Open_v'
    ),
    triggerObjects = cms.InputTag("slimmedPatTrigger"),
    triggerResults = cms.InputTag("TriggerResults","","HLT")
)


process.inclusiveJetAnalyzer = cms.EDAnalyzer("HiInclusiveJetAnalyzer",
    caloJetTag = cms.InputTag("slimmedCaloJets"),
    doCandidateBtagging = cms.untracked.bool(True),
    doGenSym = cms.untracked.bool(False),
    doGenTaus = cms.untracked.bool(False),
    doHiJetID = cms.untracked.bool(False),
    doJetConstituents = cms.untracked.bool(False),
    doLegacyBtagging = cms.untracked.bool(False),
    doStandardJetID = cms.untracked.bool(False),
    doSubEvent = cms.untracked.bool(False),
    doSubJets = cms.untracked.bool(False),
    doWTARecluster = cms.untracked.bool(False),
    eventInfoTag = cms.InputTag("generator"),
    fillGenJets = cms.untracked.bool(False),
    genjetTag = cms.InputTag("ak4HiGenJets"),
    isMC = cms.untracked.bool(False),
    jetPtMin = cms.double(5.0),
    jetTag = cms.InputTag("ak4PFJets"),
    matchJets = cms.untracked.bool(False),
    matchTag = cms.untracked.InputTag("akPu4PFpatJets"),
    pfCandidateLabel = cms.untracked.InputTag("packedPFCandidates"),
    rParam = cms.double(0.4),
    towersSrc = cms.InputTag("towerMaker"),
    trackQuality = cms.untracked.string('highPurity'),
    trackTag = cms.InputTag("hiTracks"),
    useHepMC = cms.untracked.bool(False),
    useNewBtaggers = cms.untracked.bool(False),
    useQuality = cms.untracked.bool(True),
    useRawPt = cms.untracked.bool(False),
    vtxTag = cms.InputTag("hiSelectedVertex")
)


process.l1object = cms.EDAnalyzer("L1UpgradeFlatTreeProducer",
    doEg = cms.bool(True),
    doJet = cms.bool(True),
    doMuon = cms.bool(True),
    doSum = cms.bool(True),
    doTau = cms.bool(True),
    egToken = cms.untracked.InputTag("caloStage2Digis","EGamma"),
    jetToken = cms.untracked.InputTag("caloStage2Digis","Jet"),
    maxL1Upgrade = cms.uint32(60),
    muonToken = cms.untracked.InputTag("gmtStage2Digis","Muon"),
    sumToken = cms.untracked.InputTag("caloStage2Digis","EtSum"),
    tauTokens = cms.untracked.VInputTag(cms.InputTag("caloStage2Digis","Tau"))
)


process.metFilters = cms.EDAnalyzer("FilterAnalyzer",
    hltresults = cms.InputTag("TriggerResults","","RECO"),
    superFilters = cms.vstring('')
)


process.muonAnalyzer = cms.EDAnalyzer("MuonAnalyzer",
    doGen = cms.bool(False),
    doReco = cms.untracked.bool(True),
    genparticle = cms.InputTag("packedGenParticles"),
    muonSrc = cms.InputTag("unpackedMuons"),
    simtrack = cms.InputTag("mergedtruth","MergedTrackTruth"),
    vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices")
)


process.muonAnalyzerPP = cms.EDAnalyzer("MuonAnalyzer",
    doGen = cms.bool(False),
    doReco = cms.untracked.bool(True),
    genparticle = cms.InputTag("packedGenParticles"),
    muonSrc = cms.InputTag("slimmedMuons"),
    simtrack = cms.InputTag("mergedtruth","MergedTrackTruth"),
    vertexSrc = cms.InputTag("offlineSlimmedPrimaryVertices")
)


process.particleFlowAnalyser = cms.EDAnalyzer("ParticleFlowAnalyser",
    absEtaMax = cms.double(5.0),
    pfCandidateSrc = cms.InputTag("packedPFCandidates"),
    ptMin = cms.double(0.0)
)


process.ppTracks = cms.EDAnalyzer("TrackAnalyzer",
    beamSpotSrc = cms.untracked.InputTag("offlineBeamSpot"),
    doTrack = cms.untracked.bool(True),
    trackPtMin = cms.untracked.double(0.01),
    trackSrc = cms.InputTag("unpackedTracksAndVertices"),
    vertexSrc = cms.InputTag("unpackedTracksAndVertices")
)


process.skimanalysis = cms.EDAnalyzer("FilterAnalyzer",
    hltresults = cms.InputTag("TriggerResults","","HiForest"),
    superFilters = cms.vstring('zMMSkimPath')
)


process.trackAnalyzer = cms.EDAnalyzer("TrackAnalyzer",
    beamSpotSrc = cms.untracked.InputTag("offlineBeamSpot"),
    doTrack = cms.untracked.bool(True),
    trackPtMin = cms.untracked.double(0.01),
    trackSrc = cms.InputTag("unpackedTracksAndVertices"),
    vertexSrc = cms.InputTag("unpackedTracksAndVertices")
)


process.zdcanalyzer = cms.EDAnalyzer("ZDCTreeProducer",
    calZDCDigi = cms.bool(False),
    doZDCDigi = cms.bool(True),
    doZDCRecHit = cms.bool(False),
    nZdcTs = cms.int32(6),
    verbose = cms.bool(False),
    zdcDigiSrc = cms.InputTag("hcalDigis","ZDC"),
    zdcRecHitSrc = cms.InputTag("QWzdcreco")
)


process.DQMStore = cms.Service("DQMStore",
    MEsToSave = cms.untracked.vstring(
        'Hcal/DigiTask/Occupancy/depth/depth1',
        'Hcal/DigiTask/Occupancy/depth/depth2',
        'Hcal/DigiTask/Occupancy/depth/depth3',
        'Hcal/DigiTask/Occupancy/depth/depth4',
        'Hcal/DigiTask/Occupancy/depth/depth5',
        'Hcal/DigiTask/Occupancy/depth/depth6',
        'Hcal/DigiTask/Occupancy/depth/depth7',
        'Hcal/DigiTask/Occupancy/depth/depthHO',
        'Hcal/DigiTask/OccupancyCut/depth/depth1',
        'Hcal/DigiTask/OccupancyCut/depth/depth2',
        'Hcal/DigiTask/OccupancyCut/depth/depth3',
        'Hcal/DigiTask/OccupancyCut/depth/depth4',
        'Hcal/DigiTask/OccupancyCut/depth/depth5',
        'Hcal/DigiTask/OccupancyCut/depth/depth6',
        'Hcal/DigiTask/OccupancyCut/depth/depth7',
        'Hcal/DigiTask/OccupancyCut/depth/depthHO',
        'EcalBarrel/EBOccupancyTask/EBOT digi occupancy',
        'EcalEndcap/EEOccupancyTask/EEOT digi occupancy EE -',
        'EcalEndcap/EEOccupancyTask/EEOT digi occupancy EE +',
        'EcalBarrel/EBOccupancyTask/EBOT DCC entries',
        'EcalEndcap/EEOccupancyTask/EEOT DCC entries',
        'Ecal/EventInfo/processedEvents',
        'PixelPhase1/Tracks/charge_PXBarrel',
        'PixelPhase1/Tracks/charge_PXForward',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_1',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_2',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_3',
        'PixelPhase1/Tracks/PXBarrel/charge_PXLayer_4',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_+1',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_+2',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_+3',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_-1',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_-2',
        'PixelPhase1/Tracks/PXForward/charge_PXDisk_-3',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_1',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_2',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_3',
        'PixelPhase1/Tracks/PXBarrel/size_PXLayer_4',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_+1',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_+2',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_+3',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_-1',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_-2',
        'PixelPhase1/Tracks/PXForward/size_PXDisk_-3',
        'HLT/Vertexing/hltPixelVertices/hltPixelVertices/goodvtxNbr',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_pt',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/mon_unMatched_pt',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_eta',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_hits',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_matched_pt',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_phi',
        'HLT/Tracking/ValidationWRTOffline/hltMergedWrtHighPurityPV/ref_pt',
        'HLT/Tracking/pixelTracks/GeneralProperties/Chi2Prob_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/Chi2oNDFVsEta_ImpactPoint_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/DeltaZToPVZoom_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/DistanceOfClosestApproachToPVVsPhi_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/DistanceOfClosestApproachToPVZoom_GenTk',
        'HLT/Tracking/pixelTracks/GeneralProperties/NumberOfTracks_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/Chi2Prob_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/Chi2oNDFVsEta_ImpactPoint_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/DeltaZToPVZoom_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/DistanceOfClosestApproachToPVVsPhi_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/DistanceOfClosestApproachToPVZoom_GenTk',
        'HLT/Tracking/tracks/GeneralProperties/NumberOfTracks_GenTk',
        'HLT/Tracking/tracks/LUMIanalysis/NumberEventsVsLUMI',
        'HLT/Tracking/tracks/PUmonitoring/NumberEventsVsGoodPVtx',
        'PixelPhase1/Tracks/num_clusters_ontrack_PXBarrel',
        'PixelPhase1/Tracks/num_clusters_ontrack_PXForward',
        'PixelPhase1/Tracks/clusterposition_zphi_ontrack',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_1',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_2',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_3',
        'PixelPhase1/Tracks/PXBarrel/clusterposition_zphi_ontrack_PXLayer_4',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_+1',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_+2',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_+3',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_-1',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_-2',
        'PixelPhase1/Tracks/PXForward/clusterposition_xy_ontrack_PXDisk_-3',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_1/NormalizedHitResiduals_TEC__wheel__1',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_2/NormalizedHitResiduals_TEC__wheel__2',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_3/NormalizedHitResiduals_TEC__wheel__3',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_4/NormalizedHitResiduals_TEC__wheel__4',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_5/NormalizedHitResiduals_TEC__wheel__5',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_6/NormalizedHitResiduals_TEC__wheel__6',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_7/NormalizedHitResiduals_TEC__wheel__7',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_8/NormalizedHitResiduals_TEC__wheel__8',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_9/NormalizedHitResiduals_TEC__wheel__9',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_1/NormalizedHitResiduals_TEC__wheel__1',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_2/NormalizedHitResiduals_TEC__wheel__2',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_3/NormalizedHitResiduals_TEC__wheel__3',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_4/NormalizedHitResiduals_TEC__wheel__4',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_5/NormalizedHitResiduals_TEC__wheel__5',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_6/NormalizedHitResiduals_TEC__wheel__6',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_7/NormalizedHitResiduals_TEC__wheel__7',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_8/NormalizedHitResiduals_TEC__wheel__8',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_9/NormalizedHitResiduals_TEC__wheel__9',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__1',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__2',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__3',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_4/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__4',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_5/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__5',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_6/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__6',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_7/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__7',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_8/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__8',
        'SiStrip/MechanicalView/TEC/PLUS/wheel_9/Summary_ClusterStoNCorr__OnTrack__TEC__PLUS__wheel__9',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__1',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__2',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__3',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_4/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__4',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_5/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__5',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_6/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__6',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_7/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__7',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_8/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__8',
        'SiStrip/MechanicalView/TEC/MINUS/wheel_9/Summary_ClusterStoNCorr__OnTrack__TEC__MINUS__wheel__9',
        'SiStrip/MechanicalView/TIB/layer_1/NormalizedHitResiduals_TIB__Layer__1',
        'SiStrip/MechanicalView/TIB/layer_2/NormalizedHitResiduals_TIB__Layer__2',
        'SiStrip/MechanicalView/TIB/layer_3/NormalizedHitResiduals_TIB__Layer__3',
        'SiStrip/MechanicalView/TIB/layer_4/NormalizedHitResiduals_TIB__Layer__4',
        'SiStrip/MechanicalView/TIB/layer_1/Summary_ClusterStoNCorr__OnTrack__TIB__layer__1',
        'SiStrip/MechanicalView/TIB/layer_2/Summary_ClusterStoNCorr__OnTrack__TIB__layer__2',
        'SiStrip/MechanicalView/TIB/layer_3/Summary_ClusterStoNCorr__OnTrack__TIB__layer__3',
        'SiStrip/MechanicalView/TIB/layer_4/Summary_ClusterStoNCorr__OnTrack__TIB__layer__4',
        'SiStrip/MechanicalView/TID/PLUS/wheel_1/NormalizedHitResiduals_TID__wheel__1',
        'SiStrip/MechanicalView/TID/PLUS/wheel_2/NormalizedHitResiduals_TID__wheel__2',
        'SiStrip/MechanicalView/TID/PLUS/wheel_3/NormalizedHitResiduals_TID__wheel__3',
        'SiStrip/MechanicalView/TID/MINUS/wheel_1/NormalizedHitResiduals_TID__wheel__1',
        'SiStrip/MechanicalView/TID/MINUS/wheel_2/NormalizedHitResiduals_TID__wheel__2',
        'SiStrip/MechanicalView/TID/MINUS/wheel_3/NormalizedHitResiduals_TID__wheel__3',
        'SiStrip/MechanicalView/TID/PLUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TID__PLUS__wheel__1',
        'SiStrip/MechanicalView/TID/PLUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TID__PLUS__wheel__2',
        'SiStrip/MechanicalView/TID/PLUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TID__PLUS__wheel__3',
        'SiStrip/MechanicalView/TID/MINUS/wheel_1/Summary_ClusterStoNCorr__OnTrack__TID__MINUS__wheel__1',
        'SiStrip/MechanicalView/TID/MINUS/wheel_2/Summary_ClusterStoNCorr__OnTrack__TID__MINUS__wheel__2',
        'SiStrip/MechanicalView/TID/MINUS/wheel_3/Summary_ClusterStoNCorr__OnTrack__TID__MINUS__wheel__3',
        'SiStrip/MechanicalView/TOB/layer_1/NormalizedHitResiduals_TOB__Layer__1',
        'SiStrip/MechanicalView/TOB/layer_2/NormalizedHitResiduals_TOB__Layer__2',
        'SiStrip/MechanicalView/TOB/layer_3/NormalizedHitResiduals_TOB__Layer__3',
        'SiStrip/MechanicalView/TOB/layer_4/NormalizedHitResiduals_TOB__Layer__4',
        'SiStrip/MechanicalView/TOB/layer_5/NormalizedHitResiduals_TOB__Layer__5',
        'SiStrip/MechanicalView/TOB/layer_6/NormalizedHitResiduals_TOB__Layer__6',
        'SiStrip/MechanicalView/TOB/layer_1/Summary_ClusterStoNCorr__OnTrack__TOB__layer__1',
        'SiStrip/MechanicalView/TOB/layer_2/Summary_ClusterStoNCorr__OnTrack__TOB__layer__2',
        'SiStrip/MechanicalView/TOB/layer_3/Summary_ClusterStoNCorr__OnTrack__TOB__layer__3',
        'SiStrip/MechanicalView/TOB/layer_4/Summary_ClusterStoNCorr__OnTrack__TOB__layer__4',
        'SiStrip/MechanicalView/TOB/layer_5/Summary_ClusterStoNCorr__OnTrack__TOB__layer__5',
        'SiStrip/MechanicalView/TOB/layer_6/Summary_ClusterStoNCorr__OnTrack__TOB__layer__6',
        'SiStrip/MechanicalView/MainDiagonal Position',
        'SiStrip/MechanicalView/NumberOfClustersInPixel',
        'SiStrip/MechanicalView/NumberOfClustersInStrip',
        'Tracking/TrackParameters/generalTracks/LSanalysis/Chi2oNDF_lumiFlag_GenTk',
        'Tracking/TrackParameters/generalTracks/LSanalysis/NumberOfRecHitsPerTrack_lumiFlag_GenTk',
        'Tracking/TrackParameters/generalTracks/LSanalysis/NumberOfTracks_lumiFlag_GenTk',
        'Tracking/TrackParameters/highPurityTracks/pt_1/GeneralProperties/SIPDxyToPV_GenTk',
        'Tracking/TrackParameters/highPurityTracks/pt_1/GeneralProperties/SIPDzToPV_GenTk',
        'Tracking/TrackParameters/highPurityTracks/pt_1/GeneralProperties/SIP3DToPV_GenTk',
        'Tracking/TrackParameters/generalTracks/HitProperties/NumberOfMissingOuterRecHitsPerTrack_GenTk',
        'Tracking/TrackParameters/generalTracks/HitProperties/NumberMORecHitsPerTrackVsPt_GenTk',
        'OfflinePV/offlinePrimaryVertices/tagVtxProb',
        'OfflinePV/offlinePrimaryVertices/tagType',
        'OfflinePV/Resolution/PV/pull_x',
        'OfflinePV/Resolution/PV/pull_y',
        'OfflinePV/Resolution/PV/pull_z',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_highPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_highPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_mediumPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_mediumPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_lowPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/CHFrac_lowPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_highPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_highPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_mediumPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_mediumPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_lowPt_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/ChMultiplicity_lowPt_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Constituents',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Eta',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Eta_uncor',
        'JetMET/Jet/Cleanedak4PFJetsCHS/JetEnergyCorr',
        'JetMET/Jet/Cleanedak4PFJetsCHS/NJets',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Phi',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Phi_Barrel',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Phi_EndCap',
        'JetMET/Jet/Cleanedak4PFJetsCHS/Pt',
        'JetMET/MET/pfMETT1/Cleaned/METSig',
        'JetMET/vertices',
        'JetMET/HIJetValidation/akCs4PFJets/SumPFPt',
        'JetMET/HIJetValidation/akPu4CaloJets/SumPFPt'
    ),
    assertLegacySafe = cms.untracked.bool(False),
    enableMultiThread = cms.untracked.bool(True),
    onlineMode = cms.untracked.bool(False),
    saveByLumi = cms.untracked.bool(False),
    trackME = cms.untracked.string(''),
    verbose = cms.untracked.int32(0)
)


process.MessageLogger = cms.Service("MessageLogger",
    cerr = cms.untracked.PSet(
        FwkReport = cms.untracked.PSet(
            limit = cms.untracked.int32(10000000),
            reportEvery = cms.untracked.int32(1)
        ),
        FwkSummary = cms.untracked.PSet(
            limit = cms.untracked.int32(10000000),
            reportEvery = cms.untracked.int32(1)
        ),
        INFO = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        Root_NoDictionary = cms.untracked.PSet(
            limit = cms.untracked.int32(0)
        ),
        default = cms.untracked.PSet(
            limit = cms.untracked.int32(10000000)
        ),
        enable = cms.untracked.bool(True),
        enableStatistics = cms.untracked.bool(False),
        lineLength = cms.optional.untracked.int32,
        noLineBreaks = cms.optional.untracked.bool,
        noTimeStamps = cms.untracked.bool(False),
        resetStatistics = cms.untracked.bool(False),
        statisticsThreshold = cms.untracked.string('WARNING'),
        threshold = cms.untracked.string('INFO'),
        allowAnyLabel_=cms.optional.untracked.PSetTemplate(
            limit = cms.optional.untracked.int32,
            reportEvery = cms.untracked.int32(1),
            timespan = cms.optional.untracked.int32
        )
    ),
    cout = cms.untracked.PSet(
        enable = cms.untracked.bool(False),
        enableStatistics = cms.untracked.bool(False),
        lineLength = cms.optional.untracked.int32,
        noLineBreaks = cms.optional.untracked.bool,
        noTimeStamps = cms.optional.untracked.bool,
        resetStatistics = cms.untracked.bool(False),
        statisticsThreshold = cms.optional.untracked.string,
        threshold = cms.optional.untracked.string,
        allowAnyLabel_=cms.optional.untracked.PSetTemplate(
            limit = cms.optional.untracked.int32,
            reportEvery = cms.untracked.int32(1),
            timespan = cms.optional.untracked.int32
        )
    ),
    debugModules = cms.untracked.vstring(),
    default = cms.untracked.PSet(
        limit = cms.optional.untracked.int32,
        lineLength = cms.untracked.int32(80),
        noLineBreaks = cms.untracked.bool(False),
        noTimeStamps = cms.untracked.bool(False),
        reportEvery = cms.untracked.int32(1),
        statisticsThreshold = cms.untracked.string('INFO'),
        threshold = cms.untracked.string('INFO'),
        timespan = cms.optional.untracked.int32,
        allowAnyLabel_=cms.optional.untracked.PSetTemplate(
            limit = cms.optional.untracked.int32,
            reportEvery = cms.untracked.int32(1),
            timespan = cms.optional.untracked.int32
        )
    ),
    files = cms.untracked.PSet(
        allowAnyLabel_=cms.optional.untracked.PSetTemplate(
            enableStatistics = cms.untracked.bool(False),
            extension = cms.optional.untracked.string,
            filename = cms.optional.untracked.string,
            lineLength = cms.optional.untracked.int32,
            noLineBreaks = cms.optional.untracked.bool,
            noTimeStamps = cms.optional.untracked.bool,
            output = cms.optional.untracked.string,
            resetStatistics = cms.untracked.bool(False),
            statisticsThreshold = cms.optional.untracked.string,
            threshold = cms.optional.untracked.string,
            allowAnyLabel_=cms.optional.untracked.PSetTemplate(
                limit = cms.optional.untracked.int32,
                reportEvery = cms.untracked.int32(1),
                timespan = cms.optional.untracked.int32
            )
        )
    ),
    suppressDebug = cms.untracked.vstring(),
    suppressFwkInfo = cms.untracked.vstring(),
    suppressInfo = cms.untracked.vstring(),
    suppressWarning = cms.untracked.vstring(),
    allowAnyLabel_=cms.optional.untracked.PSetTemplate(
        limit = cms.optional.untracked.int32,
        reportEvery = cms.untracked.int32(1),
        timespan = cms.optional.untracked.int32
    )
)


process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    CTPPSFastRecHits = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(1357987)
    ),
    LHCTransport = cms.PSet(
        engineName = cms.untracked.string('TRandom3'),
        initialSeed = cms.untracked.uint32(87654321)
    ),
    MuonSimHits = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(987346)
    ),
    RPSiDetDigitizer = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(137137)
    ),
    RPixDetDigitizer = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(137137)
    ),
    VtxSmeared = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(98765432)
    ),
    ecalPreshowerRecHit = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(6541321)
    ),
    ecalRecHit = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(654321)
    ),
    externalLHEProducer = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(234567)
    ),
    famosPileUp = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(918273)
    ),
    fastSimProducer = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(13579)
    ),
    fastTrackerRecHits = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(24680)
    ),
    g4SimHits = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(11)
    ),
    generator = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(123456789)
    ),
    hbhereco = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(541321)
    ),
    hfreco = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(541321)
    ),
    hiSignal = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(123456789)
    ),
    hiSignalG4SimHits = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(11)
    ),
    hiSignalLHCTransport = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(88776655)
    ),
    horeco = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(541321)
    ),
    l1ParamMuons = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(6453209)
    ),
    mix = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(12345)
    ),
    mixData = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(12345)
    ),
    mixGenPU = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(918273)
    ),
    mixRecoTracks = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(918273)
    ),
    mixSimCaloHits = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(918273)
    ),
    paramMuons = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(54525)
    ),
    saveFileName = cms.untracked.string(''),
    simBeamSpotFilter = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(87654321)
    ),
    simMuonCSCDigis = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(11223344)
    ),
    simMuonDTDigis = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(1234567)
    ),
    simMuonGEMDigis = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(1234567)
    ),
    simMuonRPCDigis = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(1234567)
    ),
    simSiStripDigiSimLink = cms.PSet(
        engineName = cms.untracked.string('MixMaxRng'),
        initialSeed = cms.untracked.uint32(1234567)
    )
)


process.TFileService = cms.Service("TFileService",
    fileName = cms.string('HiForestMiniAOD.root')
)


process.CSCGeometryESModule = cms.ESProducer("CSCGeometryESModule",
    alignmentsLabel = cms.string(''),
    appendToDataLabel = cms.string(''),
    applyAlignment = cms.bool(True),
    debugV = cms.untracked.bool(False),
    fromDD4hep = cms.bool(False),
    fromDDD = cms.bool(False),
    useCentreTIOffsets = cms.bool(False),
    useGangedStripsInME1a = cms.bool(False),
    useOnlyWiresInME1a = cms.bool(False),
    useRealWireGeometry = cms.bool(True)
)


process.CaloGeometryBuilder = cms.ESProducer("CaloGeometryBuilder",
    SelectedCalos = cms.vstring(
        'HCAL',
        'ZDC',
        'EcalBarrel',
        'EcalEndcap',
        'EcalPreshower',
        'TOWER'
    )
)


process.CaloTopologyBuilder = cms.ESProducer("CaloTopologyBuilder")


process.CaloTowerGeometryFromDBEP = cms.ESProducer("CaloTowerGeometryFromDBEP",
    applyAlignment = cms.bool(False)
)


process.CaloTowerTopologyEP = cms.ESProducer("CaloTowerTopologyEP")


process.CastorDbProducer = cms.ESProducer("CastorDbProducer",
    appendToDataLabel = cms.string('')
)


process.CastorGeometryFromDBEP = cms.ESProducer("CastorGeometryFromDBEP",
    applyAlignment = cms.bool(False)
)


process.DDCompactViewESProducer = cms.ESProducer("DDCompactViewESProducer",
    appendToDataLabel = cms.string('')
)


process.DDSpecParRegistryESProducer = cms.ESProducer("DDSpecParRegistryESProducer",
    appendToDataLabel = cms.string('')
)


process.DDVectorRegistryESProducer = cms.ESProducer("DDVectorRegistryESProducer",
    appendToDataLabel = cms.string('')
)


process.DTGeometryESModule = cms.ESProducer("DTGeometryESModule",
    DDDetector = cms.ESInputTag("",""),
    alignmentsLabel = cms.string(''),
    appendToDataLabel = cms.string(''),
    applyAlignment = cms.bool(True),
    attribute = cms.string('MuStructure'),
    fromDD4hep = cms.bool(False),
    fromDDD = cms.bool(False),
    value = cms.string('MuonBarrelDT')
)


process.EcalBarrelGeometryFromDBEP = cms.ESProducer("EcalBarrelGeometryFromDBEP",
    applyAlignment = cms.bool(True)
)


process.EcalElectronicsMappingBuilder = cms.ESProducer("EcalElectronicsMappingBuilder")


process.EcalEndcapGeometryFromDBEP = cms.ESProducer("EcalEndcapGeometryFromDBEP",
    applyAlignment = cms.bool(True)
)


process.EcalLaserCorrectionService = cms.ESProducer("EcalLaserCorrectionService",
    maxExtrapolationTimeInSec = cms.uint32(0)
)


process.EcalLaserCorrectionServiceMC = cms.ESProducer("EcalLaserCorrectionServiceMC",
    appendToDataLabel = cms.string('')
)


process.EcalPreshowerGeometryFromDBEP = cms.ESProducer("EcalPreshowerGeometryFromDBEP",
    applyAlignment = cms.bool(True)
)


process.EcalTrigTowerConstituentsMapBuilder = cms.ESProducer("EcalTrigTowerConstituentsMapBuilder",
    MapFile = cms.untracked.string('Geometry/EcalMapping/data/EndCap_TTMap.txt')
)


process.GEMGeometryESModule = cms.ESProducer("GEMGeometryESModule",
    alignmentsLabel = cms.string(''),
    appendToDataLabel = cms.string(''),
    applyAlignment = cms.bool(True),
    fromDD4hep = cms.bool(False),
    fromDDD = cms.bool(False)
)


process.GlobalTrackingGeometryESProducer = cms.ESProducer("GlobalTrackingGeometryESProducer")


process.HcalAlignmentEP = cms.ESProducer("HcalAlignmentEP")


process.HcalGeometryFromDBEP = cms.ESProducer("HcalGeometryFromDBEP",
    applyAlignment = cms.bool(True)
)


process.MuonDetLayerGeometryESProducer = cms.ESProducer("MuonDetLayerGeometryESProducer")


process.ParabolicParametrizedMagneticFieldProducer = cms.ESProducer("AutoParametrizedMagneticFieldProducer",
    label = cms.untracked.string('ParabolicMf'),
    valueOverride = cms.int32(18268),
    version = cms.string('Parabolic')
)


process.RPCGeometryESModule = cms.ESProducer("RPCGeometryESModule",
    fromDD4hep = cms.untracked.bool(False),
    fromDDD = cms.untracked.bool(False)
)


process.SiStripRecHitMatcherESProducer = cms.ESProducer("SiStripRecHitMatcherESProducer",
    ComponentName = cms.string('StandardMatcher'),
    NSigmaInside = cms.double(3.0),
    PreFilter = cms.bool(False)
)


process.StripCPEfromTrackAngleESProducer = cms.ESProducer("StripCPEESProducer",
    ComponentName = cms.string('StripCPEfromTrackAngle'),
    ComponentType = cms.string('StripCPEfromTrackAngle'),
    parameters = cms.PSet(
        mLC_P0 = cms.double(-0.326),
        mLC_P1 = cms.double(0.618),
        mLC_P2 = cms.double(0.3),
        mTEC_P0 = cms.double(-1.885),
        mTEC_P1 = cms.double(0.471),
        mTIB_P0 = cms.double(-0.742),
        mTIB_P1 = cms.double(0.202),
        mTID_P0 = cms.double(-1.427),
        mTID_P1 = cms.double(0.433),
        mTOB_P0 = cms.double(-1.026),
        mTOB_P1 = cms.double(0.253),
        maxChgOneMIP = cms.double(6000.0),
        useLegacyError = cms.bool(False)
    )
)


process.TrackerRecoGeometryESProducer = cms.ESProducer("TrackerRecoGeometryESProducer",
    usePhase2Stacks = cms.bool(False)
)


process.TransientTrackBuilderESProducer = cms.ESProducer("TransientTrackBuilderESProducer",
    ComponentName = cms.string('TransientTrackBuilder')
)


process.VolumeBasedMagneticFieldESProducer = cms.ESProducer("DD4hep_VolumeBasedMagneticFieldESProducerFromDB",
    debugBuilder = cms.untracked.bool(False),
    label = cms.untracked.string(''),
    valueOverride = cms.int32(18268)
)


process.ZdcGeometryFromDBEP = cms.ESProducer("ZdcGeometryFromDBEP",
    applyAlignment = cms.bool(False)
)


process.caloSimulationParameters = cms.ESProducer("CaloSimParametersESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True)
)


process.ctppsBeamParametersFromLHCInfoESSource = cms.ESProducer("CTPPSBeamParametersFromLHCInfoESSource",
    appendToDataLabel = cms.string(''),
    beamDivX45 = cms.double(0.1),
    beamDivX56 = cms.double(0.1),
    beamDivY45 = cms.double(0.1),
    beamDivY56 = cms.double(0.1),
    lhcInfoLabel = cms.string(''),
    vtxOffsetX45 = cms.double(0.01),
    vtxOffsetX56 = cms.double(0.01),
    vtxOffsetY45 = cms.double(0.01),
    vtxOffsetY56 = cms.double(0.01),
    vtxOffsetZ45 = cms.double(0.01),
    vtxOffsetZ56 = cms.double(0.01),
    vtxStddevX = cms.double(0.02),
    vtxStddevY = cms.double(0.02),
    vtxStddevZ = cms.double(0.02)
)


process.ctppsInterpolatedOpticalFunctionsESSource = cms.ESProducer("CTPPSInterpolatedOpticalFunctionsESSource",
    appendToDataLabel = cms.string(''),
    lhcInfoLabel = cms.string(''),
    opticsLabel = cms.string('')
)


process.ecalSimulationParametersEB = cms.ESProducer("EcalSimParametersESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True),
    name = cms.string('EcalHitsEB')
)


process.ecalSimulationParametersEE = cms.ESProducer("EcalSimParametersESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True),
    name = cms.string('EcalHitsEE')
)


process.ecalSimulationParametersES = cms.ESProducer("EcalSimParametersESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True),
    name = cms.string('EcalHitsES')
)


process.fakeForIdealAlignment = cms.ESProducer("FakeAlignmentProducer",
    appendToDataLabel = cms.string('fakeForIdeal')
)


process.hcalDDDRecConstants = cms.ESProducer("HcalDDDRecConstantsESModule",
    appendToDataLabel = cms.string('')
)


process.hcalDDDSimConstants = cms.ESProducer("HcalDDDSimConstantsESModule",
    appendToDataLabel = cms.string('')
)


process.hcalSimulationConstants = cms.ESProducer("HcalSimulationConstantsESModule",
    appendToDataLabel = cms.string('')
)


process.hcalSimulationParameters = cms.ESProducer("HcalSimParametersESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True)
)


process.hcalTopologyIdeal = cms.ESProducer("HcalTopologyIdealEP",
    Exclude = cms.untracked.string(''),
    MergePosition = cms.untracked.bool(False),
    appendToDataLabel = cms.string('')
)


process.hcal_db_producer = cms.ESProducer("HcalDbProducer",
    dump = cms.untracked.vstring(''),
    file = cms.untracked.string('')
)


process.idealForDigiCSCGeometry = cms.ESProducer("CSCGeometryESModule",
    alignmentsLabel = cms.string('fakeForIdeal'),
    appendToDataLabel = cms.string('idealForDigi'),
    applyAlignment = cms.bool(False),
    debugV = cms.untracked.bool(False),
    fromDD4hep = cms.bool(False),
    fromDDD = cms.bool(False),
    useCentreTIOffsets = cms.bool(False),
    useGangedStripsInME1a = cms.bool(False),
    useOnlyWiresInME1a = cms.bool(False),
    useRealWireGeometry = cms.bool(True)
)


process.idealForDigiDTGeometry = cms.ESProducer("DTGeometryESModule",
    DDDetector = cms.ESInputTag("",""),
    alignmentsLabel = cms.string('fakeForIdeal'),
    appendToDataLabel = cms.string('idealForDigi'),
    applyAlignment = cms.bool(False),
    attribute = cms.string('MuStructure'),
    fromDD4hep = cms.bool(False),
    fromDDD = cms.bool(False),
    value = cms.string('MuonBarrelDT')
)


process.idealForDigiTrackerGeometry = cms.ESProducer("TrackerDigiGeometryESModule",
    alignmentsLabel = cms.string('fakeForIdeal'),
    appendToDataLabel = cms.string('idealForDigi'),
    applyAlignment = cms.bool(False),
    fromDDD = cms.bool(False)
)


process.multipleScatteringParametrisationMakerESProducer = cms.ESProducer("MultipleScatteringParametrisationMakerESProducer",
    appendToDataLabel = cms.string('')
)


process.muonGeometryConstants = cms.ESProducer("MuonGeometryConstantsESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True)
)


process.muonOffsetESProducer = cms.ESProducer("MuonOffsetESProducer",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(True),
    names = cms.vstring(
        'MuonCommonNumbering',
        'MuonBarrel',
        'MuonEndcap',
        'MuonBarrelWheels',
        'MuonBarrelStation1',
        'MuonBarrelStation2',
        'MuonBarrelStation3',
        'MuonBarrelStation4',
        'MuonBarrelSuperLayer',
        'MuonBarrelLayer',
        'MuonBarrelWire',
        'MuonRpcPlane1I',
        'MuonRpcPlane1O',
        'MuonRpcPlane2I',
        'MuonRpcPlane2O',
        'MuonRpcPlane3S',
        'MuonRpcPlane4',
        'MuonRpcChamberLeft',
        'MuonRpcChamberMiddle',
        'MuonRpcChamberRight',
        'MuonRpcEndcap1',
        'MuonRpcEndcap2',
        'MuonRpcEndcap3',
        'MuonRpcEndcap4',
        'MuonRpcEndcapSector',
        'MuonRpcEndcapChamberB1',
        'MuonRpcEndcapChamberB2',
        'MuonRpcEndcapChamberB3',
        'MuonRpcEndcapChamberC1',
        'MuonRpcEndcapChamberC2',
        'MuonRpcEndcapChamberC3',
        'MuonRpcEndcapChamberE1',
        'MuonRpcEndcapChamberE2',
        'MuonRpcEndcapChamberE3',
        'MuonRpcEndcapChamberF1',
        'MuonRpcEndcapChamberF2',
        'MuonRpcEndcapChamberF3',
        'MuonRpcEndcapChamberG1',
        'MuonRpcEndcapChamberH1',
        'MuonEndcapStation1',
        'MuonEndcapStation2',
        'MuonEndcapStation3',
        'MuonEndcapStation4',
        'MuonEndcapSubrings',
        'MuonEndcapSectors',
        'MuonEndcapLayers',
        'MuonEndcapRing1',
        'MuonEndcapRing2',
        'MuonEndcapRing3',
        'MuonEndcapRingA',
        'MuonGEMEndcap',
        'MuonGEMEndcap2',
        'MuonGEMSector',
        'MuonGEMChamber'
    )
)


process.siPixelQualityESProducer = cms.ESProducer("SiPixelQualityESProducer",
    ListOfRecordToMerge = cms.VPSet(
        cms.PSet(
            record = cms.string('SiPixelQualityFromDbRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('SiPixelDetVOffRcd'),
            tag = cms.string('')
        )
    ),
    appendToDataLabel = cms.string(''),
    siPixelQualityLabel = cms.string(''),
    siPixelQualityLabel_RawToDigi = cms.string('')
)


process.siStripBackPlaneCorrectionDepESProducer = cms.ESProducer("SiStripBackPlaneCorrectionDepESProducer",
    BackPlaneCorrectionDeconvMode = cms.PSet(
        label = cms.untracked.string('deconvolution'),
        record = cms.string('SiStripBackPlaneCorrectionRcd')
    ),
    BackPlaneCorrectionPeakMode = cms.PSet(
        label = cms.untracked.string('peak'),
        record = cms.string('SiStripBackPlaneCorrectionRcd')
    ),
    LatencyRecord = cms.PSet(
        label = cms.untracked.string(''),
        record = cms.string('SiStripLatencyRcd')
    )
)


process.siStripGainESProducer = cms.ESProducer("SiStripGainESProducer",
    APVGain = cms.VPSet(
        cms.PSet(
            Label = cms.untracked.string(''),
            NormalizationFactor = cms.untracked.double(1.0),
            Record = cms.string('SiStripApvGainRcd')
        ),
        cms.PSet(
            Label = cms.untracked.string(''),
            NormalizationFactor = cms.untracked.double(1.0),
            Record = cms.string('SiStripApvGain2Rcd')
        )
    ),
    AutomaticNormalization = cms.bool(False),
    appendToDataLabel = cms.string(''),
    printDebug = cms.untracked.bool(False)
)


process.siStripLorentzAngleDepESProducer = cms.ESProducer("SiStripLorentzAngleDepESProducer",
    LatencyRecord = cms.PSet(
        label = cms.untracked.string(''),
        record = cms.string('SiStripLatencyRcd')
    ),
    LorentzAngleDeconvMode = cms.PSet(
        label = cms.untracked.string('deconvolution'),
        record = cms.string('SiStripLorentzAngleRcd')
    ),
    LorentzAnglePeakMode = cms.PSet(
        label = cms.untracked.string('peak'),
        record = cms.string('SiStripLorentzAngleRcd')
    )
)


process.siStripQualityESProducer = cms.ESProducer("SiStripQualityESProducer",
    ListOfRecordToMerge = cms.VPSet(
        cms.PSet(
            record = cms.string('SiStripDetVOffRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('SiStripDetCablingRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('RunInfoRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('SiStripBadChannelRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('SiStripBadFiberRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('SiStripBadModuleRcd'),
            tag = cms.string('')
        ),
        cms.PSet(
            record = cms.string('SiStripBadStripRcd'),
            tag = cms.string('')
        )
    ),
    PrintDebugOutput = cms.bool(False),
    ReduceGranularity = cms.bool(False),
    ThresholdForReducedGranularity = cms.double(0.3),
    UseEmptyRunInfo = cms.bool(False),
    appendToDataLabel = cms.string('')
)


process.sistripconn = cms.ESProducer("SiStripConnectivity")


process.stripCPEESProducer = cms.ESProducer("StripCPEESProducer",
    ComponentName = cms.string('stripCPE'),
    ComponentType = cms.string('SimpleStripCPE'),
    parameters = cms.PSet(

    )
)


process.trackerGeometryDB = cms.ESProducer("TrackerDigiGeometryESModule",
    alignmentsLabel = cms.string(''),
    appendToDataLabel = cms.string(''),
    applyAlignment = cms.bool(True),
    fromDDD = cms.bool(False)
)


process.trackerNumberingGeometryDB = cms.ESProducer("TrackerGeometricDetESModule",
    appendToDataLabel = cms.string(''),
    fromDD4hep = cms.bool(False),
    fromDDD = cms.bool(False)
)


process.trackerTopology = cms.ESProducer("TrackerTopologyEP",
    appendToDataLabel = cms.string('')
)


process.DDDetectorESProducerFromDB = cms.ESSource("DDDetectorESProducer",
    appendToDataLabel = cms.string(''),
    fromDB = cms.bool(True),
    label = cms.string('Extended'),
    rootDDName = cms.string('cms:OCMS')
)


process.GlobalTag = cms.ESSource("PoolDBESSource",
    DBParameters = cms.PSet(
        authenticationPath = cms.untracked.string(''),
        authenticationSystem = cms.untracked.int32(0),
        connectionTimeout = cms.untracked.int32(0),
        messageLevel = cms.untracked.int32(0),
        security = cms.untracked.string('')
    ),
    DumpStat = cms.untracked.bool(False),
    ReconnectEachRun = cms.untracked.bool(False),
    RefreshAlways = cms.untracked.bool(False),
    RefreshEachRun = cms.untracked.bool(False),
    RefreshOpenIOVs = cms.untracked.bool(False),
    connect = cms.string('frontier://FrontierProd/CMS_CONDITIONS'),
    globaltag = cms.string('132X_dataRun3_Prompt_v7'),
    pfnPostfix = cms.untracked.string(''),
    pfnPrefix = cms.untracked.string(''),
    snapshotTime = cms.string(''),
    toGet = cms.VPSet()
)


process.HcalTimeSlewEP = cms.ESSource("HcalTimeSlewEP",
    appendToDataLabel = cms.string('HBHE'),
    timeSlewParametersM2 = cms.VPSet(
        cms.PSet(
            slope = cms.double(-3.178648),
            tmax = cms.double(16.0),
            tzero = cms.double(23.960177)
        ),
        cms.PSet(
            slope = cms.double(-1.5610227),
            tmax = cms.double(10.0),
            tzero = cms.double(11.977461)
        ),
        cms.PSet(
            slope = cms.double(-1.075824),
            tmax = cms.double(6.25),
            tzero = cms.double(9.109694)
        )
    ),
    timeSlewParametersM3 = cms.VPSet(
        cms.PSet(
            cap = cms.double(6.0),
            tspar0 = cms.double(12.2999),
            tspar0_siPM = cms.double(0.0),
            tspar1 = cms.double(-2.19142),
            tspar1_siPM = cms.double(0.0),
            tspar2 = cms.double(0.0),
            tspar2_siPM = cms.double(0.0)
        ),
        cms.PSet(
            cap = cms.double(6.0),
            tspar0 = cms.double(15.5),
            tspar0_siPM = cms.double(0.0),
            tspar1 = cms.double(-3.2),
            tspar1_siPM = cms.double(0.0),
            tspar2 = cms.double(32.0),
            tspar2_siPM = cms.double(0.0)
        ),
        cms.PSet(
            cap = cms.double(6.0),
            tspar0 = cms.double(12.2999),
            tspar0_siPM = cms.double(0.0),
            tspar1 = cms.double(-2.19142),
            tspar1_siPM = cms.double(0.0),
            tspar2 = cms.double(0.0),
            tspar2_siPM = cms.double(0.0)
        ),
        cms.PSet(
            cap = cms.double(6.0),
            tspar0 = cms.double(12.2999),
            tspar0_siPM = cms.double(0.0),
            tspar1 = cms.double(-2.19142),
            tspar1_siPM = cms.double(0.0),
            tspar2 = cms.double(0.0),
            tspar2_siPM = cms.double(0.0)
        )
    )
)


process.HepPDTESSource = cms.ESSource("HepPDTESSource",
    pdtFileName = cms.FileInPath('SimGeneral/HepPDTESSource/data/pythiaparticle.tbl')
)


process.eegeom = cms.ESSource("EmptyESSource",
    firstValid = cms.vuint32(1),
    iovIsRunNotTime = cms.bool(True),
    recordName = cms.string('EcalMappingRcd')
)


process.es_hardcode = cms.ESSource("HcalHardcodeCalibrations",
    GainWidthsForTrigPrims = cms.bool(False),
    HBRecalibration = cms.bool(False),
    HBmeanenergies = cms.FileInPath('CalibCalorimetry/HcalPlugins/data/meanenergiesHB.txt'),
    HBreCalibCutoff = cms.double(100.0),
    HERecalibration = cms.bool(False),
    HEmeanenergies = cms.FileInPath('CalibCalorimetry/HcalPlugins/data/meanenergiesHE.txt'),
    HEreCalibCutoff = cms.double(100.0),
    HFRecalParameterBlock = cms.PSet(
        HFdepthOneParameterA = cms.vdouble(
            0.004123, 0.00602, 0.008201, 0.010489, 0.013379,
            0.016997, 0.021464, 0.027371, 0.034195, 0.044807,
            0.058939, 0.125497
        ),
        HFdepthOneParameterB = cms.vdouble(
            -4e-06, -2e-06, 0.0, 4e-06, 1.5e-05,
            2.6e-05, 6.3e-05, 8.4e-05, 0.00016, 0.000107,
            0.000425, 0.000209
        ),
        HFdepthTwoParameterA = cms.vdouble(
            0.002861, 0.004168, 0.0064, 0.008388, 0.011601,
            0.014425, 0.018633, 0.023232, 0.028274, 0.035447,
            0.051579, 0.086593
        ),
        HFdepthTwoParameterB = cms.vdouble(
            -2e-06, -0.0, -7e-06, -6e-06, -2e-06,
            1e-06, 1.9e-05, 3.1e-05, 6.7e-05, 1.2e-05,
            0.000157, -3e-06
        )
    ),
    HFRecalibration = cms.bool(False),
    SiPMCharacteristics = cms.VPSet(
        cms.PSet(
            crosstalk = cms.double(0.0),
            nonlin1 = cms.double(1.0),
            nonlin2 = cms.double(0.0),
            nonlin3 = cms.double(0.0),
            pixels = cms.int32(36000)
        ),
        cms.PSet(
            crosstalk = cms.double(0.0),
            nonlin1 = cms.double(1.0),
            nonlin2 = cms.double(0.0),
            nonlin3 = cms.double(0.0),
            pixels = cms.int32(2500)
        ),
        cms.PSet(
            crosstalk = cms.double(0.17),
            nonlin1 = cms.double(1.00985),
            nonlin2 = cms.double(7.84089e-06),
            nonlin3 = cms.double(2.86282e-10),
            pixels = cms.int32(27370)
        ),
        cms.PSet(
            crosstalk = cms.double(0.196),
            nonlin1 = cms.double(1.00546),
            nonlin2 = cms.double(6.40239e-06),
            nonlin3 = cms.double(1.27011e-10),
            pixels = cms.int32(38018)
        ),
        cms.PSet(
            crosstalk = cms.double(0.17),
            nonlin1 = cms.double(1.00985),
            nonlin2 = cms.double(7.84089e-06),
            nonlin3 = cms.double(2.86282e-10),
            pixels = cms.int32(27370)
        ),
        cms.PSet(
            crosstalk = cms.double(0.196),
            nonlin1 = cms.double(1.00546),
            nonlin2 = cms.double(6.40239e-06),
            nonlin3 = cms.double(1.27011e-10),
            pixels = cms.int32(38018)
        ),
        cms.PSet(
            crosstalk = cms.double(0.0),
            nonlin1 = cms.double(1.0),
            nonlin2 = cms.double(0.0),
            nonlin3 = cms.double(0.0),
            pixels = cms.int32(0)
        )
    ),
    hb = cms.PSet(
        darkCurrent = cms.vdouble(0.0),
        doRadiationDamage = cms.bool(False),
        gain = cms.vdouble(0.19),
        gainWidth = cms.vdouble(0.0),
        mcShape = cms.int32(125),
        noiseCorrelation = cms.vdouble(0.0),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(3.285),
        pedestalWidth = cms.double(0.809),
        photoelectronsToAnalog = cms.double(0.3305),
        qieOffset = cms.vdouble(-0.49, 1.8, 7.2, 37.9),
        qieSlope = cms.vdouble(0.912, 0.917, 0.922, 0.923),
        qieType = cms.int32(0),
        recoShape = cms.int32(105),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(8)
    ),
    hbUpgrade = cms.PSet(
        darkCurrent = cms.vdouble(0.01, 0.015),
        doRadiationDamage = cms.bool(True),
        gain = cms.vdouble(0.0006252),
        gainWidth = cms.vdouble(0),
        mcShape = cms.int32(206),
        noiseCorrelation = cms.vdouble(0.26, 0.254),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(17.3),
        pedestalWidth = cms.double(1.5),
        photoelectronsToAnalog = cms.double(40.0),
        qieOffset = cms.vdouble(0.0, 0.0, 0.0, 0.0),
        qieSlope = cms.vdouble(0.05376, 0.05376, 0.05376, 0.05376),
        qieType = cms.int32(2),
        radiationDamage = cms.PSet(
            depVsNeutrons = cms.vdouble(5.543e-10, 8.012e-10),
            depVsTemp = cms.double(0.0631),
            intlumiOffset = cms.double(150),
            intlumiToNeutrons = cms.double(367000000.0),
            temperatureBase = cms.double(20),
            temperatureNew = cms.double(-5)
        ),
        recoShape = cms.int32(208),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(16)
    ),
    he = cms.PSet(
        darkCurrent = cms.vdouble(0.0),
        doRadiationDamage = cms.bool(False),
        gain = cms.vdouble(0.23),
        gainWidth = cms.vdouble(0),
        mcShape = cms.int32(125),
        noiseCorrelation = cms.vdouble(0.0),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(3.163),
        pedestalWidth = cms.double(0.9698),
        photoelectronsToAnalog = cms.double(0.3305),
        qieOffset = cms.vdouble(-0.38, 2.0, 7.6, 39.6),
        qieSlope = cms.vdouble(0.912, 0.916, 0.92, 0.922),
        qieType = cms.int32(0),
        recoShape = cms.int32(105),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(9)
    ),
    heUpgrade = cms.PSet(
        darkCurrent = cms.vdouble(0.01, 0.015),
        doRadiationDamage = cms.bool(True),
        gain = cms.vdouble(0.0006252),
        gainWidth = cms.vdouble(0),
        mcShape = cms.int32(206),
        noiseCorrelation = cms.vdouble(0.26, 0.254),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(17.3),
        pedestalWidth = cms.double(1.5),
        photoelectronsToAnalog = cms.double(40.0),
        qieOffset = cms.vdouble(0.0, 0.0, 0.0, 0.0),
        qieSlope = cms.vdouble(0.05376, 0.05376, 0.05376, 0.05376),
        qieType = cms.int32(2),
        radiationDamage = cms.PSet(
            depVsNeutrons = cms.vdouble(5.543e-10, 8.012e-10),
            depVsTemp = cms.double(0.0631),
            intlumiOffset = cms.double(75),
            intlumiToNeutrons = cms.double(29200000.0),
            temperatureBase = cms.double(20),
            temperatureNew = cms.double(5)
        ),
        recoShape = cms.int32(208),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(16)
    ),
    hf = cms.PSet(
        darkCurrent = cms.vdouble(0.0),
        doRadiationDamage = cms.bool(False),
        gain = cms.vdouble(0.14, 0.135),
        gainWidth = cms.vdouble(0.0, 0.0),
        mcShape = cms.int32(301),
        noiseCorrelation = cms.vdouble(0.0),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(9.354),
        pedestalWidth = cms.double(2.516),
        photoelectronsToAnalog = cms.double(0.0),
        qieOffset = cms.vdouble(-0.87, 1.4, 7.8, -29.6),
        qieSlope = cms.vdouble(0.359, 0.358, 0.36, 0.367),
        qieType = cms.int32(0),
        recoShape = cms.int32(301),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(-9999)
    ),
    hfUpgrade = cms.PSet(
        darkCurrent = cms.vdouble(0.0),
        doRadiationDamage = cms.bool(False),
        gain = cms.vdouble(0.14, 0.135),
        gainWidth = cms.vdouble(0.0, 0.0),
        mcShape = cms.int32(301),
        noiseCorrelation = cms.vdouble(0.0),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(13.33),
        pedestalWidth = cms.double(3.33),
        photoelectronsToAnalog = cms.double(0.0),
        qieOffset = cms.vdouble(0.0697, -0.7405, 12.38, -671.9),
        qieSlope = cms.vdouble(0.297, 0.298, 0.298, 0.313),
        qieType = cms.int32(1),
        recoShape = cms.int32(301),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(-9999)
    ),
    ho = cms.PSet(
        darkCurrent = cms.vdouble(0.0),
        doRadiationDamage = cms.bool(False),
        gain = cms.vdouble(0.006, 0.0087),
        gainWidth = cms.vdouble(0.0, 0.0),
        mcShape = cms.int32(201),
        noiseCorrelation = cms.vdouble(0.0),
        noiseThreshold = cms.double(0.0),
        pedestal = cms.double(12.06),
        pedestalWidth = cms.double(0.6285),
        photoelectronsToAnalog = cms.double(4.0),
        qieOffset = cms.vdouble(-0.44, 1.4, 7.1, 38.5),
        qieSlope = cms.vdouble(0.907, 0.915, 0.92, 0.921),
        qieType = cms.int32(0),
        recoShape = cms.int32(201),
        seedThreshold = cms.double(0.1),
        zsThreshold = cms.int32(24)
    ),
    iLumi = cms.double(-1.0),
    killHE = cms.bool(False),
    testHEPlan1 = cms.bool(False),
    testHFQIE10 = cms.bool(False),
    toGet = cms.untracked.vstring('GainWidths'),
    useHBUpgrade = cms.bool(True),
    useHEUpgrade = cms.bool(True),
    useHFUpgrade = cms.bool(True),
    useHOUpgrade = cms.bool(True),
    useIeta18depth1 = cms.bool(False),
    useLayer0Weight = cms.bool(True)
)


process.prefer("es_hardcode")

process.patAlgosToolsTask = cms.Task(process.PackedPFTowers, process.ak2PFUnsubJets, process.ak3PFUnsubJets, process.akCs2PFJets, process.akCs3PFJets, process.hiPuRho, process.patJetCorrFactorsAK2PFBtag, process.patJetCorrFactorsAK2PFUnsubJets, process.patJetCorrFactorsAK3PFBtag, process.patJetCorrFactorsAK3PFUnsubJets, process.patJetCorrFactorsAKCs2PF, process.patJetCorrFactorsAKCs3PF, process.patJetGenJetMatchAK2PFUnsubJets, process.patJetGenJetMatchAK3PFUnsubJets, process.patJetGenJetMatchAKCs2PF, process.patJetGenJetMatchAKCs3PF, process.patJetPartonMatchAK2PFUnsubJets, process.patJetPartonMatchAK3PFUnsubJets, process.patJetPartonMatchAKCs2PF, process.patJetPartonMatchAKCs3PF, process.patJetsAK2PFUnsubJets, process.patJetsAK3PFUnsubJets, process.patJetsAKCs2PF, process.patJetsAKCs3PF, process.selectedPatJetsAK2PFUnsubJets, process.selectedPatJetsAK3PFUnsubJets, process.selectedPatJetsAKCs2PF, process.selectedPatJetsAKCs3PF, process.selectedUpdatedPatJetsAK2PFBtag, process.selectedUpdatedPatJetsAK3PFBtag, process.unsubAK2JetMap, process.unsubAK3JetMap, process.unsubUpdatedPatJetsAK2PF, process.unsubUpdatedPatJetsAK3PF, process.updatedPatJetsAK2PFBtag, process.updatedPatJetsAK3PFBtag)


process.svTask = cms.Task(process.candidateVertexArbitrator, process.candidateVertexMerger, process.inclusiveCandidateSecondaryVertices, process.inclusiveCandidateVertexFinder)


process.siPixelRecHitsPreSplittingTask = cms.Task(process.siPixelRecHitsPreSplitting)


process.egammaSequence = cms.Sequence(process.correctedElectrons+process.hiElectrons+process.ggHiNtuplizer)


process.trackSequencePP = cms.Sequence(process.unpackedTracksAndVertices+process.ppTracks)


process.trackSequencePbPb = cms.Sequence(process.unpackedTracksAndVertices+process.PbPbTracks)


process.rhoSequence = cms.Sequence(process.kt4PFJetsForRho+process.hiFJRhoProducerFinerBins+process.hiFJGridEmptyAreaCalculatorFinerBins+process.hiFJRhoAnalyzerFinerBins)


process.muonSequence = cms.Sequence(process.rhoSequence+process.hiMuons+process.unpackedMuons)


process.muonSequencePP = cms.Sequence(process.muonAnalyzerPP)


process.muonSequencePbPb = cms.Sequence(process.unpackedTracksAndVertices+process.unpackedMuons+process.muonAnalyzer)


process.zMMSkimSequence = cms.Sequence(process.hltfilter+process.primaryVertexFilterForZMM+process.muonSelectorForZMM+process.muonFilterForZMM+process.dimuonMassCutForZMM+process.dimuonMassCutFilterForZMM)


process.forest = cms.Path(process.zMMSkimSequence+process.HiForestInfo+process.centralityBin+process.hiEvtAnalyzer+process.hltanalysis+process.trackSequencePbPb+process.muonSequence+process.muonAnalyzer+process.metFilters+process.akCs2PFJetAnalyzer+process.akCs3PFJetAnalyzer, process.patAlgosToolsTask)


process.pclusterCompatibilityFilter = cms.Path(process.zMMSkimSequence+process.clusterCompatibilityFilter)


process.pprimaryVertexFilter = cms.Path(process.zMMSkimSequence+process.primaryVertexFilter)


process.pphfCoincFilter4Th2 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter4Th2)


process.pphfCoincFilter1Th3 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter1Th3)


process.pphfCoincFilter2Th3 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter2Th3)


process.pphfCoincFilter3Th3 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter3Th3)


process.pphfCoincFilter4Th3 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter4Th3)


process.pphfCoincFilter5Th3 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter5Th3)


process.pphfCoincFilter1Th4 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter1Th4)


process.pphfCoincFilter2Th4 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter2Th4)


process.pphfCoincFilter3Th4 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter3Th4)


process.pphfCoincFilter4Th4 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter4Th4)


process.pphfCoincFilter5Th4 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter5Th4)


process.pphfCoincFilter1Th5 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter1Th5)


process.pphfCoincFilter2Th5 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter2Th5)


process.pphfCoincFilter3Th5 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter3Th5)


process.pphfCoincFilter4Th5 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter4Th5)


process.pphfCoincFilter5Th5 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter5Th5)


process.pphfCoincFilter1Th6 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter1Th6)


process.pphfCoincFilter2Th6 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter2Th6)


process.pphfCoincFilter3Th6 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter3Th6)


process.pphfCoincFilter4Th6 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter4Th6)


process.pphfCoincFilter5Th6 = cms.Path(process.zMMSkimSequence+process.phfCoincFilter5Th6)


process.zMMSkimPath = cms.Path(process.zMMSkimSequence+process.zMMSkimSequence)


process.pAna = cms.EndPath(process.skimanalysis)


