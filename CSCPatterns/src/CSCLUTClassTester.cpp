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

	cout << "-- Testing Comparator Codes --" << endl;


	ComparatorCode c(1365);
	cout << "\t ccode: " << c.getId() << endl;
	ComparatorCode cc(c);
	cout << "\t ccode: " << cc.getId() << endl;

	cout << "-- Testing CLCTCandidates -- " << endl;

	CLCTCandidate cd(newP->front(),c, 1,1);
	cout << "\t clct: patternId: "<< cd.patternId() << " ccId: " << cd.comparatorCodeId() << endl;


}
