from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'test21_data_ZMM_PbPb24B_Prime14'

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'forest_miniAOD_run3_DATA_ZMM.py'
config.JobType.maxMemoryMB = 2500
config.JobType.maxJobRuntimeMin = 1200
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/HIPhysicsRawPrime14/HIRun2024B-PromptReco-v1/MINIAOD'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.publication = False
config.Data.inputDBS = 'global'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_Analysis_test21_ZMM_PbPb24_Prime14'

# These values only make sense for processing data
#    Select input data based on a lumi mask
config.Data.lumiMask = 'Cert_Collisions2024_HI_387853_388784_Golden.json'

# Where the output files will be transmitted to
config.Site.storageSite = 'T3_IT_Trieste'
