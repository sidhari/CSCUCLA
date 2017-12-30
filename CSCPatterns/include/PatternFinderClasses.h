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
class ChargePattern {
public:
	unsigned int getLayersMatched() const {return m_layersMatched;}
	int getPatternId() const {return m_patternId;} //hexidecimal code used to identify each possible arrangement of the charge in the pattern
	void printPattern();

	ChargePattern(bool pat[NLAYERS][3]) {
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = pat[i][j];
			}
		}
		//only calculate them once, to keep things efficient
		calculateId();
		calculateLayersMatched();
	};

	ChargePattern(const ChargePattern& c){
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = c.m_hits[i][j];
			}
		}
		m_patternId = c.m_patternId;
		m_layersMatched = c.m_layersMatched;

	}
	ChargePattern(){
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < 3; j++){
				m_hits[i][j] = 0;
			}
		}
		//only calculate them once, to keep things efficient
		calculateId();
		calculateLayersMatched();
	}

	~ChargePattern() {}

	bool m_hits[NLAYERS][3]; //all the hits within a Envelope that a chamber scan matched


private:
	int m_patternId;
	int m_layersMatched;

	void calculateId();
	void calculateLayersMatched();
};

void ChargePattern::printPattern() {
	int patCode = getPatternId();
	printf("Pattern Code: %i, layersMatched: %u\n", patCode,getLayersMatched());
	if(DEBUG) cout << "patternId is: "  << bitset<12>(patCode) << endl;
	for(int i =0; i < NLAYERS; i++){
		for(int j =0; j < 3; j++){
			printf("%i", m_hits[i][j]);

		}
		printf("\n");
	}
}

void ChargePattern::calculateId(){
	//only do this iteration once, to keep things efficient
	m_patternId = 0;
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
			m_patternId = -1;
			return;
		}
		//each column has two bits of information
		m_patternId += (rowCode << 2*column);
	}
	if(DEBUG) cout << "patternId is: "  << bitset<12>(m_patternId) << endl;
}

