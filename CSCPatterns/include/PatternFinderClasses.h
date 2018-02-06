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


struct ChamberHits {
	bool isComparator;
	unsigned int station;
	unsigned int ring;
	unsigned int endcap;
	int hits[N_MAX_HALF_STRIPS][NLAYERS];
};

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
	if(DEBUG) cout << "ComparatorCode is: "  << bitset<12>(patCode) << endl;
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
			printf("Error: unknown rowPattern - %i\n", rowPat);
			m_ComparatorCodeId = -1;
			return;
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

//keeps track of the number of occurences of an individual pattern ID
class ComparatorCodeCount{
public:
	ComparatorCodeCount(ChargeComparatorCode cp) : m_cp(cp) { isCentered = false;}

	void addPair(pair<float,float> positionSlope){m_positionSlopes.push_back(positionSlope);}
	void center();
	unsigned int count() {return m_positionSlopes.size();}
    float getMeanPos()
    {
        if(!isCentered) center();
        return mean_positionSlopes.first;
    }
	int id() { return m_cp.getComparatorCodeId();}
	int nLayers() { return m_cp.getLayersMatched();} //amount of layers the pattern matches to

	const vector<pair<float,float>>& getPosSlope() { return m_positionSlopes;}

	~ComparatorCodeCount(){}
private:
	//vector of all positions [strips] (first element in pair) and angles [strips/layer](second element)
	//of the segment associated with this pattern.
	vector<pair<float, float>> m_positionSlopes;
	pair<float, float> mean_positionSlopes;
    bool isCentered;

	const ChargeComparatorCode m_cp;
};

//shifts the location of each position and slope to average to zero
// do this only when you have finished adding everything together!
void ComparatorCodeCount::center(){
	float totalX = 0;
	float totalY = 0;
	for(unsigned int i =0; i < m_positionSlopes.size();i++){
		totalX += m_positionSlopes.at(i).first;
		totalY += m_positionSlopes.at(i).second;
	}
	mean_positionSlopes.first  = totalX / m_positionSlopes.size();
	mean_positionSlopes.second = totalY / m_positionSlopes.size();

	for(unsigned int i =0; i < m_positionSlopes.size(); i++){
		m_positionSlopes.at(i).first -= mean_positionSlopes.first;
		m_positionSlopes.at(i).second -= mean_positionSlopes.second;
	}
    isCentered = true;

}






//keeps track of the occurences of a collective of pattern ids
class ComparatorCodeList {
public:
	ComparatorCodeList(unsigned int envelopeId): m_envelopeId(envelopeId){}

	void addComparatorCode(const ChargeComparatorCode p,pair<float,float> positionSlope);
	void printList();
	void removeComparatorCodesUnder(unsigned int num);
	unsigned int totalCount();
	const vector<ComparatorCodeCount*>& getIds(){return m_ComparatorCodes;}
	void center(ofstream& f);

	~ComparatorCodeList(){
		while(m_ComparatorCodes.size()) {
			delete m_ComparatorCodes.back();
			m_ComparatorCodes.pop_back();
		}
	}

private:
	const unsigned int m_envelopeId;
	vector<ComparatorCodeCount*> m_ComparatorCodes;
};

//returns the integral number of ComparatorCodes put into this list
unsigned int ComparatorCodeList::totalCount() {
	unsigned int tc = 0;
	for(unsigned int i =0; i < m_ComparatorCodes.size(); i++)	tc += m_ComparatorCodes[i]->count();
	return tc;
}

//removes all ComparatorCodes with a count less than <num>
void ComparatorCodeList::removeComparatorCodesUnder(unsigned int num){
	for(unsigned int i =0; i < m_ComparatorCodes.size(); i++){
		if(m_ComparatorCodes[i]->count() < num){
			m_ComparatorCodes.erase(m_ComparatorCodes.begin()+i,m_ComparatorCodes.end());
			return;
		}
	}
}

//adds a ComparatorCode to the list
void ComparatorCodeList::addComparatorCode(const ChargeComparatorCode p, pair<float,float> positionSlope) {
	for(unsigned int i = 0; i < m_ComparatorCodes.size(); i++){
		//look if we have it already
		if(m_ComparatorCodes.at(i)->id() == p.getComparatorCodeId()){
			m_ComparatorCodes.at(i)->addPair(positionSlope);
			//sort as they come in
			int shift = 0;
			while(i-shift && m_ComparatorCodes.at(i-shift)->count() > m_ComparatorCodes.at(i-shift-1)->count()){
				ComparatorCodeCount* temp = m_ComparatorCodes.at(i-shift);
				m_ComparatorCodes.at(i-shift) = m_ComparatorCodes.at(i-shift-1);
				m_ComparatorCodes.at(i-shift-1) = temp;
				shift++;
			}
			return;
		}
	}
	ComparatorCodeCount* thisId = new ComparatorCodeCount(p);
	thisId->addPair(positionSlope);
	m_ComparatorCodes.push_back(thisId);
}

