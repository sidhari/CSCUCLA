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

//TightID muons
#include "DataFormats/MuonReco/interface/MuonSelectors.h"


/* TODO:
 * - Make set of checks for J/Psi to verify selection. Want to check OS and SS events,
 *   which give you coincidental background rates
 * - Check "TightId" "LooseId" defintions for muons, and verify single muon
 *   selection is done in line this way
 *
 */

CSCPatternExtractor::CSCPatternExtractor(const ParameterSet &iConfig) :
tree(iConfig.getUntrackedParameter<std::string>("NtupleFileName"),"CSCDigiTree","Tree holding CSCDigis"), //set up the tree
eventInfo(tree), //set branches you want in the tree
muonInfo(tree),
segmentInfo(tree),
recHitInfo(tree),
lctInfo(tree),
clctInfo(tree),
compInfo(tree)
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
	vtx_token = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
	csctflct_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("csctfDigiTag"));
	emtflct_token = consumes<CSCCorrelatedLCTDigiCollection>(iConfig.getParameter<edm::InputTag>("emtfDigiTag"));
	ddu_token = consumes<CSCDDUStatusDigiCollection>(iConfig.getParameter<edm::InputTag>("dduDigiTag"));
	dmb_token = consumes<CSCDMBStatusDigiCollection>(iConfig.getParameter<edm::InputTag>("dmbDigiTag"));
	tmb_token = consumes<CSCTMBStatusDigiCollection>(iConfig.getParameter<edm::InputTag>("tmbDigiTag"));

	theCSC = 0;

	/* Choose if we are going to write data by selecting muons,
	 * and if so, which selector to use
	 */

	selection = iConfig.getUntrackedParameter<std::string>("selection");
	if (selection == "minBias"){
		cout <<  "--- Running as minBias sample --- " << endl;
		selectMuons = 0;
	}else{
		if (selection == "SingleMuon"){
			cout <<  "--- Running as Single Muon sample --- " << endl;
			selectMuons = &selectSingleMuMuons;
		} else if (selection == "Charmonium") {
			cout <<  "--- Running as Charmonium sample --- " << endl;
			selectMuons = &selectJPsiMuons;
		}else if(selection == "displacedMuon") {
			cout <<  "--- Running as DisplacedMuon sample --- " << endl;
			selectMuons = &selectDisplacedMuons;
		} else { //default to single muon selection
			cout <<  "--- Defaulting as singleMu sample --- " << endl;
			selectMuons = &selectSingleMuMuons;
		}
	}

	muonQualityCuts = new MuonQualityCuts(iConfig);

	//evN = 0;

	edm::ParameterSet serviceParameters = iConfig.getParameter<edm::ParameterSet>("ServiceParameters");
	theService = new MuonServiceProxy(serviceParameters);

	edm::ParameterSet matchParameters = iConfig.getParameter<edm::ParameterSet>("MatchParameters");

	CSCSegmentTags_=matchParameters.getParameter<edm::InputTag>("CSCsegments");
	cscTightMatch = matchParameters.getParameter<bool>("TightMatchCSC");


	edm::ConsumesCollector iC  = consumesCollector();
	allSegmentsCSCToken = iC.consumes<CSCSegmentCollection>(CSCSegmentTags_) ;
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
	tree.reset();
	cout << "Filling Pattern Extractor" << endl;

	Handle<reco::MuonCollection> muons;
	iEvent.getByToken(mc_token, muons);
	tree.h_eventCuts->AddBinContent(EVENT_CUTS::allEvents);
	tree.h_muonCuts->AddBinContent(MUON_CUTS::allMuons, muons->size());
	tree.h_nAllMuons->Fill(muons->size());
	if(!muons->size()){
		cout << "\n\n\n\n No Muons Found: "<< endl;
		cout << "Event: " << iEvent.id().event() <<  endl;
		cout << "Lumi: " << iEvent.eventAuxiliary().luminosityBlock()<<  endl;
		cout << "BX: " << iEvent.eventAuxiliary().bunchCrossing()<<  endl;
		cout << "Event: " << iEvent.id().event() <<  endl;
		cout << "Run: " << iEvent.id().run() << "\n\n\n" << endl;

	}

	for(const auto& mu: *muons) {
		tree.h_allMuonsPt->Fill(mu.pt());
		tree.h_allMuonsEta->Fill(mu.eta());
		tree.h_allMuonsPhi->Fill(mu.phi());
	}

	for(unsigned int i =0; i < muons->size(); i++){
		const auto& mu1 = muons->at(i);
		for(unsigned int j = i+1; j < muons->size(); j++){
			const auto& mu2 = muons->at(j);
			TLorentzVector mu1Lorentz;
			mu1Lorentz.SetPtEtaPhiE(mu1.pt(), mu1.eta(), mu1.phi(), mu1.energy());
			TLorentzVector mu2Lorentz;
			mu2Lorentz.SetPtEtaPhiE(mu2.pt(), mu2.eta(), mu2.phi(), mu2.energy());
			float invMass = (mu1Lorentz+mu2Lorentz).M();
			tree.h_allInvMass->Fill(invMass);
		}
	}


    //Get all the segments
    edm::Handle<CSCSegmentCollection> allSegmentsCSC;
    iEvent.getByToken(allSegmentsCSCToken, allSegmentsCSC);
    tree.h_nAllSegments->Fill(allSegmentsCSC->size());

    //only care about events with endcap segments
    if(selectMuons && !allSegmentsCSC->size()) return;

    tree.h_eventCuts->AddBinContent(EVENT_CUTS::eventHasSegments);



    edm::Handle<CSCComparatorDigiCollection> compDigi;
    iEvent.getByToken(cod_token, compDigi);
    if(compDigi->begin() == compDigi->end()) return; //only care about events with comparator data
    tree.h_eventCuts->AddBinContent(EVENT_CUTS::eventHasCSCDigis);

    edm::Handle<CSCCorrelatedLCTDigiCollection> cscLCTDigi;
    iEvent.getByToken(ld_token, cscLCTDigi);

    edm::Handle<CSCCLCTDigiCollection> cscCLCTDigi;
    iEvent.getByToken(cd_token, cscCLCTDigi);

    ESHandle<CSCGeometry> cscGeom;
    iSetup.get<MuonGeometryRecord>().get(cscGeom);
    theCSC = cscGeom.product();

	eventInfo.fill(iEvent, allSegmentsCSC->size());

    if(selectMuons){
        edm::Handle<reco::VertexCollection> vertices;
        iEvent.getByToken(vtx_token, vertices);
    	if(vertices->empty()) return; //only look at events with a PV
    	tree.h_eventCuts->AddBinContent(EVENT_CUTS::eventHasVertex);

    	const reco::MuonCollection selected_muons = selectMuons(*muons,vertices->front(),tree);
    	if(!selected_muons.size()) return; //skip events without selected muons

    	for(const auto& muon: selected_muons){
    		vector<const CSCSegment*> matchedSegments = matchCSC(*muon.standAloneMuon(), allSegmentsCSC);
    		//could be barrel or endcap, only care about endcap
    		if(!matchedSegments.size()) continue;
    		muonInfo.fill(muon);
    		for(const auto& segment: matchedSegments){
    			segmentInfo.fill(*segment, theCSC, muonInfo.size()-1);
    			recHitInfo.fill(segment->specificRecHits(), muonInfo.size()-1);
    		}

    	}
    	if(!muonInfo.size()) return; //no muons with csc info
    	//fill only the muons that have csc segments associtaed with them
    	tree.h_nSelectedMuons->Fill(muonInfo.size());

    } else { //TODO: can make it so all unmatched segments are written out as well in selected case
    	for(const auto& segment: *allSegmentsCSC){
    		//TODO: might need to do some selection here
    		segmentInfo.fill(segment, theCSC, -1); //default unmatched to -1
    		recHitInfo.fill(segment.specificRecHits(), -1);
    	}
    }

    lctInfo.fill(*cscLCTDigi);
    clctInfo.fill(*cscCLCTDigi);
    compInfo.fill(*compDigi);


	tree.fill();
}


