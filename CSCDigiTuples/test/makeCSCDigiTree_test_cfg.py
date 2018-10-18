import FWCore.ParameterSet.Config as cms
import subprocess

process = cms.Process("TEST")

process.load("Configuration/StandardSequences/GeometryDB_cff")
process.load("Configuration/StandardSequences/MagneticField_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load("Configuration/StandardSequences/RawToDigi_Data_cff")
process.load("Configuration.StandardSequences.Reconstruction_cff")

process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")

from Configuration.AlCa.GlobalTag import GlobalTag


#Need to change this parameter depending on dataset you run over, tells you detector alignment
# Find here: https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions
#process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v4'
process.GlobalTag.globaltag = '101X_dataRun2_Prompt_v11' #2018 CMSSW_10_1_7
#process.GlobalTag.globaltag = '92X_dataRun2_Prompt_v11' #2017 CMSSW_9_2_13

#
# CORRECT LUMI SECTIONS
#
#import FWCore.PythonUtilities.LumiList as LumiList
#import FWCore.ParameterSet.Types as CfgTypes
#process.inputs = cms.PSet (
#    lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
#)
#JSONfile = 'test/golden2018.json'
#myList = LumiList.LumiList (filename = JSONfile).getCMSSWString().split(',')
#process.inputs.lumisToProcess.extend(myList)


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
process.options = cms.untracked.PSet( SkipEvent =
cms.untracked.vstring('ProductNotFound') )


# samples:
#   singlemu: '/store/data/Run2018A/SingleMuon/RAW-RECO/ZMu-PromptReco-v3/000/316/995/00000/FAAE6734-BA66-E811-8D2D-02163E019EBA.root',
#            '101X_dataRun2_Prompt_v11'
#
#            '/store/data/Run2017D/SingleMuon/RAW/v1/000/302/031/00000/005D3323-8A8D-E711-B3C9-02163E01A517.root' //2017
#
#    jpsi: '/store/data/Run2018C/Charmonium/RAW/v1/000/320/065/00000/FE58EEB1-178E-E811-97C5-FA163E5ED053.root',
#            '101X_dataRun2_Prompt_v11'
#
#    zerobias: '/store/data/Run2018D/ZeroBias7/RAW-RECO/LogError-PromptReco-v2/000/323/399/00000/FC46AD98-6140-1C49-BB39-7B1293D338A0.root'
#            '101X_dataRun2_Prompt_v11'
#

#dataset = '/store/data/Run2018C/Charmonium/RAW/v1/000/320/065/00000/FE58EEB1-178E-E811-97C5-FA163E5ED053.root'
#dataset = 'file:/uscms/home/wnash/eos/Charmonium/charmonium2018C/test/RAW/FE58EEB1-178E-E811-97C5-FA163E5ED053.root'
#dataset = '/store/data/Run2018C/Charmonium/RAW/v1/000/319/756/00000/50DA85EC-4D8A-E811-AB28-FA163E56AF73.root' #20182nd
#dataset = '/store/data/Run2018B/Charmonium/RAW/v1/000/317/089/00000/CCB70690-4C63-E811-A9BD-FA163E851C2E.root' #20182nd
#dataset = '/store/data/Run2018A/Charmonium/RAW/v1/000/315/257/00000/1020D514-A349-E811-AA19-FA163EDB8EB7.root' #20182nd
#dataset = '/store/data/Run2018C/Charmonium/RAW/v1/000/319/337/00000/1C4B30FD-6282-E811-96B1-02163E015211.root' #20182nd
#dataset = ' /store/data/Run2017F/Charmonium/RAW/v1/000/305/045/00000/F4100DFC-ACB0-E711-9C0C-02163E01A714.root' #tested against camerons oct 11
#dataset = '/store/data/Run2017D/SingleMuon/RAW/v1/000/302/031/00000/005D3323-8A8D-E711-B3C9-02163E01A517.root'
#dataset = '/store/data/Run2017D/SingleMuon/RAW-RECO/ZMu-PromptReco-v1/000/302/031/00000/00060D74-2D8F-E711-9FEA-02163E011A48.root'
dataset = '/store/relval/CMSSW_10_3_0_pre5/SingleMuon/RAW-RECO/ZMu-103X_dataRun2_PromptLike_v6_RelVal_sigMu2018D-v1/10000/8007A95E-58B7-1947-9987-E4F3310241F0.root'

selectionString = dataset.split('/')[4]

#selectionString = 'jPsi'
run = dataset.split('/')[3]
outfileName = 'CSCDigiTree_'+run+'_'+selectionString+'.root'


isRawReco = 'RAW-RECO' in dataset
isRawOnly = not isRawReco and 'RAW' in dataset
if not isRawReco and not isRawOnly:
    print "Error: Unknown dataset: %s"%dataset
    exit()

process.source = cms.Source ("PoolSource",
        fileNames = cms.untracked.vstring(
dataset,
            )

)
process.MessageLogger = cms.Service("MessageLogger",
    cout = cms.untracked.PSet(
        default = cms.untracked.PSet( limit = cms.untracked.int32(-1) ),
        FwkJob = cms.untracked.PSet( limit = cms.untracked.int32(0) )
    ),
    categories = cms.untracked.vstring('FwkJob'),
    destinations = cms.untracked.vstring('cout'),
     debugModules = cms.untracked.vstring('*'),
     threshold = cms.untracked.string('ERROR') 
)


"""Customise digi/reco geometry to use unganged ME1/a channels"""
process.CSCGeometryESModule.useGangedStripsInME1a = False
process.idealForDigiCSCGeometry.useGangedStripsInME1a = False


# filter on trigger path
process.triggerSelection = cms.EDFilter( "TriggerResultsFilter",
    triggerConditions = cms.vstring(''),
    hltResults = cms.InputTag( "TriggerResults", "", "HLT" ),
    l1tResults = cms.InputTag( "gtDigis" ),
    l1tIgnoreMask = cms.bool( False ),
    l1techIgnorePrescales = cms.bool( False ),
    daqPartitions = cms.uint32( 1 ),
    throw = cms.bool( True )
)

process.MakeNtuple = cms.EDAnalyzer("CSCPatternExtractor",
        NtupleFileName       = cms.untracked.string(outfileName),
             muonCollection = cms.InputTag("muons"),
             
        trigBits                        = cms.InputTag('TriggerResults','','HLT'),
        trigPrescales                   = cms.InputTag('patTrigger'),       
             
        offlineBeamSpotTag = cms.InputTag("offlineBeamSpot"),
        csctfDigiTag = cms.InputTag("csctfDigis"),
        vertices = cms.InputTag('offlinePrimaryVertices'),
        emtfDigiTag = cms.InputTag("emtfStage2Digis"),
        wireDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCWireDigi"),
        stripDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCStripDigi"),
        alctDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCALCTDigi"),
        clctDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCCLCTDigi"),
        lctDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCCorrelatedLCTDigi"),
        compDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCComparatorDigi"),
        dduDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCDDUStatusDigi"),
        dmbDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCDMBStatusDigi"),
        tmbDigiTag = cms.InputTag("muonCSCDigis", "MuonCSCTMBStatusDigi"),
        selection = cms.untracked.string(selectionString),
        MatchParameters = cms.PSet(
            DTsegments = cms.InputTag("dt4DSegments"),
            DTradius = cms.double(0.1),
            CSCsegments = cms.InputTag("cscSegments"),
            TightMatchDT = cms.bool(False),
            TightMatchCSC = cms.bool(True)
            ),
        ServiceParameters = cms.PSet(
            Propagators = cms.untracked.vstring('SteppingHelixPropagatorAny'),
            RPCLayers = cms.bool(True)
            )
        )


process.load("L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi")
process.cscTriggerPrimitiveDigis.CSCComparatorDigiProducer = "muonCSCDigis:MuonCSCComparatorDigi"
process.cscTriggerPrimitiveDigis.CSCWireDigiProducer = "muonCSCDigis:MuonCSCWireDigi"
process.cscTriggerPrimitiveDigis.tmbParam.mpcBlockMe1a = 0
process.load("L1TriggerConfig.L1CSCTPConfigProducers.L1CSCTriggerPrimitivesConfig_cff")
#test if something exists by doing python -i <filename>, then typing the object you're curious about

# >= CMSSW_10_3_+
process.l1csctpconf.alctParam.alctNplanesHitPretrig = 3
process.l1csctpconf.alctParam.alctNplanesHitAccelPretrig = 3
# < CMSSW_10_3
#process.l1csctpconf.alctParamMTCC2.alctNplanesHitPretrig = 3
#process.l1csctpconf.alctParamMTCC2.alctNplanesHitAccelPretrig = 3
process.l1csctpconf.clctParam.clctNplanesHitPretrig = 3
process.l1csctpconf.clctParam.clctHitPersist = 4

process.lctreader = cms.EDAnalyzer("CSCTriggerPrimitivesReader",
    debug = cms.untracked.bool(False),
    dataLctsIn = cms.bool(True),
    emulLctsIn = cms.bool(True),
    isMTCCData = cms.bool(False),
    printps = cms.bool(False),
    CSCLCTProducerData = cms.untracked.string("muonCSCDigis"),
    CSCLCTProducerEmul = cms.untracked.string("cscTriggerPrimitiveDigis"),
    CSCSimHitProducer = cms.InputTag("g4SimHits", "MuonCSCHits"),  # Full sim.
    CSCComparatorDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCComparatorDigi"),
    CSCWireDigiProducer = cms.InputTag("simMuonCSCDigis","MuonCSCWireDigi")
)

# CSCTF stuff
process.load("DQMServices.Components.DQMEnvironment_cfi")

#process.load("DQM.L1TMonitor.environment_file_cff")
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.dqmSaver.convention = 'Online'
process.dqmSaver.referenceHandling = 'all'
process.dqmSaver.dirName = '.'
process.dqmSaver.producer = 'DQM'
process.dqmSaver.saveByLumiSection = -1
process.dqmSaver.saveByRun = 1
process.dqmSaver.saveAtJobEnd = True
process.load("DQM.Integration.config.dqmPythonTypes")
es_prefer_GlobalTag = cms.ESPrefer('GlobalTag')
process.load("DQM.L1TMonitor.L1TMonitor_cff")
process.load("DQM.L1TMonitor.L1TSync_cff")
process.load("DQM.L1TMonitorClient.L1TMonitorClient_cff")
process.l1tSync.oracleDB = cms.string("oracle://cms_orcon_adg/CMS_COND_31X_L1T")
process.l1tSync.pathCondDB = cms.string("/afs/cern.ch/cms/DB/conddb/ADG")
process.l1tRate.oracleDB = cms.string("oracle://cms_orcon_adg/CMS_COND_31X_L1T")
process.l1tRate.pathCondDB = cms.string("/afs/cern.ch/cms/DB/conddb/ADG")
process.l1tMonitorPath = cms.Path(process.l1tMonitorOnline)
process.l1tMonitorClientPath = cms.Path(process.l1tMonitorClient)
process.l1tSyncPath = cms.Path(process.l1tSyncHltFilter+process.l1tSync)
process.l1tMonitorEndPath = cms.EndPath(process.l1tMonitorEndPathSeq)
process.l1tMonitorClientEndPath = cms.EndPath(process.l1tMonitorClientEndPathSeq)
process.l1tMonitorPath = cms.Path(process.csctfDigis * process.l1tCsctf)
process.dqmEndPath = cms.EndPath(
                                 process.dqmEnv *
                                 process.dqmSaver
                                 )
process.schedule = cms.Schedule(#process.rawToDigiPath,
                                process.l1tMonitorPath,
                                process.l1tSyncPath,
                                process.l1tMonitorClientPath,
                                #process.l1tMonitorEndPath,
                                process.l1tMonitorClientEndPath,
                                process.dqmEndPath
                                )
process.l1tMonitorOnline.remove(process.bxTiming)
process.l1tMonitorOnline.remove(process.l1tBPTX)
process.l1tMonitorOnline.remove(process.l1tDttf)
#process.l1tMonitorOnline.remove(process.l1tCsctf)
process.l1tMonitorOnline.remove(process.l1tRpctf)
process.l1tMonitorOnline.remove(process.l1tGmt)
process.l1tMonitorOnline.remove(process.l1tGt)
process.l1tMonitorOnline.remove(process.l1ExtraDqmSeq)
process.l1tMonitorOnline.remove(process.l1tRate)
process.l1tMonitorOnline.remove(process.l1tRctSeq)
process.l1tMonitorOnline.remove(process.l1tGctSeq)
#process.l1tMonitorEndPathSeq.remove(process.l1s)
#process.l1tMonitorEndPathSeq.remove(process.l1tscalers)
process.schedule.remove(process.l1tSyncPath)

process.l1tCsctf.gmtProducer = "null"
process.l1tCsctf.gangedME11a = False

from Configuration.StandardSequences.Eras import eras
eras.run2_common.toModify( process.l1tCsctf, gangedME11a = False )

#process.TFileService = cms.Service("TFileService",
#                                  fileName = cms.string('TPEHists.root')
#                                  )
# TODO: RAW-RECO appears to crash unless full reconstruction is done. Find out why
#if isRawReco:
#    process.p = cms.Path(process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)
#if isRawOnly:
#    process.p = cms.Path(process.RawToDigi * process.reconstruction * process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)
#process.p = cms.Path(process.RawToDigi * process.reconstruction * process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)
process.p = cms.Path(process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)

process.schedule.append(process.p)

