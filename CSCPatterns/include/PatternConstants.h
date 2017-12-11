/*
 * PatternConstants.h
 *
 *  Created on: Oct 13, 2017
 *      Author: williamnash
 */

#ifndef PATTERNCONSTANTS_H_
#define PATTERNCONSTANTS_H_


const int DEBUG = 0;
const int NLAYERS = 6; //6 layers
const unsigned int MAX_ENTRY = 200000; //how many events you look at (max ~2,000,000 for 2016F)
const bool USE_COMP_HITS = 0; //false uses recHits
const int MAX_PATTERN_WIDTH = 11;
const int N_MAX_HALF_STRIPS = 2*80 + 1; //+1 from staggering of chambers
const int MAX_COMP_TIME_BIN = 15;
const int TIME_RANGE = USE_COMP_HITS ? 2: 1e9; //don't care about range for recHits
const bool MAKE_MATCH_LAYER_COMPARISON = false;
const int N_LAYER_REQUIREMENT = 3;


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


#endif /* PATTERNCONSTANTS_H_ */
