from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#CRAB COPIES ALL FOLDERS NAMED "data" IN YOUR CMSSW DIRECTORY


dataset = '/MuonGun/wnash-MuonEGun-Apr2019-ede95c48d94927e80cb62859901c191e/USER'
outfileName = 'CSCDigiTree_MuonGun.root'

config.General.requestName = 'MuonGun_CSCDigiTree'
config.General.workArea = 'output'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'makeCSCDigiTree_MC_cfg.py'
#config.JobType.numCores = 8
config.JobType.allowUndistributedCMSSW = True
config.JobType.disableAutomaticOutputCollection = True
config.JobType.outputFiles = [outfileName]



config.Data.inputDataset = dataset
config.Data.inputDBS = 'phys03'
config.Data.splitting = 'Automatic'
config.Data.unitsPerJob = 200
config.Data.outLFNDirBase = '/store/user/%s' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = 'MuonGun_CSC'

config.Site.storageSite = 'T3_US_FNALLPC'
#config.Site.ignoreGlobalBlacklist = True
#config.Site.whitelist = ['T2_RU_JINR','T2_CH_CERN']
