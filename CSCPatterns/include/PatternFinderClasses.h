/*
 * PatternFitter.h
 *
 *  Created on: Oct 9, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFITTER_H_
#define PATTERNFITTER_H_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <bitset>

#include "PatternConstants.h"

using namespace std;

//need this for ChamberHits, sorry for poor organization...
bool validComparatorTime(const int time, const int startTimeWindow) {
	//numbers start at 1, so time bins really go 1-16 here
	for(int validTime = startTimeWindow; validTime < startTimeWindow+TIME_CAPTURE_WINDOW; validTime++){
		if(time < validTime) return false; //speed up zero case
		if(time == validTime) return true;
	}
	return false;
}

//this class tells us what locations within a pattern with id "m_EnvelopeId" matches
// each of the true booleans within the superchargepattern.
class ChargeComparatorCode {
public:
	unsigned int getLayersMatched() const {return m_layersMatched;}
	int getComparatorCodeId() const {return m_ComparatorCodeId;} //hexidecimal code used to identify each possible arrangement of the charge in the ComparatorCode
	void printPattern();

	ChargeComparatorCode(bool pat[NLAYERS][3]) {
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = pat[i][j];
			}
		}
		//only calculate them once, to keep things efficient
		calculateId();
		calculateLayersMatched();
	};

	ChargeComparatorCode(const ChargeComparatorCode& c){
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = c.m_hits[i][j];
			}
		}
		m_ComparatorCodeId = c.m_ComparatorCodeId;
		m_layersMatched = c.m_layersMatched;

	}
	ChargeComparatorCode(){
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = 0;
			}
		}
		//only calculate them once, to keep things efficient
		calculateId();
		calculateLayersMatched();
	}

	~ChargeComparatorCode() {}

	bool m_hits[NLAYERS][3]; //all the hits within a Envelope that a chamber scan matched


private:
	int m_ComparatorCodeId;
	int m_layersMatched;

	void calculateId();
	void calculateLayersMatched();
};

void ChargeComparatorCode::printPattern() {
	int patCode = getComparatorCodeId();
	printf("Comparator Code: %#04x, layersMatched: %u\n", patCode,getLayersMatched());
	if(DEBUG > 0) cout << "ComparatorCode is: "  << bitset<12>(patCode) << endl;
	for(int i =0; i < NLAYERS; i++){
		for(int j =0; j < 3; j++){
			printf("%i", m_hits[i][j]);

		}
		printf("\n");
	}
}

void ChargeComparatorCode::calculateId(){
	//only do this iteration once, to keep things efficient
	m_ComparatorCodeId = 0;
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
			if(DEBUG >= 0) std::cout << "Error: unknown rowPattern - " << std::bitset<3>(rowPat) << " defaulting to rowCode: 0" << std::endl;
			m_ComparatorCodeId = -1;
			return;
			//rowCode = 0; //if we come up with a robust enough algorithm, we can do this eventually
			//break;
		}
		//each column has two bits of information, largest layer is most significant bit
		m_ComparatorCodeId += (rowCode << 2*column);
	}
}

void ChargeComparatorCode::calculateLayersMatched(){
	m_layersMatched = 0;
	for(int i = 0; i < NLAYERS; i++){
		bool matched = false;
		for(int j = 0; j < 3; j++){
			if(m_hits[i][j]) matched = 1;
		}
		m_layersMatched += matched;
	}
}


// ChargeEnvelope are scanned over the entire chamber, this class is
// effectively just a small matrix, each of which (if we have a vector) represent
// a broadly encompasing general pattern which gives us preliminary information on a pattern.

class ChargeEnvelope {
public:

	const unsigned int m_id;
	const bool m_isLegacy;
	bool m_hits[MAX_PATTERN_WIDTH][NLAYERS]; //layers

	ChargeEnvelope(unsigned int id, bool isLegacy, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id), m_isLegacy(isLegacy) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
	}
	ChargeEnvelope(string name, unsigned int id, bool isLegacy, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id), m_isLegacy(isLegacy){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
		m_name = name;
	}

	ChargeEnvelope(const ChargeEnvelope &obj) : m_id(obj.m_id), m_isLegacy(obj.m_isLegacy) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = obj.m_hits[j][i];
			}
		}
		m_name = obj.m_name;
	}


	ChargeEnvelope() :m_id(-1), m_isLegacy(false){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = 0;
			}
		}
		m_name = "";
	}

	~ChargeEnvelope() {}
	ChargeEnvelope returnFlipped(unsigned int id){
		return returnFlipped("", id);
	}
	ChargeEnvelope returnFlipped(string name, unsigned int id){
		bool flippedPattern[MAX_PATTERN_WIDTH][NLAYERS];
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < MAX_PATTERN_WIDTH; j++){
				flippedPattern[j][i] = m_hits[MAX_PATTERN_WIDTH-1-j][i];
			}
		}
		return ChargeEnvelope(name, id, m_isLegacy, flippedPattern);
	}

	void printCode(int code);

	string name() {
		if(m_name.size()) return m_name;
		string name = "";
		for(unsigned int i = 0; i< MAX_PATTERN_WIDTH; i++){
			int trueCount = 0;
			for(unsigned int j = 0; j <NLAYERS; j++){
				if(m_hits[i][j]) trueCount++;
			}
			if(trueCount < 10) name += to_string(trueCount);
			else return "ERROR";
		}
		return name;
	}

private:
	string m_name;
};

//given a pattern code, prints out how it looks within a super pattern
void ChargeEnvelope::printCode(int code){
	printf("For Envelope: %i - printing code: %i (layer 1->6)\n", m_id,code);
	if(code >= 4096) {//2^12
		printf("Error: invalid pattern code\n");
		return;
	}

	//iterator 1
	int it = 1;

	for(int j=0; j < NLAYERS; j++){
		//0,1,2 or 3
		int layerPattern = (code & (it | it << 1))/it;
		if(layerPattern <0 || layerPattern > 3){
			printf("Error: invalid code\n");
			return;
		}
		int trueCounter = 3;//for each layer, should only have 3
		for(int i =0; i < MAX_PATTERN_WIDTH; i++){
			if(!m_hits[i][j]){
				printf("0");
			}else{
				if(trueCounter == layerPattern) printf("1");
				else printf("0");
				trueCounter--;
			}
		}
		it = it << 2; //bitshift the iterator to look at the next part of the code
		printf("\n");
	}
}

class SingleEnvelopeMatchInfo {
public:

	SingleEnvelopeMatchInfo(ChargeEnvelope p, int horInd, int startTime,
			bool overlap[NLAYERS][3]):
		m_Envelope(p), m_horizontalIndex(horInd), m_startTime(startTime){
		m_overlap = new ChargeComparatorCode(overlap);
		m_layerMatchCount = m_overlap->getLayersMatched();
	}

	SingleEnvelopeMatchInfo(ChargeEnvelope p, int horInd, int startTime,
			int layMatCount) : m_Envelope(p), m_horizontalIndex(horInd), m_startTime(startTime){
		m_overlap = 0;
		m_layerMatchCount = layMatCount;
	}

	void print3x6Pattern();
	void printPatternInChamber();

	~SingleEnvelopeMatchInfo() {
		if(m_overlap) delete m_overlap;
	}

	int envelopeId() {return m_Envelope.m_id;}

	//center position of the track [strips]
	float x(){return (1.*m_horizontalIndex + 0.5*(MAX_PATTERN_WIDTH - 1))/2.;}
	int comparatorCodeId();
	int layMatCount();
	const ChargeComparatorCode chargeComparatorCode() {return *m_overlap;}
	const ChargeEnvelope m_Envelope;
	const int m_horizontalIndex; //half strips, leftmost index of the pattern
	const int m_startTime;

private:

	ChargeComparatorCode* m_overlap;
	int m_layerMatchCount;

};

void SingleEnvelopeMatchInfo::print3x6Pattern(){
	if(m_overlap) m_overlap->printPattern();
	else {
		printf("Layers Match = %i\n", m_layerMatchCount);
		printf("Overlap pattern not available for this set match\n");
	}
}

void SingleEnvelopeMatchInfo::printPatternInChamber(){
	if(!m_overlap) return;
	printf("Horizontal index (from left) is %i half strips, position is %f\n", m_horizontalIndex, x());
	for(int j=0; j < NLAYERS; j++){
		int trueCounter = 0;//for each layer, should only have 3
		for(int i =0; i < MAX_PATTERN_WIDTH; i++){
			if(!m_Envelope.m_hits[i][j]){
				printf("0");
			}else{
				if(m_overlap->m_hits[j][trueCounter]) printf("1");
				else printf("0");
				trueCounter++;
			}
		}
		printf("\n");
	}
}

int SingleEnvelopeMatchInfo::layMatCount() {
	return m_layerMatchCount;
}

int SingleEnvelopeMatchInfo::comparatorCodeId(){
	if(m_overlap){
		return m_overlap->getComparatorCodeId();
	} else {
		return -1;
	}
}



struct ChamberHits {
	bool isComparator;
	unsigned int station;
	unsigned int ring;
	unsigned int endcap;
	unsigned int chamber;
	int hits[N_MAX_HALF_STRIPS][NLAYERS];

	ChamberHits(bool isComparator_, unsigned int station_, unsigned int ring_,
			unsigned int endcap_, unsigned int chamber_) {
		isComparator = isComparator_;
		station = station_;
		ring = ring_;
		endcap = endcap_;
		chamber = chamber_;
		for(unsigned int i =0; i < N_MAX_HALF_STRIPS; i++){
			for(unsigned int j = 0; j < NLAYERS; j++){
				hits[i][j] = 0;
			}
		}
	}

	ChamberHits(const ChamberHits& c) {
		isComparator = c.isComparator;
		station = c.station;
		ring = c.ring;
		endcap = c.endcap;
		chamber = c.chamber;
		for(unsigned int i =0; i < N_MAX_HALF_STRIPS; i++){
			for(unsigned int j = 0; j < NLAYERS; j++){
				hits[i][j] = c.hits[i][j];
			}
		}
	}


	struct ChamberHits& operator-=(const SingleEnvelopeMatchInfo& mi) {
		const ChargeEnvelope p = mi.m_Envelope;
		int horPos = mi.m_horizontalIndex;
		int startTimeWindow = mi.m_startTime;


		for(unsigned int y = 0; y < NLAYERS; y++) {
			for(unsigned int px = 0; px < MAX_PATTERN_WIDTH; px++){

				//check if we have a 1 in our Envelope
				if(p.m_hits[px][y]){

					//this accounts for checking patterns along the edges of the chamber that may extend
					//past the bounds
					if( horPos+(int)px < 0 ||  horPos+px >= N_MAX_HALF_STRIPS) {
						continue;
					}
					// if there is an overlap, erase the one in the chamber
					if(validComparatorTime(hits[horPos+px][y], startTimeWindow)) {
						hits[horPos+px][y] = 0;
					}
				}
			}
		}
		return *this;
	}

};


#endif /* PATTERNFITTER_H_ */
