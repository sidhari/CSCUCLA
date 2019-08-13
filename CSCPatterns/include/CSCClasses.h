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
#include <iomanip>

//lambda
#include <functional>

#include "../include/CSCConstants.h"
#include "../include/LUTClasses.h"
#include "../include/CSCInfo.h"

using namespace std;

/* @brief With clever scheme here:
 *
 * https://indico.cern.ch/event/712513/contributions/2959915/attachments/1630667/2599369/20180410_otmb_pattern_logic.pdf
 *
 * Can encode "comparator codes" within patterns.
 *
 */
class ComparatorCode{
public:
	ComparatorCode(bool hits[NLAYERS][3]);
	ComparatorCode(unsigned int comparatorCode);
	ComparatorCode(const ComparatorCode& c);
	ComparatorCode();

	~ComparatorCode() {}

	bool _hits[NLAYERS][3]; //all the hits within a pattern that a chamber scan matched

	unsigned int getLayersMatched() const;
	int getId() const;
	void printCode() const;
	static string getStringInBase4(int code);
	static bool getHits(const unsigned int comparatorCode,bool hits[NLAYERS][3]);

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
	CSCPattern(unsigned int id, bool isLegacy, const bool pat[MAX_PATTERN_WIDTH][NLAYERS]);
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
	CLCTCandidate(CSCPattern p, ComparatorCode c, int horInd, int startTime);
	CLCTCandidate(CSCPattern p, int horInd, int startTime, int layMatCount);

	~CLCTCandidate();

	const CSCPattern _pattern;
	const int _horizontalIndex; //half strips, leftmost index of the pattern
	const int _startTime;
	//pointer to whatever LUT Entry is associated with this candidate
	const LUTEntry* _lutEntry;

	int getHits(int code_hits[MAX_PATTERN_WIDTH][NLAYERS]) const;
	float keyStrip() const;
	int keyHalfStrip() const;
	float position() const;
	float slope() const;
	void print3x6Pattern() const;
	void printCodeInPattern() const;
	int comparatorCodeId() const;
	int layerCount() const;
	const ComparatorCode getComparatorCode() const;
	int patternId() const;
	const LUTKey key() const;

	//declare a function pointer used to sort the CLCT Candidates
	typedef function<bool(CLCTCandidate*, CLCTCandidate*)> QUALITY_SORT;
	static QUALITY_SORT quality;

private:
	ComparatorCode* _code;
	int _layerMatchCount;
	//float _quality;

};

class ALCTCandidate
{
	public:
		ALCTCandidate(CSCPattern p, int kwg, bool hits[NLAYERS][3]);
		void ~ALCTCandidate();

		const CSCPattern pattern;
		const int kwg;
		const int time;
		bool isValid;
		int quality;
		bool vtp;
		bool vbot; 

		int getHits(int code_hits[MAX_PATTERN_WIDTH][NLAYERS]) const;

	private:
		ALCTCandidate* next;
		ALCTCandidate* prev;
};

class ALCTCandidateCollection
{
	public:
		ALCTCandidateCollection() : CSCInfo::Object("alct_cand")
		{
			ch_id = 0;
			quality = 0; 
			accelerator = 0;
			collisionB = 0;
			keyWG = 0;
			BX = 0;
			trkNumber = 0;
			fullBX = 0;
		}
		
		ALCTCandidateCollection(TTree* t, int flag=1) : ALCTCandidateCollection()
		{
			if (flag == 0) // read
			{
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(quality)).c_str(), &quality);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(accelerator)).c_str(), &accelerator);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(collisionB)).c_str(), &collisionB);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(keyWG)).c_str(), &keyWG);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(BX)).c_str(), &BX);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(trkNumber)).c_str(), &trkNumber);
				t->SetBranchAddress(branchify(GET_VARIABLE_NAME(fullBX)).c_str(), &fullBX);
			}
			else if (flag == 1) // write (writes to trees by default)
			{
				t->Branch(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
				t->Branch(branchify(GET_VARIABLE_NAME(quality)).c_str(), &quality);
				t->Branch(branchify(GET_VARIABLE_NAME(accelerator)).c_str(), &accelerator);
				t->Branch(branchify(GET_VARIABLE_NAME(collisionB)).c_str(), &collisionB);
				t->Branch(branchify(GET_VARIABLE_NAME(keyWG)).c_str(), &keyWG);
				t->Branch(branchify(GET_VARIABLE_NAME(BX).c_str(), &BX);
				t->Branch(branchify(GET_VARIABLE_NAME(trkNumber)).c_str(), &trkNumber);
				t->Branch(branchify(GET_VARIABLE_NAME(fullBX)).c_str(), &fullBX);
			}
			else std::cout << "Error with flag value" << std::endl;
		}

		unsigned int size() const 
		{
			return ch_id ? ch_id->size() : 0;
		}

		std::vector<size16>* ch_id;
		std::vector<size8>* quality; 
		std::vector<size8>* accelerator;
		std::vector<size8>* collisionB;
		std::vector<size8>* keyWG;
		std::vector<size8>* BX; 			//bunch crossing
		std::vector<size8>* trkNumber;
		std::vector<size16>* fullBX;
}; 

