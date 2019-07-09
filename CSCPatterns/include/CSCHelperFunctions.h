/*
 * PatternFinderHelperFunctions.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNFINDERHELPERFUNCTIONS_H_
#define PATTERNFINDERHELPERFUNCTIONS_H_

#include "../include/CSCClasses.h"
#include <math.h>

#include "TTree.h"

#include "LUTClasses.h"

#include "../include/CSCInfo.h"

bool validComparatorTime(const unsigned int time, const unsigned int startTimeWindow);

int findClosestToSegment(vector<CLCTCandidate*> matches, float segmentX);

void printPattern(const CSCPattern &p);

//TODO: these functions could be put together in a better way
int printPatternCC(unsigned int pattID,int cc=-1);

//calculates the overlap of a pattern on a chamber at a given position and time bin window, returns layers matched
int getOverlap(const ChamberHits &c, const CSCPattern &p, const int horPos, const int startTimeWindow, bool overlap[NLAYERS][3]);

//looks if a chamber "c" contains an envelope "p" at the location horPos returns
//the number of matched layers
int legacyLayersMatched(const ChamberHits &c, const CSCPattern &p, const int horPos, const int startTimeWindow);

//looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers if ,
// run successfully, match info is stored in variable mi
int containsPattern(const ChamberHits &c, const CSCPattern &p,  CLCTCandidate *&mi, const vector<CLCTCandidate*>& previousCandidates=vector<CLCTCandidate*>());

//look for the best matched pattern, when we have a set of them, and return a vector possible of candidates
int searchForMatch(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, bool useBusyWindow=false);

//makes a LUT out of a properly formatted TTree
int makeLUT(TTree* t, DetectorLUTs& newLUTs, DetectorLUTs& legacyLUTs);

//sets the lut entries for all of the candidates we find in a chamber, identified by station and ring
int setLUTEntries(vector<CLCTCandidate*> candidates, const DetectorLUTs& luts, int station, int ring);

//creates the new set of patterns
vector<CSCPattern>* createNewPatterns();

//creates the currently implemented patterns in the TMB
vector<CSCPattern>* createOldPatterns();

int fillCompHits(ChamberHits& theseCompHits,
		const CSCInfo::Comparators& c); // index of what ring/station you are on

int fillRecHits(ChamberHits& theseRecHits,
		const CSCInfo::RecHits& r);


#endif /* PATTERNFINDERHELPERFUNCTIONS_H_ */
