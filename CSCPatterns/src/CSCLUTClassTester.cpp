/*
 * printPatternCC.cpp
 *
 *  Created on: Jul 28, 2018
 *      Author: wnash
 */

#include "../include/CSCHelperFunctions.h"
#include "../include/CSCHelper.h"
#include <iostream>

/*@todo
 * Make a useful and thorough unit tester of each class we have made
 *
 */


int main(int argc, char* argv[])
{
	bool printChamberDebug = false;


	cout << "== Testing CSC Classes ==" << endl;

	vector<CSCPattern>* newP = createNewPatterns();

	for(auto& patt: *newP) {
		cout << "bendBit: " << patt.bendBit() << endl;
	}

	vector<CSCPattern>* oldP = createOldPatterns();

	for(auto& patt: *oldP) {
		cout << "bendBit: " << patt.bendBit() << endl;
	}

	cout << "-- Testing Comparator Codes --" << endl;


	ComparatorCode c(4095);
	cout << "\t ccode: " << c.getId() << endl;
	c.printCode();
	newP->front().printCode(c.getId());
	ComparatorCode cc(c);
	cout << "\t ccode: " << cc.getId() << endl;
	cc.printCode();


	cout << "-- Testing CLCTCandidates -- " << endl;

	CLCTCandidate cd(newP->front(),c, 1,1);
	cout << "\t clct: patternId: "<< cd.patternId() << " ccId: " << cd.comparatorCodeId() << endl;

	CLCTCandidate ce(newP->at(1),c, 1,1);
	cout << "\t clct: patternId: "<< ce.patternId() << " ccId: " << ce.comparatorCodeId() << endl;

	CLCTCandidate cf(newP->at(2),c, 0,1);
	cout << "\t clct: patternId: "<< cf.patternId() << " ccId: " << cf.comparatorCodeId() << endl;

	CLCTCandidate cg(newP->at(3),ComparatorCode(3753), 0,1);
	cout << "\t clct: patternId: "<< cf.patternId() << " ccId: " << cf.comparatorCodeId() << endl;
	cg.printCodeInPattern();
	newP->at(3).printCode(3753);
	newP->at(3).printCode(1707);


	cout << "-- Testing Sorting --" << endl;
	vector<CLCTCandidate*> candidates;
	candidates.push_back(&cd);
	candidates.push_back(&ce);
	candidates.push_back(&cf);
	candidates.push_back(&cg);

	sort(candidates.begin(),candidates.end(), CLCTCandidate::cfebQuality);

	for(auto& cand: candidates){
		cout << "\t clct: patternId: "<< cand->patternId()
				<< " ccId: " << cand->comparatorCodeId() << endl;
	}

	cout << "-- Testing Matching Algorithm --" << endl;
	CSCInfo::Comparators comps;
	comps.ch_id = new std::vector<int>();
	comps.lay = new std::vector<size8>();
	comps.strip = new std::vector<size8>();
	comps.halfStrip = new std::vector<size8>();
	comps.bestTime = new std::vector<size8>();
	comps.nTimeOn = new std::vector<size8>();

	//for(unsigned int ihs=0; ihs < 4*CFEB_HS; ihs++){
	for(unsigned int ihs=0; ihs < CFEB_HS/4; ihs++){


		ChamberHits compHits(1,1,1,1); //fake me11b chamber
		for(unsigned int ilay=0; ilay < NLAYERS; ilay++){

			comps.ch_id->push_back(CSCHelper::serialize(1,1,1,1));
			comps.lay->push_back(ilay+1);
			comps.strip->push_back((ihs+2)/2);
			//comps.strip->push_back(ihs+1);
			comps.halfStrip->push_back(ihs%2);
			comps.bestTime->push_back(7);
			comps.nTimeOn->push_back(1);

		}
		compHits.fill(comps);
		if(printChamberDebug) compHits.print();

		vector<CLCTCandidate*> emulatedCLCTs;


		if(searchForMatch(compHits,newP, emulatedCLCTs,true)){
			emulatedCLCTs.clear();
			return -1;
		}

		if(printChamberDebug) std::cout << "khs, patt, ccode" << std::endl;
		if(printChamberDebug) std::cout << internal << setfill('0');
		const unsigned int nCLCTs = 2;
		for(unsigned int i=0; i < nCLCTs; i++){
			if(i < emulatedCLCTs.size()){

				auto& clct = emulatedCLCTs.at(i);
				if(printChamberDebug) {
				std::cout << "hs: " << clct->keyHalfStrip() <<" patt: " << clct->patternId() << " cc: " << clct->comparatorCodeId() << std::endl;
				std::cout << hex << setw(4) << clct->keyHalfStrip() << endl;
				std::cout << setw(4) << clct->patternId() << endl;
				std::cout << setw(4) << clct->comparatorCodeId() << endl;

				}
			}
		}

		comps.ch_id->clear();
		comps.lay->clear();
		comps.strip->clear();
		comps.halfStrip->clear();
		comps.bestTime->clear();
		comps.nTimeOn->clear();
	}


	delete comps.ch_id;
	delete comps.lay;
	delete comps.strip;
	delete comps.halfStrip;
	delete comps.bestTime;
	delete comps.nTimeOn;

}
