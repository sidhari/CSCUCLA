from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#CRAB COPIES ALL FOLDERS NAMED "data" IN YOUR CMSSW DIRECTORY


#run on lxplus or lpc?
runOnLpc = True

dataset = '/Charmonium/Run2018B-v1/RAW'
run = dataset.split('/')[2].split('-')[0]
selectionString = dataset.split('/')[1]
outfileName = 'CSCDigiTree_'+run+'_'+selectionString+'.root'

config.General.requestName = 'charmonium2018B'
config.General.workArea = 'output'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
#config.JobType.psetName = 'makeCSCDigiTree_RAW_legacy_cfg.py'
config.JobType.psetName = 'makeCSCDigiTree_RAW_cfg.py'
#config.JobType.numCores = 8
config.JobType.allowUndistributedCMSSW = True
config.JobType.disableAutomaticOutputCollection = True
config.JobType.outputFiles = [outfileName]



config.Data.inputDataset = dataset
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 3
if runOnLpc:
    config.Site.storageSite = 'T3_US_FNALLPC'
    config.Data.lumiMask = '/uscms/home/wnash/public/muonJSON18.txt'
else:
    config.Site.storageSite = 'T2_CH_CERN'
    config.Data.lumiMask = '/afs/cern.ch/user/w/wnash/CSCUCLA/CSCDigiTuples/crab/muonJSON18.txt' #on lxplus
config.Data.outLFNDirBase = '/store/user/%s' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = 'charmonium2018B'

