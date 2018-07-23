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
#include <string>

#include "PatternConstants.h"

using namespace std;

/* @brief With clever scheme here:
 *
 * https://indico.cern.ch/event/712513/contributions/2959915/attachments/1630667/2599369/20180410_otmb_pattern_logic.pdf
 *
 * Can encode "comparator codes" within patterns.
 *
 */
class ComparatorCode {
public:
	ComparatorCode(bool hits[NLAYERS][3]);
	ComparatorCode(const ComparatorCode& c);
	ComparatorCode();

	~ComparatorCode() {}

	bool _hits[NLAYERS][3]; //all the hits within a pattern that a chamber scan matched

	unsigned int getLayersMatched() const;
	int getId() const;
	void printCode() const;

private:
	int _id;
	int _layersMatched;

	void calculateId();
	void calculateLayersMatched();
};




/* @brief CSCPattern is a pattern used by the TMB as a first pass
 * towards creating a CLCT. Each have an acceptance for numerous tracks,
 * but give rough information as to the tracks slope.
 *
 * Comparator codes are created out of hits within a pattern.
 */
class CSCPattern {
public:
	CSCPattern(unsigned int id, bool isLegacy, const bool pat_[MAX_PATTERN_WIDTH][NLAYERS]);
	CSCPattern(string name, unsigned int id, bool isLegacy, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]);
	CSCPattern(const CSCPattern &obj);
	CSCPattern();

	~CSCPattern() {}

	const unsigned int _id;
	const bool _isLegacy;
	bool _pat[MAX_PATTERN_WIDTH][NLAYERS];

	CSCPattern makeFlipped(unsigned int id) const;
	CSCPattern makeFlipped(string name, unsigned int id) const;
	void printCode(int code) const;
	int recoverPatternCCCombination(int code, int code_hits[MAX_PATTERN_WIDTH][NLAYERS]) const;
	string getName() const;

private:
	string _name;
};


/* @brief A candidate "road" within the CSC chamber made of a series
 * of hits compressed into a comparator code
 */
class CLCTCandidate {
public:
	CLCTCandidate(CSCPattern p, int horInd, int startTime, bool hits[NLAYERS][3]);
	CLCTCandidate(CSCPattern p, int horInd, int startTime, int layMatCount);

	~CLCTCandidate();

	const CSCPattern _pattern;
	const int _horizontalIndex; //half strips, leftmost index of the pattern
	const int _startTime;

	float x() const;
	void print3x6Pattern() const;
	void printCodeInPattern() const;
	int comparatorCodeId() const;
	int layerCount() const;
	const ComparatorCode getComparatorCode() const;
	int patternId() const;

private:
	ComparatorCode* _code;
	int _layerMatchCount;

};


/* @brief Encapsulates hit information for recorded event
 * in a chamber, identified by its station, ring, endcap and chamber
 */
class ChamberHits {
public:
	ChamberHits(bool isComparator, unsigned int station, unsigned int ring,
			unsigned int endcap, unsigned int chamber);
	ChamberHits(const ChamberHits& c);

	~ChamberHits(){}

	const bool _isComparator;
	const unsigned int _station;
	const unsigned int _ring;
	const unsigned int _endcap;
	const unsigned int _chamber;
	int _hits[N_MAX_HALF_STRIPS][NLAYERS];


	ChamberHits& operator-=(const CLCTCandidate& mi);

};


#endif /* PATTERNFITTER_H_ */
