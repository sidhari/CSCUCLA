from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#CRAB COPIES ALL FOLDERS NAMED "data" IN YOUR CMSSW DIRECTORY


dataset = '/SingleMuon/Run2018D-ZMu-PromptReco-v2/RAW-RECO'
run = dataset.split('/')[2].split('-')[0]
selectionString = dataset.split('/')[1]
outfileName = 'CSCDigiTree_'+run+'_'+selectionString+'.root'

config.General.requestName = 'zskim2018D_redo_CSCDigiTree'
config.General.workArea = 'output'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = 'makeCSCDigiTree_RAW_legacy_cfg.py'
config.JobType.psetName = 'makeCSCDigiTree_cfg.py'
#config.JobType.numCores = 8
config.JobType.allowUndistributedCMSSW = True
config.JobType.disableAutomaticOutputCollection = True
config.JobType.outputFiles = [outfileName]



#config.Data.inputDataset = '/Charmonium/Run2017D-v1/RAW'
#config.Data.inputDataset = '/Charmonium/Run2018C-v1/RAW'
#config.Data.inputDataset = '/SingleMuon/CMSSW_10_3_0_pre5-ZMu-103X_dataRun2_PromptLike_v6_RelVal_sigMu2018D-v1/RAW-RECO'
config.Data.inputDataset = dataset
config.Data.inputDBS = 'global'
#config.Data.splitting = 'Automatic'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
#config.Data.lumiMask = '/uscms/home/wnash/nobackup/cscpatterns/ntuple/CMSSW_9_2_14/src/CSCUCLA/CSCDigiTuples/crab/muonJSON18.txt'
config.Data.lumiMask = '/uscms/home/wnash/CSCUCLA/CSCDigiTuples/crab/muonJSON18-w3LayerFirmware.txt'
#config.Data.lumiMask = '/uscms/home/wnash/CSCUCLA/CSCDigiTuples/crab/muonJSON17.txt'
config.Data.outLFNDirBase = '/store/user/%s' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = 'zskim2018D-redo'
#config.Data.ignoreLocality = True

config.Site.storageSite = 'T3_US_FNALLPC'
#config.Site.ignoreGlobalBlacklist = True
#config.Site.whitelist = ['T2_RU_JINR','T2_CH_CERN']
