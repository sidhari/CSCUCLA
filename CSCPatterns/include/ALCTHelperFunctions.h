/* 
 * ALCTHelperFunctions.h
 * 
 * Created on: 7 August 2019
 *      Author: Chau Dao 
 */

#ifndef ALCTHELPERFUNCTIONS_H_
#define ALCTHELPERFUNCTIONS_H_

#include "../include/CSCClasses.h"
#include "../include/CSCInfo.h"

#include <math.h>

#include "TTree.h"

// Converts a one-shot pulse to a vector of integers, where the elements of 
// the vectors are time bins in which the wire is on
std::vector<int> pulse_to_vec(unsigned int pulse);

// print out an ASCII representation of the pulse
void print_pulse(unsigned int pulse);

unsigned int extend_time(const unsigned int pulse, const int p_ext=1);

int searchForMatch(const ALCT_ChamberHits &c, const vector<CSCPattern>* ps, vector<ALCTCandidate*> &m);





#endif 