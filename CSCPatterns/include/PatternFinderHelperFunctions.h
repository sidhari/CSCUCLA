/*
 * PatternFinderHelperFunctions.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFINDERHELPERFUNCTIONS_H_
#define PATTERNFINDERHELPERFUNCTIONS_H_

#include "PatternFinderClasses.h"

void printSuperPattern(const ChargeSuperPattern &p) {
	printf("-- Printing Pattern: %i ---\n", p.m_id);
	for(unsigned int y = 0; y < NLAYERS; y++) {
		for(unsigned int x = 0; x < MAX_PATTERN_WIDTH; x++){
			if(p.m_hits[x][y]) printf("X");
			else printf("~");
		}
		printf("\n");
	}
}

void printChamber(const ChamberHits &c){
	printf("==== Printing Chamber Distribution ST = %i, RI = %i, EC = %i====\n", c.station, c.ring, c.endcap);
	bool me11 = (c.station == 1 &&(c.ring == 1 || c.ring == 4));
	for(unsigned int y = 0; y < NLAYERS; y++) {
		for(unsigned int x = 0; x < N_MAX_HALF_STRIPS; x++){
			if(c.hits[x][y]) printf("%X",c.hits[x][y]-1); //print one less, so we stay in hexadecimal (0-15)
			else {
				if(!me11 && !(y%2) && x == 0){ //not me11, even, put a space on the first one
					printf(" ");
				}else if(!me11 && y%2 && x == N_MAX_HALF_STRIPS - 1){
					printf(" ");
				} else	printf("-");
			}
		}
		printf("\n");
	}
}

bool validComparatorTime(bool isComparator, int time){
	if(isComparator){
		float lowTimeLimit = 0.5*MAX_COMP_TIME_BIN - TIME_RANGE;
		float highTimeLimit = 0.5*MAX_COMP_TIME_BIN + TIME_RANGE;

		return (time -1 > lowTimeLimit) && (time - 1 < highTimeLimit);
	} else return time;
}

//looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers if ,
// run successfully, match info is stored in variable mi
int containsPattern(const ChamberHits &c, const ChargeSuperPattern &p,  SingleSuperPatternMatchInfo &mi){

	//overlap between tested super pattern and chamber hits
	bool overlap [NLAYERS][3];
	bool bestOverlap [NLAYERS][3];
	int bestHoriontalPosition = 0; //best horizontal position in chamber, from left
	for(unsigned int i=0; i < NLAYERS; i++){
		for(unsigned int j =0; j < 3; j++){
			overlap[i][j] = false; //initialize all as false
			bestOverlap[i][j] = false;
		}
	}

	unsigned int maxMatchedLayers = 0;


	//iterate through the entire body of the chamber, we look for overlapping patterns
	//everywhere starting at the left most edge to the rightmost edge
	for(int x = -MAX_PATTERN_WIDTH+1; x < N_MAX_HALF_STRIPS; x++){

		bool matchedLayers[NLAYERS];
		for(int imlc = 0; imlc < NLAYERS; imlc++) matchedLayers[imlc] = false; //initialize

		//for each x position in the chamber, were going to iterate through
		// the pattern to see how much overlap there is at that position
		for(unsigned int y = 0; y < NLAYERS; y++) {

			unsigned int overlapColumn = 0;
			for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){



				//check if we have a 1 in our superpattern
				if(p.m_hits[px][y]){
					if(overlapColumn >= 3){ //we are considering only patterns which, for each row, should only have at most 3 true spots
						printf("Error, we have too many true booleans in a superpattern\n");
						return -1;
					}

					//this accounts for checking patterns along the edges of the chamber that may extend
					//past the bounds
					if( (int)x+(int)px < 0 ||  x+px >= N_MAX_HALF_STRIPS) {
						overlapColumn++;
						continue;
					}

					//check the overlap of the actual chamber distribution
					if(validComparatorTime(c.isComparator, (c.hits)[x+px][y])) {
						overlap[y][overlapColumn] = true;
						matchedLayers[y] = true;
					} else {
						overlap[y][overlapColumn] = false;
					}
					overlapColumn++; //we are now in the next column of
				}


			}
			if(overlapColumn != 3) {
				printf("Error: we don't have enough true booleans in a superpattern. overlapColumn = %i\n", overlapColumn);
				return -1;
			}
		}


		unsigned int matchedLayerCount = 0;
		for(int imlc = 0; imlc < NLAYERS; imlc++) matchedLayerCount += matchedLayers[imlc];


		//if we have a better match than we have had before
		if(matchedLayerCount > maxMatchedLayers) {
			maxMatchedLayers = matchedLayerCount;
			bestHoriontalPosition = x;
			for(unsigned int i=0; i < NLAYERS; i++){
				for(unsigned int j =0; j < 3; j++){
					bestOverlap[i][j] = overlap[i][j];
				}
			}
		}
		if(maxMatchedLayers == NLAYERS) {
			mi.addMatchInfo(bestOverlap,bestHoriontalPosition);
			return NLAYERS; //small optimization
		}
	}
	mi.addMatchInfo(bestOverlap,bestHoriontalPosition);
	return maxMatchedLayers;
}


//look for the best matched pattern, when we have a set of them, and fill the set match info
int searchForMatch(const ChamberHits &c, const vector<ChargeSuperPattern>* ps, SuperPatternSetMatchInfo *m){
	if(ps->size() > N_MAX_PATTERN_SET) {
		printf("ERROR: Pattern size too large\n");
		return -1;
	}

	//now we have all the rh for this segment, so check if the patterns are there
	for(unsigned int ip = 0; ip < ps->size(); ip++) {
		SingleSuperPatternMatchInfo *thisMatch = new SingleSuperPatternMatchInfo(ps->at(ip));
		if(containsPattern(c,ps->at(ip),*thisMatch) < 0) {
			printf("Error: pattern algorithm failed\n");
			return -1;
		}

		m->addSingleInfo(thisMatch);
	}

	if(DEBUG){
		printChamber(c);
		printSuperPattern(ps->at(m->bestSetIndex()));
		m->printBest3x6Pattern();
	}

	return 0;
}





vector<ChargeSuperPattern>* createGroup1Pattern(){

	vector<ChargeSuperPattern>* thisVector = new vector<ChargeSuperPattern>();


	ChargeSuperPattern id1("100",100,IDSV1_A);
	ChargeSuperPattern id2("200",200,IDSV1_B);
	ChargeSuperPattern id3 = id2.returnFlipped("300",300);
	ChargeSuperPattern id4("400",400,IDSV1_C);
	ChargeSuperPattern id5 = id4.returnFlipped("500",500);
	ChargeSuperPattern id6("600",600,IDSV1_D);
	ChargeSuperPattern id7 = id6.returnFlipped("700",700);
	ChargeSuperPattern id8("800",800, IDSV1_E);
	ChargeSuperPattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

vector<ChargeSuperPattern>* createGroup2Pattern(){

	vector<ChargeSuperPattern>* thisVector = new vector<ChargeSuperPattern>();


	ChargeSuperPattern id1("100",100,IDSV1_A);
	ChargeSuperPattern id2("200",200,IDSV1_B);
	ChargeSuperPattern id3 = id2.returnFlipped("300",300);
	ChargeSuperPattern id4("400",400,IDSV1_C);
	ChargeSuperPattern id5 = id4.returnFlipped("500",500);
	ChargeSuperPattern id6("600",600,IDSV1_D);
	ChargeSuperPattern id7 = id6.returnFlipped("700",700);
	ChargeSuperPattern id8("800",800, IDSV1_E);
	ChargeSuperPattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

vector<ChargeSuperPattern>* createGroup3Pattern(){

	vector<ChargeSuperPattern>* thisVector = new vector<ChargeSuperPattern>();


	ChargeSuperPattern id1("100",100,IDSV1_A);
	ChargeSuperPattern id2("200",200,IDSV1_B);
	ChargeSuperPattern id3 = id2.returnFlipped("300",300);
	ChargeSuperPattern id4("400",400,IDSV1_C);
	ChargeSuperPattern id5 = id4.returnFlipped("500",500);
	ChargeSuperPattern id6("600",600,IDSV1_D);
	ChargeSuperPattern id7 = id6.returnFlipped("700",700);
	ChargeSuperPattern id8("800",800, IDSV1_E);
	ChargeSuperPattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

vector<ChargeSuperPattern>* createGroup4Pattern(){

	vector<ChargeSuperPattern>* thisVector = new vector<ChargeSuperPattern>();


	ChargeSuperPattern id1("100",100,IDSV1_A);
	ChargeSuperPattern id2("200",200,IDSV1_B);
	ChargeSuperPattern id3 = id2.returnFlipped("300",300);
	ChargeSuperPattern id4("400",400,IDSV1_C);
	ChargeSuperPattern id5 = id4.returnFlipped("500",500);
	ChargeSuperPattern id6("600",600,IDSV1_D);
	ChargeSuperPattern id7 = id6.returnFlipped("700",700);
	ChargeSuperPattern id8("800",800, IDSV1_E);
	ChargeSuperPattern id9 = id8.returnFlipped("900",900);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}


int chamberSerial( int ec, int st, int ri, int ch ) {

    int kSerial = ch;
    if (st == 1 && ri == 1) kSerial = ch;
    if (st == 1 && ri == 2) kSerial = ch + 36;
    if (st == 1 && ri == 3) kSerial = ch + 72;
    if (st == 1 && ri == 4) kSerial = ch;
    if (st == 2 && ri == 1) kSerial = ch + 108;
    if (st == 2 && ri == 2) kSerial = ch + 126;
    if (st == 3 && ri == 1) kSerial = ch + 162;
    if (st == 3 && ri == 2) kSerial = ch + 180;
    if (st == 4 && ri == 1) kSerial = ch + 216;
    if (st == 4 && ri == 2) kSerial = ch + 234;  // one day...
    if (ec == 2) kSerial = kSerial + 300;
    //cout << endl << endl << "Endcap: " << ec << "  Station: " << st << "  Ring: " << ri << "  Chamber: " << ch << "  serialID: " << kSerial << endl;
    return kSerial;
}




#endif /* PATTERNFINDERHELPERFUNCTIONS_H_ */