// ------------ method called once each job just after ending the event loop  ------------
void CSCPatternExtractor::endJob() {
	cout << "Wrote to: " << tree.file->GetName() << endl;
	cout << "Finished running CSCPatternExtractor.cc" << endl;
}


const reco::MuonCollection CSCPatternExtractor::selectSingleMuMuons(const reco::MuonCollection& m,const reco::Vertex& vtx, TreeContainer& t){
	float minPt = 20.;
	float massZ = 91.1876;
	float massWindow = 10;

	vector<unsigned int> passMassCut;
	vector<unsigned int> sameSign;
	vector<unsigned int> oppositeSign;
	vector<unsigned int> selectedIndices;

	bool foundOS = false; //opposite sign
	bool foundSS = false; //same sign
	for(unsigned int i =0; i < m.size(); i++){
		const auto& mu1 = m.at(i);
		t.h_muonCuts->AddBinContent(MUON_CUTS::allMuonsAfterEventCuts);

		if(!mu1.isStandAloneMuon()) continue;
		t.h_muonCuts->AddBinContent(MUON_CUTS::isStandAlone);

		if(!mu1.isGlobalMuon()) continue;
		t.h_muonCuts->AddBinContent(MUON_CUTS::isGlobal);

		if(find(selectedIndices.begin(), selectedIndices.end(), i) != selectedIndices.end()) continue; //only match mu1 once
		//if(!mu1.isStandAloneMuon() || !mu1.isGlobalMuon()) continue; //standalone , global 1 cut
		for(unsigned int j =i+1; !foundOS && !foundSS && j < m.size(); j++){
			if(find(selectedIndices.begin(), selectedIndices.end(), j) != selectedIndices.end()) continue; //only match mu2 once
			const auto& mu2 = m.at(j);
			if(!mu2.isStandAloneMuon() || !mu2.isGlobalMuon()) continue; //standalone,global 2 cut

			TLorentzVector mu1Lorentz;
			mu1Lorentz.SetPtEtaPhiE(mu1.pt(), mu1.eta(), mu1.phi(), mu1.energy());
			TLorentzVector mu2Lorentz;
			mu2Lorentz.SetPtEtaPhiE(mu2.pt(), mu2.eta(), mu2.phi(), mu2.energy());
			float invMass = (mu1Lorentz+mu2Lorentz).M();
			t.h_premassCutInvMass->Fill(invMass);
			if(invMass < massZ - massWindow || invMass > massZ + massWindow) continue; //mass cut
			if(find(passMassCut.begin(), passMassCut.end(), i) == passMassCut.end()) passMassCut.push_back(i);
			if(find(passMassCut.begin(), passMassCut.end(), j) == passMassCut.end()) passMassCut.push_back(j);

			if(mu1.charge() == mu2.charge()){
				t.h_ssInvMass->Fill(invMass);
				if(find(sameSign.begin(), sameSign.end(), i) == sameSign.end()) sameSign.push_back(i);
				if(find(sameSign.begin(), sameSign.end(), j) == sameSign.end()) sameSign.push_back(j);
				foundSS = true;
				continue; //opposite sign cut
			}
			else{
				t.h_osInvMass->Fill(invMass);
				foundOS = true;
				if(find(oppositeSign.begin(), oppositeSign.end(), i) == oppositeSign.end()) oppositeSign.push_back(i);
				if(find(oppositeSign.begin(), oppositeSign.end(), j) == oppositeSign.end()) oppositeSign.push_back(j);
				if(m.at(i).pt() >= minPt) selectedIndices.push_back(i);
				if(m.at(j).pt() >= minPt) selectedIndices.push_back(j);
			}
		}
	}

	t.h_muonCuts->AddBinContent(MUON_CUTS::isInMassWindow, passMassCut.size());
	t.h_muonCuts->AddBinContent(MUON_CUTS::isSS, sameSign.size());
	t.h_muonCuts->AddBinContent(MUON_CUTS::isOS, oppositeSign.size());


	reco::MuonCollection selectedMuons;
	for(auto index : selectedIndices) {
		selectedMuons.push_back(m.at(index));
		t.h_muonCuts->AddBinContent(MUON_CUTS::isOSAndPtOVerThreshold);
		t.h_selectedMuonsPt->Fill(m.at(index).pt());
		t.h_selectedMuonsEta->Fill(m.at(index).eta());
	}
	return selectedMuons;
}