void ChargePattern::calculateLayersMatched(){
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
class PatternCount{
public:
	PatternCount(ChargePattern cp) : m_cp(cp) { isCentered = false;}

	void addPair(pair<float,float> positionSlope){m_positionSlopes.push_back(positionSlope);}
	void center();
	unsigned int count() {return m_positionSlopes.size();}
    float getMeanPos()
    {
        if(!isCentered) center();
        return mean_positionSlopes.first;
    }
	int id() { return m_cp.getPatternId();}
	int nLayers() { return m_cp.getLayersMatched();} //amount of layers the pattern matches to

	const vector<pair<float,float>>& getPosSlope() { return m_positionSlopes;}

	~PatternCount(){}
private:
	//vector of all positions [strips] (first element in pair) and angles [strips/layer](second element)
	//of the segment associated with this pattern.
	vector<pair<float, float>> m_positionSlopes;
	pair<float, float> mean_positionSlopes;
    bool isCentered;

	const ChargePattern m_cp;
};

//shifts the location of each position and slope to average to zero
// do this only when you have finished adding everything together!
void PatternCount::center(){
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
class PatternList {
public:
	PatternList(unsigned int envelopeId): m_envelopeId(envelopeId){}

	void addPattern(const ChargePattern p,pair<float,float> positionSlope);
	void printList();
	void removePatternsUnder(unsigned int num);
	unsigned int totalCount();
	const vector<PatternCount*>& getIds(){return m_patterns;}
	void center(ofstream& f);

	~PatternList(){
		while(m_patterns.size()) {
			delete m_patterns.back();
			m_patterns.pop_back();
		}
	}

private:
	const unsigned int m_envelopeId;
	vector<PatternCount*> m_patterns;
};

//returns the integral number of patterns put into this list
unsigned int PatternList::totalCount() {
	unsigned int tc = 0;
	for(unsigned int i =0; i < m_patterns.size(); i++){
		tc += m_patterns[i]->count();
	}
	return tc;
}

//removes all patterns with a count less than <num>
void PatternList::removePatternsUnder(unsigned int num){
	for(unsigned int i =0; i < m_patterns.size(); i++){
		if(m_patterns[i]->count() < num){
			m_patterns.erase(m_patterns.begin()+i,m_patterns.end());
			return;
		}
	}
}

//adds a pattern to the list
void PatternList::addPattern(const ChargePattern p, pair<float,float> positionSlope) {
	for(unsigned int i = 0; i < m_patterns.size(); i++){
		//look if we have it already
		if(m_patterns.at(i)->id() == p.getPatternId()){
			m_patterns.at(i)->addPair(positionSlope);
			//sort as they come in
			int shift = 0;
			while(i-shift && m_patterns.at(i-shift)->count() > m_patterns.at(i-shift-1)->count()){
				PatternCount* temp = m_patterns.at(i-shift);
				m_patterns.at(i-shift) = m_patterns.at(i-shift-1);
				m_patterns.at(i-shift-1) = temp;
				shift++;
			}
			return;
		}
	}
	PatternCount* thisId = new PatternCount(p);
	thisId->addPair(positionSlope);
	m_patterns.push_back(thisId);
}

//prints all of the patterns put into the list
void PatternList::printList() {
	printf("Id\tId(b4)\tCount\tIntegral\tLayers\n");
	unsigned int integralCount = 0;
	unsigned int size = m_patterns.size();

	cout.precision(3);
	for(unsigned int i =0; i < size; i++){
		int thisCount = m_patterns[i]->count();
		integralCount += thisCount;
		cout << m_patterns[i]->id() << "\t";
		int id = m_patterns[i]->id();
		int layers = NLAYERS;
		while (layers--){
			cout << (id % 4); // print in base 4
			id /= 4;
		}
		cout << "\t" << thisCount << "\t"
				<< 1.*integralCount/totalCount() << "\t"
				<< m_patterns[i]->nLayers() << endl;

	}
	printf("Total chambers matched: %i\n", totalCount());
}

//zeroes the distribution, be careful when to use this! writes average to output file
void PatternList::center(ofstream& f) {
	for(unsigned int i =0; i< m_patterns.size(); i++){
		m_patterns.at(i)->center();
		f << m_envelopeId << "\t" << m_patterns.at(i)->id() << "\t" << m_patterns.at(i)->count() << "\t" << m_patterns.at(i)->getMeanPos() << endl;
	}
}



// ChargeEnvelope are scanned over the entire chamber, this class is
// effectively just a small matrix, each of which (if we have a vector) represent
// a broadly encompasing general pattern which gives us preliminary information on a pattern.

class ChargeEnvelope {
public:

	const unsigned int m_id;
	bool m_hits[MAX_PATTERN_WIDTH][NLAYERS]; //layers

	ChargeEnvelope(unsigned int id, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
	}
	ChargeEnvelope(string name, unsigned int id,const bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
		m_name = name;
	}

	ChargeEnvelope(const ChargeEnvelope &obj) : m_id(obj.m_id) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = obj.m_hits[j][i];
			}
		}
		m_name = obj.m_name;
	}


	ChargeEnvelope() :m_id(-1){
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
		return ChargeEnvelope(name, id, flippedPattern);
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
		m_overlap = new ChargePattern(overlap);
	}

	void print3x6Pattern();
	void printPatternInChamber();

	~SingleEnvelopeMatchInfo() {
		if(m_overlap) delete m_overlap;
	}

	int EnvelopeId() {return m_Envelope.m_id;}

	//center position of the track
	float x(){return 1.*m_horizontalIndex + 0.5*(MAX_PATTERN_WIDTH - 1);}
	int patternId();
	int layMatCount();
	const ChargePattern chargePattern() {return *m_overlap;}


private:

	ChargePattern* m_overlap;
	const ChargeEnvelope m_Envelope;
	int m_horizontalIndex; //half strips, leftmost index of the pattern

};

void SingleEnvelopeMatchInfo::print3x6Pattern(){
	if(m_overlap) m_overlap->printPattern();
}

void SingleEnvelopeMatchInfo::printPatternInChamber(){
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
	if(m_overlap){
		return m_overlap->getLayersMatched();
	}
	return -1;
}

int SingleEnvelopeMatchInfo::patternId(){
	if(m_overlap){
		return m_overlap->getPatternId();
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
	const ChargePattern bestChargePattern();
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

const ChargePattern EnvelopeSetMatchInfo::bestChargePattern() {
	if(m_bestSetMatchIndex >=(int)m_matches->size()) return ChargePattern();
	return m_matches->at(m_bestSetMatchIndex)->chargePattern();
}

int EnvelopeSetMatchInfo::bestLayerCount(){
	if(m_bestSetMatchIndex >=(int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->layMatCount();
}

int EnvelopeSetMatchInfo::bestEnvelopeId() {
	if(m_bestSetMatchIndex >= (int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->EnvelopeId();
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
