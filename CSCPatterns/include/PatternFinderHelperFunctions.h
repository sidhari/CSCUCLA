/*
 * PatternFinderHelperFunctions.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFINDERHELPERFUNCTIONS_H_
#define PATTERNFINDERHELPERFUNCTIONS_H_

#include <math.h>

#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"

#include "PatternFinderClasses.h"
#include "LUTClasses.h"


bool validComparatorTime(const unsigned int time, const unsigned int startTimeWindow);

int findClosestToSegment(vector<CLCTCandidate*> matches, float segmentX);

void printPattern(const CSCPattern &p);

void printChamber(const ChamberHits &c);

//TODO: these functions could be put together in a better way
int printPatternCC(unsigned int pattID,int cc=-1);

//calculates the overlap of a pattern on a chamber at a given position and time bin window, returns layers matched
int getOverlap(const ChamberHits &c, const CSCPattern &p, const int horPos, const int startTimeWindow, bool overlap[NLAYERS][3]);

//looks if a chamber "c" contains an envelope "p" at the location horPos returns
//the number of matched layers
int legacyLayersMatched(const ChamberHits &c, const CSCPattern &p, const int horPos, const int startTimeWindow);

//looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers if ,
// run successfully, match info is stored in variable mi
int containsPattern(const ChamberHits &c, const CSCPattern &p,  CLCTCandidate *&mi);

//look for the best matched pattern, when we have a set of them, and return a vector possible of candidates
int searchForMatch(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m);

//makes a LUT out of a properly formatted TTree
int makeLUT(TTree* t, DetectorLUTs& newLUTs, DetectorLUTs& legacyLUTs);

//creates the new set of patterns
vector<CSCPattern>* createNewPatterns();

//creates the currently implemented patterns in the TMB
vector<CSCPattern>* createOldPatterns();

int chamberSerial( int ec, int st, int ri, int ch );

int fillCompHits(ChamberHits& theseCompHits,
		const vector< vector<int> >* compStr, //comparator strip #
		const vector< vector<int> >* compHS, //comparator half strip #
		const vector< vector< vector<int> > >* compTimeOn,
		const vector<int>* compLay,
		const vector<int>* compId); // index of what ring/station you are on

int fillRecHits(ChamberHits& theseRecHits,
		const vector<int>* rhId,
		const vector<int>* rhLay,
		const vector<float>* rhPos);


TCanvas* makeCLCTCountPlot(string descriptor, vector<TH1F*> clctCounts);

#endif /* PATTERNFINDERHELPERFUNCTIONS_H_ */