/*

class ALCTCandidateCollection
{
	public:
		ALCTs() : Object("alct_cand")
		{
			ch_id = 0;
			quality = 0; 
			accelerator = 0;
			collisionB = 0;
			keyWG = 0;
			BX = 0;
			trkNumber = 0;
			fullBX = 0;
		}

		ALCTCandidateCollection(TTree* t) : ALCTs()
		{
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(ch_id)).c_str(), &ch_id);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(quality)).c_str(), &quality);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(accelerator)).c_str(), &accelerator);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(collisionB)).c_str(), &collisionB);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(keyWG)).c_str(), &keyWG);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(BX)).c_str(), &BX);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(trkNumber)).c_str(), &trkNumber);
			t->SetBranchAddress(branchify(GET_VARIABLE_NAME(fullBX)).c_str(), &fullBX);
		}

		unsigned int size() const 
		{
			return ch_id ? ch_id->size() : 0;
		}

		std::vector<size16>* ch_id;
		std::vector<size8>* quality; 
		std::vector<size8>* accelerator;
		std::vector<size8>* collisionB;
		std::vector<size8>* keyWG;
		std::vector<size8>* BX; 			//bunch crossing
		std::vector<size8>* trkNumber;
		std::vector<size16>* fullBX;
}; 

class FillALCTCandidateInfo: public ALCTCandidateCollection, public FillInfo
{
	public:
		FillALCTCandidateCollection(TreeContainer &tree):
			ALCTCandidateCollection(),
			FillInfo(name,tree)
		{

		}

		virtual ~FillALCTCandidateCollection()
		{

		}

		virtual void reset()
		{

		}

		void splat(ALCTCandidate &alct_cand, unsigned int chamberHash);

		void fill(const std::vector<ALCTCandidate> &alct_cands, unsigned int chamberHash);
};
*/

/* @brief Encapsulates hit information for recorded event
 * in a chamber, identified by its station, ring, endcap and chamber
 */
class ChamberHits {
public:
	ChamberHits(unsigned int station=0, unsigned int ring=0,
			unsigned int endcap=0, unsigned int chamber=0, bool isComparator=true);
	ChamberHits(const ChamberHits& c);

	~ChamberHits(){}

	const bool _isComparator;
	const unsigned int _station;
	const unsigned int _ring;
	const unsigned int _endcap;
	const unsigned int _chamber;
	unsigned int minHs() const {return _minHs;}
	unsigned int maxHs() const {return _maxHs;}
	unsigned int nhits() const {return _nhits;}
	float hitMeanHS();
	float hitStdHS();
	int _hits[N_MAX_HALF_STRIPS][NLAYERS];

	bool shift(unsigned int lay) const;

	int fill(const CSCInfo::Comparators& c);
	int fill(const CSCInfo::RecHits& r);
	void print() const; //deprecated
	
	friend ostream& operator<<(ostream& os, const ChamberHits& c);
	ChamberHits& operator-=(const CLCTCandidate& mi);
private:
	unsigned int _nhits;
	unsigned int _minHs; //Halfstrip
	unsigned int _maxHs;

	float _meanHS;
	float _stdHS;
};

class ALCT_ChamberHits
{
	public:
		ALCT_ChamberHits(unsigned int station=0, unsigned int ring=0,
			unsigned int endcap=0, unsigned int chamber=0, bool isWire=true);
		
		ALCT_ChamberHits(const ALCT_ChamberHits &c);
		
		~ALCT_ChamberHits(){}

		const bool _isWire;
		const unsigned int _station;
		const unsigned int _ring;
		const unsigned int _endcap;
		const unsigned int _chamber;

		unsigned int get_minWi() const {return _minWi;}
		unsigned int get_maxWi() const {return _maxWi;}
		unsigned int get_nhits() const {return _nhits;}

		float get_hitMeanWi();
		float get_hitStdWi();

		int _hits[N_KEY_WIRE_GROUPS][NLAYERS];

		void fill(const CSCInfo::Wires &w);
		void fill(const CSCInfo::Wires &w, int time, int p_ext=1)
		
		friend ostream& operator<<(ostream& os, const ALCT_ChamberHits& c);
		//ALCT_ChamberHits& operator-=(const ALCTCandidate &mi);

	private:
		unsigned int _minWi;
		unsigned int _maxWi;
		unsigned int _nhits;

		float _meanWi;
		float _stdWi;
};


#endif /* PATTERNFITTER_H_ */
