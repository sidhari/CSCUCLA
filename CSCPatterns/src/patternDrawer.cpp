/*
 * patternDrawer.cpp
 *
 *  Created on: May 2, 2018
 *      Author: williamnash
 */


#include "../include/PatternFinderHelperFunctions.h"

using namespace std;

int patternDrawer(unsigned int pattID,int cc=-1){

	//all the new patterns
	vector<CSCPattern>* newPatterns = createNewPatterns();

	for(auto patt = newPatterns->begin(); patt != newPatterns->end(); ++patt){
		if(patt->_id == pattID){
			printf("Pattern: %i\n", pattID);
			if(cc > 0) printf("     CC:%i\n", cc);


			int hits [MAX_PATTERN_WIDTH][NLAYERS];
			if(patt->recoverPatternCCCombination(cc, hits)){
				cout << "Error: CC evaluation has failed" << endl;
				return -1;
			}
			for(unsigned int i =0; i < NLAYERS; i++){
				for(unsigned int j=0; j < MAX_PATTERN_WIDTH; j++){
					cout << (hits[j][i] ? "X" : "~");
				}
				cout << endl;
			}

		}
	}

	return 0;
}
