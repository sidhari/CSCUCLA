/*
 * EmulationTreeCreator.h
 *
 *  Created on: July 9, 2019
 *      Author: Siddharth Hariprakash
 */

#ifndef CSCPATTERNS_INCLUDE_EmulationTreeCreator_H_
#define CSCPATTERNS_INCLUDE_EmulationTreeCreator_H_

#include "../include/Processor.h"

class EmulationTreeCreator : public  Processor {
	int run(std::string inputfile, std::string outputfile, int start=0, int end=-1);
};


#endif