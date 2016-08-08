// -*- C++ -*-
//
// Package:    CSCPatterns
// Class:      CSCPatterns
// 
/**\class CSCPatterns CSCPatterns.cc Timing/NtupleMaker/src/CSCPatterns.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  "Christopher Farrell"
//         Created:  Thu Jul 22 06:19:36 CDT 2010
// $Id$
//
//


// system include files
#include <memory>
#include <iterator>

// user include files
#include "CSCUCLA/CSCDigiTuples/include/CSCPatterns.h"
//


//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

#include "DataFormats/CSCRecHit/interface/CSCSegmentCollection.h"


CSCPatterns::CSCPatterns(const edm::ParameterSet& iConfig)

{
    //now do what ever initialization is needed
    mc_token = consumes<reco::MuonCollection>( iConfig.getParameter<edm::InputTag>("muonCollection") );
    wd_token = consumes<CSCWireDigiCollection>( iConfig.getParameter<edm::InputTag>("wireDigiTag") );
    sd_token = consumes<CSCStripDigiCollection>( iConfig.getParameter<edm::InputTag>("stripDigiTag") );
    ad_token = consumes<CSCALCTDigiCollection>( iConfig.getParameter<edm::InputTag>("alctDigiTag") );
    cd_token = consumes<CSCCLCTDigiCollection>( iConfig.getParameter<edm::InputTag>("clctDigiTag") );
    ld_token = consumes<CSCCorrelatedLCTDigiCollection>( iConfig.getParameter<edm::InputTag>("lctDigiTag") );
    cod_token = consumes<CSCComparatorDigiCollection>( iConfig.getParameter<edm::InputTag>("compDigiTag") );
    obs_token = consumes<reco::BeamSpot>( iConfig.getParameter<edm::InputTag>("offlineBeamSpotTag") );
    tflct_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("csctfDigiTag"));

    minPt     = iConfig.getParameter<double>("minPt");

    muonQualityCuts = new MuonQualityCuts(iConfig);

    evN = 0;

    edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
    theService = new MuonServiceProxy(serviceParameters);

    edm::ParameterSet matchParameters = iConfig.getParameter<edm::ParameterSet>("MatchParameters");
    edm::ConsumesCollector iC  = consumesCollector();
    theMatcher = new MuonSegmentMatcher(matchParameters, iC);


    CSCSegmentTags_=matchParameters.getParameter<edm::InputTag>("CSCsegments");
    //CSCSegmentTags_ = iConfig.getParameter<edm::InputTag>("CSCsegments");
    allSegmentsCSCToken = iC.consumes<CSCSegmentCollection>(CSCSegmentTags_) ;

    filename = iConfig.getUntrackedParameter<std::string>("NtupleFileName");
    cout << endl << endl << "Filename " << filename << endl << endl;
    file = new TFile(filename.c_str(), "RECREATE");
    tree = new TTree("CSCDigiTree","Tree holding CSCDigis");
    hist = new TH1F("hist","hist",20,-0.5,19.5);
    chambernumber = new TH1F("chambernumber","chambernumber",600,0.5,600.5);
    ptmuon = new TH1F("ptmuon","ptmuon",200,0,200);
    ptmu1 = new TH1F("ptmu1","ptmu1",200,0,100);
    ptmu2 = new TH1F("ptmu2","ptmu2",200,0,100);
    etamuon = new TH1F("etamuon","etamuon",80,-4,4);
    ptsamuon = new TH1F("ptsamuon","ptsamuon",50,0,500);
    dimuonMos = new TH1F("dimuonMos","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuonMss = new TH1F("dimuonMss","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuon3M = new TH1F("dimuon3M","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,2.0,4.0);
    dimuonMos_1GS = new TH1F("dimuonMos_1GS","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuon3M_1GS = new TH1F("dimuon3M_1GS","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,2.0,4.0);
    dimuonMos_1Gl = new TH1F("dimuonMos_1Gl","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuon3M_1Gl = new TH1F("dimuon3M_1Gl","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,2.0,4.0);
    dimuonMos_1SA = new TH1F("dimuonMos_1SA","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuon3M_1SA = new TH1F("dimuon3M_1SA","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,2.0,4.0);
    dimuonMos_2Gl = new TH1F("dimuonMos_2Gl","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuon3M_2Gl = new TH1F("dimuon3M_2Gl","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,2.0,4.0);
    dimuonMos_2SA = new TH1F("dimuonMos_2SA","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,0,10);
    dimuon3M_2SA = new TH1F("dimuon3M_2SA","Mass of Dimuons;m_{#mu#mu} [GeV];Events",100,2.0,4.0);
    Nmuon_h = new TH1F("Nmuon_h","Number of Muons;Number of Muons;Events",11,-0.5,10.5);


    tree->Branch("Event_EventNumber",&Event_EventNumber,"Event_EventNumber/I");
    tree->Branch("Event_RunNumber",&Event_RunNumber,"Event_RunNumber/I");
    tree->Branch("Event_LumiSection",&Event_LumiSection,"Event_LumiSection/I");
    tree->Branch("Event_BXCrossing",&Event_BXCrossing,"Event_BXCrossing/I");

    tree->Branch("ss",&ss);
    tree->Branch("os",&os);
    tree->Branch("Pt",&Pt,"Pt/D");
    tree->Branch("eta",&eta,"eta/D");
    tree->Branch("phi",&phi,"phi/D");
    tree->Branch("q",&q,"q/I");

    tree->Branch("Nseg",&Nseg,"Nseg/I");
    tree->Branch("segEc",&segEc);
    tree->Branch("segSt",&segSt);
    tree->Branch("segRi",&segRi);
    tree->Branch("segCh",&segCh);
    tree->Branch("segX",&segX);
    tree->Branch("segY",&segY);
    tree->Branch("segdXdZ",&segdXdZ);
    tree->Branch("segdYdZ",&segdYdZ);

    tree->Branch("rhId",&rhId);
    tree->Branch("rhLay",&rhLay);
    tree->Branch("rhPos",&rhPos);
    tree->Branch("rhE",&rhE);
    tree->Branch("rhMax",&rhMax);

    tree->Branch("lctId",&lctId);
    tree->Branch("lctQ",&lctQ);
    tree->Branch("lctPat",&lctPat);
    tree->Branch("lctKWG",&lctKWG);
    tree->Branch("lctKHS",&lctKHS);
    tree->Branch("lctBend",&lctBend);

    tree->Branch("tflctId",&tflctId);
    tree->Branch("tflctQ",&tflctQ);
    tree->Branch("tflctPat",&tflctPat);
    tree->Branch("tflctKWG",&tflctKWG);
    tree->Branch("tflctKHS",&tflctKHS);
    tree->Branch("tflctBend",&tflctBend);

    tree->Branch("clctId",&clctId);
    tree->Branch("clctQ",&clctQ);
    tree->Branch("clctPat",&clctPat);
    tree->Branch("clctKHS",&clctKHS);
    tree->Branch("clctCFEB",&clctCFEB);
    tree->Branch("clctBend",&clctBend);

    tree->Branch("alctId",&alctId);
    tree->Branch("alctQ",&alctQ);
    tree->Branch("alctKWG",&alctKWG);
    tree->Branch("alctAc",&alctAc);
    tree->Branch("alctPB",&alctPB);

    tree->Branch("compId",&compId);
    tree->Branch("compLay",&compLay);
    tree->Branch("compStr",&compStr);
    tree->Branch("compHS",&compHS);
    tree->Branch("compTimeOn",&compTimeOn);

    tree->Branch("wireId",&wireId);
    tree->Branch("wireLay",&wireLay);
    tree->Branch("wireGrp",&wireGrp);
    tree->Branch("wireTimeOn",&wireTimeOn);

    tree->Branch("stripId",&stripId);
    tree->Branch("stripLay",&stripLay);
    tree->Branch("strip",&strip);
    tree->Branch("stripADCs",&stripADCs);

}


CSCPatterns::~CSCPatterns()
{
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    file->cd();
    tree->Write();
    hist->Write();
    ptmuon->Write();
    ptmu1->Write();
    ptmu2->Write();
    dimuonMos->Write();
    dimuonMss->Write();
    dimuon3M->Write();
    dimuonMos_1GS->Write();
    dimuon3M_1GS->Write();
    dimuonMos_1Gl->Write();
    dimuon3M_1Gl->Write();
    dimuonMos_1SA->Write();
    dimuon3M_1SA->Write();
    dimuonMos_2Gl->Write();
    dimuon3M_2Gl->Write();
    dimuonMos_2SA->Write();
    dimuon3M_2SA->Write();
    etamuon->Write();
    ptsamuon->Write();
    Nmuon_h->Write();
    chambernumber->Write();
    file->Close();


}


//
// member functions
//

// ------------ method called to for each event  ------------
    void
CSCPatterns::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    using namespace std;

    Event_EventNumber     = iEvent.id().event();
    Event_RunNumber       = iEvent.id().run();
    Event_LumiSection     = iEvent.eventAuxiliary().luminosityBlock();
    Event_BXCrossing      = iEvent.eventAuxiliary().bunchCrossing();

    evN++;

    // Get the CSC Geometry :
    ESHandle<CSCGeometry> cscGeom;
    iSetup.get<MuonGeometryRecord>().get(cscGeom);
    theCSC = cscGeom.product();

    edm::Handle<CSCWireDigiCollection> cscWireDigi;
    iEvent.getByToken(wd_token,cscWireDigi);

    edm::Handle<CSCStripDigiCollection> cscStripDigi;
    iEvent.getByToken(sd_token,cscStripDigi);

    Handle<reco::MuonCollection> muons;
    iEvent.getByToken(mc_token, muons);

    edm::Handle<CSCSegmentCollection> allSegmentsCSC;
    iEvent.getByToken(allSegmentsCSCToken, allSegmentsCSC);
    hist->Fill(allSegmentsCSC->size());
    if(allSegmentsCSC->size()>14) return;

    edm::Handle<CSCALCTDigiCollection> cscALCTDigi;
    iEvent.getByToken(ad_token, cscALCTDigi);

    edm::Handle<CSCCLCTDigiCollection> cscCLCTDigi;
    iEvent.getByToken(cd_token, cscCLCTDigi);

    edm::Handle<CSCCorrelatedLCTDigiCollection> cscLCTDigi;
    iEvent.getByToken(ld_token, cscLCTDigi);

    edm::Handle<CSCCorrelatedLCTDigiCollection> tfLCTs;
    iEvent.getByToken(tflct_token, tfLCTs);

    edm::Handle<CSCComparatorDigiCollection> compDigi;
    iEvent.getByToken(cod_token, compDigi);

    edm::Handle<reco::BeamSpot> beamSpotHandle;
    iEvent.getByToken(obs_token, beamSpotHandle);

    int Nmuon = 0;
    int N_SAmuon = 0;
    vector<TLorentzVector> muP4;
    vector<int> muQ;
    vector<bool> muGlobal;
    vector<bool> muSA;
    vector<bool> muTracker;
    TLorentzVector sp4(0.0,0.0,0.0,0.0);

    for (reco::MuonCollection::const_iterator muon = muons->begin(); muon!= muons->end(); muon++) 
    {
        Nmuon++;
        ptmuon->Fill(muon->pt());
        etamuon->Fill(muon->eta());
        TLorentzVector muBuf;
        muBuf.SetPtEtaPhiE(muon->pt(),muon->eta(),muon->phi(),muon->energy());
        muP4.push_back(muBuf);
        muQ.push_back(muon->charge());
        muGlobal.push_back(muon->isGlobalMuon());
        if(!muon->standAloneMuon()) muSA.push_back(false);
        else muSA.push_back(true);
        muTracker.push_back(muon->isTrackerMuon());
        if (!muon->standAloneMuon()) continue;
        if(N_SAmuon < 2) 
        {
            TLorentzVector pp;
            pp.SetPtEtaPhiE(muon->pt(),muon->eta(),muon->phi(),muon->energy());
            sp4 += pp;
        }
        N_SAmuon++;
        ptsamuon->Fill(muon->standAloneMuon()->pt());
    }

    Nmuon_h->Fill(Nmuon);
    int mu1Ios = -9;
    int mu2Ios = -9;
    int mu1Iss = -9;
    int mu2Iss = -9;
    for(int i = 0; i < int(muQ.size());i++)
    {
        for(int j = i + 1; j < int(muQ.size()); j++)
        {
            if(mu1Ios == -9 && (muGlobal.at(i) && muGlobal.at(j)) && muQ.at(i) != muQ.at(j))
            {
                mu1Ios = i;
                mu2Ios = j;
            }
            if(mu1Iss == -9 && (muGlobal.at(i) && muGlobal.at(j)) && muQ.at(i) == muQ.at(j))
            {
                mu1Iss = i;
                mu2Iss = j;
            }
        }
    }
    if(mu1Ios == -9 && mu1Iss == -9) return;
    /*if(muGlobal.at(mu1Ios) || muGlobal.at(mu2Ios))
    {
        dimuonMos_1Gl->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
        dimuon3M_1Gl->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
    }
    if(muGlobal.at(mu1Ios) && muGlobal.at(mu2Ios))
    {
        dimuonMos_2Gl->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
        dimuon3M_2Gl->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
    }
    if(muSA.at(mu1Ios) || muSA.at(mu2Ios))
    {
        dimuonMos_1SA->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
        dimuon3M_1SA->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
    }
    if(muSA.at(mu1Ios) && muSA.at(mu2Ios))
    {
        dimuonMos_2SA->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
        dimuon3M_2SA->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
    }
    if( (muSA.at(mu1Ios) || muSA.at(mu2Ios)) && (muGlobal.at(mu1Ios) || muGlobal.at(mu2Ios)) )
    {
        dimuonMos_1GS->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
        dimuon3M_1GS->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
    }*/


    float massJpsi = 3.0969;
    float massWin = 0.1;
    bool inMos = false;
    bool inMss = false;
    if(mu1Ios >= 0)
    {
        if( ( (muP4.at(mu1Ios) + muP4.at(mu2Ios)).M() < massJpsi - massWin ) || ( (muP4.at(mu1Ios) + muP4.at(mu2Ios)).M() > massJpsi + massWin ) ) inMos = true;
        ptmu1->Fill(muP4.at(mu1Ios).Pt());
        ptmu2->Fill(muP4.at(mu2Ios).Pt());
        dimuonMos->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
        dimuon3M->Fill((muP4.at(mu1Ios) + muP4.at(mu2Ios)).M());
    }
    if(mu1Iss >= 0)
    {
        if( ( (muP4.at(mu1Iss) + muP4.at(mu2Iss)).M() < massJpsi - massWin ) || ( (muP4.at(mu1Iss) + muP4.at(mu2Iss)).M() > massJpsi + massWin ) ) inMss = true;
        ptmu1->Fill(muP4.at(mu1Iss).Pt());
        ptmu2->Fill(muP4.at(mu2Iss).Pt());
        dimuonMss->Fill((muP4.at(mu1Iss) + muP4.at(mu2Iss)).M());
    }
    if(!(inMos || inMss)) return;
    cout << "Event " << evN << " Processed!" << endl;
    cout << "inMos: " << inMos << " inMss: " << inMss << " mu1Ios: " << mu1Ios << " mu2Ios: " << mu2Ios << " mu1Iss: " << mu1Iss << " mu2Iss: " << mu2Iss << endl;

    int ind = -1;
    //Loop over muons applying the selection used for the timing analysis
    for (reco::MuonCollection::const_iterator muon = muons->begin(); muon!= muons->end(); muon++) 
    {
        ind++;
        os = false;
        ss = false;
        if( !((inMos && (ind==mu1Ios || ind==mu2Ios)) || (inMss && (ind==mu1Iss || ind==mu2Iss))) ) continue;
        if(ind==mu1Ios || ind==mu2Ios) os = true;
        if(ind==mu1Iss || ind==mu2Iss) ss = true;

        if (!muon->standAloneMuon()) continue;
        if (muon->pt()<minPt) continue;
        //if (!muonQualityCuts->isGoodMuon(iEvent, muon, beamSpotHandle)) continue;

        segEc.clear();
        segSt.clear();
        segRi.clear();
        segCh.clear();
        segX.clear();
        segY.clear();
        segdXdZ.clear();
        segdYdZ.clear();

        rhId.clear();
        rhLay.clear();
        rhPos.clear();
        rhE.clear();
        rhMax.clear();

        lctId.clear();
        lctQ.clear();
        lctPat.clear();
        lctKWG.clear();
        lctKHS.clear();
        lctBend.clear();

        tflctId.clear();
        tflctQ.clear();
        tflctPat.clear();
        tflctKWG.clear();
        tflctKHS.clear();
        tflctBend.clear();

        clctId.clear();
        clctQ.clear();
        clctPat.clear();
        clctKHS.clear();
        clctCFEB.clear();
        clctBend.clear();

        alctId.clear();
        alctQ.clear();
        alctKWG.clear();
        alctAc.clear();
        alctPB.clear();

        compId.clear();
        compLay.clear();
        compStr.clear();
        compHS.clear();
        compTimeOn.clear();

        wireId.clear();
        wireLay.clear();
        wireGrp.clear();
        wireTimeOn.clear();

        stripId.clear();
        stripLay.clear();
        strip.clear();
        stripADCs.clear();

        Pt=muon->pt();
        eta=muon->eta();
        phi=muon->phi();
        q=muon->charge();


        //Match selected muons to CSCSegments
        vector<const CSCSegment*> range = theMatcher->matchCSC(*muon->standAloneMuon(),iEvent);

        Nseg = range.size();

        vector<int> ch_serialID;

        for (vector<const CSCSegment*>::iterator iseg = range.begin(); iseg!=range.end();++iseg) 
        {
            // Create the ChamberId
            DetId id = (*iseg)->geographicalId();

            CSCDetId chamberId(id.rawId());
            const CSCChamber *segChamber = theCSC->chamber(chamberId);
            const CSCLayer *segLay3 = segChamber->layer(3);
            const CSCLayer *segLay4 = segChamber->layer(4);
            const CSCLayerGeometry *segLay3Geo = segLay3->geometry();
            int chamber = chamberSerial(chamberId);

            LocalPoint lzero(0.0,0.0,0.0);
            GlobalPoint lay4zero = segLay4->toGlobal(lzero);
            LocalPoint lay4zeroIn3 = segLay3->toLocal(lay4zero);
            float cm2lay = fabs(lay4zeroIn3.z());

            segEc.push_back(chamberId.endcap());
            segSt.push_back(chamberId.station());
            segRi.push_back(chamberId.ring());
            segCh.push_back(chamberId.chamber());

            LocalPoint segLPlayer = segLay3->toLocal(segChamber->toGlobal((*iseg)->localPosition()));
            LocalVector segLVlayer = segLay3->toLocal(segChamber->toGlobal((*iseg)->localDirection()));
            float scale = -1.0*segLPlayer.z()/segLVlayer.z();
            LocalVector tV = scale*segLVlayer;
            LocalPoint tP = segLPlayer + tV;
            float segStr = segLay3Geo->strip(tP);
            int strI = floor(segStr);
            float cm2strip = fabs( segLay3Geo->xOfStrip(strI,tP.y()) - segLay3Geo->xOfStrip(strI+1,tP.y()) );
            cout << "LP z: " << tP.z() << endl;

            segX.push_back(segLay3Geo->strip(tP));
            segY.push_back((*iseg)->localPosition().y());
            segdXdZ.push_back( ( (*iseg)->localDirection().x() / cm2strip ) / ( (*iseg)->localDirection().z() / cm2lay ) );
            segdYdZ.push_back( (*iseg)->localDirection().y() / (*iseg)->localDirection().z() );

            const vector<CSCRecHit2D> hits2d = (*iseg)->specificRecHits();
            for (vector<CSCRecHit2D>::const_iterator hiti=hits2d.begin(); hiti!=hits2d.end(); hiti++)
            {
                DetId idd = (hiti)->geographicalId();
                CSCDetId hitID(idd.rawId());
                //int nStr = hiti->nStrips();
                //int nWireG = hiti->nWireGroups();
                int idBuf = chamberSerial(hitID);
                if(idBuf != chamber) continue;
                rhId.push_back(idBuf);
                rhLay.push_back(hitID.layer());

                float rhMaxBuf = -999.0;
                int centerID = hiti->nStrips()/2;

                for(int tI = 0; tI < int(hiti->nTimeBins()); tI++)
                {
                    if(hiti->adcs(centerID,tI) > rhMaxBuf) rhMaxBuf = hiti->adcs(centerID,tI);
                }
                int centerStr = hiti->channels(centerID);
                float rhPosBuf = float(centerStr) + hiti->positionWithinStrip();
                float rhEBuf = hiti->energyDepositedInLayer();
                rhPos.push_back(rhPosBuf);
                rhE.push_back(rhEBuf);
                rhMax.push_back(rhMaxBuf);
            }

            // Check to see if chamber has already been extracted
            bool chDone = false;
            for(vector<int>::iterator chid = ch_serialID.begin(); chid!=ch_serialID.end(); ++chid)
            {
                if((*chid) == chamber) chDone = true;
            }

            if(chDone) continue;
            ch_serialID.push_back(chamber);
            chambernumber->Fill(chamber);

            // Extract LCT for all strips in this chamber
            for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator lctDigi_id=cscLCTDigi->begin(); lctDigi_id!=cscLCTDigi->end(); lctDigi_id++)
            {
                CSCDetId lctID = (*lctDigi_id).first;
                int idBuf = chamberSerial(lctID);
                if(idBuf != chamber) continue;
                lctId.push_back(idBuf);

                vector<int> lctQBuf;
                vector<int> lctPatBuf;
                vector<int> lctKWGBuf;
                vector<int> lctKHSBuf;
                vector<int> lctBendBuf;

                const CSCCorrelatedLCTDigiCollection::Range& range =(*lctDigi_id).second;
                for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
                {
                    lctQBuf.push_back((*digiItr).getQuality());
                    lctPatBuf.push_back((*digiItr).getPattern());
                    lctKWGBuf.push_back((*digiItr).getKeyWG());
                    lctKHSBuf.push_back((*digiItr).getStrip());
                    lctBendBuf.push_back((*digiItr).getBend());
                }
                lctQ.push_back(lctQBuf);
                lctPat.push_back(lctPatBuf);
                lctKWG.push_back(lctKWGBuf);
                lctKHS.push_back(lctKHSBuf);
                lctBend.push_back(lctBendBuf);
            }

            //Extact tfLCTs in this event
            for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator lctDigi_id=tfLCTs->begin(); lctDigi_id!=tfLCTs->end(); lctDigi_id++)
            {
                CSCDetId tflctID = (*lctDigi_id).first;
                int idBuf = chamberSerial(tflctID);
                if(idBuf != chamber) continue;
                tflctId.push_back(idBuf);

                vector<int> tflctQBuf;
                vector<int> tflctPatBuf;
                vector<int> tflctKWGBuf;
                vector<int> tflctKHSBuf;
                vector<int> tflctBendBuf;

                const CSCCorrelatedLCTDigiCollection::Range& range =(*lctDigi_id).second;
                for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
                {
                    tflctQBuf.push_back((*digiItr).getQuality());
                    tflctPatBuf.push_back((*digiItr).getPattern());
                    tflctKWGBuf.push_back((*digiItr).getKeyWG());
                    tflctKHSBuf.push_back((*digiItr).getStrip());
                    tflctBendBuf.push_back((*digiItr).getBend());
                }
                tflctQ.push_back(tflctQBuf);
                tflctPat.push_back(tflctPatBuf);
                tflctKWG.push_back(tflctKWGBuf);
                tflctKHS.push_back(tflctKHSBuf);
                tflctBend.push_back(tflctBendBuf);
            }

            // Extract CLCT for all strips in this chamber
            for (CSCCLCTDigiCollection::DigiRangeIterator clctDigi_id=cscCLCTDigi->begin(); clctDigi_id!=cscCLCTDigi->end(); clctDigi_id++)
            {
                CSCDetId clctID = (*clctDigi_id).first;
                int idBuf = chamberSerial(clctID);
                if(idBuf != chamber) continue;
                clctId.push_back(idBuf);

                vector<int> clctQBuf;
                vector<int> clctPatBuf;
                vector<int> clctKHSBuf;
                vector<int> clctCFEBBuf;
                vector<int> clctBendBuf;

                const CSCCLCTDigiCollection::Range& range =(*clctDigi_id).second;
                for(CSCCLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
                {
                    clctQBuf.push_back((*digiItr).getQuality());
                    clctPatBuf.push_back((*digiItr).getPattern());
                    clctKHSBuf.push_back((*digiItr).getStrip());
                    clctCFEBBuf.push_back((*digiItr).getCFEB());
                    clctBendBuf.push_back((*digiItr).getBend());
                }
                clctQ.push_back(clctQBuf);
                clctPat.push_back(clctPatBuf);
                clctKHS.push_back(clctKHSBuf);
                clctCFEB.push_back(clctCFEBBuf);
                clctBend.push_back(clctBendBuf);
            }

            // Extract ALCT for all strips in this chamber
            for (CSCALCTDigiCollection::DigiRangeIterator alctDigi_id=cscALCTDigi->begin(); alctDigi_id!=cscALCTDigi->end(); alctDigi_id++)
            {
                CSCDetId alctID = (*alctDigi_id).first;
                int idBuf = chamberSerial(alctID);
                if(idBuf != chamber) continue;
                alctId.push_back(idBuf);

                vector<int> alctQBuf;
                vector<int> alctKWGBuf;
                vector<int> alctAcBuf;
                vector<int> alctPBBuf;

                const CSCALCTDigiCollection::Range& range =(*alctDigi_id).second;
                for(CSCALCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
                {
                    alctQBuf.push_back((*digiItr).getQuality());
                    alctKWGBuf.push_back((*digiItr).getKeyWG());
                    alctAcBuf.push_back((*digiItr).getAccelerator());
                    alctPBBuf.push_back((*digiItr).getCollisionB());
                }
                alctQ.push_back(alctQBuf);
                alctKWG.push_back(alctKWGBuf);
                alctAc.push_back(alctAcBuf);
                alctPB.push_back(alctPBBuf);
            }

            // Extract Comparator Data
            for (CSCComparatorDigiCollection::DigiRangeIterator compDigi_id=compDigi->begin(); compDigi_id!=compDigi->end(); compDigi_id++)
            {
                CSCDetId compID = (*compDigi_id).first;
                int idBuf = chamberSerial(compID);
                if(idBuf != chamber) continue;
                compId.push_back(idBuf);
                compLay.push_back(compID.layer());

                vector<int> compStrBuf;
                vector<int> compHSBuf;
                vector<vector<int> > compTimeOnBuf;

                const CSCComparatorDigiCollection::Range& range =(*compDigi_id).second;
                for(CSCComparatorDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
                {
                    compStrBuf.push_back((*digiItr).getStrip());
                    compHSBuf.push_back((*digiItr).getComparator());
                    compTimeOnBuf.push_back((*digiItr).getTimeBinsOn());
                }
                compStr.push_back(compStrBuf);
                compHS.push_back(compHSBuf);
                compTimeOn.push_back(compTimeOnBuf);
            }

            // Extract data from WireDigis
            for (CSCWireDigiCollection::DigiRangeIterator wireDigi_id=cscWireDigi->begin(); wireDigi_id!=cscWireDigi->end(); wireDigi_id++)
            {
                CSCDetId wireID = (*wireDigi_id).first;
                int idBuf = chamberSerial(wireID);
                if(idBuf != chamber) continue;
                wireId.push_back(idBuf);
                wireLay.push_back(wireID.layer());

                vector<int> wireGrpBuf;
                vector<vector<int> > wireTimeOnBuf;

                const CSCWireDigiCollection::Range& range =(*wireDigi_id).second;
                for(CSCWireDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
                {
                    wireGrpBuf.push_back((*digiItr).getWireGroup());
                    wireTimeOnBuf.push_back((*digiItr).getTimeBinsOn());
                }
                wireGrp.push_back(wireGrpBuf);
                wireTimeOn.push_back(wireTimeOnBuf);
            }

            // Extract ADCCounts for all strips in this chamber
            for (CSCStripDigiCollection::DigiRangeIterator stripDigi_id=cscStripDigi->begin(); stripDigi_id!=cscStripDigi->end(); stripDigi_id++)
            {
                CSCDetId stripID = (*stripDigi_id).first;
                int idBuf = chamberSerial(stripID);
                if(idBuf != chamber) continue;
                stripId.push_back(idBuf);
                stripLay.push_back(stripID.layer());

                vector<int> stripBuf;
                vector<vector<int>> stripADCBuf;

                vector<CSCStripDigi>::const_iterator digiItr = (*stripDigi_id).second.first;
                vector<CSCStripDigi>::const_iterator last = (*stripDigi_id).second.second;
                for( ; digiItr != last; ++digiItr)
                {
                    stripBuf.push_back(digiItr->getStrip());
                    stripADCBuf.push_back(digiItr->getADCCounts());
                }
                strip.push_back(stripBuf);
                stripADCs.push_back(stripADCBuf);

            }

        }// Matched CSCSegment loop

        cout << "Number of pats: " << clctPat.size() << " number of segments: " << range.size() <<  endl;
        if(range.size() > 0) tree->Fill();
    }// Muon loop
}


int CSCPatterns::chamberSerial( CSCDetId id ) {
    int st = id.station();
    int ri = id.ring();
    int ch = id.chamber();
    int ec = id.endcap();
    //int lay = id.layer();
    int kSerial = ch;
    //cout<<"Endcap "<<ec<<" Station "<<st<<" Ring "<<ri<<" Chamber "<<ch<<" Layer "<<lay<<endl;
    if (st == 1 && ri == 1) kSerial = ch;
    if (st == 1 && ri == 2) kSerial = ch + 36;
    if (st == 1 && ri == 3) kSerial = ch + 72;
    if (st == 1 && ri == 4) kSerial = ch;
    if (st == 2 && ri == 1) kSerial = ch + 108;
    if (st == 2 && ri == 2) kSerial = ch + 126;
    if (st == 3 && ri == 1) kSerial = ch + 162;
    if (st == 3 && ri == 2) kSerial = ch + 180;
    if (st == 4 && ri == 1) kSerial = ch + 216;
    if (st == 4 && ri == 2) kSerial = ch + 234;  // one day...
    if (ec == 2) kSerial = kSerial + 300;
    return kSerial;
}

double CSCPatterns::FindAnodeTime(vector<CSCRecHit2D>::const_iterator  hiti,  const edm::Handle<CSCWireDigiCollection> cscWireDigi, double local_t0) {
    // Create the ChamberId
    DetId id = (hiti)->geographicalId();
    CSCDetId chamberId(id.rawId());
    double Anode_time=0;

    int wire_digis=0;
    double time_guess=local_t0+205;   // 205 is anode cathode time offset
    //  double time_guess=local_t0;
    for (CSCWireDigiCollection::DigiRangeIterator j=cscWireDigi->begin(); j!=cscWireDigi->end(); j++)
    {
        CSCDetId wire_cscId = (*j).first;
        //  Check if in same chamber and layer.  Wire digis only use rings 1-3, RecHits 1-4 (for ME1/1A)
        if(chamberSerial(wire_cscId) == chamberSerial(chamberId) && wire_cscId.layer()==chamberId.layer()) {
            //	CSCRecHit2D::ChannelContainer wgroups = hiti->wgroups();
            int hit_wire = hiti->hitWire();

            vector<CSCWireDigi>::const_iterator digiItr = (*j).second.first;
            vector<CSCWireDigi>::const_iterator last = (*j).second.second;
            for( ; digiItr != last; ++digiItr) {
                if (digiItr->getWireGroup()==hit_wire) {
                    vector <int> TimeBins = digiItr->getTimeBinsOn();
                    wire_digis++;
                    double Digi_time=-99999.;
                    double diff_min=99999., best_match=-1;
                    for (uint j=0; j<TimeBins.size(); j++) {
                        double diff=(time_guess-TimeBins[j]*25);
                        if (abs(diff)<abs(diff_min)) {
                            diff_min=diff;
                            best_match=j;
                            Digi_time=TimeBins[j];
                        }
                    }
                    int side=diff_min/abs(diff_min);
                    bool unchanged=true;
                    if ((best_match+side)>-1 && (best_match+side)<TimeBins.size()) {      // Make sure one next to it exists
                        if (TimeBins[best_match]==(TimeBins[best_match+side]-side)) {
                            Digi_time=Digi_time+(double)side/2.;
                            unchanged=false;
                        }
                    }

                    if ((best_match-side)>-1 && (best_match-side)<TimeBins.size() && unchanged) {
                        if (TimeBins[best_match]==(TimeBins[best_match-side]+side)) {
                            Digi_time=Digi_time-(double)side/2.;
                            unchanged=false;
                        }
                    }
                    Anode_time+=Digi_time;
                }
            }
        }
    }
    if (wire_digis==0) Anode_time=-1;
    else Anode_time=(Anode_time/wire_digis)*25.;  //Convert from time bin to ns
    return Anode_time;
}



// ------------ method called once each job just before starting event loop  ------------
    void 
CSCPatterns::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
CSCPatterns::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCPatterns);
