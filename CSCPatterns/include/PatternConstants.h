/*
 * PatternConstants.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNCONSTANTS_H_
#define PATTERNCONSTANTS_H_

#include <string>


const int DEBUG = 0;
const int NLAYERS = 6; //6 layers
const unsigned int MAX_ENTRY = 20000; //how many events you look at (max ~2,000,000 for 2016F)
const bool USE_COMP_HITS = 0; //false uses recHits
const int MAX_PATTERN_WIDTH = 11;
const int N_MAX_HALF_STRIPS = 2*80 + 1; //+1 from staggering of chambers
const int MAX_COMP_TIME_BIN = 15;
const int TIME_RANGE = USE_COMP_HITS ? 2: 1e9; //don't care about range for recHits
const bool MAKE_MATCH_LAYER_COMPARISON = false;
const int N_LAYER_REQUIREMENT = 3;

const std::string INPUT_FILENAME = "CSCDigiTree161031.root";


//labels of all envelopes
const int NENVELOPES = 5;
const int ENVELOPE_IDS[NENVELOPES] = {100,400,500, 800, 900};


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

bool idABools[NLAYERS][MAX_PATTERN_WIDTH] = {
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


#endif /* PATTERNCONSTANTS_H_ */
