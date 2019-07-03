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
#include <iterator>
#include <utility> //pair

//lambda
#include <functional>
#include <algorithm>

#include "TTree.h"

#include "../include/PatternFinderClasses.h"


using namespace std;


/* @brief Key used to index the LUT
 */
class LUTKey {
public:
	//default -1 if not new codes
	LUTKey(int pattern, int code = -1);
	LUTKey(const LUTKey&  k);

	~LUTKey() {}

	//necessary for maps!
	bool operator<(const LUTKey& l) const;
	bool operator==(const LUTKey& l) const;

	int _pattern;
	int _code; //comparator code
};




/* @brief Describes each entry in the LUT (see below)
 */
class LUTEntry {
public:
	LUTEntry();
	LUTEntry(float position, float slope, unsigned long nsegments, float pt, unsigned long nclcts,
			float multiplicity, float quality, unsigned int layers, float chi2);
	//LUTEntry(TTree* tree, float quality, float layers, float chi2);


	~LUTEntry() {}

	/* TODO: add lumi?
	 *
	 */

	int loadTree(TTree* tree);
	int addCLCT(unsigned int multiplicity=1, float pt=-1.,float posOffset=-999., float slopeoffset=-999.); //no associated segment

	bool operator<(const LUTEntry& l) const;
	bool operator==(const LUTEntry& l) const;
	float position() const; //strips
	float slope() const; //strips/layer
	unsigned int nsegments() const; //amount of segments used to construct LUT entry
	float pt() const;
	unsigned int nclcts() const;
	float quality() const;
	void setquality(int qual);
	float probability() const;
	float multiplicity() const; //calculates average multiplicity for how many clcts it is associated with

	TTree* makeTree(const string& name) const;


	int makeFinal();


	const unsigned int _layers; //layers in code
	const float _chi2; //chi2 of fit

private:
	bool _isFinal; //if we have already calculated the offsets with all the segments
	/*
	 * TODO: Nov 13
	 * - organize everything so it is per CLCT, with -999. for pt, offset entries without segments
	 * - make tree writer class belong to each entry specifically
	 *
	 */
	vector<bool> _hasSegment; //if we ahve a segment associated with this one or not
	vector<float> _positionOffsets; //segment - clct
	vector<float> _slopeOffsets;
	vector<float> _pts; //all pts associated with segments put into entry
	vector<unsigned int> _clctMultiplicities; //fill for each clct we find how many there are in the chamber

	float _pt; // [GeV] average pt of associated segments
	float _position;
	float _slope;
	unsigned long _nsegments; //amount of segments used to construct LUT entry
	unsigned long _nclcts; //amount of clcts we found when making this entry (not necessarily matched with segments)
	float _multiplicity;


	float _quality; //quality parameter used choose between CLCTs

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
	LUT(const string& name, bool isLegacy=false);
	LUT(const string& name, const string& lutfile, bool isLegacy=false);

	~LUT() {};

	const string _name;

	int setEntry(const LUTKey& k,const LUTEntry& e);
	int editEntry(const LUTKey& k, LUTEntry*& e);
	int getEntry(const LUTKey&k, const LUTEntry*& e, bool debug=false) const;
	void print(unsigned int minClcts=0,unsigned int minSegments=0, unsigned int minLayers=0);
	void printPython(unsigned int minClcts=0,unsigned int minSegments=0,unsigned int minLayers=0) {print(minClcts,minSegments,minLayers);} //because python keywords...

	int loadROOT(const string& rootfile);
	int loadText(const string& textfile);
	int writeToText(const string& filename);
	int writeToROOT(const string& filename);
	int writeToPSLs(const string& fileprefix);
	int makeFinal();
	int sort(const string& sortOrder);
	int size() const {return _orderedLUT.size();}

	int nclcts();
	int nsegments();

	void setqual();
	
private:
	bool _isFinal;
	const bool _isLegacy;
	int _nclcts;
	int _nsegments;
	string _sortOrder;
	/* @brief Lambda function used to optimize LUTEntrys in the set,
	 * mostly for printing
	 */
	typedef function<bool(pair<LUTKey,LUTEntry>, pair<LUTKey,LUTEntry>)> LUTLambda;
	LUTLambda _lutFunc =
			[this](pair<LUTKey,LUTEntry> l1, pair<LUTKey,LUTEntry> l2)
			{

			//cout << "sorting: l1.pat: "<< l1.first._pattern << " l1.code: " << l1.first._code << " l2.pat" << l2.first._pattern <<
			//		" l2.code: " << l2.first._code << endl;
			//return l1.first < l2.first;
			//TODO: some bug here, doesn't work for first lut??
			//May 14: Ignoring old warning, not sure where this would be happening, will keep an eye out
			auto& l1e = l1.second;
			auto& l2e = l2.second;
			for(auto c: _sortOrder){
				//cout << c << endl;
				switch (c) {
				case 'p':
					if(l1e.probability() == l2e.probability()) continue;
					return l1e.probability() > l2e.probability();
				case 's':
					if(l1e.nsegments() == l2e.nsegments()) continue;
					return l1e.nsegments() > l2e.nsegments();
				case 'c':
					if(l1e.nclcts() == l2e.nclcts()) continue;
					return l1e.nclcts() > l2e.nclcts();
				case 'l':
					if(l1e._layers == l2e._layers) continue;
					return l1e._layers > l2e._layers;
				case 'm':
					if(l1e.multiplicity() == l2e.multiplicity()) continue;
					return l1e.multiplicity() > l2e.multiplicity();
				case 'x': //x^2
					if(l1e._chi2 == l2e._chi2) continue;
					return l1e._chi2 < l2e._chi2;
				case 'e': //energy
					if(l1e.pt() == l2e.pt()) continue;
					return l1e.pt() > l2e.pt();
				case 'k': //key
					if(l1.first == l2.first) continue;
					return l1.first < l2.first;
				default:
					return l1.second < l2.second;
				}
			}
			return false;
		};


	set<pair<LUTKey, LUTEntry>, LUTLambda> _orderedLUT;
	map<LUTKey,LUTEntry> _lut;

	static int convertToPSLLine(const LUTEntry& e);

public:
	set<pair<LUTKey, LUTEntry>, LUTLambda>::iterator begin() {return _orderedLUT.begin();}
	set<pair<LUTKey, LUTEntry>, LUTLambda>::iterator end() {return _orderedLUT.end();}


};

class DetectorLUTs {

public:
	DetectorLUTs(bool isLegacy=false);

	~DetectorLUTs(){};

	int addEntry(const string& name, int station, int ring,
			const string& lutpath = "");
	int editLUT(int station, int ring, LUT*& lut);
	int getLUT(int station, int ring, const LUT*& lut) const;
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