const reco::MuonCollection CSCPatternExtractor::selectJPsiMuons(const reco::MuonCollection& m,const reco::Vertex& vtx,TreeContainer& t) {

	float minPt = 2.0;
	float massJPsi = 3.0969; //GeV
	float massWindow = 0.1; //GeV

	vector<unsigned int> passMassCut;
	vector<unsigned int> sameSign;
	vector<unsigned int> oppositeSign;
	vector<unsigned int> selectedIndices;

	bool foundOS = false; //opposite sign
	bool foundSS = false; //same sign
	for(unsigned int i =0; i < m.size(); i++){
		const auto& mu1 = m.at(i);
		t.h_muonCuts->AddBinContent(MUON_CUTS::allMuonsAfterEventCuts);

		if(!mu1.isStandAloneMuon()) continue;
		t.h_muonCuts->AddBinContent(MUON_CUTS::isStandAlone);

		if(!mu1.isGlobalMuon()) continue;
		t.h_muonCuts->AddBinContent(MUON_CUTS::isGlobal);

		if(find(selectedIndices.begin(), selectedIndices.end(), i) != selectedIndices.end()) continue; //only match mu1 once
		for(unsigned int j =i+1; !foundOS && !foundSS && j < m.size(); j++){
			if(find(selectedIndices.begin(), selectedIndices.end(), j) != selectedIndices.end()) continue; //only match mu2 once
			const auto& mu2 = m.at(j);
			if(!mu2.isStandAloneMuon() || !mu2.isGlobalMuon()) continue; //standalone,global 2 cut

			TLorentzVector mu1Lorentz;
			mu1Lorentz.SetPtEtaPhiE(mu1.pt(), mu1.eta(), mu1.phi(), mu1.energy());
			TLorentzVector mu2Lorentz;
			mu2Lorentz.SetPtEtaPhiE(mu2.pt(), mu2.eta(), mu2.phi(), mu2.energy());
			float invMass = (mu1Lorentz+mu2Lorentz).M();
			t.h_premassCutInvMass->Fill(invMass);
			if(invMass < massJPsi - massWindow || invMass > massJPsi + massWindow) continue; //mass cut
			if(find(passMassCut.begin(), passMassCut.end(), i) == passMassCut.end()) passMassCut.push_back(i);
			if(find(passMassCut.begin(), passMassCut.end(), j) == passMassCut.end()) passMassCut.push_back(j);

			if(mu1.charge() == mu2.charge()){
				t.h_ssInvMass->Fill(invMass);
				if(find(sameSign.begin(), sameSign.end(), i) == sameSign.end()) sameSign.push_back(i);
				if(find(sameSign.begin(), sameSign.end(), j) == sameSign.end()) sameSign.push_back(j);
				foundSS = true;
				continue; //opposite sign cut
			}
			else{
				t.h_osInvMass->Fill(invMass);
				foundOS = true;
				if(find(oppositeSign.begin(), oppositeSign.end(), i) == oppositeSign.end()) oppositeSign.push_back(i);
				if(find(oppositeSign.begin(), oppositeSign.end(), j) == oppositeSign.end()) oppositeSign.push_back(j);
				if(m.at(i).pt() >= minPt) selectedIndices.push_back(i);
				if(m.at(j).pt() >= minPt) selectedIndices.push_back(j);
			}
		}
	}

	t.h_muonCuts->AddBinContent(MUON_CUTS::isInMassWindow, passMassCut.size());
	t.h_muonCuts->AddBinContent(MUON_CUTS::isSS, sameSign.size());
	t.h_muonCuts->AddBinContent(MUON_CUTS::isOS, oppositeSign.size());


	reco::MuonCollection selectedMuons;
	for(auto index : selectedIndices) {
		selectedMuons.push_back(m.at(index));
		t.h_muonCuts->AddBinContent(MUON_CUTS::isOSAndPtOVerThreshold);
		t.h_selectedMuonsPt->Fill(m.at(index).pt());
		t.h_selectedMuonsEta->Fill(m.at(index).eta());
		t.h_selectedMuonsPhi->Fill(m.at(index).phi());
	}
	return selectedMuons;
}

const reco::MuonCollection CSCPatternExtractor::selectDisplacedMuons(const reco::MuonCollection& m,const reco::Vertex& vtx, TreeContainer& t) {
	reco::MuonCollection selectedMuons;
	for(const auto& muon : m){
		if(!muon.isStandAloneMuon()) continue; //selecting on standalone muons (necessary?)
		selectedMuons.push_back(muon);
	}
	return selectedMuons;
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
