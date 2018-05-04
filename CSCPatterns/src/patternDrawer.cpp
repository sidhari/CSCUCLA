/*
 * patternDrawer.cpp
 *
 *  Created on: May 2, 2018
 *      Author: williamnash
 */


#include "../include/PatternFinderHelperFunctions.h"

using namespace std;

int patternDrawer(unsigned int pattID,unsigned int cc){

	//all the new patterns
	vector<ChargePattern>* newPatterns = createNewEnvelopes();

	for(auto patt = newPatterns->begin(); patt != newPatterns->end(); ++patt){
		if(patt->m_id == pattID){
			int hits [MAX_PATTERN_WIDTH][NLAYERS];
			if(patt->recoverPatternCCCombination(cc, hits)){
				cout << "Error: CC evaluation has failed" << endl;
				return -1;
			}
			for(unsigned int i =0; i < NLAYERS; i++){
				for(unsigned int j=0; j < MAX_PATTERN_WIDTH; j++){
					cout << hits[j][i];
				}
				cout << endl;
			}

		}
	}

	return 0;
}