//prints all of the ComparatorCodes put into the list
void ComparatorCodeList::printList() {
	printf("Id\tId(b4)\tCount\tIntegral\tLayers\n");
	unsigned int integralCount = 0;
	unsigned int size = m_ComparatorCodes.size();

	cout.precision(3);
	for(unsigned int i =0; i < size; i++){
		int thisCount = m_ComparatorCodes[i]->count();
		integralCount += thisCount;
		cout << m_ComparatorCodes[i]->id() << "\t";
		int id = m_ComparatorCodes[i]->id();
		int layers = NLAYERS;
		while (layers--){
			cout << (id % 4); // print in base 4
			id /= 4;
		}
		cout << "\t" << thisCount << "\t"
				<< 1.*integralCount/totalCount() << "\t"
				<< m_ComparatorCodes[i]->nLayers() << endl;

	}
	printf("Total chambers matched: %i\n", totalCount());
}

//zeroes the distribution, be careful when to use this! writes average to output file
void ComparatorCodeList::center(ofstream& f) {
	for(unsigned int i =0; i< m_ComparatorCodes.size(); i++){
		m_ComparatorCodes.at(i)->center();
		f << m_envelopeId << "\t" << m_ComparatorCodes.at(i)->id() << "\t" << m_ComparatorCodes.at(i)->count() << "\t" << m_ComparatorCodes.at(i)->getMeanPos() << endl;
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

	SingleEnvelopeMatchInfo(ChargeEnvelope p, int horInd, bool overlap[NLAYERS][3]):
		m_Envelope(p){
		m_horizontalIndex = horInd;
		//m_layerMatchCount = -1;
		m_overlap = new ChargeComparatorCode(overlap);
		m_layerMatchCount = m_overlap->getLayersMatched();
	}

	SingleEnvelopeMatchInfo(ChargeEnvelope p, int horInd, int layMatCount) : m_Envelope(p){
		m_horizontalIndex = horInd;
		m_overlap = 0;
		m_layerMatchCount = layMatCount;
	}

	void print3x6Pattern();
	void printPatternInChamber();

	~SingleEnvelopeMatchInfo() {
		if(m_overlap) delete m_overlap;
	}

	int envelopeId() {return m_Envelope.m_id;}

	//center position of the track
	float x(){return 1.*m_horizontalIndex + 0.5*(MAX_PATTERN_WIDTH - 1);}
	int ComparatorCodeId();
	int layMatCount();
	const ChargeComparatorCode chargeComparatorCode() {return *m_overlap;}


private:

	ChargeComparatorCode* m_overlap;
	const ChargeEnvelope m_Envelope;
	int m_horizontalIndex; //half strips, leftmost index of the pattern
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

int SingleEnvelopeMatchInfo::ComparatorCodeId(){
	if(m_overlap){
		return m_overlap->getComparatorCodeId();
	} else {
		return -1;
	}
}

class EnvelopeSetMatchInfo {
public:

	EnvelopeSetMatchInfo() {
		m_bestSetMatchIndex = 0;
		m_matches = new vector<SingleEnvelopeMatchInfo*>();
	}
	int bestLayerCount();
	int bestEnvelopeId();
	const ChargeComparatorCode bestChargeComparatorCode();
	float bestX(); //best hs position (from 0)
	int bestSetIndex() {return m_bestSetMatchIndex;}
	void printBest3x6Pattern();
	void addSingleInfo(SingleEnvelopeMatchInfo* smi);
	~EnvelopeSetMatchInfo() {
		while(m_matches->size()) {
			delete m_matches->back();
			m_matches->pop_back();
		}
		delete m_matches;
	}
private:

	//index in the m_matches vector that corresponds
	//to the best super pattern match
	int m_bestSetMatchIndex;

	vector<SingleEnvelopeMatchInfo*>* m_matches;

};

//prints the overlap between the best super pattern and the chamber
void EnvelopeSetMatchInfo::printBest3x6Pattern(){
	if(m_matches->size()) {
		m_matches->at(m_bestSetMatchIndex)->print3x6Pattern();
		m_matches->at(m_bestSetMatchIndex)->printPatternInChamber();
	}
}

const ChargeComparatorCode EnvelopeSetMatchInfo::bestChargeComparatorCode() {
	if(m_bestSetMatchIndex >=(int)m_matches->size()) return ChargeComparatorCode();
	return m_matches->at(m_bestSetMatchIndex)->chargeComparatorCode();
}

int EnvelopeSetMatchInfo::bestLayerCount(){
	if(m_bestSetMatchIndex >=(int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->layMatCount();
}

int EnvelopeSetMatchInfo::bestEnvelopeId() {
	if(m_bestSetMatchIndex >= (int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->envelopeId();
}

float EnvelopeSetMatchInfo::bestX() {
	if(m_bestSetMatchIndex >= (int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->x();
}

void EnvelopeSetMatchInfo::addSingleInfo(SingleEnvelopeMatchInfo* smi) {
	if(smi->layMatCount() > bestLayerCount()){
		m_bestSetMatchIndex = m_matches->size();
	}
	m_matches->push_back(smi);
}






#endif /* PATTERNFITTER_H_ */
