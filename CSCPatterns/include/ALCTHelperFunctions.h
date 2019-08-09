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


std::vector<int> pulse_to_vec(unsigned int pulse);

void print_pulse(unsigned int pulse);

//int searchForMatch(const ALCTChamberHits &c, const vector<CSCPattern>* ps, vector<ALCTCandidate*> &m);

#endif 