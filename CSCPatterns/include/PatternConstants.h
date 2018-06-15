/*
 * PatternConstants.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNCONSTANTS_H_
#define PATTERNCONSTANTS_H_

#include <string>

const unsigned int BATCH_EVENTS = 1000; //amount of events to be run over in a single batch job
const int DEBUG = -1;
//const std::string INPUT_FILENAME = "CSCDigiTree161031.root";
//const std::string INPUT_FILENAME = "CSCDigiTree_Charmonium2017.root";
//const std::string INPUT_FILENAME = "../data/CSCDigiTree-2017D-1.root";
const bool USE_COMP_HITS = 1; //false uses recHits
const unsigned int NLAYERS = 6; //6 layers
const unsigned int MAX_PATTERN_WIDTH = 11;
const unsigned int N_MAX_HALF_STRIPS = 2*80 + 1; //+1 from staggering of chambers
const unsigned int N_LAYER_REQUIREMENT = 3;
const unsigned int TIME_CAPTURE_WINDOW = 4; //allow for 4 consecutive time bins when looking at comparator hits


//labels of all envelopes
const unsigned int NENVELOPES = 5;
const unsigned  int ENVELOPE_IDS[NENVELOPES] = {100,90,80, 70, 60};


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
