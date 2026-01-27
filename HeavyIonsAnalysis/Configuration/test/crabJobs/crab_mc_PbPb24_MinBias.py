from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'test20_mc_PbPb24_MinBias'

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'forest_miniAOD_run3_MC_MinBias.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 1200
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/MinBias_Drum5F_5p36TeV_hydjet/HINPbPbWinter24MiniAOD-NoPU_141X_mcRun3_2024_realistic_HI_v14-v2/MINIAODSIM'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.publication = False
config.Data.inputDBS = 'global'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_Analysis_test20_mc_PbPb24_MinBias'

# These values only make sense for processing data
#    Select input data based on a lumi mask
#config.Data.lumiMask = 'Cert_Collisions2023HI_374288_375823_Golden.json'

# Where the output files will be transmitted to
config.Site.storageSite = 'T3_IT_Trieste'
