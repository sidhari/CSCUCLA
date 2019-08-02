/*
 * CSCCMSSWTester.cpp
 *
 *  Created on: Jul 22, 2019
 *      Author: wnash
 *
 *  used to test changes made in CMSSW while
 *  implementing new patterns
 */

#include <iostream>

//user include files
#include "CSCUCLA/CSCDigiTuples/include/CSCCMSSWTester.h"

CSCCMSSWTester::CSCCMSSWTester(const edm::ParameterSet &iConfig) /*:
tree(iConfig.getUntrackedParameter<std::string>("NtupleFileName"),
		"CSCDigiTree",
		"Tree holding CSCDigis",
		iConfig.getUntrackedParameter<std::string>("selection")),
		clctInfo(tree),
		compInfo(tree)*/
{
	std::cout << "-- Starting CSCCMSSWTester -- " << std::endl;

	//now do what ever initialization is needed
	//cd_token = consumes<CSCCLCTDigiCollection>( iConfig.getParameter<edm::InputTag>("clctDigiTag") );
	//cod_token = consumes<CSCComparatorDigiCollection>( iConfig.getParameter<edm::InputTag>("compDigiTag") );
}


CSCCMSSWTester::~CSCCMSSWTester(){
	// do anything here that needs to be done at desctruction time
	// (e.g. close files, deallocate resources etc.)

	//tree.write();
}

// ------------ method called once each job just before starting event loop  ------------
void CSCCMSSWTester::beginJob() {
}

// ------------ method called to for each event  ------------
void CSCCMSSWTester::analyze(const edm::Event&iEvent, const edm::EventSetup& iSetup){
	/*
	tree.reset();

	edm::Handle<CSCComparatorDigiCollection> compDigi;
	iEvent.getByToken(cod_token, compDigi);
	if(compDigi->begin() == compDigi->end()) return; //only care about events with comparator data
	tree.h.h_eventCuts->Fill(EVENT_CUTS::hasCSCDigis);

	edm::Handle<CSCCLCTDigiCollection> cscCLCTDigi;
	iEvent.getByToken(cd_token, cscCLCTDigi);


	tree.fill();
	*/
}

// ------------ method called once each job just after ending the event loop  ------------
void CSCCMSSWTester::endJob() {

	//cout << "Wrote to: " << tree.file->GetName() << endl;
	std::cout << "Finished running CSCCMSSWTester.cc" << std::endl;
}
