from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'test24_mc_ZMM_DYto2L2Jets'

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'forest_miniAOD_run3_MC_ZMM_Gen.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 1200
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/DYto2L-2Jets_MLL-50_TuneCP5_5p36TeV_amcatnloFXFX-pythia8/HINPbPbSpring23MiniAOD-132X_mcRun3_2023_realistic_HI_v9-v3/MINIAODSIM'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.inputDBS = 'global'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_Analysis_test24_ZMM_DYto2L2Jets'

# These values only make sense for processing data
#    Select input data based on a lumi mask
#config.Data.lumiMask = 'Cert_Collisions2023HI_374288_375823_Golden.json'

# Where the output files will be transmitted to
config.Site.storageSite = 'T3_IT_Trieste'
