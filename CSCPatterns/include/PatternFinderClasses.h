/*
 * PatternFitter.h
 *
 *  Created on: Oct 9, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFITTER_H_
#define PATTERNFITTER_H_

#include <iostream>

//#include "PatternF.h"
#include "PatternConstants.h"

using namespace std;


struct ChamberHits {
	bool isComparator;
	unsigned int station;
	unsigned int ring;
	unsigned int endcap;
	int hits[N_MAX_HALF_STRIPS][NLAYERS];
};


class IdCount{
public:
	IdCount(int id) : m_id(id) {
		m_count = 1;
	}
	~IdCount(){}
private:
	int m_count;
	const int m_id;
};



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

// ChargeSuperPattern are scanned over the entire chamber, this class is
// effectively just a small matrix, each of which (if we have a vector) represent
// a broadly encompasing general pattern which gives us preliminary information on a pattern.

class ChargeSuperPattern {
public:

	const unsigned int m_id;
	bool m_hits[MAX_PATTERN_WIDTH][NLAYERS]; //layers

	ChargeSuperPattern(unsigned int id, bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
	}
	ChargeSuperPattern(string name, unsigned int id, bool pat[MAX_PATTERN_WIDTH][NLAYERS]) : m_id(id){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = pat[j][i];
			}
		}
		m_name = name;
	}

	ChargeSuperPattern(const ChargeSuperPattern &obj) : m_id(obj.m_id) {
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = obj.m_hits[j][i];
			}
		}
		m_name = obj.m_name;
	}


	ChargeSuperPattern() :m_id(-1){
		for(unsigned int i = 0; i < NLAYERS; i++){
			for(unsigned int j = 0; j < MAX_PATTERN_WIDTH; j++){
				m_hits[j][i] = 0;
			}
		}
		m_name = "";
	}

	~ChargeSuperPattern() {}
	ChargeSuperPattern returnFlipped(unsigned int id){
		return returnFlipped("", id);
	}
	ChargeSuperPattern returnFlipped(string name, unsigned int id){
		bool flippedPattern[MAX_PATTERN_WIDTH][NLAYERS];
		for(int i = 0; i < NLAYERS; i++){
			for(int j = 0; j < MAX_PATTERN_WIDTH; j++){
				flippedPattern[j][i] = m_hits[MAX_PATTERN_WIDTH-1-j][i];
			}
		}
		return ChargeSuperPattern(name, id, flippedPattern);
	}

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


class SingleSuperPatternMatchInfo {
public:

	SingleSuperPatternMatchInfo(ChargeSuperPattern p, bool overlap[NLAYERS][3]): m_superPattern(p) {
		m_layerMatchCount = -1;
		m_overlap = new ChargePattern(overlap);
	}
	SingleSuperPatternMatchInfo(ChargeSuperPattern p) : m_superPattern(p){
		m_layerMatchCount = -1;
		m_overlap = 0;
	}
	void addOverlap(bool pat[NLAYERS][3]);
	void print3x6Pattern();
	~SingleSuperPatternMatchInfo() {
		if(m_overlap) delete m_overlap;
	}

	int patId() {return m_superPattern.m_id;}
	int layMatCount();


private:
	int m_layerMatchCount;

	ChargePattern* m_overlap = 0;
	const ChargeSuperPattern m_superPattern;
};

void SingleSuperPatternMatchInfo::print3x6Pattern(){
	if(m_overlap) m_overlap->printPattern();
}

void SingleSuperPatternMatchInfo::addOverlap(bool pat[NLAYERS][3]) {
	if(m_overlap) delete m_overlap;
	m_overlap = new ChargePattern(pat);
}

int SingleSuperPatternMatchInfo::layMatCount() {
	if(m_layerMatchCount < 0 && m_overlap){
		m_layerMatchCount = m_overlap->getLayersMatched();
	}
	return m_layerMatchCount;
}


class SuperPatternSetMatchInfo {
public:

	SuperPatternSetMatchInfo() {
		m_bestSetMatchIndex = 0;
		m_matches = new vector<SingleSuperPatternMatchInfo*>();
	}
	int bestLayerCount();
	int bestPatternId();
	int bestSetIndex() {return m_bestSetMatchIndex;}
	void printBest3x6Pattern();
	void addSingleInfo(SingleSuperPatternMatchInfo* smi);
	~SuperPatternSetMatchInfo() {
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

	vector<SingleSuperPatternMatchInfo*>* m_matches;

};

//prints the overlap between the best super pattern and the chamber
void SuperPatternSetMatchInfo::printBest3x6Pattern(){
	if(m_matches->size()) m_matches->at(m_bestSetMatchIndex)->print3x6Pattern();
}

int SuperPatternSetMatchInfo::bestLayerCount(){
	if(m_bestSetMatchIndex >=(int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->layMatCount();
}

int SuperPatternSetMatchInfo::bestPatternId() {
	if(m_bestSetMatchIndex >= (int)m_matches->size()) return -1;
	return m_matches->at(m_bestSetMatchIndex)->patId();
}

void SuperPatternSetMatchInfo::addSingleInfo(SingleSuperPatternMatchInfo* smi) {
	if(smi->layMatCount() > bestLayerCount()){
		m_bestSetMatchIndex = m_matches->size();
	}
	m_matches->push_back(smi);
}


class PatternIDMatchPlots {
public:
	vector<TH1F*> m_plots;
	vector<vector<int>> m_matchIds; //tells us what pattern ids are contained in each respective plot
	const string m_name;
	TH1F* m_denominator;
	vector<ChargeSuperPattern>* m_patterns; //patterns used to test against

	PatternIDMatchPlots(string name, vector<ChargeSuperPattern>* patterns) : m_name(name){

		m_patterns = new vector<ChargeSuperPattern>();
		for(unsigned int i =0; i < patterns->size(); i++){
			m_patterns->push_back(patterns->at(i));
		}

		m_denominator = 0;
	}
	~PatternIDMatchPlots() {
		//should also delete all the stuff in the vector
		delete m_denominator;
	};
};





#endif /* PATTERNFITTER_H_ */
