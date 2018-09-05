// -*- C++ -*-
//
// Package:    CSCPatternExtractor
// Class:      CSCPatternExtractor
//
/*

Description: Pulls CSC digis associated with different types of events,
		such as J/Psi, Single Muon, Displaced Muon Monte Carlo, Min-Bias.
		Includes selection for each of the available samples.


 Original Author:  Christopher Farrell
         Created:  Thu Jul 22 06:19:36 CDT 2010

          Author: Cameron Bravo
  Date Inherited: ?

    	  Author: William Nash
  Date Inherited: August 24th 2018
*/



//user include files
#include "CSCUCLA/CSCDigiTuples/include/CSCPatternExtractor.h"
//#include "CSCUCLA/CSCDigiTuples/include/CSCExtractorClasses.h"



/*TODO:
 * - find / write something that converts from channelId -> ec, ri, st, ch
 *
 */


CSCPatternExtractor::CSCPatternExtractor(const ParameterSet &iConfig):
tree(iConfig.getUntrackedParameter<std::string>("NtupleFileName"),"CSCDigiTree","Tree holding CSCDigis"), //set up the tree
eventInfo(tree) //set branches you want in the tree
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
	csctflct_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("csctfDigiTag"));
	emtflct_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("emtfDigiTag"));
	ddu_token = consumes<CSCDDUStatusDigiCollection>(iConfig.getParameter<edm::InputTag>("dduDigiTag"));
	dmb_token = consumes<CSCDMBStatusDigiCollection>(iConfig.getParameter<edm::InputTag>("dmbDigiTag"));
	tmb_token = consumes<CSCTMBStatusDigiCollection>(iConfig.getParameter<edm::InputTag>("tmbDigiTag"));


	theCSC = 0;

	minPt     = iConfig.getParameter<double>("minPt");


	/* Choose if we are going to write data by selecting muons,
	 * and if so, which selector to use
	 */

	dataType = iConfig.getUntrackedParameter<std::string>("dataType");
	if (dataType == "minBias"){
		cout <<  "--- Running as minBias sample --- " << endl;
		selectMuons = 0;
	}else{
		if (dataType == "singleMu"){
			cout <<  "--- Running as singleMu sample --- " << endl;
			selectMuons = &selectSingleMuMuons;
		} else if (dataType == "jPsi") {
			cout <<  "--- Running as jPsi sample --- " << endl;
			selectMuons = &selectJPsiMuons;
		}else if(dataType == "displacedMuon") {
			cout <<  "--- Running as DisplacedMuon sample --- " << endl;
			selectMuons = &selectDisplacedMuons;
		} else { //default to single muon selection
			cout <<  "--- Defaulting as singleMu sample --- " << endl;
			selectMuons = &selectSingleMuMuons;
		}
	}

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


	//TEMP
	ss = true;
	os = true;
	Pt = 69;
	eta = 69;
	phi = 69;
	q = -69;


	/*
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
*/

	/*

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

	tree->Branch("Nseg",&segs.Nseg,"Nseg/I");
	tree->Branch("segEc",&segs.Ec);
	tree->Branch("segSt",&segs.St);
	tree->Branch("segRi",&segs.Ri);
	tree->Branch("segCh",&segs.Ch);
	tree->Branch("segX",&segs.X);
	tree->Branch("segY",&segs.Y);
	tree->Branch("segdXdZ",&segs.dXdZ);
	tree->Branch("segdYdZ",&segs.dYdZ);

	tree->Branch("rhId",&recHits.id);
	tree->Branch("rhLay",&recHits.Lay);
	tree->Branch("rhPos",&recHits.Pos);
	tree->Branch("rhE",&recHits.E);
	tree->Branch("rhMax",&recHits.Max);

	tree->Branch("lctId",&lcts.id);
	tree->Branch("lctQ",&lcts.Q);
	tree->Branch("lctPat",&lcts.Pat);
	tree->Branch("lctKWG",&lcts.KWG);
	tree->Branch("lctKHS",&lcts.KHS);
	tree->Branch("lctBend",&lcts.Bend);
	tree->Branch("lctBX",&lcts.BX);

	tree->Branch("csctflctId",&csctfLcts.id);
	tree->Branch("csctflctQ",&csctfLcts.Q);
	tree->Branch("csctflctPat",&csctfLcts.Pat);
	tree->Branch("csctflctKWG",&csctfLcts.KWG);
	tree->Branch("csctflctKHS",&csctfLcts.KHS);
	tree->Branch("csctflctBend",&csctfLcts.Bend);
	tree->Branch("csctflctBX",&csctfLcts.BX);

	tree->Branch("emtflctId",&emtfLcts.id);
	tree->Branch("emtflctQ",&emtfLcts.Q);
	tree->Branch("emtflctPat",&emtfLcts.Pat);
	tree->Branch("emtflctKWG",&emtfLcts.KWG);
	tree->Branch("emtflctKHS",&emtfLcts.KHS);
	tree->Branch("emtflctBend",&emtfLcts.Bend);
	tree->Branch("emtflctBX",&emtfLcts.BX);

	tree->Branch("clctId",&clcts.id);
	tree->Branch("clctQ",&clcts.Q);
	tree->Branch("clctPat",&clcts.Pat);
	tree->Branch("clctKHS",&clcts.KHS);
	tree->Branch("clctCFEB",&clcts.CFEB);
	tree->Branch("clctBend",&clcts.Bend);
	tree->Branch("clctBX",&clcts.BX);
	tree->Branch("clctFBX",&clcts.FBX);

	tree->Branch("alctId",&alcts.id);
	tree->Branch("alctQ",&alcts.Q);
	tree->Branch("alctKWG",&alcts.KWG);
	tree->Branch("alctAc",&alcts.Ac);
	tree->Branch("alctPB",&alcts.PB);
	tree->Branch("alctBX",&alcts.BX);
	tree->Branch("alctFBX",&alcts.FBX);

	tree->Branch("compId",&comps.id);
	tree->Branch("compLay",&comps.Lay);
	tree->Branch("compStr",&comps.Str);
	tree->Branch("compHS",&comps.HS);
	tree->Branch("compTimeOn",&comps.TimeOn);

	tree->Branch("wireId",&wires.id);
	tree->Branch("wireLay",&wires.Lay);
	tree->Branch("wireGrp",&wires.Grp);
	tree->Branch("wireTimeOn",&wires.TimeOn);

	tree->Branch("stripId",&strips.id);
	tree->Branch("stripLay",&strips.Lay);
	tree->Branch("strip",&strips.strip);
	tree->Branch("stripADCs",&strips.stripADCs);

	tree->Branch("dduId",&ddus.id);
	tree->Branch("dduHeader",&ddus.header);
	tree->Branch("dduTrailer",&ddus.trailer);

	tree->Branch("dmbId",&dmbs.id);
	tree->Branch("dmbHeader",&dmbs.header);
	tree->Branch("dmbTrailer",&dmbs.trailer);

	tree->Branch("tmbId",&tmbs.id);
	tree->Branch("tmbHeader",&tmbs.header);
	tree->Branch("tmbTrailer",&tmbs.trailer);
	*/

}

