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
process.GlobalTag.globaltag = '101X_dataRun2_Prompt_v11'

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.options = cms.untracked.PSet( SkipEvent =
cms.untracked.vstring('ProductNotFound') )

process.source = cms.Source ("PoolSource",
        fileNames = cms.untracked.vstring(
             '/store/data/Run2018A/SingleMuon/RAW-RECO/ZMu-PromptReco-v3/000/316/995/00000/FAAE6734-BA66-E811-8D2D-02163E019EBA.root',
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
     threshold = cms.untracked.string('DEBUG') 
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

#process.load("EventFilter.CSCRawToDigi.cscUnpacker_cfi")
#process.load("RecoLocalMuon.Configuration.RecoLocalMuon_cff")

process.MakeNtuple = cms.EDAnalyzer("CSCPatternExtractor",
        NtupleFileName       = cms.untracked.string('CSCDigiTree.root'),
             muonCollection = cms.InputTag("muons"),
        offlineBeamSpotTag = cms.InputTag("offlineBeamSpot"),
        csctfDigiTag = cms.InputTag("csctfDigis"),
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
        minPt = cms.double(20.),
        dataType = cms.untracked.string('jPsi'),
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
process.l1csctpconf.alctParamMTCC2.alctNplanesHitPretrig = 3
process.l1csctpconf.alctParamMTCC2.alctNplanesHitAccelPretrig = 3
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

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('TPEHists.root')
                                   )

process.p = cms.Path(process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)
#process.p = cms.Path(process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis)


process.schedule.append(process.p)


#process.run = cms.Path(process.muonCSCDigis+process.cscTriggerPrimitiveDigis+process.MakeNtuple)

