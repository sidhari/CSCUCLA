from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'charmonium2017B_CSCDigiTree'
config.General.workArea = 'output'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'makeCSCDigiTree_RAW_cfg.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.disableAutomaticOutputCollection = True
config.JobType.outputFiles = ['CSCDigiTree.root']

config.Data.inputDataset = '/Charmonium/Run2017B-v1/RAW'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
config.Data.lumiMask = '/uscms/home/wnash/nobackup/Old/cscpatterns/ntuple/CMSSW_9_2_14/src/CSCUCLA/CSCDigiTuples/crab/muonJSON17.txt'
config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = 'charmonium2017B'
config.Data.ignoreLocality = True

config.Site.storageSite = 'T3_US_FNALLPC'
config.Site.ignoreGlobalBlacklist = True
config.Site.whitelist = ['T2_RU_JINR','T2_CH_CERN']
