import FWCore.ParameterSet.Config as cms
import subprocess

#CHECK ERA, GLOBALTAG, GEOMETRY

from Configuration.StandardSequences.Eras import eras
process = cms.Process("TEST",eras.Run2_2018)

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

process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data_promptlike', '')


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


#
# CHARMONIUM TEST
#
#dataset = '/store/data/Run2018C/Charmonium/RAW/v1/000/319/337/00000/1C4B30FD-6282-E811-96B1-02163E015211.root' #20182nd
#dataset = ' /store/data/Run2017F/Charmonium/RAW/v1/000/305/045/00000/F4100DFC-ACB0-E711-9C0C-02163E01A714.root' #tested against camerons oct 11


#
# SINGLE MUON TEST
#
dataset = '/store/data/Run2018D/SingleMuon/RAW-RECO/ZMu-PromptReco-v2/000/324/970/00000/FFCA0A00-2503-4242-8CAE-E1AC6E574063.root'

#
# MUON GUN TEST
#
#dataset = ' /store/group/phys_muon/abbiendi/MuonGun/MuonGun_PTOT-5-2500/crab_MuonGun_step3_asympt_FixedGT-TrkAli2017-v3/181215_114558/0000/step3_asympt_APE_FixedGT-TrackerAlignment-Upgrade2017-realistic-v3_99.root'
#self-made sample
#dataset = 'file:/uscms/home/wnash/eos/MuonGun/MuonGunPt100.root'


#
# ZERO BIAS TEST
#
#dataset = '/store/data/Run2018D/ZeroBias1/RAW/v1/000/324/725/00000/F54897BA-7012-8546-A9E0-48CE9AD9EBD7.root'
#selectionString = 'ZeroBias'

#all the selections that the extractor is capable of handling
selections = ['SingleMuon','Charmonium', 'MuonGun']
for selectionString in selections:
    if selectionString in dataset: #see if the selection is in the dataset to use as default
        break
    
print "Data Selection is :" + selectionString


run = dataset.split('/')[3]
#
# Need to match file name with the one specified in CRAB
#
outfileName = 'CSCDigiTree_'+run+'_'+selectionString+'.root'


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


process.load("L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi")
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


process.p = cms.Path(process.gtDigis * process.muonCSCDigis * process.csc2DRecHits * process.cscSegments * process.cscTriggerPrimitiveDigis * process.MakeNtuple)
process.schedule = cms.Schedule(process.p)

process.schedule.associate(process.patTask)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)


from Configuration.DataProcessing.RecoTLR import customisePostEra_Run2_2018 
process = customisePostEra_Run2_2018(process)



process.options = cms.untracked.PSet(

)