CSCPatternExtractor::~CSCPatternExtractor(){
	tree.write();
	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)


	/*
	file->cd();
	tree->Write();
	hist->Write();
	ptmuon->Write();
	ptmu1->Write();
	ptmu2->Write();

	// Could get rid of some of these
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
	*/


}


// ------------ method called once each job just before starting event loop  ------------
void CSCPatternExtractor::beginJob() {
}


// ------------ method called to for each event  ------------
void CSCPatternExtractor::analyze(const edm::Event&iEvent, const edm::EventSetup& iSetup){
	eventInfo.fill(iEvent);

	/*
    Event_EventNumber     = iEvent.id().event();
    Event_RunNumber       = iEvent.id().run();
    Event_LumiSection     = iEvent.eventAuxiliary().luminosityBlock();
    Event_BXCrossing      = iEvent.eventAuxiliary().bunchCrossing();

    evN++;
    int Nmuon = 0;
    int N_SAmuon = 0;
    vector<TLorentzVector> muP4;
    vector<int> muQ;
    vector<bool> muGlobal;
    vector<bool> muSA;
    vector<bool> muTracker;
    TLorentzVector sp4(0.0,0.0,0.0,0.0);

    Handle<reco::MuonCollection> muons;
    iEvent.getByToken(mc_token, muons);


    if(selectMuons){
    	vector<reco::MuonCollection> selectedMuons = *selectMuons(muons);
    } else {

    }
    */

    //now read out all the stuff that is not muon specific

    /* for each event:
     *
     * vector<muons> muons;
     *
     * if Min Bias :
     * 		extractSegmentSpecificDigis();
     * else:
     * 		vector<muons> selectedMuons;
     *
     * 		if Z:
     * 			selectZmuons(muons, selectedMuons);
     * 		else if J/Psi:
     * 			selectJPsiMuons(muons, selectedMuons);
     * 		else if DisplacedMC:
     * 			selectDisplaced(muons, selectedMuons);
     * 		else:
     * 			error
     *
     * 		for muon in selectedMuons:
     * 			for segment in muon:
     * 				extractSegmentSpecificDigis(muon);
     *
     * 	extractNonSegmentSpecificDigis();
     *
     */


/*
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

    edm::Handle<CSCCorrelatedLCTDigiCollection> csctfLCTs;
    iEvent.getByToken(csctflct_token, csctfLCTs);

    edm::Handle<CSCCorrelatedLCTDigiCollection> emtfLCTs;
    iEvent.getByToken(emtflct_token, emtfLCTs);

    edm::Handle<CSCComparatorDigiCollection> compDigi;
    iEvent.getByToken(cod_token, compDigi);

    edm::Handle<CSCDDUStatusDigiCollection> dduDigi;
    iEvent.getByToken(ddu_token, dduDigi);

    edm::Handle<CSCDMBStatusDigiCollection> dmbDigi;
    iEvent.getByToken(dmb_token, dmbDigi);

    edm::Handle<CSCTMBStatusDigiCollection> tmbDigi;
    iEvent.getByToken(tmb_token, tmbDigi);

    edm::Handle<reco::BeamSpot> beamSpotHandle;
    iEvent.getByToken(obs_token, beamSpotHandle);
*/


}


