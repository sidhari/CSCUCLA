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


/*Mysterious bug, likely to do with Object_> const string name in CSCInfo.h...
 *
 */

CSCPatternExtractor::CSCPatternExtractor(const ParameterSet &iConfig) :
tree(iConfig.getUntrackedParameter<std::string>("NtupleFileName"),"CSCDigiTree","Tree holding CSCDigis"), //set up the tree

eventInfo(tree) //set branches you want in the tree
//muonInfo(tree)
//segmentInfo(tree),
//recHitInfo(tree)
/*
lctInfo(tree),
clctInfo(tree),
compInfo(tree)
*/
{
	cout << "-- Starting CSCPatternExtractor --" << endl;

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

	CSCSegmentTags_=matchParameters.getParameter<edm::InputTag>("CSCsegments");
	cscTightMatch = matchParameters.getParameter<bool>("TightMatchCSC");


	edm::ConsumesCollector iC  = consumesCollector();
	allSegmentsCSCToken = iC.consumes<CSCSegmentCollection>(CSCSegmentTags_) ;


	//TEMP
	ss = true;
	os = true;
	Pt = 69;
	eta = 69;
	phi = 69;
	q = -69;

}

CSCPatternExtractor::~CSCPatternExtractor(){
	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)
	tree.write();
}


// ------------ method called once each job just before starting event loop  ------------
void CSCPatternExtractor::beginJob() {
}


// ------------ method called to for each event  ------------
void CSCPatternExtractor::analyze(const edm::Event&iEvent, const edm::EventSetup& iSetup){
	resetFillInfo();
	cout << "Filling Pattern Extractor" << endl;

	eventInfo.fill(iEvent);

    // Get the CSC Geometry :
    ESHandle<CSCGeometry> cscGeom;
    iSetup.get<MuonGeometryRecord>().get(cscGeom);
    theCSC = cscGeom.product();

    //Get all the segments
    edm::Handle<CSCSegmentCollection> allSegmentsCSC;
    iEvent.getByToken(allSegmentsCSCToken, allSegmentsCSC);

    edm::Handle<CSCCorrelatedLCTDigiCollection> cscLCTDigi;
    iEvent.getByToken(ld_token, cscLCTDigi);

    edm::Handle<CSCCLCTDigiCollection> cscCLCTDigi;
    iEvent.getByToken(cd_token, cscCLCTDigi);

    edm::Handle<CSCComparatorDigiCollection> compDigi;
    iEvent.getByToken(cod_token, compDigi);

    if(selectMuons){
    	Handle<reco::MuonCollection> muons;
    	iEvent.getByToken(mc_token, muons);

    	const reco::MuonCollection selected_muons = selectMuons(*muons);
    	//muonInfo.fill(selected_muons);

    	for(unsigned int mu_index = 0; mu_index < selected_muons.size(); mu_index++){
    		auto muon = selected_muons.at(mu_index);

    		//vector<const CSCSegment*> matchedSegments = theMatcher->matchCSC(*muon.standAloneMuon(), iEvent);
    		vector<const CSCSegment*> matchedSegments = matchCSC(*muon.standAloneMuon(), allSegmentsCSC);
    		for(const auto& segment: matchedSegments){
    			//segmentInfo.fill(*segment, theCSC, mu_index);
    			//recHitInfo.fill(segment->specificRecHits(), mu_index);
    		}
    	}
    } else { //TODO: can make it so all unmatched segments are written out as well in selected case
    	for(const auto& segment: *allSegmentsCSC){
    		//TODO: might need to do some selection here
    		//segmentInfo.fill(segment, theCSC, -1); //default unmatched to -1
    		//recHitInfo.fill(segment.specificRecHits(), -1);
    	}
    }

    //lctInfo.fill(*cscLCTDigi);
    //clctInfo.fill(*cscCLCTDigi);
    //compInfo.fill(*compDigi);
	/*

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
    */

	tree.fill();
}


// ------------ method called once each job just after ending the event loop  ------------
void CSCPatternExtractor::endJob() {
	cout << "Finished running CSCPatternExtractor.cc" << endl;
}


const reco::MuonCollection CSCPatternExtractor::selectSingleMuMuons(const reco::MuonCollection& m){
	reco::MuonCollection selectedMuons;
	for(const auto& muon : m){
		if(!muon.isStandAloneMuon()) continue; //selecting on standalone muons (necessary?)
		selectedMuons.push_back(muon);
	}
	return selectedMuons;
}

const reco::MuonCollection CSCPatternExtractor::selectJPsiMuons(const reco::MuonCollection& m) {
	reco::MuonCollection selectedMuons;
	for(const auto& muon : m){
		if(!muon.isStandAloneMuon()) continue; //selecting on standalone muons (necessary?)
		selectedMuons.push_back(muon);
	}
	return selectedMuons;
}

const reco::MuonCollection CSCPatternExtractor::selectDisplacedMuons(const reco::MuonCollection& m) {
	reco::MuonCollection selectedMuons;
	for(const auto& muon : m){
		if(!muon.isStandAloneMuon()) continue; //selecting on standalone muons (necessary?)
		selectedMuons.push_back(muon);
	}
	return selectedMuons;
}

