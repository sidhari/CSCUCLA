/*
 * LUTClasses.h
 *
 *  Created on: Jul 25, 2018
 *      Author: wnash
 */

#ifndef CSCPATTERNS_INCLUDE_LUTCLASSES_H_
#define CSCPATTERNS_INCLUDE_LUTCLASSES_H_

#include <set>
#include <map>
#include <string>

//lambda
#include <functional>
#include <algorithm>

#include "../include/PatternFinderClasses.h"


using namespace std;


/* @brief Key used to index the LUT
 */
class LUTKey {
public:
	LUTKey(int pattern, int code);
	LUTKey(int pattern);
	LUTKey(const LUTKey&  k);

	~LUTKey() {}

	//necessary for maps!
	bool operator<(const LUTKey& l) const;
	bool operator==(const LUTKey& l) const;

	int _pattern;
	int _code; //comparator code
	bool _isLegacy;
};




/* @brief Describes each entry in the LUT (see below)
 */
class LUTEntry {
public:
	LUTEntry();
	LUTEntry(float position, float slope, unsigned long nsegments, unsigned long nclcts,
			float multiplicity, float quality, unsigned int layers, float chi2);


	~LUTEntry() {}

	int addSegment(float positionOffset, float slopeOffset);
	int addCLCT(unsigned int multiplicity=1);
	bool operator<(const LUTEntry& l) const;
	bool operator==(const LUTEntry& l) const;
	float position() const;
	float slope() const;
	unsigned int nsegments() const; //amount of segments used to construct LUT entry
	unsigned int nclcts() const;
	float quality() const;
	float probability() const;
	float multiplicity() const; //calculates average multiplicity for how many clcts it is associated with
	int makeFinal();


	const unsigned int _layers; //layers in code
	const float _chi2; //chi2 of fit

private:
	bool _isFinal; //if we have already calculated the offsets with all the segments
	vector<float> _positionOffsets; //segment - clct
	vector<float> _slopeOffsets;

	float _position;
	float _slope;
	unsigned long _nsegments; //amount of segments used to construct LUT entry
	unsigned long _nclcts; //amount of clcts we found when making this entry (not necessarily matched with segments)
	float _multiplicity;

	vector<unsigned int> _clctMultiplicities; //fill for each clct we find how many there are in the chamber
	float _quality; //quality parameter used choose between CLCTs

};


/* @brief Lambda function used to optimize LUTEntrys in the set,
 * mostly for printing
 */
typedef function<bool(pair<LUTKey,LUTEntry>, pair<LUTKey,LUTEntry>)> LUTLambda;
LUTLambda LUT_FUNT =
		[](pair<LUTKey,LUTEntry> l1, pair<LUTKey,LUTEntry> l2)
		{
		return l1.second < l2.second;
		};


/* @brief Lookup table to be used in the (O)TMB to translate
 * from
 *
 * [pattern, ccode] -> [position, slope, quality, ...]
 *
 */
class LUT {
public:
	LUT();
	LUT(const string& name);
	LUT(const string& name, const string& filename);

	~LUT() {};

	const string _name;

	int setEntry(const LUTKey& k,const LUTEntry& e);
	int editEntry(const LUTKey& k, LUTEntry*& e);
	int getEntry(const LUTKey&k, const LUTEntry*& e, bool debug=false) const;
	void print(unsigned int minSegments=0);
	int writeToText(const string& filename);
	int writeToROOT(const string& filename);
	int writeToPSLs(const string& fileprefix);
	int makeFinal();

private:
	bool _isFinal;
	set<pair<LUTKey, LUTEntry>, LUTLambda> _orderedLUT;
	map<LUTKey,LUTEntry> _lut;

	static int convertToPSLLine(const LUTEntry& e);

};

class DetectorLUTs {

public:
	DetectorLUTs(bool isLegacy=false);

	~DetectorLUTs(){};

	int addEntry(const string& name, int station, int ring,
			const string& lutpath = LINEFIT_LUT_PATH);
	int editLUT(int station, int ring, LUT*& lut);
	int getLUT(int station, int ring, const LUT*& lut);
	int makeFinal();
	int writeAll(const string& path);
	int loadAll(const string& path);
	unsigned int size() const;

private:
	const bool _isLegacy;
	//the look up table for each ST, RI
	map<const pair<int,int>, LUT> _luts;
};



#endif /* CSCPATTERNS_INCLUDE_LUTCLASSES_H_ */