// ------------ method called once each job just after ending the event loop  ------------
void CSCPatternExtractor::endJob() {
	cout << "Finished running CSCPatternExtractor.cc" << endl;
}

/*
// --- writes the segment specific digis associated with chamber id "chamberId" from event "iEvent"
int CSCPatternExtractor::extractSegmentSpecificDigis(const edm::Event& iEvent,int chamberId) {

    edm::Handle<CSCCorrelatedLCTDigiCollection> cscLCTDigi;
    iEvent.getByToken(ld_token, cscLCTDigi);

    edm::Handle<CSCCorrelatedLCTDigiCollection> emtfLCTs;
    iEvent.getByToken(emtflct_token, emtfLCTs);

    edm::Handle<CSCCorrelatedLCTDigiCollection> csctfLCTs;
    iEvent.getByToken(csctflct_token, csctfLCTs);

    edm::Handle<CSCCLCTDigiCollection> cscCLCTDigi;
    iEvent.getByToken(cd_token, cscCLCTDigi);

    edm::Handle<CSCALCTDigiCollection> cscALCTDigi;
    iEvent.getByToken(ad_token, cscALCTDigi);

    edm::Handle<CSCComparatorDigiCollection> compDigi;
    iEvent.getByToken(cod_token, compDigi);

    edm::Handle<CSCWireDigiCollection> cscWireDigi;
    iEvent.getByToken(wd_token,cscWireDigi);

    edm::Handle<CSCStripDigiCollection> cscStripDigi;
    iEvent.getByToken(sd_token,cscStripDigi);

    //if we want to only look at a specific id of a given segment
    bool matchToSegmentId = (chamberId != -1);


    // Extract LCT for all strips in this chamber
    for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator lctDigi_id=cscLCTDigi->begin(); lctDigi_id!=cscLCTDigi->end(); lctDigi_id++)
    {
        CSCDetId lctID = (*lctDigi_id).first;
        int idBuf = chamberSerial(lctID);
        if(matchToSegmentId && idBuf != chamberId) continue;
        lctId.push_back(idBuf);

        vector<int> lctQBuf;
        vector<int> lctPatBuf;
        vector<int> lctKWGBuf;
        vector<int> lctKHSBuf;
        vector<int> lctBendBuf;
        vector<int> lctBXBuf;

        const CSCCorrelatedLCTDigiCollection::Range& range =(*lctDigi_id).second;
        for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
        {
            lctQBuf.push_back((*digiItr).getQuality());
            lctPatBuf.push_back((*digiItr).getPattern());
            lctKWGBuf.push_back((*digiItr).getKeyWG());
            lctKHSBuf.push_back((*digiItr).getStrip());
            lctBendBuf.push_back((*digiItr).getBend());
            lctBXBuf.push_back((*digiItr).getBX());
        }
        lctQ.push_back(lctQBuf);
        lctPat.push_back(lctPatBuf);
        lctKWG.push_back(lctKWGBuf);
        lctKHS.push_back(lctKHSBuf);
        lctBend.push_back(lctBendBuf);
        lctBX.push_back(lctBXBuf);
    }

    //Extact emtfLCTs in this event
    for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator lctDigi_id=emtfLCTs->begin(); lctDigi_id!=emtfLCTs->end(); lctDigi_id++)
    {
        CSCDetId emtflctID = (*lctDigi_id).first;
        int idBuf = chamberSerial(emtflctID);
        if(matchToSegmentId && idBuf != chamberId) continue;
        emtflctId.push_back(idBuf);

        vector<int> emtflctQBuf;
        vector<int> emtflctPatBuf;
        vector<int> emtflctKWGBuf;
        vector<int> emtflctKHSBuf;
        vector<int> emtflctBendBuf;
        vector<int> emtflctBXBuf;

        const CSCCorrelatedLCTDigiCollection::Range& range =(*lctDigi_id).second;
        for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
        {
            emtflctQBuf.push_back((*digiItr).getQuality());
            emtflctPatBuf.push_back((*digiItr).getPattern());
            emtflctKWGBuf.push_back((*digiItr).getKeyWG());
            emtflctKHSBuf.push_back((*digiItr).getStrip());
            emtflctBendBuf.push_back((*digiItr).getBend());
            emtflctBXBuf.push_back((*digiItr).getBX());
        }
        emtflctQ.push_back(emtflctQBuf);
        emtflctPat.push_back(emtflctPatBuf);
        emtflctKWG.push_back(emtflctKWGBuf);
        emtflctKHS.push_back(emtflctKHSBuf);
        emtflctBend.push_back(emtflctBendBuf);
        emtflctBX.push_back(emtflctBXBuf);
    }

    //Extact csctfLCTs in this event
    for (CSCCorrelatedLCTDigiCollection::DigiRangeIterator lctDigi_id=csctfLCTs->begin(); lctDigi_id!=csctfLCTs->end(); lctDigi_id++)
    {
        CSCDetId csctflctID = (*lctDigi_id).first;
        int idBuf = chamberSerial(csctflctID);
        if(matchToSegmentId && idBuf != chamberId) continue;
        csctflctId.push_back(idBuf);

        vector<int> csctflctQBuf;
        vector<int> csctflctPatBuf;
        vector<int> csctflctKWGBuf;
        vector<int> csctflctKHSBuf;
        vector<int> csctflctBendBuf;
        vector<int> csctflctBXBuf;

        const CSCCorrelatedLCTDigiCollection::Range& range =(*lctDigi_id).second;
        for(CSCCorrelatedLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
        {
            csctflctQBuf.push_back((*digiItr).getQuality());
            csctflctPatBuf.push_back((*digiItr).getPattern());
            csctflctKWGBuf.push_back((*digiItr).getKeyWG());
            csctflctKHSBuf.push_back((*digiItr).getStrip());
            csctflctBendBuf.push_back((*digiItr).getBend());
            csctflctBXBuf.push_back((*digiItr).getBX());
        }
        csctflctQ.push_back(csctflctQBuf);
        csctflctPat.push_back(csctflctPatBuf);
        csctflctKWG.push_back(csctflctKWGBuf);
        csctflctKHS.push_back(csctflctKHSBuf);
        csctflctBend.push_back(csctflctBendBuf);
        csctflctBX.push_back(csctflctBXBuf);
    }

    // Extract CLCT for all strips in this chamber
    for (CSCCLCTDigiCollection::DigiRangeIterator clctDigi_id=cscCLCTDigi->begin(); clctDigi_id!=cscCLCTDigi->end(); clctDigi_id++)
    {
        CSCDetId clctID = (*clctDigi_id).first;
        int idBuf = chamberSerial(clctID);
        if(matchToSegmentId && idBuf != chamberId) continue;
        clctId.push_back(idBuf);

        vector<int> clctQBuf;
        vector<int> clctPatBuf;
        vector<int> clctKHSBuf;
        vector<int> clctCFEBBuf;
        vector<int> clctBendBuf;
        vector<int> clctBXBuf;
        vector<int> clctFBXBuf;

        const CSCCLCTDigiCollection::Range& range =(*clctDigi_id).second;
        for(CSCCLCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
        {
            clctQBuf.push_back((*digiItr).getQuality());
            clctPatBuf.push_back((*digiItr).getPattern());
            clctKHSBuf.push_back((*digiItr).getStrip());
            clctCFEBBuf.push_back((*digiItr).getCFEB());
            clctBendBuf.push_back((*digiItr).getBend());
            clctBXBuf.push_back((*digiItr).getBX());
            clctFBXBuf.push_back((*digiItr).getFullBX());
        }
        clctQ.push_back(clctQBuf);
        clctPat.push_back(clctPatBuf);
        clctKHS.push_back(clctKHSBuf);
        clctCFEB.push_back(clctCFEBBuf);
        clctBend.push_back(clctBendBuf);
        clctBX.push_back(clctBXBuf);
        clctFBX.push_back(clctFBXBuf);
    }

    // Extract ALCT for all strips in this chamber
    for (CSCALCTDigiCollection::DigiRangeIterator alctDigi_id=cscALCTDigi->begin(); alctDigi_id!=cscALCTDigi->end(); alctDigi_id++)
    {
        CSCDetId alctID = (*alctDigi_id).first;
        int idBuf = chamberSerial(alctID);
        if(matchToSegmentId && idBuf != chamberId) continue;
        alctId.push_back(idBuf);

        vector<int> alctQBuf;
        vector<int> alctKWGBuf;
        vector<int> alctAcBuf;
        vector<int> alctPBBuf;
        vector<int> alctBXBuf;
        vector<int> alctFBXBuf;

        const CSCALCTDigiCollection::Range& range =(*alctDigi_id).second;
        for(CSCALCTDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
        {
            alctQBuf.push_back((*digiItr).getQuality());
            alctKWGBuf.push_back((*digiItr).getKeyWG());
            alctAcBuf.push_back((*digiItr).getAccelerator());
            alctPBBuf.push_back((*digiItr).getCollisionB());
            alctBXBuf.push_back((*digiItr).getBX());
            alctFBXBuf.push_back((*digiItr).getFullBX());
        }
        alctQ.push_back(alctQBuf);
        alctKWG.push_back(alctKWGBuf);
        alctAc.push_back(alctAcBuf);
        alctPB.push_back(alctPBBuf);
        alctBX.push_back(alctBXBuf);
        alctFBX.push_back(alctFBXBuf);
    }

    // Extract Comparator Data
    for (CSCComparatorDigiCollection::DigiRangeIterator compDigi_id=compDigi->begin(); compDigi_id!=compDigi->end(); compDigi_id++)
    {
        CSCDetId compID = (*compDigi_id).first;
        int idBuf = chamberSerial(compID);
        if(matchToSegmentId && idBuf != chamberId) continue;
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
        if(matchToSegmentId && idBuf != chamberId) continue;
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
        if(matchToSegmentId && idBuf != chamberId) continue;
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

	return 0;
}


// --- writes non-segment specific digis associated with event "iEvent"
int CSCPatternExtractor::extractNonSegmentSpecificDigis(const edm::Event& iEvent){
    edm::Handle<CSCDDUStatusDigiCollection> dduDigi;
    iEvent.getByToken(ddu_token, dduDigi);

    edm::Handle<CSCDMBStatusDigiCollection> dmbDigi;
    iEvent.getByToken(dmb_token, dmbDigi);

    edm::Handle<CSCTMBStatusDigiCollection> tmbDigi;
    iEvent.getByToken(tmb_token, tmbDigi);

	// Extract DDU Status Digis
	for (CSCDDUStatusDigiCollection::DigiRangeIterator dduDigi_id=dduDigi->begin(); dduDigi_id!=dduDigi->end(); dduDigi_id++)
	{
		CSCDetId dduID = (*dduDigi_id).first;
		int idBuf = chamberSerial(dduID);
		//if(idBuf != chamber) continue;
		dduId.push_back(idBuf);

		vector<vector<int>> dduHeaderBuf;
		vector<vector<int>> dduTrailerBuf;

		const CSCDDUStatusDigiCollection::Range& range =(*dduDigi_id).second;
		for(CSCDDUStatusDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
		{
			const uint16_t * headBuf = (*digiItr).header();
			const uint16_t * trailBuf = (*digiItr).trailer();
			vector<int> dduHeaderBuf;
			vector<int> dduTrailerBuf;
			for(int i = 0; i < 12; i++)
			{
				dduHeaderBuf.push_back(*(headBuf+i));
				dduTrailerBuf.push_back(*(trailBuf+i));
			}
			dduHeader.push_back(dduHeaderBuf);
			dduTrailer.push_back(dduTrailerBuf);
		}
	}
	// Extract DMB Status Digis
	for (CSCDMBStatusDigiCollection::DigiRangeIterator dmbDigi_id=dmbDigi->begin(); dmbDigi_id!=dmbDigi->end(); dmbDigi_id++)
	{
		CSCDetId dmbID = (*dmbDigi_id).first;
		int idBuf = chamberSerial(dmbID);
		//if(idBuf != chamber) continue;
		dmbId.push_back(idBuf);

		vector<vector<int>> dmbHeaderBuf;
		vector<vector<int>> dmbTrailerBuf;

		const CSCDMBStatusDigiCollection::Range& range =(*dmbDigi_id).second;
		for(CSCDMBStatusDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
		{
			const uint16_t * headBuf = (*digiItr).header();
			const uint16_t * trailBuf = (*digiItr).trailer();
			vector<int> dmbHeaderBuf;
			vector<int> dmbTrailerBuf;
			for(int i = 0; i < 8; i++)
			{
				dmbHeaderBuf.push_back(*(headBuf+i));
				dmbTrailerBuf.push_back(*(trailBuf+i));
			}
			dmbHeader.push_back(dmbHeaderBuf);
			dmbTrailer.push_back(dmbTrailerBuf);
		}
	}
	// Extract TMB Status Digis
	for (CSCTMBStatusDigiCollection::DigiRangeIterator tmbDigi_id=tmbDigi->begin(); tmbDigi_id!=tmbDigi->end(); tmbDigi_id++)
	{
		CSCDetId tmbID = (*tmbDigi_id).first;
		int idBuf = chamberSerial(tmbID);
		//if(idBuf != chamber) continue;
		tmbId.push_back(idBuf);
		//cout << "tmbID: " << idBuf << endl;

		vector<vector<int>> tmbHeaderBuf;
		vector<vector<int>> tmbTrailerBuf;

		const CSCTMBStatusDigiCollection::Range& range =(*tmbDigi_id).second;
		for(CSCTMBStatusDigiCollection::const_iterator digiItr = range.first; digiItr != range.second; ++digiItr)
		{
			const uint16_t * headBuf = (*digiItr).header();
			const uint16_t * trailBuf = (*digiItr).trailer();
			vector<int> tmbHeaderBuf;
			vector<int> tmbTrailerBuf;
			for(int i = 0; i < 43; i++)
			{
				tmbHeaderBuf.push_back(*(headBuf+i));
				if(i < 8) tmbTrailerBuf.push_back(*(trailBuf+i));
			}
			tmbHeader.push_back(tmbHeaderBuf);
			tmbTrailer.push_back(tmbTrailerBuf);
		}
	}
	return 0;
}
*/

const reco::MuonCollection CSCPatternExtractor::selectSingleMuMuons(const reco::MuonCollection& m){
	return m;
}

const reco::MuonCollection CSCPatternExtractor::selectJPsiMuons(const reco::MuonCollection& m) {
	return m;
}

const reco::MuonCollection CSCPatternExtractor::selectDisplacedMuons(const reco::MuonCollection& m) {
	return m;
}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCPatternExtractor);