void CSCPatternExtractor::resetFillInfo(){

	eventInfo.reset();

//	muonInfo.reset();
//	segmentInfo.reset();
//	recHitInfo.reset();
/*
	lctInfo.reset();
	clctInfo.reset();
	compInfo.reset();
	*/
}


/* Finds all the segments associated with a muon in the CSC system, taken from:
 * https://github.com/cms-sw/cmssw/blob/master/RecoMuon/TrackingTools/src/MuonSegmentMatcher.cc
 *
 * Author: Alan Tua
 *
 * Needed to modify arguments to modularize searching
 *
 *
 */
vector<const CSCSegment*> CSCPatternExtractor::matchCSC(const reco::Track& muon, edm::Handle<CSCSegmentCollection> allSegmentsCSC){
	//using namespace edm;

	//edm::Handle<CSCSegmentCollection> allSegmentsCSC;
	//event.getByToken(allSegmentsCSCToken, allSegmentsCSC);

	vector<const CSCSegment*> pointerToCSCSegments;

	double matchRatioCSC=0;
	int numCSC = 0;
	double CSCXCut = 0.001;
	double CSCYCut = 0.001;
	double countMuonCSCHits = 0;

	for(CSCSegmentCollection::const_iterator segmentCSC = allSegmentsCSC->begin(); segmentCSC != allSegmentsCSC->end(); segmentCSC++) {
		double CSCcountAgreeingHits=0;

		if ( !segmentCSC->isValid()) continue;


		numCSC++;
		const vector<CSCRecHit2D>& CSCRechits2D = segmentCSC->specificRecHits();
		countMuonCSCHits = 0;
		CSCDetId myChamber((*segmentCSC).geographicalId().rawId());

		bool segments = false;

		for(trackingRecHit_iterator hitC = muon.recHitsBegin(); hitC != muon.recHitsEnd(); ++hitC) {
			if (!(*hitC)->isValid()) continue;
			if ( (*hitC)->geographicalId().det() != DetId::Muon ) continue;
			if ( (*hitC)->geographicalId().subdetId() != MuonSubdetId::CSC ) continue;
			if (!(*hitC)->isValid()) continue;
			if ( (*hitC)->recHits().size()>1) segments = true;

			//DETECTOR CONSTRUCTION
			DetId id = (*hitC)->geographicalId();
			CSCDetId cscDetIdHit(id.rawId());

			if (segments) {
				if(!(myChamber.rawId()==cscDetIdHit.rawId())) continue;

				// and compare the local positions
				LocalPoint positionLocalCSC = (*hitC)->localPosition();
				LocalPoint segLocalCSC = segmentCSC->localPosition();
				if ((fabs(positionLocalCSC.x()-segLocalCSC.x())<CSCXCut) &&
						(fabs(positionLocalCSC.y()-segLocalCSC.y())<CSCYCut))
					pointerToCSCSegments.push_back(&(*segmentCSC));
				continue;
			}

			if(!(cscDetIdHit.ring()==myChamber.ring())) continue;
			if(!(cscDetIdHit.station()==myChamber.station())) continue;
			if(!(cscDetIdHit.endcap()==myChamber.endcap())) continue;
			if(!(cscDetIdHit.chamber()==myChamber.chamber())) continue;

			countMuonCSCHits++;

			LocalPoint positionLocalCSC = (*hitC)->localPosition();

			for (vector<CSCRecHit2D>::const_iterator hiti=CSCRechits2D.begin(); hiti!=CSCRechits2D.end(); hiti++) {

				if ( !hiti->isValid()) continue;
				CSCDetId cscDetId((hiti->geographicalId()).rawId());

				if ((*hitC)->geographicalId().rawId()!=(hiti->geographicalId()).rawId()) continue;

				LocalPoint segLocalCSC = hiti->localPosition();
				//		cout<<"Layer Id (MuonHit) =  "<<cscDetIdHit<<" Muon Local Position (det frame) "<<positionLocalCSC <<endl;
				//		cout<<"Layer Id  (CSCHit) =  "<<cscDetId<<"  Hit Local Position (det frame) "<<segLocalCSC <<endl;
				if((fabs(positionLocalCSC.x()-segLocalCSC.x())<CSCXCut) &&
						(fabs(positionLocalCSC.y()-segLocalCSC.y())<CSCYCut)) {
					CSCcountAgreeingHits++;
					//		  cout << "   Matched." << endl;
				}
			}//End 2D rechit iteration
		}//End muon hit iteration

		matchRatioCSC = countMuonCSCHits == 0 ? 0 : CSCcountAgreeingHits/countMuonCSCHits;

		if ((matchRatioCSC>0.9) && ((countMuonCSCHits>1) || !cscTightMatch)) pointerToCSCSegments.push_back(&(*segmentCSC));

	} //End CSC Segment Iteration

	return pointerToCSCSegments;


}

//define this as a plug-in
DEFINE_FWK_MODULE(CSCPatternExtractor);
