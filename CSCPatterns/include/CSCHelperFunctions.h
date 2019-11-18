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

//looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers
//If run successfully, match info is stored in variable mi
//only looks in time bins 6-9 since comparator data read from tree is centered here
int containsPattern(const ChamberHits &c, const CSCPattern &p,  CLCTCandidate *&mi,const vector<CLCTCandidate*>& previousCandidates=vector<CLCTCandidate*>());

//Looks if a chamber "c" contains a pattern "p". returns -1 if error, and the number of matched layers
//If run successfully, match info is stored in variable mi
//Can look in any time window (used for pretrigger and trigger)
int containsPattern_v1(const ChamberHits &c, const CSCPattern &p,  CLCTCandidate *&mi, unsigned int t, const vector<CLCTCandidate*>& previousCandidates=vector<CLCTCandidate*>());

//Look for the best matched pattern, when we have a set of them, and return a vector possible of candidates
//Designed to look through windows of 4 time bins each (pretrigger and trigger mechanism implemented)
//Comparators read from tree
int searchForMatch_v1(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, CSCInfo::Comparators comparators, bool useBusyWindow=false);

//Cycles through time windows of 4 time bins each, pretriggers if atleast 1 CLCT with min req are met
//User generated comparators
int searchForMatch_pretrigger(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, Comparators_gen comparators, bool useBusyWindow=false);

//Cycles through time windows of 4 time bins each, pretriggers if atleast 1 CLCT with min req are met
//Comparators read from tree
int searchForMatch_pretrigger(const ChamberHits &c, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, CSCInfo::Comparators comparators, bool useBusyWindow=false);

//If pretrigger, then look 2 bx later for CLCTs, returns a vector of CLCT candidates
//User generated comparators
int searchForMatch_trigger(const ChamberHits &chamber_time, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, Comparators_gen comparators, unsigned int triggertime, bool useBusyWindow=false);

//If pretrigger, then look 2 bx later for CLCTs, returns a vector of CLCT candidates
//Comparators read from tree
int searchForMatch_trigger(const ChamberHits &chamber_time, const vector<CSCPattern>* ps, vector<CLCTCandidate*>& m, CSCInfo::Comparators comparators, unsigned int triggertime, bool useBusyWindow=false);

//makes a LUT out of a properly formatted TTree
int makeLUT(TTree* t, DetectorLUTs& newLUTs, DetectorLUTs& legacyLUTs);

//sets the lut entries for all of the candidates we find in a chamber, identified by station and ring
int setLUTEntries(vector<CLCTCandidate*> candidates, const DetectorLUTs& luts, int station, int ring);

//creates the new set of patterns
vector<CSCPattern>* createNewPatterns();

//creates the currently implemented patterns in the TMB
vector<CSCPattern>* createOldPatterns();

void writeToMEMFiles(const ChamberHits& c, std::ofstream CFEBStreams[MAX_CFEBS]);

//time window cycling
//comparators user generated
void writeToMEMFiles_v1(const ChamberHits& c, Comparators_gen comparators, std::ofstream CFEBStreams[MAX_CFEBS]);

//time window cycling
//comparators read from tree
void writeToMEMFiles_v1(const ChamberHits& c, CSCInfo::Comparators comparators, std::ofstream CFEBStreams[MAX_CFEBS]);


#endif /* PATTERNFINDERHELPERFUNCTIONS_H_ */
