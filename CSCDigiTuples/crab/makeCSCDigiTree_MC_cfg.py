import FWCore.ParameterSet.Config as cms
import subprocess

from Configuration.StandardSequences.Eras import eras
process = cms.Process("TEST")

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.Skims_cff')
process.load('CommonTools.ParticleFlow.EITopPAG_cff')
process.load('PhysicsTools.PatAlgos.slimming.metFilterPaths_cff')
process.load('Configuration.StandardSequences.PAT_cff')
process.load('Configuration.StandardSequences.AlCaRecoStreams_cff')
process.load('DQMOffline.Configuration.DQMOffline_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")

from Configuration.AlCa.GlobalTag import GlobalTag

#different between reco - mc
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:mc', '')



process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

dataset = 'file:/uscms/home/wnash/eos/MuonGun/MuonEGun-Jan2019/190117_224028/0000/GenRawRecoE0-4000_MuonGun_1.root'

#all the selections that the extractor is capable of handling
selections = ['SingleMuon','Charmonium', 'MuonGun']
for selectionString in selections:
    if selectionString in dataset: #see if the selection is in the dataset to use as default
        break
    
print "Data Selection is :" + selectionString


outfileName = 'CSCDigiTree_'+selectionString+'.root'


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
        pfCandTag = cms.InputTag("particleFlow"),
        rhDigiTag = cms.InputTag("csc2DRecHits"),
        genDigiTag = cms.InputTag("genParticles"),
        simDigiTag = cms.InputTag("g4SimHits:MuonCSCHits"),
        ebCaloDigiTag = cms.InputTag("g4SimHits:EcalHitsEB"),
        eeCaloDigiTag = cms.InputTag("g4SimHits:EcalHitsEE"),
        esCaloDigiTag = cms.InputTag("g4SimHits:EcalHitsES"),
        hCaloDigiTag = cms.InputTag("g4SimHits:HcalHits"),
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

"""Customise digi/reco geometry to use unganged ME1/a channels"""
process.CSCGeometryESModule.useGangedStripsInME1a = False
process.idealForDigiCSCGeometry.useGangedStripsInME1a = False


process.load("L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi")
#nick change,temporarily removed
process.cscTriggerPrimitiveDigis.CSCComparatorDigiProducer = "muonCSCDigis:MuonCSCComparatorDigi"
process.cscTriggerPrimitiveDigis.CSCWireDigiProducer = "muonCSCDigis:MuonCSCWireDigi"


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

# TODO: RAW-RECO appears to crash unless full reconstruction is done. Find out why

process.p = cms.Path(process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)
process.schedule = cms.Schedule(process.p)


process.schedule.associate(process.patTask)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)



process.options = cms.untracked.PSet(

)


