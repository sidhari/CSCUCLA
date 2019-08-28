/*
 * printPatternCC.cpp
 *
 *  Created on: Jul 28, 2018
 *      Author: wnash
 */

#include "../include/CSCHelperFunctions.h"
#include <iostream>



int main(int argc, char* argv[])
{
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
	ComparatorCode cc(c);
	cout << "\t ccode: " << cc.getId() << endl;

	cout << "-- Testing CLCTCandidates -- " << endl;

	CLCTCandidate cd(newP->front(),c, 1,1);
	cout << "\t clct: patternId: "<< cd.patternId() << " ccId: " << cd.comparatorCodeId() << endl;


	CLCTCandidate ce(newP->at(1),c, 1,1);
	cout << "\t clct: patternId: "<< ce.patternId() << " ccId: " << ce.comparatorCodeId() << endl;

	CLCTCandidate cf(newP->at(2),c, 0,1);
	cout << "\t clct: patternId: "<< cf.patternId() << " ccId: " << cf.comparatorCodeId() << endl;


	cout << "-- Testing Sorting --" << endl;
	vector<CLCTCandidate*> candidates;
	candidates.push_back(&cd);
	candidates.push_back(&ce);
	candidates.push_back(&cf);

	sort(candidates.begin(),candidates.end(), CLCTCandidate::cfebQuality);

	for(auto& cand: candidates){
		cout << "\t clct: patternId: "<< cand->patternId()
				<< " ccId: " << cand->comparatorCodeId() << endl;
	}

}
