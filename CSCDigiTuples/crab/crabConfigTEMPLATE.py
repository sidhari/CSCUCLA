from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

#CRAB COPIES ALL FOLDERS NAMED "data" IN YOUR CMSSW DIRECTORY

dataset   = '' #FILLTHIS
lumimask  = '' #FILLTHIS
outputtag = '' #FILLTHIS

#
# lumimask examples:
# '/uscms/home/wnash/nobackup/cscpatterns/ntuple/CMSSW_9_2_14/src/CSCUCLA/CSCDigiTuples/crab/muonJSON18.txt'
# '/uscms/home/wnash/CSCUCLA/CSCDigiTuples/crab/muonJSON18-w3LayerFirmware.txt'
# '/uscms/home/wnash/nobackup/cscpatterns/ntuple/CMSSW_9_2_14/src/CSCUCLA/CSCDigiTuples/crab/muonJSON17.txt'
#


run = dataset.split('/')[2].split('-')[0]
selectionString = dataset.split('/')[1]
outfileName = 'CSCDigiTree_'+run+'_'+selectionString+'.root'

config.General.requestName = ''
config.General.workArea = 'output'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'makeCSCDigiTree_cfg.py'
config.JobType.allowUndistributedCMSSW = True
config.JobType.disableAutomaticOutputCollection = True
config.JobType.outputFiles = [outfileName]


config.Data.inputDataset = dataset
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
config.Data.lumiMask = lumimask
config.Data.outLFNDirBase = '/store/user/%s' % (getUsernameFromSiteDB())
config.Data.publication = False
config.Data.outputDatasetTag = outputtag


config.Site.storageSite = 'T3_US_FNALLPC'

