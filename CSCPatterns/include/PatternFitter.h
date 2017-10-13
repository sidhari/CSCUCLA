/*
 * PatternFitter.h
 *
 *  Created on: Oct 9, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFITTER_H_
#define PATTERNFITTER_H_

#include <iostream>

//#include "PatternFinder.h"

const int DEBUG = 1;
const int NLAYERS = 6; //6 layers




//this class tells us what locations within a pattern with id "m_superPatternId" matches
// each of the true booleans within the superchargepattern.
class ChargePattern {
public:
	//int m_superPatternId; //id of the super pattern which contains this charge pattern
	unsigned int getLayersMatched();
	int getPatternId(); //hexidecimal code used to identify each possible arrangement of the charge in the pattern
	void printPattern();

	ChargePattern(bool pat[NLAYERS][3]) {
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = pat[i][j];
			}
		}
	};
	~ChargePattern() {}
private:
	bool m_hits[NLAYERS][3]; //all the hits within a superpattern that a chamber scan matched
};

void ChargePattern::printPattern() {
	printf("Pattern ID: %i, layersMatched: %u\n", getPatternId(),getLayersMatched());
	for(int i =0; i < NLAYERS; i++){
		for(int j =0; j < 3; j++){
			printf("%i", m_hits[i][j]);
		}
		printf("\n");
	}
}

int ChargePattern::getPatternId() {
	int patternId = 0;

	for(int column = 0; column < NLAYERS; column++){
		int rowPat = 0; //physical arrangement of the three bits
		int rowCode = 0; //code used to identify the arrangement
		for(int row = 0; row < 3; row++){
			rowPat = rowPat << 1; //bitshift the last number to the left
			rowPat += m_hits[column][row];
		}
		switch(rowPat) {
			case 0 : //000
					rowCode = 0;
					break;
			case 1 : //001
					rowCode = 1;
					break;
			case 2 : //010
					rowCode = 2;
					break;
			case 4 : //100
					rowCode = 3;
					break;
			default:
				printf("Error: unknown rowPattern - %i\n", rowPat);
				return -1;
		}
		//each column has two bits of information
		patternId += (rowCode << 2*column);
	}
	if(DEBUG) cout << "patternId is: "  << bitset<12>(patternId) << endl;
	return patternId;
}


unsigned int ChargePattern::getLayersMatched() {
	unsigned int layersMatched = 0;
	for(int i = 0; i < NLAYERS; i++){
		bool matched = false;
		for(int j = 0; j < 3; j++){
			if(m_hits[i][j]) matched = 1;
		}
		layersMatched += matched;
	}
	return layersMatched;
}

#endif /* PATTERNFITTER_H_ */
