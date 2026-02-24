from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'test24_ZMM_Prime9'

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'forest_miniAOD_run3_DATA_ZMM.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 1200
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/HIPhysicsRawPrime9/HIRun2023A-PromptReco-v2/MINIAOD'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.publication = False
config.Data.inputDBS = 'global'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_Analysis_test24_ZMM_Prime9'

# These values only make sense for processing data
#    Select input data based on a lumi mask
config.Data.lumiMask = 'Cert_Collisions2023HI_374288_375823_Golden.json'

# Where the output files will be transmitted to
config.Site.storageSite = 'T3_IT_Trieste'
