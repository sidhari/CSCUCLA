from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#CRAB COPIES ALL FOLDERS NAMED "data" IN YOUR CMSSW DIRECTORY


selectionString = 'MuonGun'
outfileName = 'GenRawRecoE0-4000_'+selectionString+'.root'

config.General.requestName = 'MuonEGun'
config.General.workArea = 'output'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'SingleMuE0-4000_generator.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.disableAutomaticOutputCollection = True
config.JobType.outputFiles = [outfileName]


config.Data.splitting = 'EventBased'
config.Data.unitsPerJob = 100
config.Data.totalUnits = config.Data.unitsPerJob * 10000
config.Data.outLFNDirBase = '/store/user/%s' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'MuonEGun-Apr2019'
config.Data.outputPrimaryDataset = 'MuonGun'


config.Site.storageSite = 'T3_US_FNALLPC'

