from CRABClient.UserUtilities import config
from CRABClient.UserUtilities import config, getLumiListInValidFiles
from FWCore.PythonUtilities.LumiList import LumiList

config = config()

config.General.requestName = 'test11_Prime21_missing'

goodLumi = LumiList(filename='Cert_Collisions2023HI_374288_375823_Golden.json')

processedLumi = LumiList(filename='/gfsvol01/cms/users/kdeleo/work/Analysis_HI_2025/CMSSW_13_2_4/src/HeavyIonsAnalysis/Configuration/test/crabJobs/crab_test11_Prime21/results/processedLumis.json')

newLumi = goodLumi - processedLumi

newLumi.writeJSON('newLumi_test11_Prime21_missing.json')

config.JobType.pluginName = 'Analysis'
# Name of the CMSSW configuration file
config.JobType.psetName = 'forest_miniAOD_run3_DATA_Zmumu.py'
config.JobType.maxMemoryMB = 2200
config.JobType.maxJobRuntimeMin = 1200
config.JobType.allowUndistributedCMSSW = True

config.Data.inputDataset = '/HIPhysicsRawPrime21/HIRun2023A-PromptReco-v2/MINIAOD'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 10
config.Data.publication = False
config.Data.inputDBS = 'global'
# This string is used to construct the output dataset name
config.Data.outputDatasetTag = 'CRAB3_Analysis_test11_Prime21'

# These values only make sense for processing data
#    Select input data based on a lumi mask
config.Data.lumiMask = 'newLumi_test11_Prime21_missing.json'

# Where the output files will be transmitted to
config.Site.storageSite = 'T3_IT_Trieste'
config.Site.ignoreGlobalBlacklist = True
