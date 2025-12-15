from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'test16_ZMM_PPRefSingleMuon0'

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'forest_miniAOD_run3_ppref_DATA_ZMM.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 1200
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/PPRefSingleMuon0/Run2024J-PromptReco-v1/MINIAOD'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.publication = False
config.Data.inputDBS = 'global'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_Analysis_test16_ZMM_PPRefSingleMuon0'

# These values only make sense for processing data
#    Select input data based on a lumi mask
config.Data.lumiMask = 'Cert_Collisions2024_ppref_387474_387721_golden.json'

# Where the output files will be transmitted to
config.Site.storageSite = 'T3_IT_Trieste'
