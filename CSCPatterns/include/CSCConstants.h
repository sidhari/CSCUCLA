/*
 * PatternConstants.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef CSCCONSTANTS_H_
#define CSCCONSTANTS_H_

#include <string>

const unsigned int BATCH_EVENTS = 1000; //amount of events to be run over in a single batch job
const int DEBUG = -1;
const bool USE_COMP_HITS = 1; //false uses recHits
const unsigned int NLAYERS = 6; //6 layers
const unsigned int MAX_PATTERN_WIDTH = 11;
/* plus / minus half strip window to not look in if you found a clct there
 * warning, making this too small (smaller than the width of the pattern may
 *  cause behavior not consistent with the real tmb, since this emulation
 *  removes hits within the pattern
 */
const unsigned int BUSY_WINDOW = 10;
const unsigned int N_MAX_HALF_STRIPS = 2*80 + 1; //+1 from staggering of chambers
const unsigned int N_LAYER_REQUIREMENT = 3;
const unsigned int TIME_CAPTURE_WINDOW = 4; //allow for 4 consecutive time bins when looking at comparator hits
const unsigned int CFEB_HS = 32;

const std::string LINEFIT_LUT_PATH = "/uscms/home/wnash/CSCUCLA/CSCPatterns/dat/linearFits.lut";
//const std::string LINEFIT_LUT_PATH = "/home/wnash/workspace/CSCUCLA/CSCPatterns/dat/linearFits.lut";

//labels of all envelopes
const unsigned int NPATTERNS = 5;
const unsigned int PATTERN_IDS[NPATTERNS] = {100,90,80, 70, 60};
const unsigned int NCOMPARATOR_CODES = 4096; //codes per pattern 2^12, without accounting for degeneracy, etc

const unsigned int NLEGACYPATTERNS = 9;
const unsigned int LEGACY_PATTERN_IDS[NLEGACYPATTERNS] = {10,9,8,7,6,5,4,3,2};



const unsigned int NCHAMBERS = 10;
const std::string CHAMBER_NAMES[NCHAMBERS] = {
		"ME11B",
		"ME11A",
		"ME12",
		"ME13",
		"ME21",
		"ME22",
		"ME31",
		"ME32",
		"ME41",
		"ME42"
};

//to denote if we are using a legacy chamber or not
const std::string LEGACY_SUFFIX = "-Legacy";

const unsigned int CHAMBER_ST_RI[NCHAMBERS][2] = {
		{1,1},
		{1,4},
		{1,2},
		{1,3},
		{2,1},
		{2,2},
		{3,1},
		{3,2},
		{4,1},
		{4,2}
};


const bool IDSV1_A[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},	
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{1,1,1,1,1,1},
		{1,1,1,1,1,1},
		{1,1,1,1,1,1},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};


const bool IDSV1_B[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,1,1,0,0},
		{1,1,1,1,1,1},
		{0,0,1,1,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};


const bool IDSV1_C[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{1,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,1,1,0,0},
		{0,1,1,1,1,0},
		{0,0,1,1,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,1},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0}
};

const bool IDSV1_D[MAX_PATTERN_WIDTH][NLAYERS] = {
		{0,0,0,0,0,0},
		{1,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,1,0,0,0},
		{0,1,1,0,0,0},
		{0,0,1,1,0,0},
		{0,0,0,1,1,0},
		{0,0,0,1,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,1},
		{0,0,0,0,0,0}
};

const bool IDSV1_E[MAX_PATTERN_WIDTH][NLAYERS] = {
		{1,0,0,0,0,0},
		{1,1,0,0,0,0},
		{1,1,0,0,0,0},
		{0,1,1,0,0,0},
		{0,0,1,0,0,0},
		{0,0,1,1,0,0},
		{0,0,0,1,0,0},
		{0,0,0,1,1,0},
		{0,0,0,0,1,1},
		{0,0,0,0,1,1},
		{0,0,0,0,0,1}
};

//i wrote these once, and dont want to rewrite them...
const bool id2Bools[NLAYERS][MAX_PATTERN_WIDTH] = {
		{0,0,0,0,0,0,0,0,1,1,1},
		{0,0,0,0,0,0,1,1,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,1,1,1,0,0,0,0,0},
		{0,1,1,1,0,0,0,0,0,0,0},
		{1,1,1,0,0,0,0,0,0,0,0}};

const bool id4Bools[NLAYERS][MAX_PATTERN_WIDTH] = {
		{0,0,0,0,0,0,0,1,1,1,0},
		{0,0,0,0,0,0,1,1,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,1,1,0,0,0,0,0,0},
		{0,1,1,1,0,0,0,0,0,0,0},
		{0,1,1,1,0,0,0,0,0,0,0}
		};

const bool id6Bools[NLAYERS][MAX_PATTERN_WIDTH] = {
		{0,0,0,0,0,0,1,1,1,0,0},
		{0,0,0,0,0,1,1,0,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,1,0,0,0,0,0},
		{0,0,0,1,1,0,0,0,0,0,0},
		{0,0,1,1,1,0,0,0,0,0,0}
		};

const bool id8Bools[NLAYERS][MAX_PATTERN_WIDTH] = {
		{0,0,0,0,0,1,1,1,0,0,0},
		{0,0,0,0,0,1,1,0,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,1,0,0,0,0,0},
		{0,0,0,1,1,1,0,0,0,0,0},
		{0,0,0,1,1,1,0,0,0,0,0}
		};

const bool idABools[NLAYERS][MAX_PATTERN_WIDTH] = {
		{0,0,0,0,1,1,1,0,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,1,1,0,0,0,0},
		{0,0,0,0,1,1,1,0,0,0,0}
		};


bool IDA_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID2_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID3_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID4_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID5_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID6_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID7_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID8_BASE [MAX_PATTERN_WIDTH][NLAYERS];
bool ID9_BASE [MAX_PATTERN_WIDTH][NLAYERS];


#endif /* CSCCONSTANTS_H_ */
