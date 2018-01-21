/*
 * PatternFinderHelperFunctions.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFINDERHELPERFUNCTIONS_H_
#define PATTERNFINDERHELPERFUNCTIONS_H_

#include "PatternFinderClasses.h"

void printEnvelope(const ChargeEnvelope &p) {
	printf("-- Printing Envelope: %i ---\n", p.m_id);
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

int getOverlap(const ChamberHits &c, const ChargeEnvelope &p, const int horPos, bool overlap[NLAYERS][3]){
	//for each x position in the chamber, were going to iterate through
	// the pattern to see how much overlap there is at that position
	for(unsigned int y = 0; y < NLAYERS; y++) {

		unsigned int overlapColumn = 0;
		for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){

			//check if we have a 1 in our Envelope
			if(p.m_hits[px][y]){
				if(overlapColumn >= 3){ //we are considering only patterns which, for each row, should only have at most 3 true spots
					printf("Error, we have too many true booleans in a Envelope\n");
					return -1;
				}

				//this accounts for checking patterns along the edges of the chamber that may extend
				//past the bounds
				if( horPos+(int)px < 0 ||  horPos+px >= N_MAX_HALF_STRIPS) {
					overlap[y][overlapColumn] = false;
					overlapColumn++;
					continue;
				}

				//check the overlap of the actual chamber distribution
				if(validComparatorTime(c.isComparator, (c.hits)[horPos+px][y])) {
					overlap[y][overlapColumn] = true;
				} else {
					overlap[y][overlapColumn] = false;
				}
				overlapColumn++; //we are now in the next column of overlap
			}
		}
		if(overlapColumn != 3) {
			printf("Error: we don't have enough true booleans in a Envelope. overlapColumn = %i\n", overlapColumn);
			return -1;
		}
	}
	return 0;
}



//looks if a chamber "c" contains an envelope "p" at the location horPos returns
//the number of matched layers
int legacyLayersMatched(const ChamberHits &c, const ChargeEnvelope &p, const int horPos){

	bool matchedLayers[NLAYERS];
	for(int imlc = 0; imlc < NLAYERS; imlc++) matchedLayers[imlc] = false; //initialize

	//for each x position in the chamber, were going to iterate through
	// the pattern to see how much overlap there is at that position
	for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){
		//go through the entire vertical dimension as well
		for(unsigned int y = 0; y < NLAYERS; y++) {

			//this accounts for checking patterns along the edges of the chamber that may extend
			//past the bounds
			if( (int)horPos+(int)px < 0 ||  horPos+px >= N_MAX_HALF_STRIPS) continue;
			if((c.hits)[horPos+px][y] && p.m_hits[px][y]) {
				matchedLayers[y] = true;
			}
		}
	}

	unsigned int matchedLayerCount = 0;
	for(int imlc = 0; imlc < NLAYERS; imlc++) matchedLayerCount += matchedLayers[imlc];

	return matchedLayerCount;
}

//looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers if ,
// run successfully, match info is stored in variable mi
int containsPattern(const ChamberHits &c, const ChargeEnvelope &p,  SingleEnvelopeMatchInfo *&mi){

	//overlap between tested super pattern and chamber hits
	bool overlap [NLAYERS][3];
	int bestHorizontalIndex = 0;
	for(unsigned int i=0; i < NLAYERS; i++){
		for(unsigned int j =0; j < 3; j++){
			overlap[i][j] = false; //initialize all as false
		}
	}

	unsigned int maxMatchedLayers = 0;

	//iterate through the entire body of the chamber, we look for overlapping patterns
	//everywhere starting at the left most edge to the rightmost edge
	for(int x = -MAX_PATTERN_WIDTH+1; x < N_MAX_HALF_STRIPS; x++){

		unsigned int matchedLayerCount = 0;

		//legacy code doesnt use overlaps, so we have a slightly different algorithm
		if(p.m_isLegacy){
			matchedLayerCount = legacyLayersMatched(c,p,x);
		} else {
			if(getOverlap(c,p,x,overlap)){
				printf("Error: cannot get overlap for pattern\n");
				return -1;
			}

			for(int ilay = 0; ilay < NLAYERS; ilay++) {
				bool inLayer = false;
				for(int icol = 0; icol< 3; icol++){
					inLayer |= overlap[ilay][icol]; //bitwise or
				}
				matchedLayerCount += inLayer;
			}
		}



		//if we have a better match than we have had before
		if(matchedLayerCount == NLAYERS){ //optimization
			if(p.m_isLegacy){
				mi = new SingleEnvelopeMatchInfo(p,x,matchedLayerCount);
			}else{
				mi = new SingleEnvelopeMatchInfo(p, x, overlap);
				if(mi->patternId() < 0) return -1;
			}
			return matchedLayerCount;
		}
		if(matchedLayerCount > maxMatchedLayers) {
			maxMatchedLayers = matchedLayerCount;
			bestHorizontalIndex = x;
		}

	}

	//refill the overlap with the best found location
	if(!p.m_isLegacy && getOverlap(c,p,bestHorizontalIndex,overlap)){
		printf("Error: cannot get overlap for pattern\n");
		return -1;
	}

	if(p.m_isLegacy){
		mi = new SingleEnvelopeMatchInfo(p, bestHorizontalIndex, maxMatchedLayers);
	}else {
		mi = new SingleEnvelopeMatchInfo(p, bestHorizontalIndex, overlap);
		if(mi->patternId() < 0) return -1;
	}
	if(DEBUG > 1){
		printChamber(c);
		printEnvelope(p);
		mi->print3x6Pattern();
	}
	return maxMatchedLayers;
}


//look for the best matched pattern, when we have a set of them, and fill the set match info
int searchForMatch(const ChamberHits &c, const vector<ChargeEnvelope>* ps, EnvelopeSetMatchInfo *m){

	//now we have all the rh for this segment, so check if the patterns are there
	for(unsigned int ip = 0; ip < ps->size(); ip++) {
		SingleEnvelopeMatchInfo *thisMatch = 0;
		if(containsPattern(c,ps->at(ip),thisMatch) < 0) {
			printf("Error: pattern algorithm failed\n");
			printChamber(c);
			return -1;
		}
		m->addSingleInfo(thisMatch);
	}

	if(DEBUG){
		printf("~~~~ BEST MATCH ~~~\n");
		printChamber(c);
		printEnvelope(ps->at(m->bestSetIndex()));
		m->printBest3x6Pattern();
	}

	return 0;
}




//creates the new set of envelopes
vector<ChargeEnvelope>* createNewEnvelopes(){

	vector<ChargeEnvelope>* thisVector = new vector<ChargeEnvelope>();

	ChargeEnvelope id1("100",ENVELOPE_IDS[0],false,IDSV1_A);
	ChargeEnvelope id4("400",ENVELOPE_IDS[1],false,IDSV1_C);
	ChargeEnvelope id5 = id4.returnFlipped("500",ENVELOPE_IDS[2]);
	ChargeEnvelope id8("800",ENVELOPE_IDS[3], false, IDSV1_E);
	ChargeEnvelope id9 = id8.returnFlipped("900",ENVELOPE_IDS[4]);

	thisVector->push_back(id1);
	thisVector->push_back(id4);
	thisVector->push_back(id5);
	thisVector->push_back(id8);
	thisVector->push_back(id9);

	return thisVector;
}

//creates the currently implemented patterns in the TMB, here treated as envelopes
vector<ChargeEnvelope>* createOldEnvelopes(){
	vector<ChargeEnvelope>* thisVector = new vector<ChargeEnvelope>();

	//fill in the correctly oriented matrices, should change eventually...
	for(unsigned int x = 0; x < MAX_PATTERN_WIDTH; x++){
		for(unsigned int y = 0; y< NLAYERS; y++){
			ID2_BASE[x][y] = id2Bools[NLAYERS-1-y][x];
			ID3_BASE[MAX_PATTERN_WIDTH-x-1][y] =id2Bools[NLAYERS-1-y][x];
			ID4_BASE[x][y] = id4Bools[NLAYERS-1-y][x];
			ID5_BASE[MAX_PATTERN_WIDTH-x-1][y] =id4Bools[NLAYERS-1-y][x];
			ID6_BASE[x][y] = id6Bools[NLAYERS-1-y][x];
			ID7_BASE[MAX_PATTERN_WIDTH-x-1][y] =id6Bools[NLAYERS-1-y][x];
			ID8_BASE[x][y] = id8Bools[NLAYERS-1-y][x];
			ID9_BASE[MAX_PATTERN_WIDTH-x-1][y] =id8Bools[NLAYERS-1-y][x];
			IDA_BASE[x][y] = idABools[NLAYERS-1-y][x];
		}
	}


	ChargeEnvelope id2("ID2",2,true, ID2_BASE);
	ChargeEnvelope id3("ID3",3,true,ID3_BASE);
	ChargeEnvelope id4("ID4",4,true,ID4_BASE);
	ChargeEnvelope id5("ID5",5,true,ID5_BASE);
	ChargeEnvelope id6("ID6",6,true,ID6_BASE);
	ChargeEnvelope id7("ID7",7,true,ID7_BASE);
	ChargeEnvelope id8("ID8",8,true,ID8_BASE);
	ChargeEnvelope id9("ID9",9,true,ID9_BASE);
	ChargeEnvelope idA("IDA",10,true,IDA_BASE);

	thisVector->push_back(idA);
	thisVector->push_back(id9);
	thisVector->push_back(id8);
	thisVector->push_back(id7);
	thisVector->push_back(id6);
	thisVector->push_back(id5);
	thisVector->push_back(id4);
	thisVector->push_back(id3);
	thisVector->push_back(id2);

	//ChargeEnvelope id1("100",ENVELOPE_IDS[0],IDSV1_A);

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

    return kSerial;
}





#endif /* PATTERNFINDERHELPERFUNCTIONS_H_ */
